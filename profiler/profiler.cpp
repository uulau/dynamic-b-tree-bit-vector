#include "be-spsi.hpp"
#include "packed_vector.hpp"

using namespace std;
using namespace be;

int main()
{
	auto tree = new spsi<packed_vector, 128, 16>(128);

	auto const count = 10000000;

	for (int i = 0; i < count; i++) {
		tree->insert(i, i);
	}

	for (int i = 0; i < count; i++) {
		tree->at(i);
	}

	for (int i = 0; i < count; i++) {
		tree->set(i, 1);
	}

	for (int i = 0; i < count; i++) {
		tree->remove(0);
	}

	delete tree;
}