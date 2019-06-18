#pragma once

#include "be-spsi.hpp"
#include "bb-spsi.hpp"
#include "spsi.hpp"
#include "packed_vector.hpp"

// templates: leaf type, leaf size, node fanout
typedef b::spsi<packed_vector, 256, 16> btree;
typedef bb::spsi<packed_vector, 256, 16> bbtree;
typedef be::spsi<packed_vector, 256, 16> betree;

template<class T> T* generate_tree(uint64_t amount) {
	auto tree = new T(0, 0, 256);

	for (int i = 0; i < amount; i++) {
		tree->push_back(i);
	}

	return tree;
};

template <class T> void size_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	EXPECT_EQ(tree->size(), size);
	delete tree;
}

template <class T> void insert_test(uint64_t size) {
	auto tree = generate_tree<T>(0);

	for (auto i = 0; i <= size; i++) {
		tree->insert(i, size - i);
		auto val = tree->at(i);
		EXPECT_EQ(val, size - i);
		if (val != size - i) {
			break;
		}
	}

	delete tree;
}

template <class T> void update_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	for (auto i = 0; i < size; i++) {
		tree->set(i, size - i);
		auto val = tree->at(i);
		EXPECT_EQ(val, size - i);
		if (val != size - i) {
			break;
		}
	}
	delete tree;
}

template <class T> void sum_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	uint64_t sum = 0;
	for (auto i = 0; i < size; i++) {
		sum += i;
		auto val = tree->psum(i);
		EXPECT_EQ(sum, val);
		if (sum != val) {
			break;
		}
	}
	delete tree;
}

template <class T> void search_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	uint64_t sum = 0;
	for (auto i = 0; i < size; i++) {
		sum += tree->at(i);
		auto val = tree->search(sum);
		EXPECT_EQ(val, i);
		if (val != i) {
			break;
		}
	}
	delete tree;
}

template <class T> void remove_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	for (auto i = 0; i < size - 1; i++) {
		tree->remove(0);
		auto val = tree->at(0);
		EXPECT_TRUE(i + 1 == val);
		if (i + 1 != val) {
			break;
		}
	}
	delete tree;
}