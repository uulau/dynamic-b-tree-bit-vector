// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#pragma once

#include <iso646.h>
#include "includes.hpp"
#include "b-node.hpp"
#include "bv_reference.hpp"

namespace dyn {
	template<class leaf_type> class b_bv {
	public:
		using bv_ref = bv_reference<b_bv>;

		b_bv() = delete;

		explicit b_bv(const b_bv& sp) {
			B = sp.B;
			B_LEAF = sp.B_LEAF;
			root = new b_node<leaf_type>(*sp.root);
			this->B = B;
			this->B_LEAF = B_LEAF;
		}

		explicit b_bv(uint32_t B, uint32_t B_LEAF, uint64_t message_count = 20) {
			this->B = B;
			this->B_LEAF = B_LEAF;
			root = new b_node <leaf_type >(B, B_LEAF, message_count);
		}

		~b_bv() {
			root->free_mem();

			assert(root != NULL);

			delete root;
			root = NULL;
		}

		void operator=(const b_bv& sp) {
			root->free_mem();
			delete root;

			root = new b_node(*sp.root);
		}

		bv_ref operator[](uint64_t i) {
			return { *this, i };
		}

		uint64_t select(uint64_t i, bool b = true) const {
			assert(b ? i < rank(size(), true) : i < rank(size(), false));
			return b ? search(i + 1) : search_0(i + 1);
		}

		/*
		 * number of bits equal to b before position i EXCLUDED
		 */
		uint64_t rank(uint64_t i, bool b = true) const {
			assert(i <= size());

			auto r1 = i == 0 ? 0 : psum(i - 1);

			return  b ? r1 : i - r1;
		}

		void remove(uint64_t i) {
			b_node<leaf_type>* new_root = root->remove(i);
			if (new_root != NULL) {
				delete root;
				root = new_root;
			}
		}

		uint64_t size() const {
			assert(root != NULL);
			return root->size();
		}

		bool at(uint64_t i) const {
			assert(size() > 0);
			assert(i < size());

			return root->at(i);
		}

		void insert(uint64_t i, bool x) {
			assert(i <= root->size());

			b_node<leaf_type>* new_root = root->insert(i, x);

			if (new_root != NULL) {
				root = new_root;
			}
		}

		void set(uint64_t i, bool x) {
			assert(size() > 0);
			assert(i < size());

			root->increment(i, x);
		}

		void push_back(bool x) {
			insert(size(), x);
		}

	private:
		uint32_t B;

		uint32_t B_LEAF;

		/*
		* returns I_0 + ... + I_i = sum up to i-th integer included
		*/
		uint64_t psum(uint64_t i) const {
			if (size() == 0) {
				return 0;
			}

			assert(i < size());

			return root->psum(i);
		}

		/*
		 * returns smallest i such that I_0 + ... + I_i >= x
		 */
		uint64_t search(uint64_t x) const {
			assert(x <= psum(size() - 1));

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
			assert(x <= size() - psum(size() - 1));

			return root->search_0(x);
		}

		/*
		 * true iif x is one of the partial sums  0, I_0, I_0+I_1, ...
		 */
		bool contains(uint64_t x) const {
			assert(x <= psum());

			return root->contains(x);
		}

		b_node<leaf_type>* root = NULL;
	};

}