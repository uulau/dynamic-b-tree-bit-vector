#include "packed_vector2.hpp"
#include "succinct-bitvector.hpp"
#include "sdsl-bv.hpp"
#include "b-spsi.hpp"
#include "test-case.hpp"
#include <chrono>
#include "int_vector.hpp"
#include "rank_support_v5.hpp"
#include "util.hpp"

using namespace std;
using namespace dyn;
using namespace std::chrono;

template <int64_t B_LEAF, int64_t B> uint64_t test_tree()
{
	const uint64_t inserts = 8000000000;
	const uint64_t ranks = 100000000;

	cout << "B: " << B << "\n";
	cout << "B_LEAF: " << B_LEAF << "\n";
	cout << "Inserts: " << inserts << "\n";
	cout << "Ranks: " << ranks << "\n";

	uint64_t count = 0;

	succinct_bitvector<packed_vector, B_LEAF, B, 0, b_spsi> tree;

	std::random_device rd;

	std::default_random_engine generator(rd());

	std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	for (uint64_t i = 0; i < inserts / 64; ++i)
	{
		tree.push_word(distribution(generator), 64);
	}

	cout << "Total size in bits: " << tree.bit_size() << "\n";

	vector<test_message> messages;

	for (uint64_t i = 0; i < ranks; ++i)
	{
		messages.push_back(test_message::rank_message(distribution(generator) % (inserts + 1)));
	}

	const auto t1 = high_resolution_clock::now();

	for (const auto& message : messages)
	{
		count += tree.rank(message.index);
	}

	const auto t2 = high_resolution_clock::now();

	const auto duration = duration_cast<microseconds>(t2 - t1).count();

	cout << "Tree depth: " << tree.depth() << "\n";
	cout << "Size (amount of bits inserted): " << tree.size() << "\n";
	cout << "Time taken in microseconds: " << duration << "\n";
	cout << "\n";

	return count;
}

uint64_t test_packed()
{
	uint64_t count = 0;
	uint64_t size = 256;
	uint64_t ranks = 100000000;

	std::random_device rd;

	std::default_random_engine generator(rd());

	std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	// Empty bit vector
	packed_vector vector{};

	for (uint64_t i = 0; i < size / 64; ++i)
	{
		vector.insert_word(vector.size(), distribution(generator), 1, 64);
	}

	std::vector<test_message> messages;

	for (uint64_t i = 0; i < ranks; ++i)
	{
		messages.push_back(test_message::rank_message(distribution(generator) % size));
	}

	const auto t1 = high_resolution_clock::now();

	for (const auto& message : messages)
	{
		count += vector.psum(message.index);
	}

	const auto t2 = high_resolution_clock::now();

	const auto duration = duration_cast<microseconds>(t2 - t1).count();

	cout << "Initial insertions: " << size << "\n";
	cout << "Ranks: " << ranks << "\n";
	cout << "Time taken in microseconds: " << duration << "\n";
	cout << "\n";

	return count;
}

uint64_t test_packed2()
{
	uint64_t count = 0;
	uint64_t size = 256;
	uint64_t inserts = 10000000;

	std::random_device rd;

	std::default_random_engine generator(rd());

	std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	// Empty bit vector
	packed_vector vector{};

	for (uint64_t i = 0; i < size / 64; ++i)
	{
		vector.insert_word(vector.size(), distribution(generator), 1, 64);
	}

	std::vector<test_message> messages;

	for (uint64_t i = 0; i < inserts; ++i)
	{
		messages.push_back(test_message::insert_message(distribution(generator) % size, rand() % 2));
	}

	const auto t1 = high_resolution_clock::now();

	for (const auto& message : messages)
	{
		if (vector.size() == size) {
			vector.split();
		}
		vector.insert(message.index, message.val);
	}

	const auto t2 = high_resolution_clock::now();

	const auto duration = duration_cast<microseconds>(t2 - t1).count();

	cout << "Initial insertions: " << size << "\n";
	cout << "Following insertions: " << inserts << "\n";
	cout << "Time taken in microseconds: " << duration << "\n";
	cout << "\n";

	return count;
}

uint64_t test_sdsl()
{
	uint64_t count = 0;
	uint64_t size = 8000000000;
	uint64_t ranks = 100000000;

	std::random_device rd;

	std::default_random_engine generator(rd());

	std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	// Empty bit vector
	sdsl::bit_vector bv(size);

	const sdsl::rank_support_v5<1, 1> rs(&bv);

	for (uint64_t i = 0; i < size; ++i)
	{
		bv[i] = rand() % 2;
	}

	std::vector<test_message> messages;

	for (uint64_t i = 0; i < ranks; ++i)
	{
		messages.push_back(test_message::rank_message(distribution(generator) % size));
	}

	const auto t1 = high_resolution_clock::now();

	for (const auto& message : messages)
	{
		count += rs.rank(message.index);
	}

	const auto t2 = high_resolution_clock::now();

	const auto duration = duration_cast<microseconds>(t2 - t1).count();

	cout << "Initial insertions: " << size << "\n";
	cout << "Ranks: " << ranks << "\n";
	cout << "Time taken in microseconds: " << duration << "\n";
	cout << "\n";

	return count;
}

