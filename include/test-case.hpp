#include <iostream>
#include <fstream>
#include <ctime>
#include <cassert>
#include "message.hpp"
#include <vector>
#include <string>

using namespace std;
using namespace dyn;

void generate_test(uint64_t operations, int ratio) {
	assert(operations > 0);

	double insert_ratio = (float)ratio / (float)100;

	// Seed random number generator with time
	srand(time(0));
	auto insert_counter = 0;
	ofstream file("..\\test-" + to_string(operations) + "-" + to_string(ratio) + ".txt");
	for (uint64_t i = 0; i < operations; i++) {
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
				file << "RANK " << position << '\n';
			}
		}
	}
	file.close();
}

const vector<message> read_test() {
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
			else if (parts[0] == "RANK") {
				messages.push_back(rank_message(stoi(parts[1])));
			}
		}
		file.close();
	}
	file.close();
	return messages;
}

const vector<message> generate_ram_test(uint64_t operations, int ratio) {
	assert(operations > 0);

	double insert_ratio = (float)ratio / (float)100;

	auto messages = vector<message>();
	// Seed random number generator with time
	srand(time(0));
	auto insert_counter = 0;
	for (uint64_t i = 0; i < operations; i++) {
		auto insert = insert_counter == 0 || rand() % 11 <= insert_ratio * 10;
		if (insert) {
			const auto position = rand() % (insert_counter + 1);
			const auto bit = rand() % 2;
			messages.push_back(insert_message(position, bit));
			insert_counter++;
		}
		else {
			const auto position = rand() % (insert_counter);
			messages.push_back(rank_message(position + 1));
		}
	}
	return messages;
}

template<class K> void execute_test(const vector<message>& messages, K& tree) {
	for (const auto& message : messages) {
		if (message.get_type() == message_type::insert) {
			tree.insert(message.get_index(), message.get_val());
		}
		else if (message.get_type() == message_type::remove) {
			tree.remove(message.get_index());
		}
		else if (message.get_type() == message_type::update) {
			tree.set(message.get_index(), message.get_val());
		}
		else if (message.get_type() == message_type::rank) {
			tree.rank(message.get_index());
		}
		else {
			throw "Invalid message type.";
		}
	}
}