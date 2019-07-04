#include "benchmark.h"
#include <iostream>
#include "packed_vector.hpp"
#include "b-bv.hpp"
#include "test-case.hpp"
#include "b-node.hpp"
#include "be-node.hpp"
#include "int_vector.hpp"
#include "rank_support_v5.hpp"
#include "util.hpp"

typedef b_bv<packed_vector, b_node> bbv;
typedef b_bv<packed_vector, be_node> bebv;

static int const no_value = 0;

static int const branching_min = 4;
static int const branching_max = 16;
static int const branching_multiplier = 2;

static int const leaf_min = 256;
static int const leaf_max = 4096;
static int const leaf_multiplier = 4;

static int const buffer_min = 4;
static int const buffer_max = 16;
static int const buffer_multiplier = 2;

static int data_size = 100000;

static void be_tree(benchmark::internal::Benchmark* benchmark) {
	for (auto b = branching_min; b <= branching_max; b *= branching_multiplier)
	{
		for (auto b_leaf = leaf_min; b_leaf <= leaf_max; b_leaf *= leaf_multiplier)
		{
			for (auto buffer = buffer_min; buffer <= buffer_max; buffer *= buffer_multiplier)
			{
				benchmark->Args({ b, b_leaf, buffer });
			}
		}
	}
}

static void b_tree(benchmark::internal::Benchmark* benchmark) {
	for (auto b = branching_min; b <= branching_max; b *= branching_multiplier)
	{
		for (auto b_leaf = leaf_min; b_leaf <= leaf_max; b_leaf *= leaf_multiplier)
		{
			benchmark->Args({ b, b_leaf, no_value });
		}
	}
}

static void sdsl_tree(benchmark::internal::Benchmark* benchmark) {

	for (auto buffer = buffer_min; buffer <= buffer_max; buffer *= buffer_multiplier)
	{
		benchmark->Args({ no_value, no_value, buffer });
	}
}

static void rank_build(benchmark::State& state) {
	sdsl::bit_vector bv(state.range(0));

	for (int i = 0; i < bv.size(); ++i)
	{
		if (i % 2)
		{
			bv[i] = true;
		}
		else
		{
			bv[i] = false;
		}
	}

	sdsl::rank_support_v5<1, 1> rs;

	for (auto _ : state) {
		sdsl::util::init_support(rs, &bv);
	}
}

//BENCHMARK(rank_build)->Range(1024, 33554432);

static void rank_performance(benchmark::State& state) {
	sdsl::bit_vector bv(state.range(0));

	for (auto i = 0; i < bv.size(); ++i)
	{
		if (i % 2)
		{
			bv[i] = true;
		}
		else
		{
			bv[i] = false;
		}
	}

	sdsl::rank_support_v5<1, 1> rs;

	sdsl::util::init_support(rs, &bv);

	vector<int> randoms;
	const auto max = bv.size();

	for (auto i = 0; i < state.range(1); ++i)
	{
		randoms.push_back(rand() % max);
	}

	for (auto _ : state) {
		for (int64_t i = 0; i < state.range(1); ++i)
		{
			benchmark::DoNotOptimize(rs.rank(randoms[i]));
		}
	}
}

//BENCHMARK(rank_performance)->Ranges({ {1024, 33554432}, {1024, 33554432} });

template <class T> static void Query(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

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

BENCHMARK_TEMPLATE(Query, bbv)->Apply(b_tree);
BENCHMARK_TEMPLATE(Query, bebv)->Apply(be_tree);

template <class T> static void Insertion(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

	uint64_t index = 0;
	for (auto _ : state) {
		tree.insert(index, true);
		index++;
	}
}

BENCHMARK_TEMPLATE(Insertion, bbv)->Apply(b_tree);
BENCHMARK_TEMPLATE(Insertion, bebv)->Apply(be_tree);

template <class T> static void Deletion(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

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

BENCHMARK_TEMPLATE(Deletion, bbv)->Apply(b_tree);
BENCHMARK_TEMPLATE(Deletion, bebv)->Apply(be_tree);

template <class T> static void Rank(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

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

BENCHMARK_TEMPLATE(Rank, bbv)->Apply(b_tree);
BENCHMARK_TEMPLATE(Rank, bebv)->Apply(be_tree);

template <class T> static void Select(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

	for (auto i = 0; i < data_size; i++) {
		tree.push_back(true);
	}

	uint64_t index = 0;
	for (auto _ : state) {
		tree.select(index);
		++index;
		if (index == data_size)
		{
			index = 0;
		}
	}
}

BENCHMARK_TEMPLATE(Select, bbv)->Apply(b_tree);
BENCHMARK_TEMPLATE(Select, bebv)->Apply(be_tree);

template <class T> static void Operations(benchmark::State& state) {
	auto messages = generate_ram_test(10000000, 10);

	for (auto _ : state) {
		T tree(state.range(0), state.range(1), state.range(2));
		execute_test(messages, tree);
	}
}

BENCHMARK_TEMPLATE(Operations, bbv)->Apply(b_tree);
BENCHMARK_TEMPLATE(Operations, bebv)->Apply(be_tree);

int main(int argc, char** argv)
{
	::benchmark::Initialize(&argc, argv);

	if (::benchmark::ReportUnrecognizedArguments(argc, argv))
	{
		return 1;
	}

	::benchmark::RunSpecifiedBenchmarks();
}
