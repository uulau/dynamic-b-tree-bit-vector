#include "packed_vector.hpp"
#include "b-bv.hpp"
#include "sdsl-bv.hpp"
#include "be-node.hpp"

using namespace std;
using namespace dyn;

int main()
{
	auto const count = 1000000;

	auto tree = new sdsl_bv(uint64_t(256));

	for (uint64_t i = 0; i < count; i++) {
		tree->insert(i, i % 2);
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

	//for (uint64_t i = 0; i < count; i++) {
	//	tree->set(i, (i + 1) % 2);
	//}

	//for (uint64_t i = 0; i < count; i++) {
	//	tree->remove(0);
	//}

	delete tree;
}