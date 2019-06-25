// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#pragma once

#include "includes.hpp"
#include "msvc.hpp"
#include "pv_reference.hpp"

namespace dyn {
	class packed_vector {

	public:

		using pv_ref = pv_reference<packed_vector>;

		/* new packed vector containing size integers (initialized to 0) of width bits each*/
		packed_vector(uint64_t size = 0) {

			/* if size > 0, then width must be > 0*/
			assert(size == 0 || width_ > 0);

			size_ = size;
			psum_ = 0;

			if (size_ == 0) {
				words = vector<uint64_t>();
			}
			else {
				words = vector<uint64_t>(size_ >> 6 + (size_ & int_per_word_ - 1 != 0));
			}
		}

		packed_vector(vector<uint64_t>& words, uint64_t new_size) {
			this->words = vector<uint64_t>(words);
			this->size_ = new_size;
			psum_ = psum(size_ - 1);
		}

		/*
		 * high-level access to the vector. Supports assign, access,
		 * increment (++, +=), decrement (--, -=)
		 */
		pv_ref operator[](uint64_t i) {

			return { *this, i };

		}

		bool at(uint64_t i) {
			// Divide by 64 (2^6)
			auto word_index = i >> 6;
			// int_per_word power of two, fast mod
			auto offset = i & (int_per_word_ - 1);
			// Get last bit
			return MASK & (words[word_index] >> offset);
		}

		uint64_t psum() {
			return psum_;
		}

		/*
		 * inclusive partial sum (i.e. up to element i included)
		 */
		uint64_t psum(uint64_t i) {

			assert(i < size_);

			i++;

			uint64_t s = 0;
			uint64_t pos = 0;

			for (uint64_t j = 0; j < (i >> 6); ++j) {
				s += __builtin_popcountll(words[j]);
				pos += 64;
			}

			auto mod = i & (int_per_word_ - 1);

			if (width_ <= 1 && mod != 0) {
				__builtin_popcountll(words[i >> 6] & ((uint64_t(1) << mod) - 1));
			}

			for (uint64_t j = pos; j < i; ++j) {
				s += at(j);
			}

			return s;
		}

		/*
		 * smallest index j such that psum(j)>=x
		 */
		uint64_t search(uint64_t x) {

			assert(size_ > 0);
			assert(x <= psum_);

			uint64_t s = 0;
			uint64_t pop = 0;
			uint64_t pos = 0;

			// optimization for bitvectors

			for (uint64_t j = 0; j < (size_ / 64) * (width_ == 1) and s < x; ++j) {

				pop = __builtin_popcountll(words[j]);
				pos += 64;
				s += pop;

			}

			// end optimization for bitvectors

			pos -= 64 * (pos > 0);
			s -= pop;

			for (; pos < size_ and s < x; ++pos) {

				s += at(pos);

			}

			pos -= pos != 0;

			return pos;

		}

		/*
		 * this function works only for bitvectors, and
		 * is designed to support select_0. Returns first
		 * position i such that the number of zeros before
		 * i (included) is == x
		 */
		uint64_t search_0(uint64_t x) {

			assert(size_ > 0);
			assert(width_ == 1);
			assert(x <= size_ - psum_);

			uint64_t s = 0;
			uint64_t pop = 0;
			uint64_t pos = 0;

			// optimization for bitvectors

			for (uint64_t j = 0; j < size_ / 64 and s < x; ++j) {

				pop = 64 - __builtin_popcountll(words[j]);
				pos += 64;
				s += pop;

			}

			// end optimization for bitvectors

			pos -= 64 * (pos > 0);
			s -= pop;

			for (; pos < size_ and s < x; ++pos) {

				s += (1 - at(pos));

			}

			pos -= pos != 0;

			return pos;

		}

