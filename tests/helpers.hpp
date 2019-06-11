#pragma once

#include "be-spsi.hpp"
#include "bb-spsi.hpp"
#include "spsi.hpp"
#include "packed_vector.hpp"

// templates: leaf type, leaf size, node fanout
typedef be::spsi<packed_vector, 8192, 16> succinct_spsi;
typedef be::spsi <packed_vector, 256, 16> packed_spsi;

b::spsi<packed_vector, 8192, 16> * generate_b_tree(int amount) {
	auto tree = new b::spsi<packed_vector, 8192, 16>();

	for (int i = 0; i < amount; i++) {
		tree->push_back(i);
	}

	return tree;
};

be::spsi<packed_vector, 8192, 16> * generate_be_tree(int amount) {
	auto tree = new be::spsi<packed_vector, 8192, 16>();

	for (int i = 0; i < amount; i++) {
		tree->push_back(i);
	}

	return tree;
};

bb::spsi<packed_vector, 8192, 16> * generate_bb_tree(int amount) {
	auto tree = new bb::spsi<packed_vector, 8192, 16>();

	for (int i = 0; i < amount; i++) {
		tree->push_back(i);
	}

	return tree;
};

namespace be {
	void size_test(int size) {
		auto tree = generate_be_tree(size);
		EXPECT_EQ(tree->size(), size);
		delete tree;
	}

	void insert_test(int size) {
		auto tree = generate_be_tree(size);
		for (int i = 0; i <= size; i++) {
			tree->insert(i, size - i);
			auto val = tree->at(i);
			EXPECT_EQ(val, size - i);
			if (val != size - i) break;
		}
		delete tree;
	}

	void update_test(int size) {
		auto tree = generate_be_tree(size);
		for (int i = 0; i < size; i++) {
			tree->set(i, size - i);
			EXPECT_EQ(tree->at(i), size - i);
			if (tree->at(i) != size - i) break;
		}
		delete tree;
	}

	void sum_test(int size) {
		auto tree = generate_be_tree(size);
		int sum = 0;
		for (int i = 0; i < size; i++) {
			sum += i;
		}
		EXPECT_EQ(sum, tree->psum());
		delete tree;
	}

	void search_test(int size) {
		auto tree = generate_be_tree(size);
		int sum = 0;
		for (int i = 0; i < size; i++) {
			sum += tree->at(i);
			EXPECT_EQ(tree->search(sum), i);
			if (tree->search(sum) != i) break;
		}
		delete tree;
	}

	void remove_test(int size) {
		auto tree = generate_be_tree(size);
		for (int i = 0; i < size - 1; i++) {
			tree->remove(0);
			auto val = tree->at(0);
			EXPECT_TRUE(i + 1 == val);
			if (i + 1 != val) break;
		}
		delete tree;
	}
}

namespace bb {
	void size_test(int size) {
		auto tree = generate_bb_tree(size);
		EXPECT_EQ(tree->size(), size);
		delete tree;
	}

	void insert_test(int size) {
		auto tree = generate_bb_tree(size);
		for (int i = 0; i <= size; i++) {
			tree->insert(i, size - i);
			auto val = tree->at(i);
			EXPECT_EQ(val, size - i);
			if (val != size - i) break;
		}
		delete tree;
	}

	void update_test(int size) {
		auto tree = generate_bb_tree(size);
		for (int i = 0; i < size; i++) {
			tree->set(i, size - i);
			EXPECT_EQ(tree->at(i), size - i);
			if (tree->at(i) != size - i) break;
		}
		delete tree;
	}

	void sum_test(int size) {
		auto tree = generate_bb_tree(size);
		int sum = 0;
		for (int i = 0; i < size; i++) {
			sum += i;
		}
		EXPECT_EQ(sum, tree->psum());
		delete tree;
	}

	void search_test(int size) {
		auto tree = generate_bb_tree(size);
		int sum = 0;
		for (int i = 0; i < size; i++) {
			sum += tree->at(i);
			EXPECT_EQ(tree->search(sum), i);
			if (tree->search(sum) != i) break;
		}
		delete tree;
	}

	void remove_test(int size) {
		auto tree = generate_bb_tree(size);
		for (int i = 0; i < size - 1; i++) {
			tree->remove(0);
			auto val = tree->at(0);
			EXPECT_TRUE(i + 1 == val);
			if (i + 1 != val) break;
		}
		delete tree;
	}
}

namespace b {
	void size_test(int size) {
		auto tree = generate_b_tree(size);
		EXPECT_EQ(tree->size(), size);
		delete tree;
	}

	void insert_test(int size) {
		auto tree = generate_b_tree(size);
		for (int i = 0; i <= size; i++) {
			tree->insert(i, size - i);
			auto val = tree->at(i);
			EXPECT_EQ(val, size - i);
			if (val != size - i) break;
		}
		delete tree;
	}

	void update_test(int size) {
		auto tree = generate_b_tree(size);
		for (int i = 0; i < size; i++) {
			tree->set(i, size - i);
			EXPECT_EQ(tree->at(i), size - i);
			if (tree->at(i) != size - i) break;
		}
		delete tree;
	}

	void sum_test(int size) {
		auto tree = generate_b_tree(size);
		int sum = 0;
		for (int i = 0; i < size; i++) {
			sum += i;
		}
		EXPECT_EQ(sum, tree->psum());
		delete tree;
	}

	void search_test(int size) {
		auto tree = generate_b_tree(size);
		int sum = 0;
		for (int i = 0; i < size; i++) {
			sum += tree->at(i);
			EXPECT_EQ(tree->search(sum), i);
			if (tree->search(sum) != i) break;
		}
		delete tree;
	}

	void remove_test(int size) {
		auto tree = generate_b_tree(size);
		for (int i = 0; i < size - 1; i++) {
			tree->remove(0);
			auto val = tree->at(0);
			EXPECT_TRUE(i + 1 == val);
			if (i + 1 != val) break;
		}
		delete tree;
	}
}