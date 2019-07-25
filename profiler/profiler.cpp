#include "packed_vector.hpp"
#include "succinct-bitvector.hpp"
#include "sdsl-bv.hpp"
#include "b-spsi.hpp"
#include "test-case.hpp"
#include <chrono>

using namespace std;
using namespace dyn;
using namespace std::chrono;

template <int64_t B_LEAF, int64_t B> uint64_t test()
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

int main() {
	test< 4096, 16>();
	test< 4096, 254>();
	test< 4096, 1024>();
	test< 4096, 4096>();
	test< 4096, 8192>();
}