		/*
		 * smallest index j such that psum(j)+j>=x
		 */
		uint64_t search_r(uint64_t x) {

			assert(size_ > 0);
			assert(x <= psum_ + size_);

			uint64_t s = 0;
			uint64_t pop = 0;
			uint64_t pos = 0;

			// optimization for bitvectors

			for (uint64_t j = 0; j < (size_ / 64) * (width_ == 1) and s < x; ++j) {

				pop = 64 + __builtin_popcountll(words[j]);
				pos += 64;
				s += pop;

			}

			// end optimization for bitvectors

			pos -= 64 * (pos > 0);
			s -= pop;

			for (; pos < size_ and s < x; ++pos) {

				s += (1 + at(pos));

			}

			pos -= pos != 0;

			return pos;

		}

		/*
		 * true iif x is one of the partial sums  0, I_0, I_0+I_1, ...
		 */
		bool contains(uint64_t x) {

			assert(size_ > 0);
			assert(x <= psum_);

			uint64_t s = 0;

			for (uint64_t j = 0; j < size_ and s < x; ++j) {

				s += at(j);

			}

			return s == x;

		}

		/*
		 * true iif x is one of  0, I_0+1, I_0+I_1+2, ...
		 */
		bool contains_r(uint64_t x) {

			assert(size_ > 0);
			assert(x <= psum_ + size_);

			uint64_t s = 0;

			for (uint64_t j = 0; j < size_ and s < x; ++j) {

				s += (at(j) + 1);

			}

			return s == x;

		}

		void increment(uint64_t i, bool val) {

			assert(i < size_);

			set_without_psum_update(i, val);
			val ? psum_++ : psum_--;
		}

		void append(uint64_t x) {

			insert(size(), x);

		}

		void remove(uint64_t i) {
			auto x = this->at(i);

			//shift ints left, from position i + 1 onwords
			shift_left(i);


			while ((words.size() - extra_ - 1) * (int_per_word_) >= size_ - 1) {
				//more than extra_ extra words, delete
				if (words.size() < extra_)
					break;

				words.pop_back();

			}

			--size_;
			psum_ -= x;

		}

		void insert(uint64_t i, uint64_t x) {

			//not enough space for the new element:
			//alloc extra_ new words
			if (size_ + 1 > (words.size() * (int_per_word_))) {

				//resize words
				auto temp = vector<uint64_t>(words.size() + extra_, 0);

				uint64_t j = 0;
				for (auto t : words) temp[j++] = t;

				words = vector<uint64_t>(temp);
			}

			//shift right elements starting from number i
			shift_right(i);

			//insert x
			set_without_psum_update(i, x);

			psum_ += x;
			++size_;
		}



		/*
		 * efficient push-back, implemented with a push-back on the underlying container
		 * the insertion of an element whose bit-size exceeds the current width causes a
		 * rebuild of the whole vector!
		 */
		void push_back(bool x) {

			//not enough space for the new element:
			//push back a new word
			if (size_ + 1 > (words.size() * (int_per_word_))) words.push_back(0);

			//insert x
			set_without_psum_update(size(), x);

			psum_ += x;
			size_++;

		}

		uint64_t size() {
			return size_;
		}

		/*
		 * split content of this vector into 2 packed blocks:
		 * Left part remains in this block, right part in the
		 * new returned block
		 */
		packed_vector* split() {

			uint64_t tot_words = (size_ / int_per_word_) + (size_ % int_per_word_ != 0);

			assert(tot_words <= words.size());

			uint64_t nr_left_words = tot_words / 2;

			assert(nr_left_words > 0);

			uint64_t nr_left_ints = nr_left_words * int_per_word_;

			assert(size_ > nr_left_ints);
			uint64_t nr_right_ints = size_ - nr_left_ints;

			auto right_words = vector<uint64_t>(words.begin() + nr_left_words, words.begin() + tot_words);
			words = vector<uint64_t>(words.begin(), words.begin() + nr_left_words + extra_);

			size_ = nr_left_ints;
			psum_ = psum(size_ - 1);

			auto right = new packed_vector(right_words, nr_right_ints);

			return right;

		}

