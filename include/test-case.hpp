#include <iostream>
#include <fstream>
#include <time.h>
#include <cassert>
#include "message.hpp"
#include <vector>
#include <string>
#include "succinct_bitvector.hpp"

using namespace std;
using namespace dyn;

void generate_test(uint64_t operations, double insert_ratio, double query_ratio);

vector<message> read_test();

void generate_test(uint64_t operations, double insert_ratio, double query_ratio) {
	assert(insert_ratio + query_ratio == 1);
	assert(operations > 0);

	auto insert_count = operations * insert_ratio;
	auto query_count = operations * query_ratio;
	// Seed random number generator with time
	srand(time(0));
	auto insert_counter = 0;
	ofstream file("..\\test.txt");
	for (auto i = 0; i < operations; i++) {
		auto insert = insert_counter == 0 || rand() % 11 <= insert_ratio * 10;
		if (file.is_open()) {
			if (insert) {
				auto position = rand() % (insert_counter + 1);
				auto bit = rand() % 2;
				file << "INSERT " << position << " " << bit << '\n';
				insert_counter++;
			}
			else {
				auto position = rand() % (insert_counter);
				file << "QUERY " << position << '\n';
			}
		}
	}
	file.close();
}

vector<message> read_test() {
	vector<message> messages;
	ifstream file("..\\test.txt");

	string line;
	if (file.is_open())
	{
		string delimiter = " ";
		while (getline(file, line))
		{
			size_t pos = 0;
			vector<string> parts;
			while ((pos = line.find(delimiter)) != string::npos) {
				parts.push_back(line.substr(0, pos));
				line.erase(0, pos + delimiter.length());
			}
			parts.push_back(line);
			if (parts[0] == "INSERT") {
				messages.push_back(insert_message(stoi(parts[1]), stoi(parts[2])));
			}
			else if (parts[0] == "QUERY") {
				messages.push_back(query_message(stoi(parts[1])));
			}
		}
		file.close();
	}
	file.close();
	return messages;
}

vector<message> generate_ram_test(uint64_t operations, double insert_ratio, double query_ratio) {
	assert(insert_ratio + query_ratio == 1);
	assert(operations > 0);

	vector<message> messages;
	auto insert_count = operations * insert_ratio;
	auto query_count = operations * query_ratio;
	// Seed random number generator with time
	srand(time(0));
	auto insert_counter = 0;
	for (auto i = 0; i < operations; i++) {
		auto insert = insert_counter == 0 || rand() % 11 <= insert_ratio * 10;
		if (insert) {
			auto position = rand() % (insert_counter + 1);
			auto bit = rand() % 2;
			messages.push_back(insert_message(position, bit));
			insert_counter++;
		}
		else {
			auto position = rand() % (insert_counter);
			messages.push_back(query_message(position));
		}
	}
	return messages;
}

template<class K> void execute_test(const vector<message>& messages, K tree) {
	for (message message : messages) {
		if (message.type == message_type::insert) {
			tree.insert(message.index, message.value);
		}
		else if (message.type == message_type::query) {
			tree.at(message.index);
		}
		else if (message.type == message_type::remove) {
			tree.remove(message.index);
		}
		else if (message.type == message_type::update) {
			tree.set(message.index, message.value);
		}
		else {
			throw "Invalid message type.";
		}
	}
}