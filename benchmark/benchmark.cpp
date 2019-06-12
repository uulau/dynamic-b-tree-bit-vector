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
	for (auto _ : state) {
		succ_bitvector bv;
		for (int i = 0; i < state.range(0); i++) {
			bv.insert(i, false);
		}
	}
}
BENCHMARK(BTreeInsertion)->Range(8, 8 << 10);

static void BBTreeInsertion(benchmark::State& state) {
	for (auto _ : state) {
		bb_succ_bitvector bv;
		bv.spsi_.init_messages(state.range(1));
		for (int i = 0; i < state.range(0); i++) {
			bv.insert(i, false);
		}
	}
}
BENCHMARK(BBTreeInsertion)->Ranges({ { 1, 8 << 10 }, { 1, 8 << 10 } });

static void BTreeQuery(benchmark::State& state) {
	succ_bitvector bv;

	for (int i = 0; i < state.range(0); i++) {
		bv.insert(i, false);
	}

	for (auto _ : state) {
		for (int i = 0; i < state.range(0); i++) {
			bv.at(i);
		}
	}
}
BENCHMARK(BTreeQuery)->Range(8, 8 << 10);

static void BBTreeQuery(benchmark::State& state) {
	bb_succ_bitvector bv;
	bv.spsi_.init_messages(state.range(1));

	for (int i = 0; i < state.range(0); i++) {
		bv.insert(i, false);
	}

	for (auto _ : state) {
		for (int i = 0; i < state.range(0); i++) {
			bv.at(i);
		}
	}
}
BENCHMARK(BBTreeQuery)->Ranges({ { 1, 8 << 10 }, { 1, 8 << 10 } });

class OperationFixture : public benchmark::Fixture {
public:
	vector<message> messages;

	void SetUp(const ::benchmark::State& state) {
		vector<message> messages = generate_ram_test(1000000, 0.1, 0.9);
	}

	void TearDown(const ::benchmark::State& state) {
		messages.clear();
	}
};

BENCHMARK_F(OperationFixture, BTreeOperations)(benchmark::State& st) {
	succ_bitvector bv;
	for (auto _ : st) {
		execute_test(messages, bv);
	}
}

BENCHMARK_F(OperationFixture, BBTreeOperations)(benchmark::State& st) {
	bb_succ_bitvector bv;
	for (auto _ : st) {
		execute_test(messages, bv);
	}
}

BENCHMARK_MAIN();