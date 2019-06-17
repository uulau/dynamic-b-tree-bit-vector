#include "benchmark.h"
#include <iostream>
#include "packed_vector.hpp"
#include "be-spsi.hpp"
#include "bb-spsi.hpp"
#include "spsi.hpp"
#include "succinct_bitvector.hpp"
#include "test-case.hpp"

typedef b::spsi<packed_vector, 256, 16> btree;
typedef bb::spsi<packed_vector, 256, 16> bbtree;
typedef be::spsi<packed_vector, 256, 16> betree;

static int UNIT_MAX = 8 << 12;

static void BTreeInsertion(benchmark::State& state) {
	auto tree = btree(0, 0, 0);

	for (auto _ : state) {
		tree.push_back(0);
	}
}
BENCHMARK(BTreeInsertion);

static void BBTreeInsertion(benchmark::State& state) {
	auto tree = bbtree(0, 0, state.range(0));

	for (auto _ : state) {
		tree.push_back(0);
	}
}
BENCHMARK(BBTreeInsertion)->Range(1, UNIT_MAX);

static void BETreeInsertion(benchmark::State& state) {
	auto tree = betree(0, 0, state.range(0));

	for (auto _ : state) {
		tree.push_back(0);
	}
}
BENCHMARK(BETreeInsertion)->Range(1, UNIT_MAX);

class OperationFixture : public benchmark::Fixture {
public:
	vector<message> messages;

	void SetUp(const ::benchmark::State& state) {
		messages = generate_ram_test(1000000, 0.05, 0.95);
	}

	void TearDown(const ::benchmark::State& state) {
		// Nothing
	}
};

BENCHMARK_F(OperationFixture, BTreeOperations)(benchmark::State& state) {
	auto tree = btree(0, 0, 0);
	for (auto _ : state) {
		execute_test(messages, tree);
	}
}

BENCHMARK_DEFINE_F(OperationFixture, BBTreeOperations)(benchmark::State& state) {
	auto tree = bbtree(0, 0, state.range(0));
	for (auto _ : state) {
		execute_test(messages, tree);
	}
}

BENCHMARK_REGISTER_F(OperationFixture, BBTreeOperations)->Range(1, UNIT_MAX);

BENCHMARK_DEFINE_F(OperationFixture, BETreeOperations)(benchmark::State& state) {
	auto tree = betree(0, 0, state.range(0));
	for (auto _ : state) {
		execute_test(messages, tree);
	}
}

BENCHMARK_REGISTER_F(OperationFixture, BETreeOperations)->Range(1, UNIT_MAX);

BENCHMARK_MAIN();