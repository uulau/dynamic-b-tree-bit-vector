#include "benchmark.h"
#include <iostream>
#include "packed_vector.hpp"
#include "be-bv.hpp"
#include "b-bv.hpp"
#include "succinct_bitvector.hpp"
#include "test-case.hpp"

typedef b_bv<packed_vector> btree;
typedef be_bv<packed_vector> betree;

static int UNIT_MIN = 1;
static int UNIT_MAX = 8 << 12;


template <class T> static void Query(benchmark::State& state) {
	auto tree{ T(state.range(1), state.range(2), state.range(3)) };

	for (int i = 0; i < state.range(0); i++) {
		tree.push_back(0);
	}

	for (auto _ : state) {
		tree.at(tree.size() - 1);
	}
}

BENCHMARK_TEMPLATE(Query, btree)->Ranges({ { UNIT_MIN, UNIT_MAX }, { UNIT_MIN, UNIT_MAX }, { UNIT_MIN, UNIT_MAX } });
BENCHMARK_TEMPLATE(Query, betree)->Ranges({ { UNIT_MIN, UNIT_MAX }, {UNIT_MIN, UNIT_MAX}, {UNIT_MIN, UNIT_MAX}, {UNIT_MIN, UNIT_MAX} });

template <class T> static void Insertion(benchmark::State& state) {
	auto tree{ T(state.range(1), state.range(2), state.range(3)) };

	for (auto _ : state) {
		tree.push_back(1);
	}
}

BENCHMARK_TEMPLATE(Insertion, btree);
BENCHMARK_TEMPLATE(Insertion, betree)->Range(UNIT_MIN, UNIT_MAX);

BENCHMARK_MAIN();