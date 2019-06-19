#include "gtest.h"
#include "helpers.hpp"

TEST(BEpsilonTree, Insertion10) {
	insert_test<betree>(10);
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

TEST(BEpsilonTree, Insertion100000) {
	insert_test<betree>(100000);
}

TEST(BEpsilonTree, Insertion1000000) {
	insert_test<betree>(1000000);
}

TEST(BEpsilonTree, Sum10) {
	sum_test<betree>(10);
}

TEST(BEpsilonTree, Sum100) {
	sum_test<betree>(100);
}

TEST(BEpsilonTree, Sum1000) {
	sum_test<betree>(1000);
}

TEST(BEpsilonTree, Sum10000) {
	sum_test<betree>(10000);
}

TEST(BEpsilonTree, Sum100000) {
	sum_test<betree>(100000);
}

TEST(BEpsilonTree, Sum1000000) {
	sum_test<betree>(1000000);
}

TEST(BEpsilonTree, Size10) {
	size_test<betree>(10);
}

TEST(BEpsilonTree, Size100) {
	size_test<betree>(100);
}

TEST(BEpsilonTree, Size1000) {
	size_test<betree>(1000);
}

TEST(BEpsilonTree, Size10000) {
	size_test<betree>(10000);
}

TEST(BEpsilonTree, Size100000) {
	size_test<betree>(100000);
}

TEST(BEpsilonTree, Size1000000) {
	size_test<betree>(1000000);
}
TEST(BEpsilonTree, Remove10) {
	remove_test<betree>(10);
}

TEST(BEpsilonTree, Remove100) {
	remove_test<betree>(100);
}

TEST(BEpsilonTree, Remove1000) {
	remove_test<betree>(1000);
}

TEST(BEpsilonTree, Remove10000) {
	remove_test<betree>(10000);
}

TEST(BEpsilonTree, Remove100000) {
	remove_test<betree>(100000);
}

TEST(BEpsilonTree, Remove1000000) {
	remove_test<betree>(1000000);
}

TEST(BEpsilonTree, Update10) {
	update_test<betree>(10);
}

TEST(BEpsilonTree, Update100) {
	update_test<betree>(100);
}

TEST(BEpsilonTree, Update1000) {
	update_test<betree>(1000);
}

TEST(BEpsilonTree, Update10000) {
	update_test<betree>(10000);
}

TEST(BEpsilonTree, Update100000) {
	update_test<betree>(100000);
}

TEST(BEpsilonTree, Update1000000) {
	update_test<betree>(1000000);
}

TEST(BEpsilonTree, Search10) {
	search_test<betree>(10);
}

TEST(BEpsilonTree, Search100) {
	search_test<betree>(100);
}

TEST(BEpsilonTree, Search1000) {
	search_test<betree>(1000);
}

TEST(BEpsilonTree, Search10000) {
	search_test<betree>(10000);
}

TEST(BEpsilonTree, Search100000) {
	search_test<betree>(100000);
}

TEST(BEpsilonTree, Search1000000) {
	search_test<betree>(1000000);
}

//TEST(BufferedBTree, Insertion10) {
//	insert_test<bbtree>(10);
//}
//
//TEST(BufferedBTree, Insertion100) {
//	insert_test<bbtree>(100);
//}
//
//TEST(BufferedBTree, Insertion1000) {
//	insert_test<bbtree>(1000);
//}
//
//TEST(BufferedBTree, Insertion10000) {
//	insert_test<bbtree>(10000);
//}
//
//TEST(BufferedBTree, Insertion100000) {
//	insert_test<bbtree>(100000);
//}
//
//TEST(BufferedBTree, Sum10) {
//	sum_test<bbtree>(10);
//}
//
//TEST(BufferedBTree, Sum100) {
//	sum_test<bbtree>(100);
//}
//
//TEST(BufferedBTree, Sum1000) {
//	sum_test<bbtree>(1000);
//}
//
//TEST(BufferedBTree, Sum10000) {
//	sum_test<bbtree>(10000);
//}
//
//TEST(BufferedBTree, Sum100000) {
//	sum_test<bbtree>(100000);
//}
//
//TEST(BufferedBTree, Size10) {
//	size_test<bbtree>(10);
//}
//
//TEST(BufferedBTree, Size100) {
//	size_test<bbtree>(100);
//}
//
//TEST(BufferedBTree, Size1000) {
//	size_test<bbtree>(1000);
//}
//
//TEST(BufferedBTree, Size10000) {
//	size_test<bbtree>(10000);
//}
//
//TEST(BufferedBTree, Size100000) {
//	size_test<bbtree>(100000);
//}
//
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
//
//TEST(BufferedBTree, Remove100000) {
//	remove_test<bbtree>(100000);
//}
//
//TEST(BufferedBTree, Update10) {
//	update_test<bbtree>(10);
//}
//
//TEST(BufferedBTree, Update100) {
//	update_test<bbtree>(100);
//}
//
//TEST(BufferedBTree, Update1000) {
//	update_test<bbtree>(1000);
//}
//
//TEST(BufferedBTree, Update10000) {
//	update_test<bbtree>(10000);
//}
//
//TEST(BufferedBTree, Update100000) {
//	update_test<bbtree>(100000);
//}
//
//TEST(BufferedBTree, Search10) {
//	search_test<bbtree>(10);
//}
//
//TEST(BufferedBTree, Search100) {
//	search_test<bbtree>(100);
//}
//
//TEST(BufferedBTree, Search1000) {
//	search_test<bbtree>(1000);
//}
//
//TEST(BufferedBTree, Search10000) {
//	search_test<bbtree>(10000);
//}
//
//TEST(BufferedBTree, Search100000) {
//	search_test<bbtree>(100000);
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

TEST(BTree, Insertion100000) {
	insert_test<btree>(100000);
}

TEST(BTree, Insertion1000000) {
	insert_test<btree>(1000000);
}

TEST(BTree, Sum10) {
	sum_test<btree>(10);
}

TEST(BTree, Sum100) {
	sum_test<btree>(100);
}

TEST(BTree, Sum1000) {
	sum_test<btree>(1000);
}

TEST(BTree, Sum10000) {
	sum_test<btree>(10000);
}

TEST(BTree, Sum100000) {
	sum_test<btree>(100000);
}

TEST(BTree, Sum1000000) {
	sum_test<btree>(1000000);
}

TEST(BTree, Size10) {
	size_test<btree>(10);
}

TEST(BTree, Size100) {
	size_test<btree>(100);
}

TEST(BTree, Size1000) {
	size_test<btree>(1000);
}

TEST(BTree, Size10000) {
	size_test<btree>(10000);
}

TEST(BTree, Size100000) {
	size_test<btree>(100000);
}

TEST(BTree, Size1000000) {
	size_test<btree>(1000000);
}

TEST(BTree, Remove10) {
	remove_test<btree>(10);
}

TEST(BTree, Remove100) {
	remove_test<btree>(100);
}

TEST(BTree, Remove1000) {
	remove_test<btree>(1000);
}

TEST(BTree, Remove10000) {
	remove_test<btree>(10000);
}

TEST(BTree, Remove100000) {
	remove_test<btree>(100000);
}

TEST(BTree, Remove1000000) {
	remove_test<btree>(1000000);
}

TEST(BTree, Update10) {
	update_test<btree>(10);
}

TEST(BTree, Update100) {
	update_test<btree>(100);
}

TEST(BTree, Update1000) {
	update_test<btree>(1000);
}

TEST(BTree, Update10000) {
	update_test<btree>(10000);
}

TEST(BTree, Update100000) {
	update_test<btree>(100000);
}

TEST(BTree, Update1000000) {
	update_test<btree>(1000000);
}

TEST(BTree, Search10) {
	search_test<btree>(10);
}

TEST(BTree, Search100) {
	search_test<btree>(100);
}

TEST(BTree, Search1000) {
	search_test<btree>(1000);
}

TEST(BTree, Search10000) {
	search_test<btree>(10000);
}

TEST(BTree, Search100000) {
	search_test<btree>(100000);
}

TEST(BTree, Search1000000) {
	search_test<btree>(1000000);
}