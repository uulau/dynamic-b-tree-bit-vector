#include "benchmark.h"
#include "succinct-bitvector.hpp"
#include "b-spsi.hpp"
#include "unbuffered_packed_vector.hpp"

using namespace dyn;

const auto data_size = 100000;

inline uint32_t find_child1(const std::vector<uint32_t>& subtree_sizes, uint32_t i)
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

inline uint32_t find_child2(const std::vector<uint32_t>& subtree_sizes, uint32_t i)
{
	auto begin = subtree_sizes.begin();
	return std::upper_bound(begin, begin + subtree_sizes.size() - 1, i) - begin;
}

inline uint32_t find_child3(const std::vector<uint32_t>& subtree_sizes, uint32_t i)
{
	uint32_t j = 0;
	while (subtree_sizes[j] <= i) {
		j++;
		assert(j < subtree_sizes.size());
	}
	return j;
}

inline uint64_t find_child4(const std::vector<uint64_t>& subtree_sizes, uint64_t i)
{
	uint32_t size = subtree_sizes.size();
	uint32_t low = 0;

	while (size > 0) {
		uint32_t half = size / 2;
		uint32_t other_half = size - half;
		uint32_t probe = low + half;
		uint32_t other_low = low + other_half;
		uint64_t v = subtree_sizes[probe];
		size = half;
		low = v >= i ? low : other_low;
	}

	return low;
}

inline uint32_t find_child6(const std::vector<uint32_t>& subtree_sizes, uint32_t i)
{
	uint32_t index = 0;
	auto counter = _mm256_setzero_si256();
	auto val_mask = _mm256_set1_epi32(i);

	while (subtree_sizes.size() - index > 7) {
		auto size_vec = _mm256_loadu_si256((__m256i*) & subtree_sizes[index]);
		auto result_vec = _mm256_cmpgt_epi32(size_vec, val_mask);
		counter = _mm256_sub_epi32(counter, result_vec);
		index += 8;
	}

	auto high = _mm256_extractf128_si256(counter, 1);
	auto low = _mm256_extractf128_si256(counter, 0);
	auto result = _mm_add_epi32(high, low);

	auto shuffle = _mm_shuffle_epi32(result, _MM_SHUFFLE(1, 0, 3, 2));
	result = _mm_add_epi32(result, shuffle);
	auto shuffle2 = _mm_shuffle_epi32(result, _MM_SHUFFLE(2, 3, 0, 1));

	auto val = _mm_cvtsi128_si32(result);

	while (!index || (index < subtree_sizes.size() && subtree_sizes[index] > i)) {
		++index;
		++val;
	}

	return subtree_sizes.size() - val;
}

static void UnrolledLinear(benchmark::State& state) {
	std::vector<uint32_t> vals;

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

static void CoolBinary(benchmark::State& state) {
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
BENCHMARK(CoolBinary);

static void UpperBound(benchmark::State& state) {
	std::vector<uint32_t> vals;

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
	std::vector<uint32_t> vals;

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

static void AVX2(benchmark::State& state) {
	std::vector<uint32_t> vals;

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

template <class T> static void Insertion(benchmark::State& state) {
	T tree{};

	uint64_t index = 0;
	for (auto _ : state) {
		tree.insert(index, true);
		index++;
	}
}

//BENCHMARK_TEMPLATE(Insertion, succinct_bitvector<packed_vector, 256, 16, 0, b_spsi>);

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

int main(int argc, char** argv)
{
	::benchmark::Initialize(&argc, argv);

	if (::benchmark::ReportUnrecognizedArguments(argc, argv))
	{
		return 1;
	}

	::benchmark::RunSpecifiedBenchmarks();
}