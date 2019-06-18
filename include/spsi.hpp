// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 * spsi.hpp
 *
 *  Created on: Oct 19, 2015
 *      Author: nico
 *
 *  Searchable partial sums with insert.
 *
 *  represents a vector of integers I_0, ..., I_(n-1) >= 0
 *  supports random access, set, partial sum, search, insert, update (increment/decrement).
 *
 *  The structure is a B+-tree. This improves data locality and space efficiency.
 *
 */
#include <iso646.h>

#include "includes.hpp"
#include "node.hpp"
#include "spsi_reference.hpp"

using namespace dyn;

namespace b {
	template<class leaf_type, 	//underlying representation of the integers
		uint32_t B_LEAF,	//number of integers m allowed for a
		//leaf is B_LEAF <= m <= 2*B_LEAF (except at the beginning)
		uint32_t B		//Order of the tree: number of elements n in each internal node
		//is always B <= n <= 2B+1  (except at the beginning)
		//Alan: Actually, B + 1 <= n <= 2B+2  (except at the beginning)
	>
		class spsi {
		public:
			/*
			 * copy constructor
			 */
			spsi(const spsi& sp) {
				root = new node<leaf_type, B, B_LEAF>(*sp.root);
			}

			/*
			 * copy operator
			 */
			void operator=(const spsi& sp) {

				root->free_mem();
				delete root;

				root = new node(*sp.root);

			}

			using spsi_ref = spsi_reference<spsi>;

			/*
			 * create empty spsi. Input parameters are not used (legacy option). This structure
			 * does not need a max size, and width is automatically detected.
			 */
			spsi(uint64_t message_count = 0) {

				root = new node<leaf_type, B, B_LEAF>();

			}

			~spsi() {

				root->free_mem();

				assert(root != NULL);

				delete root;
				root = NULL;

			}

			/*
			 * high-level access to the SPSI. Supports assign, access,
			 * increment (++, +=), decrement (--, -=)
			 */
			spsi_ref operator[](uint64_t i) {

				return { *this, i };

			}

			/*
			 * returns I_0 + ... + I_i = sum up to i-th integer included
			 */
			uint64_t psum(uint64_t i) const {

				//empty sum
				if (size() == 0) return 0;

				assert(i < size());

				return root->psum(i);

			}

			/*
			 * returns smallest i such that I_0 + ... + I_i >= x
			 */
			uint64_t search(uint64_t x) const {

				assert(x <= psum());

				return root->search(x);

			}

			/*
			 * Works only on bitvectors! (failed assertion otherwise).
			 *
			 * This function corresponds to select_0:
			 *
			 * returns i such that number of zeros before position i (included)
			 * is == x
			 */
			uint64_t search_0(uint64_t x) const {

				assert(x <= size() - psum());

				return root->search_0(x);

			}

			/*
			 * returns smallest i such that (i+1) + I_0 + ... + I_i >= x
			 */
			uint64_t search_r(uint64_t x) const {

				assert(x <= psum() + size());

				return root->search_r(x);

			}

			/*
			 * true iif x is one of the partial sums  0, I_0, I_0+I_1, ...
			 */
			bool contains(uint64_t x) const {

				assert(x <= psum());

				return root->contains(x);

			}

			/*
			 * true iif x is one of  0, I_0+1, I_0+I_1+2, ...
			 */
			 /*bool contains_r(uint64_t x){

		   assert(x<=psum()+size());

		   return root->contains_r(x);

		   }*/

			void push_back(uint64_t x) {
				insert(size(), x);
			}

			/*
			 * insert a new integer x at position i
			 */
			void insert(uint64_t i, uint64_t x) {
				assert(i <= root->size());

				node<leaf_type, B, B_LEAF>* new_root = root->insert(i, x);

				if (new_root != NULL) {

					root = new_root;

				}
			}

			/*
			 * remove the integer x at position i
			 */
			void remove(uint64_t i) {
				node<leaf_type, B, B_LEAF>* new_root = root->remove(i);

				if (new_root != NULL) {
					delete root;
					root = new_root;
				}
			}

			/*
			 * return number of integers stored in the structure
			 */
			uint64_t size() const {
				assert(root != NULL);
				return root->size();
			}

			/*
			 * return sum of all integers
			 */
			uint64_t psum() const {
				return root->psum();
			}

			/*
			 * Total number of bits allocated in RAM for this structure
			 */
			uint64_t bit_size() const {

				assert(root != NULL);

				uint64_t bs = 8 * sizeof(spsi<leaf_type, B_LEAF, B>);

				if (root != NULL) bs += root->bit_size();
				return bs;

			}

			/*
			 * return i-th integer
			 */
			uint64_t at(uint64_t i) const {

				assert(size() > 0);
				assert(i < size());

				return root->at(i);
			}

			/*
			 * decrement/increment i-th integer by delta units
			 */
			void decrement(uint64_t i, uint64_t delta) {
				increment(i, delta, true);
			}

			void increment(uint64_t i, uint64_t delta, bool subtract = false) {

				assert(size() > 0);
				assert(i < size());

				assert(not subtract or delta <= at(i));

				root->increment(i, delta, subtract);
			}

			/*
			 * set i-th element to x
			 */
			void set(uint64_t i, uint64_t x) {

				auto val = at(i);

				increment(i, (val > x ? val - x : x - val), x < val);

			}

			ulint serialize(ostream& out) const {

				assert(root);
				return root->serialize(out);

			}

			void load(istream& in) {

				root = new node<leaf_type, B, B_LEAF>();
				root->load(in);

			}

		private:
			node<leaf_type, B, B_LEAF>* root = NULL;		//tree root
	};

}