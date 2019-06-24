#include "packed_vector.hpp"
#include "be-bv.hpp"

using namespace std;
using namespace dyn;

int main()
{
	auto const count = 10000000;

	auto tree = new be_bv<packed_vector>(16, 128, 128);

	for (int i = 0; i < count; i++) {
		tree->insert(i, true);
		auto d = tree->at(i);
		if (d != true) {
			throw;
		}
	}

	for (int i = 0; i < count; i++) {
		tree->at(i);
	}

	for (int i = 0; i < count; i++) {
		tree->set(i, false);
		auto d = tree->at(i);
		if (d != false) {
			throw;
		}
	}

	for (int i = 0; i < count; i++) {
		auto size = tree->size();
		tree->remove(0);
		if (tree->size() != size - 1) {
			throw;
		}
	}

	delete tree;
}