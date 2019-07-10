#include "gtest.h"
#include "helpers.hpp"
#include "b-bv.hpp"
#include "packed_vector.hpp"
#include "be-node.hpp"
#include "b-node.hpp"
#include "sdsl-bv.hpp"

typedef b_bv<packed_vector, b_node> bbv;
typedef b_bv<packed_vector, be_node> bebv;
typedef sdsl_bv sdslbv;

TEST(BEBV, Insertion10) {
	insert_test<bebv>(10);
}

TEST(BEBV, Insertion100) {
	insert_test<bebv>(100);
}

TEST(BEBV, Insertion1000) {
	insert_test<bebv>(1000);
}

TEST(BEBV, Insertion10000) {
	insert_test<bebv>(10000);
}

TEST(BEBV, Insertion100000) {
	insert_test<bebv>(100000);
}

TEST(BEBV, Insertion1000000) {
	insert_test<bebv>(1000000);
}

TEST(BEBV, Rank10) {
	rank_test<bebv>(10);
}

TEST(BEBV, Rank100) {
	rank_test<bebv>(100);
}

TEST(BEBV, Rank1000) {
	rank_test<bebv>(1000);
}

TEST(BEBV, Rank10000) {
	rank_test<bebv>(10000);
}

TEST(BEBV, Rank100000) {
	rank_test<bebv>(100000);
}

TEST(BEBV, Rank1000000) {
	rank_test<bebv>(1000000);
}

TEST(BEBV, Remove10) {
	remove_test<bebv>(10);
}

TEST(BEBV, Remove100) {
	remove_test<bebv>(100);
}

TEST(BEBV, Remove1000) {
	remove_test<bebv>(1000);
}

TEST(BEBV, Remove10000) {
	remove_test<bebv>(10000);
}

TEST(BEBV, Remove100000) {
	remove_test<bebv>(100000);
}

TEST(BEBV, Remove1000000) {
	remove_test<bebv>(1000000);
}

TEST(BEBV, Update10) {
	update_test<bebv>(10);
}

TEST(BEBV, Update100) {
	update_test<bebv>(100);
}

TEST(BEBV, Update1000) {
	update_test<bebv>(1000);
}

TEST(BEBV, Update10000) {
	update_test<bebv>(10000);
}

TEST(BEBV, Update100000) {
	update_test<bebv>(100000);
}

TEST(BEBV, Update1000000) {
	update_test<bebv>(1000000);
}

TEST(BEBV, Select10) {
	select_test<bebv>(10);
}

TEST(BEBV, Select100) {
	select_test<bebv>(100);
}

TEST(BEBV, Select1000) {
	select_test<bebv>(1000);
}

TEST(BEBV, Select10000) {
	select_test<bebv>(10000);
}

TEST(BEBV, Select100000) {
	select_test<bebv>(100000);
}

TEST(BEBV, Select1000000) {
	select_test<bebv>(1000000);
}

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

TEST(SDSLBV, Insertion10) {
	insert_test<sdslbv>(10);
}

TEST(SDSLBV, Insertion100) {
	insert_test<sdslbv>(100);
}

TEST(SDSLBV, Insertion1000) {
	insert_test<sdslbv>(1000);
}

TEST(SDSLBV, Insertion10000) {
	insert_test<sdslbv>(10000);
}

TEST(SDSLBV, Insertion100000) {
	insert_test<sdslbv>(100000);
}

TEST(SDSLBV, Insertion1000000) {
	insert_test<sdslbv>(1000000);
}

TEST(SDSLBV, Rank10) {
	rank_test<sdslbv>(10);
}

TEST(SDSLBV, Rank100) {
	rank_test<sdslbv>(100);
}

TEST(SDSLBV, Rank1000) {
	rank_test<sdslbv>(1000);
}

TEST(SDSLBV, Rank10000) {
	rank_test<sdslbv>(10000);
}

TEST(SDSLBV, Rank100000) {
	rank_test<sdslbv>(100000);
}

TEST(SDSLBV, Rank1000000) {
	rank_test<sdslbv>(1000000);
}

TEST(SDSLBV, Remove10) {
	remove_test<sdslbv>(10);
}

TEST(SDSLBV, Remove100) {
	remove_test<sdslbv>(100);
}

TEST(SDSLBV, Remove1000) {
	remove_test<sdslbv>(1000);
}

TEST(SDSLBV, Remove10000) {
	remove_test<sdslbv>(10000);
}

TEST(SDSLBV, Remove100000) {
	remove_test<sdslbv>(100000);
}

TEST(SDSLBV, Remove1000000) {
	remove_test<sdslbv>(1000000);
}

TEST(SDSLBV, Update10) {
	update_test<sdslbv>(10);
}

TEST(SDSLBV, Update100) {
	update_test<sdslbv>(100);
}

TEST(SDSLBV, Update1000) {
	update_test<sdslbv>(1000);
}

TEST(SDSLBV, Update10000) {
	update_test<sdslbv>(10000);
}

TEST(SDSLBV, Update100000) {
	update_test<sdslbv>(100000);
}

TEST(SDSLBV, Update1000000) {
	update_test<sdslbv>(1000000);
}

TEST(SDSLBV, Select10) {
	select_test<sdslbv>(10);
}

TEST(SDSLBV, Select100) {
	select_test<sdslbv>(100);
}

TEST(SDSLBV, Select1000) {
	select_test<sdslbv>(1000);
}

TEST(SDSLBV, Select10000) {
	select_test<sdslbv>(10000);
}

TEST(SDSLBV, Select100000) {
	select_test<sdslbv>(100000);
}

TEST(SDSLBV, Select1000000) {
	select_test<sdslbv>(1000000);
}