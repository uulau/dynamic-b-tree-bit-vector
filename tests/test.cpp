#include "gtest.h"
#include "helpers.hpp"
#include "b-bv.hpp"
#include "be-bv.hpp"
#include "sdsl-bv.hpp"
#include "packed_vector.hpp"

typedef b_bv<packed_vector> bbv;
typedef be_bv<packed_vector> bebv;
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

TEST(BEBV, Sum10) {
	sum_test<bebv>(10);
}

TEST(BEBV, Sum100) {
	sum_test<bebv>(100);
}

TEST(BEBV, Sum1000) {
	sum_test<bebv>(1000);
}

TEST(BEBV, Sum10000) {
	sum_test<bebv>(10000);
}

TEST(BEBV, Sum100000) {
	sum_test<bebv>(100000);
}

TEST(BEBV, Sum1000000) {
	sum_test<bebv>(1000000);
}

TEST(BEBV, Size10) {
	size_test<bebv>(10);
}

TEST(BEBV, Size100) {
	size_test<bebv>(100);
}

TEST(BEBV, Size1000) {
	size_test<bebv>(1000);
}

TEST(BEBV, Size10000) {
	size_test<bebv>(10000);
}

TEST(BEBV, Size100000) {
	size_test<bebv>(100000);
}

TEST(BEBV, Size1000000) {
	size_test<bebv>(1000000);
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

TEST(BEBV, Search10) {
	search_test<bebv>(10);
}

TEST(BEBV, Search100) {
	search_test<bebv>(100);
}

TEST(BEBV, Search1000) {
	search_test<bebv>(1000);
}

TEST(BEBV, Search10000) {
	search_test<bebv>(10000);
}

TEST(BEBV, Search100000) {
	search_test<bebv>(100000);
}

TEST(BEBV, Search1000000) {
	search_test<bebv>(1000000);
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

TEST(BBV, Sum10) {
	sum_test<bbv>(10);
}

TEST(BBV, Sum100) {
	sum_test<bbv>(100);
}

TEST(BBV, Sum1000) {
	sum_test<bbv>(1000);
}

TEST(BBV, Sum10000) {
	sum_test<bbv>(10000);
}

TEST(BBV, Sum100000) {
	sum_test<bbv>(100000);
}

TEST(BBV, Sum1000000) {
	sum_test<bbv>(1000000);
}

TEST(BBV, Size10) {
	size_test<bbv>(10);
}

TEST(BBV, Size100) {
	size_test<bbv>(100);
}

TEST(BBV, Size1000) {
	size_test<bbv>(1000);
}

TEST(BBV, Size10000) {
	size_test<bbv>(10000);
}

TEST(BBV, Size100000) {
	size_test<bbv>(100000);
}

TEST(BBV, Size1000000) {
	size_test<bbv>(1000000);
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

TEST(BBV, Search10) {
	search_test<bbv>(10);
}

TEST(BBV, Search100) {
	search_test<bbv>(100);
}

TEST(BBV, Search1000) {
	search_test<bbv>(1000);
}

TEST(BBV, Search10000) {
	search_test<bbv>(10000);
}

TEST(BBV, Search100000) {
	search_test<bbv>(100000);
}

TEST(BBV, Search1000000) {
	search_test<bbv>(1000000);
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

TEST(SDSLBV, Sum10) {
	sum_test<sdslbv>(10);
}

TEST(SDSLBV, Sum100) {
	sum_test<sdslbv>(100);
}

TEST(SDSLBV, Sum1000) {
	sum_test<sdslbv>(1000);
}

TEST(SDSLBV, Sum10000) {
	sum_test<sdslbv>(10000);
}

TEST(SDSLBV, Sum100000) {
	sum_test<sdslbv>(100000);
}

TEST(SDSLBV, Sum1000000) {
	sum_test<sdslbv>(1000000);
}

TEST(SDSLBV, Size10) {
	size_test<sdslbv>(10);
}

TEST(SDSLBV, Size100) {
	size_test<sdslbv>(100);
}

TEST(SDSLBV, Size1000) {
	size_test<sdslbv>(1000);
}

TEST(SDSLBV, Size10000) {
	size_test<sdslbv>(10000);
}

TEST(SDSLBV, Size100000) {
	size_test<sdslbv>(100000);
}

TEST(SDSLBV, Size1000000) {
	size_test<sdslbv>(1000000);
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

TEST(SDSLBV, Search10) {
	search_test<sdslbv>(10);
}

TEST(SDSLBV, Search100) {
	search_test<sdslbv>(100);
}

TEST(SDSLBV, Search1000) {
	search_test<sdslbv>(1000);
}

TEST(SDSLBV, Search10000) {
	search_test<sdslbv>(10000);
}

TEST(SDSLBV, Search100000) {
	search_test<sdslbv>(100000);
}

TEST(SDSLBV, Search1000000) {
	search_test<sdslbv>(1000000);
}