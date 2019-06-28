#include "packed_vector.hpp"
#include "be-bv.hpp"
#include "b-bv.hpp"
#include "sdsl-bv.hpp"

using namespace std;
using namespace dyn;

int main()
{
	auto const count = 100000;

	auto tree = new sdsl_bv(0, 10, 0);

	for (int i = 0; i < count; i++) {
		tree->insert(i, true);
	}

	//for (int i = 0; i < count; i++) {
	//	tree->at(i);
	//}

	//for (int i = 0; i < count; i++) {
	//	auto rank = tree->rank(i + 1);
	//	if (rank != i + 1)
	//	{
	//		throw;
	//	}
	//}

	for (int i = 0; i < count; i++) {
		tree->set(i, false);
		auto rank = tree->rank(i + 1);
		if (rank != 0)
		{
			throw;
		}
	}



	//for (int i = 0; i < count; i++) {
	//	tree->select(i);
	//}

	//for (int i = 0; i < count; i++) {
	//	tree->set(i, false);
	//}

	//for (int i = 0; i < count; i++) {
	//	tree->remove(0);
	//}

	delete tree;
}