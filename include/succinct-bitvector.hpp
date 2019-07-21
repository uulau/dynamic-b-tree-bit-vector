// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#pragma once

#include <istream>
#include <ostream>
#include "bv_reference.hpp"

using namespace std;

namespace dyn {
	template
		<
		class leaf_type,
		uint32_t B_LEAF,
		uint32_t B,
		uint64_t buffer_size,
		template <
		class,
		uint32_t,
		uint32_t,
		uint64_t
		> class spsi_type
		>
		class succinct_bitvector {

		public:

			using bv_ref = bv_reference<succinct_bitvector>;

			/*
			 * create empty dynamic bitvector
			 */
			succinct_bitvector()
			{
				spsi_ = spsi_type <leaf_type, B_LEAF, B, buffer_size>();
			}

			/*
			 * number of bits in the bitvector
			 */
			uint64_t size() const {

				return spsi_.size();

			}

			/*
			 * high-level access to the bitvector. Supports assign (operator=) and access
			 */
			bv_ref operator[](uint64_t i) {

				assert(i < size());
				return { *this, i };

			}

			/*
			 * access
			 */
			bool at(uint64_t i) const {

				assert(i < size());
				return spsi_.at(i);

			}

			uint64_t select(uint64_t i, bool b = true) const {

				return b ? select1(i) : select0(i);

			}

			/*
			 * position of i-th bit not set. 0 =< i < rank(size(),0)
			 */
			uint64_t select0(uint64_t i) const {

				assert(i < rank0(size()));
				return spsi_.search_0(i);

			}

			/*
			 * position of i-th bit set. 0 =< i < rank(size(),1)
			 */
			uint64_t select1(uint64_t i) const {

				assert(i < rank1(size()));
				return spsi_.search(i);

			}

			/*
			 * number of bits equal to b before position i EXCLUDED
			 */
			uint64_t rank(uint64_t i, bool b = true) const {

				assert(i <= size());

				auto r1 = i == 0 ? 0 : spsi_.psum(i - 1);

				return  b ? r1 : i - r1;

			}

			/*
			 * number of bits equal to 0 before position i EXCLUDED
			 */
			uint64_t rank0(uint64_t i) const {

				assert(i <= size());
				return (i == 0 ? 0 : i - spsi_.psum(i - 1));

			}

			/*
			 * number of bits equal to 1 before position i EXCLUDED
			 */
			uint64_t rank1(uint64_t i) const {

				assert(i <= size());
				return (i == 0 ? 0 : spsi_.psum(i - 1));

			}

			/*
			 * total number of bits not set
			 */
			uint64_t rank0() const {

				return rank0(size());

			}

			/*
			 * total number of bits set
			 */
			uint64_t rank1() const {

				return rank1(size());


			}

			/*
			 * insert a bit b at position i
			 */
			void insert(uint64_t i, bool b) {

				spsi_.insert(i, b);

			}

			/*
			 * remove the bit at position i
			 */
			void remove(uint64_t i) {

				spsi_.remove(i);

			}

			/* append b at the end of the bitvector */
			void push_back(bool b) {
				insert(size(), b);
			}

			/*
			 * push back n bits packed into word
			 */
			void push_word(uint64_t word, uint8_t n) {

				// insert n least significant bits from word
				spsi_.push_word(word, 1, n);

			}

			/*
			 * insert a bit not set at position i
			 */
			void insert0(uint64_t i) {

				insert(i, false);

			}

			/*
			 * insert a bit set at position i
			 */
			void insert1(uint64_t i) {

				insert(i, true);

			}

			void push_front(bool b) {

				insert(0, b);

			}

			/*
			 * sets i-th bit to value.
			 */
			void set(uint64_t i, bool value = true) {

				spsi_[i] = value;

			}

			/*
			 * Total number of bits allocated in RAM for this structure
			 */
			uint64_t bit_size() const {
				return sizeof(succinct_bitvector<leaf_type, B_LEAF, B, buffer_size, spsi_type>) * 8 + spsi_.bit_size();

			}

			uint64_t serialize(ostream& out) const {

				return spsi_.serialize(out);

			}

			void load(istream& in) {

				spsi_.load(in);

			}

		private:
			//underlying Searchable partial sum with inserts structure.
			//the spsi contains only integers 0 and 1
			spsi_type<leaf_type, B_LEAF, B, buffer_size> spsi_;
	};
}