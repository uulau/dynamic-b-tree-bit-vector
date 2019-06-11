#include "benchmark.h"
#include <iostream>
#include "packed_vector.hpp"
#include "be-spsi.hpp"
#include "bb-spsi.hpp"
#include "spsi.hpp"
#include "succinct_bitvector.hpp"
#include "test-case.hpp"

typedef succinct_bitvector<b::spsi<packed_vector, 8192, 16>> succ_bitvector;
typedef succinct_bitvector<bb::spsi<packed_vector, 8192, 16>> bb_succ_bitvector;

static void BTreeInsertion(benchmark::State& state) {
	succ_bitvector bv;
	for (auto _ : state) {
		for (int i = 0; i < state.range(0); i++) {
			bv.insert(i, false);
		}
	}
}
BENCHMARK(BTreeInsertion)->Range(8, 8 << 10);

static void BBTreeInsertion(benchmark::State& state) {
	bb_succ_bitvector bv;
	bv.spsi_.init_messages(state.range(0));
	for (auto _ : state) {
		for (int i = 0; i < state.range(1); i++) {
			bv.insert(i, false);
		}
	}
}
BENCHMARK(BBTreeInsertion)->Ranges({ { 1, 8 << 10 }, { 1, 8 << 10 } });

static void BTreeQuery(benchmark::State& state) {
	succ_bitvector bv;
	bv.insert(0, false);

	for (int i = 0; i < state.range(1); i++) {
		bv.insert(i, false);
	}

	for (auto _ : state) {
		for (int i = 0; i < state.range(1); i++) {
			bv.at(i);
		}
	}
}
BENCHMARK(BTreeQuery)->Range(8, 8 << 10);

static void BBTreeQuery(benchmark::State& state) {
	bb_succ_bitvector bv;
	bv.insert(0, false);

	for (auto _ : state) {
		bv.at(0);
	}
}
BENCHMARK(BBTreeQuery);

static void BTreeOperations(benchmark::State& state) {
	succ_bitvector bv;

	vector<message> messages = read_test();

	for (auto _ : state) {
		execute_test(messages, bv);
	}
}
BENCHMARK(BTreeOperations);

static void BBTreeOperations(benchmark::State& state) {
	bb_succ_bitvector bv;
	bv.insert(0, false);

	vector<message> messages = read_test();

	for (auto _ : state) {
		execute_test(messages, bv);
	}
}
BENCHMARK(BBTreeOperations);

BENCHMARK_MAIN();