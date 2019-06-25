#include "benchmark.h"
#include <iostream>
#include "packed_vector.hpp"
#include "be-bv.hpp"
#include "b-bv.hpp"
#include "succinct_bitvector.hpp"
#include "test-case.hpp"

typedef b_bv<packed_vector> btree;
typedef be_bv<packed_vector> betree;

static int const UNIT_MIN = 1;
static int const UNIT_MAX = 8 << 12;
static int const NO_BUFFER = 0;
static int const BRANCHING_MAX = 16;
static int const LEAF_MAX = 128;

template <class T> static void Query(benchmark::State& state) {
	auto tree{ T(state.range(0), state.range(1), state.range(2)) };

	for (auto _ : state) {
		state.PauseTiming();
		tree.push_back(1);
		auto index = tree.size() - 1;
		state.ResumeTiming();
		tree.at(index);
	}
}

BENCHMARK_TEMPLATE(Query, btree)->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Query, betree)->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Insertion(benchmark::State& state) {
	auto tree{ T(state.range(0), state.range(1), state.range(2)) };

	for (auto _ : state) {
		tree.push_back(1);
	}
}

BENCHMARK_TEMPLATE(Insertion, btree)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Insertion, betree)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Deletion(benchmark::State& state) {
	auto tree{ T(state.range(0), state.range(1), state.range(2)) };

	for (auto _ : state) {
		state.PauseTiming();
		tree.push_back(1);
		state.ResumeTiming();
		tree.remove(0);
	}
}

BENCHMARK_TEMPLATE(Deletion, btree)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Deletion, betree)
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

BENCHMARK_TEMPLATE(Rank, btree)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Rank, betree)
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

BENCHMARK_TEMPLATE(Select, btree)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {NO_BUFFER, NO_BUFFER} });
BENCHMARK_TEMPLATE(Select, betree)
->Ranges({ { BRANCHING_MAX, BRANCHING_MAX }, {LEAF_MAX, LEAF_MAX}, {UNIT_MIN, UNIT_MAX} });

BENCHMARK_MAIN();