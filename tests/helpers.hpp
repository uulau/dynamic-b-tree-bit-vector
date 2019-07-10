#pragma once

template<class T> T* generate_tree(uint64_t amount) {
	auto tree = new T(4, 64, 128);

	for (uint64_t i = 0; i < amount; i++) {
		tree->push_back(i % 2);
	}

	return tree;
}

template <class T> void insert_test(uint64_t size) {
	auto tree = generate_tree<T>(0);

	for (uint64_t i = 0; i <= size; i++) {
		auto set = i % 2;
		tree->insert(i, set);
		auto val = tree->at(i);
		EXPECT_EQ(val, set);
		if (val != set) {
			break;
		}
		auto s = tree->size();
		EXPECT_EQ(s, i + 1);
		if (s != i + 1) {
			break;
		}
	}

	delete tree;
}

template <class T> void update_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	for (uint64_t i = 0; i < size; i++) {
		bool set = (i + 1) % 2;
		tree->set(i, set);
		auto val = tree->at(i);
		EXPECT_EQ(val, set);
		if (val != set) {
			break;
		}
	}
	delete tree;
}

template <class T> void rank_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	int64_t sum = 0;
	for (uint64_t i = 1; i <= size; i++) {
		auto val = tree->rank(i);
		sum += tree->at(i - 1);
		EXPECT_EQ(sum, val);
		if (i != val) {
			break;
		}
	}
	delete tree;
}

template <class T> void select_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	for (uint64_t i = 0; i < size; i++) {
		uint64_t val = tree->select(i);
		EXPECT_EQ(val, i + 1);
		if (val != i) {
			break;
		}
	}
	delete tree;
}

template <class T> void remove_test(uint64_t size) {
	auto tree = generate_tree<T>(size);
	for (uint64_t i = 0; i < size - 1; i++) {
		auto s = tree->size();
		auto old = tree->at(0);
		tree->remove(0);
		auto new_size = tree->size();
		EXPECT_TRUE(s - 1 == new_size);
		if (s - 1 != new_size) {
			break;
		}
		auto set = !old;
		auto val = tree->at(0);
		EXPECT_EQ(val, set);
		if (val != set) {
			break;
		}
	}
	delete tree;
}