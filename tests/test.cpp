#include "gtest.h"
#include "helpers.hpp"

TEST(BEpsilonTree, Insertion10) {
	insert_test<betree>(10);
}

TEST(BEpsilonTree, Insertion20) {
	insert_test<betree>(20);
}

TEST(BEpsilonTree, Insertion100) {
	insert_test<betree>(100);
}

TEST(BEpsilonTree, Insertion1000) {
	insert_test<betree>(1000);
}

TEST(BEpsilonTree, Insertion10000) {
	insert_test<betree>(10000);
}

//TEST(BEpsilonTree, Insertion100000) {
//	insert_test<betree>(100000);
//}

//TEST(BEpsilonTree, Remove10) {
//	remove_test<betree>(10);
//}
//
//TEST(BEpsilonTree, Remove100) {
//	remove_test<betree>(100);
//}
//
//TEST(BEpsilonTree, Remove1000) {
//	remove_test<betree>(1000);
//}
//
//TEST(BEpsilonTree, Remove10000) {
//	remove_test<betree>(10000);
//}

TEST(BufferedBTree, Insertion10) {
	insert_test<bbtree>(10);
}

TEST(BufferedBTree, Insertion100) {
	insert_test<bbtree>(100);
}

TEST(BufferedBTree, Insertion1000) {
	insert_test<bbtree>(1000);
}

TEST(BufferedBTree, Insertion10000) {
	insert_test<bbtree>(10000);
}

//TEST(BufferedBTree, Insertion100000) {
//	insert_test<bbtree>(100000);
//}

//TEST(BufferedBTree, Remove10) {
//	remove_test<bbtree>(10);
//}
//
//TEST(BufferedBTree, Remove100) {
//	remove_test<bbtree>(100);
//}
//
//TEST(BufferedBTree, Remove1000) {
//	remove_test<bbtree>(1000);
//}
//
//TEST(BufferedBTree, Remove10000) {
//	remove_test<bbtree>(10000);
//}

TEST(BTree, Insertion10) {
	insert_test<btree>(10);
}

TEST(BTree, Insertion100) {
	insert_test<btree>(100);
}

TEST(BTree, Insertion1000) {
	insert_test<btree>(1000);
}

TEST(BTree, Insertion10000) {
	insert_test<btree>(10000);
}

//TEST(BTree, Insertion100000) {
//	insert_test<btree>(100000);
//}

//TEST(BTree, Remove10) {
//	remove_test<btree>(10);
//}
//
//TEST(BTree, Remove100) {
//	remove_test<btree>(100);
//}
//
//TEST(BTree, Remove1000) {
//	remove_test<btree>(1000);
//}
//
//TEST(BTree, Remove10000) {
//	remove_test<btree>(10000);
//}