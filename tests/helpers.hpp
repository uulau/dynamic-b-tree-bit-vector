#pragma once

template<class T> T* generate_tree(uint64_t amount, bool val = false) {
	auto tree = new T(16, 128, 128);

	for (uint64_t i = 0; i < amount; i++) {
		tree->push_back(val);
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

	for (uint64_t i = 0; i <= size; i++) {
		tree->insert(i, true);
		auto val = tree->at(i);
		EXPECT_EQ(val, true);
		if (val != true) {
			break;
		}
	}

	delete tree;
}

template <class T> void update_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	for (uint64_t i = 0; i < size; i++) {
		tree->set(i, true);
		auto val = tree->at(i);
		EXPECT_EQ(val, true);
		if (val != true) {
			break;
		}
	}
	delete tree;
}

template <class T> void sum_test(uint64_t size) {
	auto tree = generate_tree<T>(size, true);
	for (uint64_t i = 0; i < size; i++) {
		auto val = tree->rank(i + 1);
		EXPECT_EQ(i + 1, val);
		if (i + 1 != val) {
			break;
		}
	}
	delete tree;
}

template <class T> void select_test(uint64_t size) {
	auto tree = generate_tree<T>(size, true);
	for (uint64_t i = 0; i < size; i++) {
		uint64_t val = tree->select(i);
		EXPECT_EQ(val, i);
		if (val != i) {
			break;
		}
	}
	delete tree;
}

template <class T> void remove_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	for (uint64_t i = 0; i < size - 1; i++) {
		auto size = tree->size();
		tree->remove(0);
		auto new_size = tree->size();
		EXPECT_TRUE(size - 1 == new_size);
		if (size - 1 != new_size) {
			break;
		}
	}
	delete tree;
}