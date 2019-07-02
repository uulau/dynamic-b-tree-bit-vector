#include "packed_vector.hpp"
#include "b-bv.hpp"
#include "be-node.hpp"

using namespace std;
using namespace dyn;

int main()
{
	auto const count = 100000000;

	auto tree = new b_bv<packed_vector, be_node>(uint32_t(8), uint32_t(4096), uint64_t(256));

	for (int i = 0; i < count; i++) {
		tree->insert(i, true);
	}

	for (int i = 0; i < count; i++) {
		tree->at(i);
	}

	for (int i = 0; i < count; i++) {
		tree->rank(i + 1);
	}

	for (int i = 0; i < count; i++) {
		tree->select(i);
	}

	for (int i = 0; i < count; i++) {
		tree->set(i, false);
	}

	for (int i = 0; i < count; i++) {
		tree->remove(0);
	}

	delete tree;
}