uint64_t test_array()
{
	uint64_t count = 0;
	uint64_t size = 8000000000;
	uint64_t queries = 100000000;

	std::random_device rd;

	std::default_random_engine generator(rd());

	std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	bool* ar = new bool[size];

	for (uint64_t i = 0; i < size; ++i)
	{
		ar[i] = rand() % 2;
	}

	std::vector<test_message> messages;

	for (uint64_t i = 0; i < queries; ++i)
	{
		messages.push_back(test_message::rank_message(distribution(generator) % size));
	}

	const auto t1 = high_resolution_clock::now();

	for (const auto& message : messages)
	{
		count += ar[message.index];
	}

	const auto t2 = high_resolution_clock::now();

	const auto duration = duration_cast<microseconds>(t2 - t1).count();

	cout << "Initial insertions: " << size << "\n";
	cout << "Queries: " << queries << "\n";
	cout << "Time taken in microseconds: " << duration << "\n";
	cout << "\n";

	delete[] ar;
	return count;
}

uint64_t test_sdsl_array()
{
	uint64_t count = 0;
	uint64_t size = 8000000000;
	uint64_t queries = 100000000;

	std::random_device rd;

	std::default_random_engine generator(rd());

	std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	// Empty bit vector
	sdsl::bit_vector bv(size);

	for (uint64_t i = 0; i < size; ++i)
	{
		bv[i] = rand() % 2;
	}

	std::vector<test_message> messages;

	for (uint64_t i = 0; i < queries; ++i)
	{
		messages.push_back(test_message::rank_message(distribution(generator) % size));
	}

	const auto t1 = high_resolution_clock::now();

	for (const auto& message : messages)
	{
		count += bv[message.index];
	}

	const auto t2 = high_resolution_clock::now();

	const auto duration = duration_cast<microseconds>(t2 - t1).count();

	cout << "Initial insertions: " << size << "\n";
	cout << "Queries: " << queries << "\n";
	cout << "Time taken in microseconds: " << duration << "\n";
	cout << "\n";

	return count;
}

uint64_t packed_vector_test() {
	packed_vector v{};

	uint64_t ranks = 0;
	uint64_t vals = 0;
	for (int i = 0; i < 32767; ++i) {
		auto val = rand() % 2;

		if (val) ++ranks;
		vals += val;

		v.insert(v.size(), val);

		if (v.size() != i + 1) {
			throw;
		}

		if (v.psum(i) != ranks) {
			throw;
		}

		if (v.at(i) != val) {
			throw;
		}

		if (vals != v.psum()) {
			throw;
		}
	}
	return vals;
}

int main() {
	uint64_t count = 0;

	succinct_bitvector<packed_vector, 4096, 256, 0, b_spsi> tree;

	//for (uint64_t i = 0; i < 80000000; ++i) {
	//	tree.insert(i >> 2, i % 2);
	//}

	//for (uint64_t i = 0; i < 8000000000; i += 64) {
	//	tree.push_word(i, 64);
	//}

	//for (uint64_t i = 0; i < 1000000000; ++i) {
	//	count += tree.rank(i + 1);
	//}

	/*count += tree.size();*/

	//for (uint64_t i = 0; i < 8000000000; i += 64) {
	//	tree.push_word(i, 64);
	//}

	//for (uint64_t ranks = 0; ranks < 8000000000; ++ranks) {
	//	count += tree.rank(ranks + 1);
	//}

	//count += tree.size();

	//for (uint64_t i = 0; i < 8000000000; ++i) {
	//	count += tree.rank(i + 1);
	//}

	//return packed_vector_test();
	//return test_sdsl();
	//count += test_packed();
	//count += test_sdsl_array();
	//count += test_array();
	count += test_tree< 4096, 16>();
	count += test_tree< 4096, 256>();
	count += test_tree< 4096, 1024>();
	count += test_tree< 4096, 4096>();
	count += test_tree< 4096, 8192>();
	//count += test_tree< 4096, 2 * 8192>();
	//count += test_tree< 4096, 3 * 8192>();
	//count += test_tree< 4096, 65536>();
	//count += test_tree< 2 * 4096, 1024>();
	//count += test_tree< 4 * 4096, 1024>();
	//count += test_tree< 6 * 4096, 1024>();
	//count += test_tree< 8 * 4096, 1024>();
	//count += test_tree< 12 * 4096, 1024>();
	//count += test_tree< 16 * 4096, 1024>();
	//count += packed_vector_test();
	return count;
}

