#include "packed_vector.hpp"
#include "be-bv.hpp"
#include "b-bv.hpp"
#include "sdsl-bv.hpp"

using namespace std;
using namespace dyn;

int main()
{
	auto const count = 1000000;

	//auto tree = new be_bv<packed_vector>(16, 128, 128);
	auto tree = new sdsl_bv(128);

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

	//auto tree2 = new b_bv<packed_vector>(16, 128, 128);

	//for (int i = 0; i < count; i++) {
	//	tree2->insert(i, true);
	//}

	//for (int i = 0; i < count; i++) {
	//	tree2->at(i);
	//}

	//for (int i = 0; i < count; i++) {
	//	tree2->rank(i + 1);
	//}

	//for (int i = 0; i < count; i++) {
	//	tree2->select(i);
	//}

	//for (int i = 0; i < count; i++) {
	//	tree2->set(i, false);
	//}

	//for (int i = 0; i < count; i++) {
	//	tree2->remove(0);
	//}

	//delete tree2;
}