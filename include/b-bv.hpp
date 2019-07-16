// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#pragma once

namespace dyn {
	template
		<
		typename leaf_type,
		template <typename> typename node_type
		>
		class b_bv {
		public:
			b_bv() = delete;

			explicit b_bv(const b_bv& sp) {
				B = sp.B;
				B_LEAF = sp.B_LEAF;
				root = new node_type<leaf_type>(*sp.root);
			}

			explicit b_bv(uint32_t B, uint32_t B_LEAF, uint64_t message_count = 0) {
				this->B = B;
				this->B_LEAF = B_LEAF;
				root = new node_type <leaf_type >(B, B_LEAF, message_count);
			}

			~b_bv() {
				root->free_mem();

				assert(root != nullptr);

				delete root;
				root = nullptr;
			}

			void operator=(const b_bv& sp) {
				root->free_mem();
				delete root;

				root = new node_type(*sp.root);
			}

			uint64_t select(uint64_t i, bool b = true) const {
				assert(b ? i < rank(size(), true) : i < rank(size(), false));
				return b ? search(i) : search_0(i);
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
				node_type<leaf_type>* new_root = root->remove(i);
				if (new_root != nullptr) {
					delete root;
					root = new_root;
				}
			}

			uint64_t size() const {
				assert(root != nullptr);
				return root->size();
			}

			bool at(uint64_t i) const {
				assert(size() > 0);
				assert(i < size());

				return root->at(i);
			}

			void insert(uint64_t i, bool x) {
				assert(i <= root->size());

				node_type<leaf_type>* new_root = root->insert(i, x);

				if (new_root != nullptr) {
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

			node_type<leaf_type>* root{ nullptr };
	};
}