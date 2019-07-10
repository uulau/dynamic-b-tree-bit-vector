#include "packed_vector.hpp"
#include "b-bv.hpp"
#include "sdsl-bv.hpp"
#include "be-node.hpp"

using namespace std;
using namespace dyn;

int main()
{
	auto const count = 100;

	auto tree = new sdsl_bv(0, 0, 10);

	for (uint64_t i = 0; i < count; i++) {
		assert(tree->size() == i);
		tree->insert(i, i % 2);
		assert(tree->size() == i + 1);
		assert(tree->at(i) == i % 2);
	}

	for (uint64_t i = 0; i < count; i++) {
		tree->at(i);
	}

	for (uint64_t i = 0; i < count; i++) {
		tree->rank(i + 1);
	}

	for (uint64_t i = 0; i < count; i++) {
		tree->select(i);
	}

	for (uint64_t i = 0; i < count; i++) {
		assert(tree->at(i) == i % 2);
		auto size = tree->size();
		tree->set(i, (i + 1) % 2);
		assert(tree->at(i) == (i + 1) % 2);
		assert(size == tree->size());
	}

	for (uint64_t i = 0; i < count; i++) {
		int size = tree->size();
		tree->remove(0);
		assert(size == tree->size() + 1);
	}

	delete tree;
}