#include "gtest.h"
#include "helpers.hpp"
#include "succinct-bitvector.hpp"
#include "unbuffered_packed_vector.hpp"
#include "b-spsi.hpp"

using namespace dyn;

typedef succinct_bitvector<packed_vector, 4056, 256, 0, b_spsi> bbv;

TEST(BBV, Insertion10) {
	insert_test<bbv>(10);
}

TEST(BBV, Insertion100) {
	insert_test<bbv>(100);
}

TEST(BBV, Insertion1000) {
	insert_test<bbv>(1000);
}

TEST(BBV, Insertion10000) {
	insert_test<bbv>(10000);
}

TEST(BBV, Insertion100000) {
	insert_test<bbv>(100000);
}

TEST(BBV, Insertion1000000) {
	insert_test<bbv>(1000000);
}

TEST(BBV, Mixture10) {
	mixture_test<bbv>(10);
}

TEST(BBV, Mixture100) {
	mixture_test<bbv>(100);
}

TEST(BBV, Mixture1000) {
	mixture_test<bbv>(1000);
}

TEST(BBV, Mixture10000) {
	mixture_test<bbv>(10000);
}

TEST(BBV, Mixture100000) {
	mixture_test<bbv>(100000);
}

TEST(BBV, Mixture1000000) {
	mixture_test<bbv>(1000000);
}

TEST(BBV, Rank10) {
	rank_test<bbv>(10);
}

TEST(BBV, Rank100) {
	rank_test<bbv>(100);
}

TEST(BBV, Rank1000) {
	rank_test<bbv>(1000);
}

TEST(BBV, Rank10000) {
	rank_test<bbv>(10000);
}

TEST(BBV, Rank100000) {
	rank_test<bbv>(100000);
}

TEST(BBV, Rank1000000) {
	rank_test<bbv>(1000000);
}

TEST(BBV, Remove10) {
	remove_test<bbv>(10);
}

TEST(BBV, Remove100) {
	remove_test<bbv>(100);
}

TEST(BBV, Remove1000) {
	remove_test<bbv>(1000);
}

TEST(BBV, Remove10000) {
	remove_test<bbv>(10000);
}

TEST(BBV, Remove100000) {
	remove_test<bbv>(100000);
}

TEST(BBV, Remove1000000) {
	remove_test<bbv>(1000000);
}

TEST(BBV, Update10) {
	update_test<bbv>(10);
}

TEST(BBV, Update100) {
	update_test<bbv>(100);
}

TEST(BBV, Update1000) {
	update_test<bbv>(1000);
}

TEST(BBV, Update10000) {
	update_test<bbv>(10000);
}

TEST(BBV, Update100000) {
	update_test<bbv>(100000);
}

TEST(BBV, Update1000000) {
	update_test<bbv>(1000000);
}

TEST(BBV, Select10) {
	select_test<bbv>(10);
}

TEST(BBV, Select100) {
	select_test<bbv>(100);
}

TEST(BBV, Select1000) {
	select_test<bbv>(1000);
}

TEST(BBV, Select10000) {
	select_test<bbv>(10000);
}

TEST(BBV, Select100000) {
	select_test<bbv>(100000);
}

TEST(BBV, Select1000000) {
	select_test<bbv>(1000000);
}