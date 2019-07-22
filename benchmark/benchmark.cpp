#include "benchmark.h"
#include "packed_vector.hpp"
#include "succinct-bitvector.hpp"
#include "test-case.hpp"
#include "b-spsi.hpp"
#include "be-spsi.hpp"
#include "int_vector.hpp"
#include "sdsl-bv.hpp"

static int64_t const no_value = 0;

static int64_t const branching_min = 4;
static int64_t const branching_max = 16;
static int64_t const branching_multiplier = 2;

static int64_t const leaf_min = 64;
static int64_t const leaf_max = 4096;
static int64_t const leaf_multiplier = 4;

static int64_t const buffer_min = 4;
static int64_t const buffer_max = 16;
static int64_t const buffer_multiplier = 2;
static int64_t const sdsl_max = 8196;

static uint64_t data_size = 10000;

template <class T> static void Query(benchmark::State& state) {
	T tree{};

	for (auto i = 0; i < data_size; i++) {
		tree.push_back(true);
	}

	uint64_t index = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(tree.at(index));
		index++;
		if (index == data_size)
		{
			index = 0;
		}
	}
}

BENCHMARK_TEMPLATE(Query, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
BENCHMARK_TEMPLATE(Query, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);
BENCHMARK_TEMPLATE(Query, sdsl_bv<64, 64>);
BENCHMARK_TEMPLATE(Query, sdsl_bv<128, 128>);
BENCHMARK_TEMPLATE(Query, sdsl_bv<256, 256>);
BENCHMARK_TEMPLATE(Query, sdsl_bv<512, 512>);
BENCHMARK_TEMPLATE(Query, sdsl_bv<1024, 1024>);

template <class T> static void Insertion(benchmark::State& state) {
	T tree{};

	uint64_t index = 0;
	for (auto _ : state) {
		tree.insert(index, true);
		index++;
	}
}

BENCHMARK_TEMPLATE(Insertion, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
BENCHMARK_TEMPLATE(Insertion, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);
BENCHMARK_TEMPLATE(Insertion, sdsl_bv<64, 64>);
BENCHMARK_TEMPLATE(Insertion, sdsl_bv<128, 128>);
BENCHMARK_TEMPLATE(Insertion, sdsl_bv<256, 256>);
BENCHMARK_TEMPLATE(Insertion, sdsl_bv<512, 512>);
BENCHMARK_TEMPLATE(Insertion, sdsl_bv<1024, 1024>);

template <class T> static void Deletion(benchmark::State& state) {
	T tree{};

	for (auto i = 0; i < data_size; i++) {
		tree.push_back(true);
	}

	uint64_t index = 0;
	for (auto _ : state) {
		tree.remove(0);
		++index;
		if (index == data_size)
		{
			state.PauseTiming();
			for (auto i = 0; i < data_size; i++) {
				tree.push_back(true);
			}
			index = 0;
			state.ResumeTiming();
		}
	}
}

BENCHMARK_TEMPLATE(Deletion, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Deletion, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);

template <class T> static void Rank(benchmark::State& state) {
	T tree{};

	for (auto i = 0; i < data_size; i++) {
		tree.push_back(true);
	}

	uint64_t index = 0;
	for (auto _ : state) {
		tree.rank(index + 1);
		++index;
		if (index == data_size)
		{
			index = 0;
		}
	}
}

BENCHMARK_TEMPLATE(Rank, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
BENCHMARK_TEMPLATE(Rank, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);
BENCHMARK_TEMPLATE(Rank, sdsl_bv<64, 64>);
BENCHMARK_TEMPLATE(Rank, sdsl_bv<128, 128>);
BENCHMARK_TEMPLATE(Rank, sdsl_bv<256, 256>);
BENCHMARK_TEMPLATE(Rank, sdsl_bv<512, 512>);
BENCHMARK_TEMPLATE(Rank, sdsl_bv<1024, 1024>);

template <class T> static void Select(benchmark::State& state) {
	T tree{};

	for (auto i = 0; i < data_size; i++) {
		tree.push_back(true);
	}

	uint64_t index = 1;
	for (auto _ : state) {
		tree.select(index);
		++index;
		if (index == data_size / 2)
		{
			index = 1;
		}
	}
}

BENCHMARK_TEMPLATE(Select, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
BENCHMARK_TEMPLATE(Select, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);
BENCHMARK_TEMPLATE(Select, sdsl_bv<64, 64>);
BENCHMARK_TEMPLATE(Select, sdsl_bv<128, 128>);
BENCHMARK_TEMPLATE(Select, sdsl_bv<256, 256>);
BENCHMARK_TEMPLATE(Select, sdsl_bv<512, 512>);
BENCHMARK_TEMPLATE(Select, sdsl_bv<1024, 1024>);

template <class T> static void Operations(benchmark::State& state) {
	auto messages = generate_ram_test(10000, 100);

	T tree{};

	for (auto _ : state) {
		tree = T();
		execute_test(messages, tree);
	}
}

BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);
BENCHMARK_TEMPLATE(Operations, sdsl_bv<64, 256>);
BENCHMARK_TEMPLATE(Operations, sdsl_bv<128, 256>);
BENCHMARK_TEMPLATE(Operations, sdsl_bv<256, 256>);
BENCHMARK_TEMPLATE(Operations, sdsl_bv<512, 256>);
BENCHMARK_TEMPLATE(Operations, sdsl_bv<1024, 256>);

int main(int argc, char** argv)
{
	::benchmark::Initialize(&argc, argv);

	if (::benchmark::ReportUnrecognizedArguments(argc, argv))
	{
		return 1;
	}

	::benchmark::RunSpecifiedBenchmarks();
}