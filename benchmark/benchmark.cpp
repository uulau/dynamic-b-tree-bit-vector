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

inline uint64_t find_child1(const std::vector<uint64_t>& subtree_sizes, uint64_t i)
{
	int j = 0;

	if (i < subtree_sizes[j]) return j;

	while (true) {
		if (i < subtree_sizes[++j]) return j;
		if (i < subtree_sizes[++j]) return j;
		if (i < subtree_sizes[++j]) return j;
		if (i < subtree_sizes[++j]) return j;
		if (i < subtree_sizes[++j]) return j;
	}
}

inline uint64_t find_child2(const std::vector<uint64_t>& subtree_sizes, uint64_t i)
{
	auto begin = subtree_sizes.begin();
	return std::upper_bound(begin, begin + subtree_sizes.size() - 1, i) - begin;
}

inline uint64_t find_child3(const std::vector<uint64_t>& subtree_sizes, uint64_t i)
{
	uint64_t j = 0;
	while (subtree_sizes[j] <= i) {
		j++;
		assert(j < subtree_sizes.size());
	}
	return j;
}

inline uint64_t find_child4(const std::vector<uint64_t>& subtree_sizes, uint64_t i)
{
	uint64_t index = 0;
	uint64_t result[2];
	auto val_mask = _mm_set_epi64x(i, i);
	while (subtree_sizes.size() - index > 1) {
		auto size_vec = _mm_loadu_si128((__m128i*) & subtree_sizes[index]);
		auto result_vec = _mm_cmpgt_epi64(size_vec, val_mask);
		_mm_storeu_si128((__m128i*) & result, result_vec);
		if (result[0] == 0xFFFFFFFFFFFFFFFF) return index;
		if (result[1] == 0xFFFFFFFFFFFFFFFF) return index + 1;
		index += 2;
	}

	while (index < subtree_sizes.size() && subtree_sizes[index] <= i) {
		index++;
		assert(index < subtree_sizes.size());
	}
	return index;
}

inline uint64_t find_child5(const std::vector<uint64_t>& subtree_sizes, uint64_t i)
{
	uint64_t index = 0;
	auto counter = _mm_setzero_si128();
	auto val_mask = _mm_set1_epi64x(i);

	while (subtree_sizes.size() - index > 1) {
		auto size_vec = _mm_loadu_si128((__m128i*) & subtree_sizes[index]);
		auto result_vec = _mm_cmpgt_epi64(size_vec, val_mask);
		counter = _mm_sub_epi64(counter, result_vec);
		index += 2;
	}

	auto shuffle = _mm_shuffle_epi32(counter, _MM_SHUFFLE(1, 0, 3, 2));

	counter = _mm_add_epi64(counter, shuffle);

	auto val = _mm_cvtsi128_si64(counter);

	if (!index || index < subtree_sizes.size()) {
		if (i < subtree_sizes[index]) {
			++val;
			assert(index < subtree_sizes.size());
		}
	}

	return subtree_sizes.size() - val;
}

inline uint64_t find_child6(const std::vector<uint64_t>& subtree_sizes, uint64_t i)
{
	uint64_t index = 0;
	auto counter = _mm256_setzero_si256();
	auto val_mask = _mm256_set1_epi64x(i);

	while (subtree_sizes.size() - index > 3) {
		auto size_vec = _mm256_loadu_si256((__m256i*) & subtree_sizes[index]);
		auto result_vec = _mm256_cmpgt_epi64(size_vec, val_mask);
		counter = _mm256_sub_epi64(counter, result_vec);
		index += 4;
	}

	auto high = _mm256_extractf128_si256(counter, 1);
	auto low = _mm256_extractf128_si256(counter, 0);
	auto result = _mm_add_epi64(high, low);
	auto shuffle = _mm_shuffle_epi32(result, _MM_SHUFFLE(1, 0, 3, 2));
	result = _mm_add_epi64(result, shuffle);
	auto val = _mm_cvtsi128_si64(result);

	while (!index || (index < subtree_sizes.size() && subtree_sizes[index] > i)) {
		++index;
		++val;
	}

	return subtree_sizes.size() - val;
}

static void UnrolledLinear(benchmark::State& state) {
	std::vector<uint64_t> vals;

	for (int i = 0; i < 128; ++i) {
		vals.push_back(i + 1);
	}

	for (auto _ : state) {
		for (int i = 0; i < 128; ++i) {
			benchmark::DoNotOptimize(find_child1(vals, i));
		}
	}
}
BENCHMARK(UnrolledLinear);

static void UpperBound(benchmark::State& state) {
	std::vector<uint64_t> vals;

	for (int i = 0; i < 128; ++i) {
		vals.push_back(i + 1);
	}

	for (auto _ : state) {
		for (int i = 0; i < 128; ++i) {
			benchmark::DoNotOptimize(find_child2(vals, i));
		}
	}
}
BENCHMARK(UpperBound);

static void Linear(benchmark::State& state) {
	std::vector<uint64_t> vals;

	for (int i = 0; i < 128; ++i) {
		vals.push_back(i + 1);
	}

	for (auto _ : state) {
		for (int i = 0; i < 128; ++i) {
			benchmark::DoNotOptimize(find_child3(vals, i));
		}
	}
}
BENCHMARK(Linear);


static void SIMDLinear(benchmark::State& state) {
	std::vector<uint64_t> vals;

	for (int i = 0; i < 128; ++i) {
		vals.push_back(i + 1);
	}

	for (auto _ : state) {
		for (int i = 0; i < 128; ++i) {
			benchmark::DoNotOptimize(find_child4(vals, i));
		}
	}
}
BENCHMARK(SIMDLinear);

static void SIMDLinearCount(benchmark::State& state) {
	std::vector<uint64_t> vals;

	for (int i = 0; i < 128; ++i) {
		vals.push_back(i + 1);
	}

	for (auto _ : state) {
		for (int i = 0; i < 128; ++i) {
			benchmark::DoNotOptimize(find_child5(vals, i));
		}
	}
}
BENCHMARK(SIMDLinearCount);

static void AVX2(benchmark::State& state) {
	std::vector<uint64_t> vals;

	for (int i = 0; i < 128; ++i) {
		vals.push_back(i + 1);
	}

	for (auto _ : state) {
		for (int i = 0; i < 128; ++i) {
			benchmark::DoNotOptimize(find_child6(vals, i));
		}
	}
}
BENCHMARK(AVX2);

static void TreeInsertion(benchmark::State& state) {
	succinct_bitvector<packed_vector, 4096, 256, 0, b_spsi> tree;

	uint64_t inserts = 0;

	for (auto _ : state) {
		tree.insert(inserts >> 1, inserts & 2);
		tree.insert(0, inserts & 2);
		tree.insert(inserts, inserts & 2);
		++inserts;
		benchmark::ClobberMemory();
	}
}

//BENCHMARK(TreeInsertion);

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