#include "benchmark.h"
#include <iostream>
#include "packed_vector.hpp"
#include "be-bv.hpp"
#include "b-bv.hpp"
#include "test-case.hpp"

typedef b_bv<packed_vector> bbv;
typedef be_bv<packed_vector> bebv;

static int const UNIT_MIN = 1;
static int const UNIT_MAX = 8 << 12;
static int const NO_BUFFER = 0;
static int const BRANCHING_MAX = 16;
static int const LEAF_MAX = 128;

static int DATA_SIZE = 100000;

template <class T> static void Query(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

	for (auto i = 0; i < DATA_SIZE; i++) {
		tree.push_back(true);
	}

	uint64_t index = 0;
	for (auto _ : state) {
		tree.at(index);
		index++;
		if (index == DATA_SIZE)
		{
			index = 0;
		}
	}
}

BENCHMARK_TEMPLATE(Query, bbv)->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, 8192}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Query, bebv)->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, 8192}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Insertion(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

	uint64_t index = 0;
	for (auto _ : state) {
		tree.insert(index, true);
		index++;
	}
}

BENCHMARK_TEMPLATE(Insertion, bbv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, 8192}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Insertion, bebv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, 8192}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Deletion(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

	for (auto i = 0; i < DATA_SIZE; i++) {
		tree.push_back(true);
	}

	uint64_t index = 0;
	for (auto _ : state) {
		tree.remove(0);
		++index;
		if (index == DATA_SIZE)
		{
			state.PauseTiming();
			for (auto i = 0; i < DATA_SIZE; i++) {
				tree.push_back(true);
			}
			index = 0;
			state.ResumeTiming();
		}
	}
}

BENCHMARK_TEMPLATE(Deletion, bbv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Deletion, bebv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Rank(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

	for (auto i = 0; i < DATA_SIZE; i++) {
		tree.push_back(true);
	}

	uint64_t index = 0;
	for (auto _ : state) {
		tree.rank(index + 1);
		++index;
		if (index == DATA_SIZE)
		{
			index = 0;
		}
	}
}

BENCHMARK_TEMPLATE(Rank, bbv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Rank, bebv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Select(benchmark::State& state) {
	T tree(state.range(0), state.range(1), state.range(2));

	for (auto i = 0; i < DATA_SIZE; i++) {
		tree.push_back(true);
	}

	uint64_t index = 0;
	for (auto _ : state) {
		tree.select(index);
		++index;
		if (index == DATA_SIZE)
		{
			index = 0;
		}
	}
}

BENCHMARK_TEMPLATE(Select, bbv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Select, bebv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Operations(benchmark::State& state) {
	auto messages = generate_ram_test(10000000, 10);

	for (auto _ : state) {
		T tree(state.range(0), state.range(1), state.range(2));
		execute_test(messages, tree);
	}
}

BENCHMARK_TEMPLATE(Operations, bbv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Operations, bebv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

BENCHMARK_MAIN();