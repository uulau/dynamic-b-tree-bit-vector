#include "benchmark.h"
#include <iostream>
#include "packed_vector.hpp"
#include "be-spsi.hpp"
#include "bb-spsi.hpp"
#include "spsi.hpp"
#include "succinct_bitvector.hpp"
#include "test-case.hpp"

typedef b::spsi<packed_vector, 8192, 16> btree;
typedef bb::spsi<packed_vector, 8192, 16> bbtree;
typedef be::spsi<packed_vector, 8192, 16> betree;

static int UNIT_MAX = 8 << 15;
static int BUFFER_MAX = 8 << 12;

static void BTreeInsertion(benchmark::State& state) {
	for (auto _ : state) {
		btree bv;
		for (int i = 0; i < state.range(0); i++) {
			bv.insert(i, state.range(0) - i);
		}
	}
}
BENCHMARK(BTreeInsertion)->Range(8, 8 << 10);

static void BBTreeInsertion(benchmark::State& state) {
	for (auto _ : state) {
		auto bv = bbtree(0, 0, state.range(1));
		for (int i = 0; i < state.range(0); i++) {
			bv.insert(i, state.range(0) - i);
		}
	}
}
BENCHMARK(BBTreeInsertion)->Ranges({ { 1, UNIT_MAX }, { 1, BUFFER_MAX } });

static void BETreeInsertion(benchmark::State& state) {
	for (auto _ : state) {
		auto bv = betree(0, 0, state.range(1));
		for (int i = 0; i < state.range(0); i++) {
			bv.insert(i, state.range(0) - i);
		}
	}
}
BENCHMARK(BETreeInsertion)->Ranges({ { 1, UNIT_MAX }, { 1, BUFFER_MAX } });

static void BTreeQuery(benchmark::State& state) {
	btree bv;

	for (int i = 0; i < state.range(0); i++) {
		bv.insert(i, state.range(0) - i);
	}

	for (auto _ : state) {
		for (int i = 0; i < state.range(0); i++) {
			bv.at(i);
		}
	}
}
BENCHMARK(BTreeQuery)->Range(1, UNIT_MAX);

static void BBTreeQuery(benchmark::State& state) {
	auto bv = bbtree(0, 0, state.range(1));

	for (int i = 0; i < state.range(0); i++) {
		bv.insert(i, state.range(0) - i);
	}

	for (auto _ : state) {
		for (int i = 0; i < state.range(0); i++) {
			bv.at(i);
		}
	}
}
BENCHMARK(BBTreeQuery)->Ranges({ { 1, UNIT_MAX }, { 1, BUFFER_MAX } });

static void BETreeQuery(benchmark::State& state) {
	auto bv = betree(0, 0, state.range(1));

	for (int i = 0; i < state.range(0); i++) {
		bv.insert(i, state.range(0) - i);
	}

	for (auto _ : state) {
		for (int i = 0; i < state.range(0); i++) {
			bv.at(i);
		}
	}
}
BENCHMARK(BETreeQuery)->Ranges({ { 1, UNIT_MAX }, { 1, BUFFER_MAX } });

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

BENCHMARK_MAIN();

BENCHMARK_F(OperationFixture, BTreeOperations)(benchmark::State& st) {
	btree bv;
	for (auto _ : st) {
		execute_test(messages, bv);
	}
}

BENCHMARK_DEFINE_F(OperationFixture, BBTreeOperations)(benchmark::State& st) {
	bbtree bv;
	for (auto _ : st) {
		execute_test(messages, bv);
	}
}

BENCHMARK_DEFINE_F(OperationFixture, BETreeOperations)(benchmark::State& st) {
	betree bv;
	for (auto _ : st) {
		execute_test(messages, bv);
	}
}