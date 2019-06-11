#include "gtest.h"
#include "helpers.hpp"

namespace be {
	TEST(BEpsilonTree, Insertion10) {
		insert_test(10);
	}

	TEST(BEpsilonTree, Insertion100) {
		insert_test(100);
	}

	TEST(BEpsilonTree, Insertion1000) {
		insert_test(1000);
	}

	TEST(BEpsilonTree, Insertion10000) {
		insert_test(10000);
	}

	TEST(BEpsilonTree, Insertion100000) {
		insert_test(100000);
	}

	TEST(BEpsilonTree, Remove10) {
		remove_test(10);
	}

	TEST(BEpsilonTree, Remove100) {
		remove_test(100);
	}

	TEST(BEpsilonTree, Remove1000) {
		remove_test(1000);
	}

	TEST(BEpsilonTree, Remove10000) {
		remove_test(10000);
	}

	//TEST(BEpsilonTree, Remove100000) {
	//	remove_test(100000);
	//}
}

namespace bb {
	TEST(BufferedBTree, Insertion10) {
		insert_test(10);
	}

	TEST(BufferedBTree, Insertion100) {
		insert_test(100);
	}

	TEST(BufferedBTree, Insertion1000) {
		insert_test(1000);
	}

	TEST(BufferedBTree, Insertion10000) {
		insert_test(10000);
	}

	TEST(BufferedBTree, Insertion100000) {
		insert_test(100000);
	}

	TEST(BufferedBTree, Remove10) {
		remove_test(10);
	}

	TEST(BufferedBTree, Remove100) {
		remove_test(100);
	}

	TEST(BufferedBTree, Remove1000) {
		remove_test(1000);
	}

	TEST(BufferedBTree, Remove10000) {
		remove_test(10000);
	}

	//TEST(BufferedBTree, Remove100000) {
	//	remove_test(100000);
	//}
}

namespace b {
	TEST(BTree, Insertion10) {
		insert_test(10);
	}

	TEST(BTree, Insertion100) {
		insert_test(100);
	}

	TEST(BTree, Insertion1000) {
		insert_test(1000);
	}

	TEST(BTree, Insertion10000) {
		insert_test(10000);
	}

	TEST(BTree, Insertion100000) {
		insert_test(100000);
	}

	TEST(BTree, Remove10) {
		remove_test(10);
	}

	TEST(BTree, Remove100) {
		remove_test(100);
	}

	TEST(BTree, Remove1000) {
		remove_test(1000);
	}

	TEST(BTree, Remove10000) {
		remove_test(10000);
	}

	//TEST(BTree, Remove100000) {
	//	remove_test(100000);
	//}
}


