#include <iostream>
#include <fstream>
#include <ctime>
#include <cassert>
#include "message.hpp"
#include <vector>
#include <string>

using namespace std;
using namespace dyn;

struct test_message
{
	enum message_type
	{
		insert = 0,
		rank = 1
	};

	message_type type;
	uint64_t index;
	bool val;

	static test_message insert_message(uint64_t i, bool v)
	{
		return test_message{ insert, i , v };
	}

	static test_message rank_message(uint64_t i)
	{
		return test_message{ rank, i, 0 };
	}
};

inline void generate_test(const uint64_t operations, const int ratio) {
	assert(operations > 0);

	const double insert_ratio = float(ratio) / float(100);

	// Seed random number generator with time
	srand(time(0));
	auto insert_counter = 0;
	ofstream file("..\\test-" + to_string(operations) + "-" + to_string(ratio) + ".txt");
	for (uint64_t i = 0; i < operations; i++) {
		const auto insert = insert_counter == 0 || rand() % 11 <= insert_ratio * 10;
		if (file.is_open()) {
			if (insert) {
				const auto position = rand() % (insert_counter + 1);
				const auto bit = rand() % 2;
				file << "INSERT " << position << " " << bit << '\n';
				insert_counter++;
			}
			else {
				const auto position = rand() % (insert_counter);
				file << "RANK " << position << '\n';
			}
		}
	}
	file.close();
}

inline vector<message> read_test()
{
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

inline vector<test_message> generate_ram_test(const uint64_t operations, const int ratio)
{
	assert(operations > 0);

	const double insert_ratio = float(ratio) / float(100);

	auto messages = vector<test_message>();
	// Seed random number generator with time
	srand(time(nullptr));
	auto insert_counter = 0;
	for (uint64_t i = 0; i < operations; i++) {
		const auto insert = insert_counter == 0 || rand() % 11 <= insert_ratio * 10;
		if (insert) {
			const auto position = rand() % (insert_counter + 1);
			const auto bit = rand() % 2;
			messages.push_back(test_message::insert_message(position, bit));
			insert_counter++;
		}
		else {
			const auto position = rand() % (insert_counter);
			messages.push_back(test_message::rank_message(position + 1));
		}
	}
	return messages;
}

template<class K> void execute_test(const vector<test_message>& messages, K& tree) {
	for (const auto& message : messages) {
		if (message.type == test_message::message_type::insert) {
			tree.insert(message.index, message.val);
		}
		else if (message.type == test_message::message_type::rank) {
			tree.rank(message.index);
		}
		else {
			throw "Invalid message type.";
		}
	}
}