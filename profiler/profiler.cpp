#include "packed_vector.hpp"
#include "succinct-bitvector.hpp"
#include "sdsl-bv.hpp"
#include "be-spsi.hpp"
#include "b-spsi.hpp"

using namespace std;
using namespace dyn;

int main()
{
	auto const count = 10000000;

	auto tree = new succinct_bitvector<packed_vector, 16, 254, 16, be_spsi>();

	for (uint64_t i = 0; i < count; i++) {
		tree->insert(i, i % 2);
	}

	//for (uint64_t i = 0; i < count; i++) {
	//	tree->at(i);
	//}

	//for (uint64_t i = 0; i < count; i++) {
	//	tree->rank(i + 1);
	//}

	//for (uint64_t i = 0; i < count / 2; i++) {
	//	tree->select(i + 1);
	//}

	//for (uint64_t i = 0; i < count; i++) {
	//	tree->set(i, (i + 1) % 2);
	//}

	//for (uint64_t i = 0; i < count; i++) {
	//	tree->remove(0);
	//}

	delete tree;
}