		/* set i-th element to x. updates psum */
		void set(uint64_t i, uint64_t x) {

			auto y = at(i);

			psum_ = x < y ? psum_ - (y - x) : psum_ + (x - y);

			uint64_t word_nr = i / int_per_word_;
			uint8_t pos = i % int_per_word_;

			//set to 0 i-th entry
			uint64_t MASK1 = ~(MASK << (width_ * pos));
			words[word_nr] &= MASK1;

			//insert x inside i-th position
			words[word_nr] |= (x << (width_ * pos));

		}

		/*
		 * return total number of bits occupied in memory by this object instance
		 */
		ulint bit_size() {
			return (sizeof(packed_vector) + words.capacity() * sizeof(ulint)) * 8;
		}

		uint64_t width() {
			return width_;
		}

	private:

		void set_without_psum_update(uint64_t i, bool x) {
			uint64_t word_nr = i >> 6;
			uint8_t pos = i & int_per_word_ - 1;

			if (x) {
				words[word_nr] |= (MASK << pos);
			}
			else {
				words[word_nr] &= ~(MASK << pos);
			}
		}

		//shift right of 1 position elements starting
		//from the i-th.
		//assumption: last element does not overflow!
		void shift_right(uint64_t i) {

			//number of integers that fit in a memory word
			assert(int_per_word_ > 0);

			assert(size_ + 1 <= words.size() * int_per_word_);

			// Divide by 64 (and truncate)
			uint64_t current_word = i >> 6;

			uint64_t falling_out_idx = (current_word << 6) + (int_per_word_ - 1);

			//integer that falls out from the right of current word
			bool falling_out = at(falling_out_idx);

			for (uint64_t j = falling_out_idx; j > i; --j) {
				set_without_psum_update(j, at(j - 1));
			}

			//now for the remaining integers we can work blockwise

			uint64_t falling_out_temp;

			for (uint64_t j = current_word + 1; j < words.size(); ++j) {
				auto val = j << 6;

				falling_out_temp = at(val + (int_per_word_ - 1));

				words[j] = words[j] << width_;

				assert(at(j * int_per_word_) == 0);

				set_without_psum_update(val, falling_out);

				falling_out = falling_out_temp;

			}

		}

		//shift left of 1 position elements starting
		//from the (i + 1)-st.
		void shift_left(uint64_t i) {

			//number of integers that fit in a memory word
			assert(int_per_word_ > 0);

			if (i == (size_ - 1)) {
				set_without_psum_update(i, 0);

				return;
			}

			// Divide by 64
			uint64_t current_word = i >> 6;

			//integer that falls in from the right of current word
			uint64_t falling_in_idx = (current_word + 1) << 6;
			uint64_t falling_in;
			if (falling_in_idx > (size_ - 1)) {
				//nothing falls in
				falling_in = 0;
				falling_in_idx = size_ - 1;
			}


			for (uint64_t j = i; j <= falling_in_idx - 1; ++j) {
				set_without_psum_update(j, at(j + 1));
			}

			//now for the remaining integers we can work blockwise
			for (uint64_t j = current_word + 1; j < words.size(); ++j) {
				words[j] = words[j] >> width_;

				if (j < words.size() - 1) {
					falling_in = at((j + 1) << 6);

					set_without_psum_update((j << 6) + int_per_word_ - 1, falling_in);
				}

			}

		}

		uint64_t sum(packed_vector& vec) {
			uint64_t res = 0;

			for (ulint i = 0; i < vec.size(); ++i) {

				auto x = vec[i];
				res += x;

			}
			return res;
		}

		static constexpr uint8_t width_ = 1;
		static constexpr uint8_t int_per_word_ = 64;
		static constexpr uint64_t MASK = 1;
		static constexpr uint8_t extra_ = 2;
		vector<uint64_t> words;
		uint64_t psum_ = 0;
		uint64_t size_ = 0;
	};

}