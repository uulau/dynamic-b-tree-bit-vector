#include <iostream>
#include <cassert>
#include <time.h>
#include <fstream>

using namespace std;

void generate_test(uint64_t operations, double insert_ratio) {
	assert(operations > 0);

	// Seed random number generator with time
	srand(time(0));
	auto insert_counter = 0;
	ofstream file("..\\test.txt");
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
				file << "QUERY " << position << '\n';
			}
		}
	}
	file.close();
}

int main()
{
	generate_test(1000000, 0.1);
}