#include "packed_vector.hpp"
#include "be-bv.hpp"

using namespace std;
using namespace dyn;

int main()
{
	auto const count = 100000000;

	auto tree = new be_bv<packed_vector>(16, 128, 128);

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