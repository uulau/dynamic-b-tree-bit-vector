#include "be-spsi.hpp"
#include "packed_vector.hpp"

using namespace std;
using namespace be;

int main()
{
	auto tree = new spsi<packed_vector, 128, 16>(128);

	for (int i = 0; i < 1000000; i++) {
		tree->insert(i, 0);
	}

	for (int i = 0; i < 10000000000; i++) {
		tree->at(100000 - 1);
	}

	delete tree;
}