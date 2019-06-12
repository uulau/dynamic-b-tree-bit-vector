#pragma once

#include "be-spsi.hpp"
#include "bb-spsi.hpp"
#include "spsi.hpp"
#include "packed_vector.hpp"

// templates: leaf type, leaf size, node fanout
typedef b::spsi<packed_vector, 9, 1> btree;
typedef bb::spsi<packed_vector, 8192, 16> bbtree;
typedef be::spsi<packed_vector, 9, 1> betree;

template<class T> T* generate_tree(int amount) {
	auto tree = new T();

	for (int i = 0; i < amount; i++) {
		tree->push_back(i);
	}

	return tree;
};

template <class T> void size_test(int size) {
	auto tree = generate_tree<T>(size);
	EXPECT_EQ(tree->size(), size);
	delete tree;
}

template <class T> void insert_test(int size) {
	auto tree = generate_tree<T>(size);
	for (int i = 0; i <= size; i++) {
		tree->insert(i, size - i);
		auto val = tree->at(i);
		EXPECT_EQ(val, size - i);
		if (val != size - i) {
			break;
		}
	}
	delete tree;
}

template <class T> void update_test(int size) {
	auto tree = generate_tree<T>(size);
	for (int i = 0; i < size; i++) {
		tree->set(i, size - i);
		EXPECT_EQ(tree->at(i), size - i);
		if (tree->at(i) != size - i) break;
	}
	delete tree;
}

template <class T> void sum_test(int size) {
	auto tree = generate_tree<T>(size);
	int sum = 0;
	for (int i = 0; i < size; i++) {
		sum += i;
	}
	EXPECT_EQ(sum, tree->psum());
	delete tree;
}

template <class T> void search_test(int size) {
	auto tree = generate_tree<T>(size);
	int sum = 0;
	for (int i = 0; i < size; i++) {
		sum += tree->at(i);
		EXPECT_EQ(tree->search(sum), i);
		if (tree->search(sum) != i) break;
	}
	delete tree;
}

template <class T> void remove_test(int size) {
	auto tree = generate_tree<T>(size);
	for (int i = 0; i < size - 1; i++) {
		tree->remove(0);
		auto val = tree->at(0);
		EXPECT_TRUE(i + 1 == val);
		if (i + 1 != val) break;
	}
	delete tree;
}