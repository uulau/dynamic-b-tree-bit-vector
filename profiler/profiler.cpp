#include "be-spsi.hpp"
#include "packed_vector.hpp"

using namespace std;
using namespace be;

int main()
{
	spsi<packed_vector, 256, 16> tree(100);

	for (int i = 0; i < 1000000; i++) {
		tree.insert(i, 1000000 - i);
		if (tree.at(i) != 1000000 - i) {
			throw;
		}
	}
}