#include "benchmark.h"
#include <iostream>
#include "packed_vector.hpp"
#include "be-bv.hpp"
#include "b-bv.hpp"
#include "succinct_bitvector.hpp"
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
	auto tree{ T(state.range(0), state.range(1), state.range(2)) };

	auto i = 0;
	for (auto _ : state) {
		tree.at(i);
		state.PauseTiming();
		tree.push_back(true);
		state.ResumeTiming();
	}
}

BENCHMARK_TEMPLATE(Query, bbv)->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Query, bebv)->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Insertion(benchmark::State& state) {
	auto tree{ T(state.range(0), state.range(1), state.range(2)) };

	for (auto _ : state) {
		state.PauseTiming();
		auto size = tree.size();
		state.ResumeTiming();
		tree.insert(size, true);
	}
}

BENCHMARK_TEMPLATE(Insertion, bbv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Insertion, bebv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Deletion(benchmark::State& state) {
	auto tree{ T(state.range(0), state.range(1), state.range(2)) };

	for (auto _ : state) {
		state.PauseTiming();
		if (tree.size() == 0) {
			for (int i = 0; i < 100000; i++) {
				tree.push_back(true);
			}
		}
		state.ResumeTiming();
		tree.remove(0);
	}
}

BENCHMARK_TEMPLATE(Deletion, bbv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Deletion, bebv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Rank(benchmark::State& state) {
	auto tree{ T(state.range(0), state.range(1), state.range(2)) };

	for (auto _ : state) {
		state.PauseTiming();
		tree.push_back(1);
		auto index = tree.size();
		state.ResumeTiming();
		tree.rank(index);
	}
}

BENCHMARK_TEMPLATE(Rank, bbv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Rank, bebv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Select(benchmark::State& state) {
	auto tree{ T(state.range(0), state.range(1), state.range(2)) };

	for (auto _ : state) {
		state.PauseTiming();
		tree.push_back(1);
		auto index = tree.size();
		state.ResumeTiming();
		tree.select(index - 1);
	}
}

BENCHMARK_TEMPLATE(Select, bbv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Select, bebv)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

BENCHMARK_MAIN();