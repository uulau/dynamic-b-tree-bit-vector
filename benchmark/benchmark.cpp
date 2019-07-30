#include "benchmark.h"
#include "packed_vector.hpp"
#include "succinct-bitvector.hpp"
#include "test-case.hpp"
#include "b-spsi.hpp"
#include "be-spsi.hpp"
#include "int_vector.hpp"
#include "sdsl-bv.hpp"
#include "util.hpp"
#include "rank_support_v5.hpp"

static uint64_t data_size = 10000;

template <uint64_t T> static void SDSL2Benchmark(benchmark::State& state) {

	bit_vector bv(T);

	rank_support_v5 rs;

	std::random_device rd;

	std::default_random_engine generator(rd());

	std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	vector<uint64_t> queries;

	for (uint64_t i = 0; i < 10000000; ++i) {
		queries.emplace_back(distribution(generator) % T);
	}

	for (auto i = 0; i < T; i++) {
		bv[i] = rand() % 2;
	}

	sdsl::util::init_support(rs, &bv);

	uint64_t index = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(rs.rank(queries[index]));
		if (++index == T) index = 10000000;
	}
}

//BENCHMARK_TEMPLATE(SDSL2Benchmark, 8000000000);

template <uint64_t T> static void ArrayQuery(benchmark::State& state) {

	bool* ar = new bool[T];

	std::random_device rd;

	std::default_random_engine generator(rd());

	std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	vector<uint64_t> queries;

	for (uint64_t i = 0; i < 10000000; ++i) {
		queries.emplace_back(distribution(generator) % T);
	}

	for (auto i = 0; i < T; i++) {
		ar[i] = rand() % 2;
	}

	for (auto _ : state) {
		for (const auto& query : queries) {
			benchmark::DoNotOptimize(ar[query]);
		}
	}

	delete[] ar;
}

//BENCHMARK_TEMPLATE(ArrayQuery, 8000000000);

template <uint64_t T> static void SDSLBenchmark(benchmark::State& state) {

	bit_vector bv(T);

	std::random_device rd;

	std::default_random_engine generator(rd());

	std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	vector<uint64_t> queries;

	for (uint64_t i = 0; i < 10000000; ++i) {
		queries.emplace_back(distribution(generator) % T);
	}

	for (auto i = 0; i < T; i++) {
		bv[i] = rand() % 2;
	}

	for (auto _ : state) {
		for (const auto& query : queries) {
			benchmark::DoNotOptimize(bv[query]);
		}
	}
}

//BENCHMARK_TEMPLATE(SDSLBenchmark, 8000000000);


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

//BENCHMARK_TEMPLATE(Query, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Query, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);
//BENCHMARK_TEMPLATE(Query, sdsl_bv<64, 64>);
//BENCHMARK_TEMPLATE(Query, sdsl_bv<128, 128>);
//BENCHMARK_TEMPLATE(Query, sdsl_bv<256, 256>);
//BENCHMARK_TEMPLATE(Query, sdsl_bv<512, 512>);
//BENCHMARK_TEMPLATE(Query, sdsl_bv<1024, 1024>);

template <class T> static void Insertion(benchmark::State& state) {
	T tree{};

	uint64_t index = 0;
	for (auto _ : state) {
		tree.insert(index, true);
		index++;
	}
}

//BENCHMARK_TEMPLATE(Insertion, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Insertion, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);
//BENCHMARK_TEMPLATE(Insertion, sdsl_bv<64, 64>);
//BENCHMARK_TEMPLATE(Insertion, sdsl_bv<128, 128>);
//BENCHMARK_TEMPLATE(Insertion, sdsl_bv<256, 256>);
//BENCHMARK_TEMPLATE(Insertion, sdsl_bv<512, 512>);
//BENCHMARK_TEMPLATE(Insertion, sdsl_bv<1024, 1024>);

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

//BENCHMARK_TEMPLATE(Deletion, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
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

//BENCHMARK_TEMPLATE(Rank, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Rank, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);
//BENCHMARK_TEMPLATE(Rank, sdsl_bv<64, 64>);
//BENCHMARK_TEMPLATE(Rank, sdsl_bv<128, 128>);
//BENCHMARK_TEMPLATE(Rank, sdsl_bv<256, 256>);
//BENCHMARK_TEMPLATE(Rank, sdsl_bv<512, 512>);
//BENCHMARK_TEMPLATE(Rank, sdsl_bv<1024, 1024>);

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

//BENCHMARK_TEMPLATE(Select, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Select, succinct_bitvector<packed_vector, 256, 16, 16, be_spsi>);
//BENCHMARK_TEMPLATE(Select, sdsl_bv<64, 64>);
//BENCHMARK_TEMPLATE(Select, sdsl_bv<128, 128>);
//BENCHMARK_TEMPLATE(Select, sdsl_bv<256, 256>);
//BENCHMARK_TEMPLATE(Select, sdsl_bv<512, 512>);
//BENCHMARK_TEMPLATE(Select, sdsl_bv<1024, 1024>);

template <class T> static void Operations(benchmark::State& state) {
	auto messages = generate_ram_test(10000000, 10, 8000000000);

	T tree{};

	for (uint64_t i = 0; i < 8000000000; ++i)
	{
		tree.push_back(i % 2);
	}

	auto set = false;
	for (auto _ : state) {
		execute_test(messages, tree);
		if (!set) {
			state.counters["Size"] = tree.bit_size();
			set = true;
		}
	}
}

//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 512, 4, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 512, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 512, 64, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 512, 256, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 512, 1024, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 512, 4096, 0, b_spsi>);
//
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 1024, 4, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 1024, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 1024, 64, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 1024, 256, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 1024, 1024, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 1024, 4096, 0, b_spsi>);
//
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 2048, 4, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 2048, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 2048, 64, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 2048, 256, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 2048, 1024, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 2048, 4096, 0, b_spsi>);

//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 4096, 4, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 4096, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 4096, 64, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 4096, 256, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 4096, 1024, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 4096, 4096, 0, b_spsi>);

//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 8192, 4, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 8192, 16, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 8192, 64, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 8192, 256, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 8192, 1024, 0, b_spsi>);
//BENCHMARK_TEMPLATE(Operations, succinct_bitvector<packed_vector, 8192, 4096, 0, b_spsi>);

//BENCHMARK_TEMPLATE(Operations, sdsl_bv<64, 256>);
//BENCHMARK_TEMPLATE(Operations, sdsl_bv<128, 256>);
//BENCHMARK_TEMPLATE(Operations, sdsl_bv<256, 256>);
//BENCHMARK_TEMPLATE(Operations, sdsl_bv<512, 256>);
//BENCHMARK_TEMPLATE(Operations, sdsl_bv<1024, 256>);

int main(int argc, char** argv)
{
	::benchmark::Initialize(&argc, argv);

	if (::benchmark::ReportUnrecognizedArguments(argc, argv))
	{
		return 1;
	}

	::benchmark::RunSpecifiedBenchmarks();
}