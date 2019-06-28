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

		static uint64_t fast_mod(const uint64_t num)
		{
			return num & (int_per_word_ - 1);
		}

		static uint64_t fast_div(const uint64_t num)
		{
			return num >> 6;
		}

		static uint64_t fast_mul(const uint64_t num)
		{
			return num << 6;
		}

		using pv_ref = pv_reference<packed_vector>;

		/* new packed vector containing size integers (initialized to 0) of width bits each*/
		explicit packed_vector(const uint64_t size = 0) {

			/* if size > 0, then width must be > 0*/
			assert(size == 0 || width_ > 0);

			size_ = size;
			psum_ = 0;

			if (size_ == 0) {
				words = vector<uint64_t>();
			}
			else {
				words = vector<uint64_t>(fast_div(size_) + (fast_mod(size_) != 0));
			}
		}

		packed_vector(vector<uint64_t>& words, const uint64_t new_size) {
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
			return MASK & (words[fast_div(i)] >> fast_mod(i));
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

			for (uint64_t j = 0; j < (fast_div(i)); ++j) {
				s += __builtin_popcountll(words[j]);
				pos += 64;
			}

			auto const mod = fast_mod(i);
			if (mod) {
				s += __builtin_popcountll(words[fast_div(i)] & ((uint64_t(1) << mod) - 1));
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

			for (uint64_t j = 0; j < fast_div(size_) && s < x; ++j) {

				pop = __builtin_popcountll(words[j]);
				pos += 64;
				s += pop;

			}

			// end optimization for bitvectors

			pos -= fast_mul(pos > 0);
			s -= pop;

			for (; pos < size_ && s < x; ++pos) {
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

			for (uint64_t j = 0; j < fast_div(size_) && s < x; ++j) {

				pop = 64 - __builtin_popcountll(words[j]);
				pos += 64;
				s += pop;

			}

			pos -= fast_mul(pos > 0);
			s -= pop;

			for (; pos < size_ && s < x; ++pos) {

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

			for (uint64_t j = 0; j < fast_div(size_) && s < x; ++j) {

				pop = 64 + __builtin_popcountll(words[j]);
				pos += 64;
				s += pop;

			}

			pos -= fast_mul(pos > 0);
			s -= pop;

			for (; pos < size_ && s < x; ++pos) {

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

			for (uint64_t j = 0; j < size_ && s < x; ++j) {

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

			for (uint64_t j = 0; j < size_ && s < x; ++j) {

				s += (at(j) + 1);

			}

			return s == x;

		}

		void increment(uint64_t i, bool val) {

			assert(i < size_);

			set<false>(i, val);
			val ? psum_++ : psum_--;
		}

		void append(uint64_t x) {

			insert(size(), x);

		}

		void remove(uint64_t i) {
			//shift ints left, from position i + 1 onwords
			shift_left(i);

			while (fast_mul(words.size() - extra_ - 1) >= size_ - 1) {
				//more than extra_ extra words, delete
				if (words.size() < extra_) {
					break;
				}
				words.pop_back();
			}

			--size_;
			psum_--;
		}

		void insert(uint64_t i, uint64_t x) {

			//not enough space for the new element:
			//alloc extra_ new words
			if (size_ + 1 > fast_mul(words.size())) {

				//resize words
				auto temp = vector<uint64_t>(words.size() + extra_, 0);

				uint64_t j = 0;
				for (auto t : words) {
					temp[j++] = t;
				}

				words = vector<uint64_t>(temp);
			}

			//shift right elements starting from number i
			shift_right(i);

			//insert x
			set<false>(i, x);

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
			if (size_ + 1 > (fast_mul(words.size()))) {
				words.push_back(0);
			}

			//insert x
			set<false>(size(), x);

			psum_++;
			size_++;
		}

		uint64_t size() const
		{
			return size_;
		}

		/*
		 * split content of this vector into 2 packed blocks:
		 * Left part remains in this block, right part in the
		 * new returned block
		 */
		packed_vector* split() {
			const auto tot_words = fast_div(size_) + (fast_mod(size_) != 0);

			assert(tot_words <= words.size());

			const auto nr_left_words = tot_words >> 1;

			assert(nr_left_words > 0);

			const auto nr_left_ints = fast_mul(nr_left_words);

			assert(size_ > nr_left_ints);
			const auto nr_right_ints = size_ - nr_left_ints;

			auto right_words = vector<uint64_t>(words.begin() + nr_left_words, words.begin() + tot_words);
			words = vector<uint64_t>(words.begin(), words.begin() + nr_left_words + extra_);

			size_ = nr_left_ints;
			psum_ = psum(size_ - 1);

			const auto right = new packed_vector(right_words, nr_right_ints);

			return right;
		}

		/* set i-th element to x. updates psum */
		template <bool psum> void set(const uint64_t i, const bool x) {
			if constexpr (psum) {
				x ? psum_++ : psum_--;
			}

			const auto word_nr = fast_div(i);
			const uint8_t pos = fast_mod(i);

			if (x) {
				words[word_nr] |= (MASK << pos);
			}
			else {
				words[word_nr] &= ~(MASK << pos);
			}
		}

		/*
		 * return total number of bits occupied in memory by this object instance
		 */
		ulint bit_size() const
		{
			return (sizeof(packed_vector) + words.capacity() * sizeof(ulint)) * 8;
		}

		static uint64_t width() {
			return width_;
		}

	private:
		//shift right of 1 position elements starting
		//from the i-th.
		//assumption: last element does not overflow!
		void shift_right(uint64_t i) {

			//number of integers that fit in a memory word
			assert(int_per_word_ > 0);

			assert(size_ + 1 <= words.size() * int_per_word_);

			// Divide by 64 (and truncate)
			uint64_t current_word = fast_div(i);

			uint64_t falling_out_idx = fast_mul(current_word) + (int_per_word_ - 1);

			//integer that falls out from the right of current word
			bool falling_out = at(falling_out_idx);

			for (uint64_t j = falling_out_idx; j > i; --j) {
				set<false>(j, at(j - 1));
			}

			//now for the remaining integers we can work blockwise

			uint64_t falling_out_temp;

			for (uint64_t j = current_word + 1; j < words.size(); ++j) {
				auto val = fast_mul(j);

				falling_out_temp = at(val + (int_per_word_ - 1));

				words[j] = words[j] << width_;

				assert(at(j * int_per_word_) == 0);

				set<false>(val, falling_out);

				falling_out = falling_out_temp;

			}

		}

		//shift left of 1 position elements starting
		//from the (i + 1)-st.
		void shift_left(uint64_t i) {

			//number of integers that fit in a memory word
			assert(int_per_word_ > 0);

			if (i == (size_ - 1)) {
				set<false>(i, false);
				return;
			}

			// Divide by 64
			auto current_word = fast_div(i);

			//integer that falls in from the right of current word
			auto falling_in_idx = fast_mul(current_word + 1);
			if (falling_in_idx > (size_ - 1)) {
				falling_in_idx = size_ - 1;
			}


			for (auto j = i; j <= falling_in_idx - 1; ++j) {
				set<false>(j, at(j + 1));
			}

			//now for the remaining integers we can work blockwise
			for (uint64_t j = current_word + 1; j < words.size(); ++j) {
				words[j] = words[j] >> width_;

				if (j < words.size() - 1) {
					const uint64_t falling_in = at(fast_mul(j + 1));

					set<false>(fast_mul(j) + int_per_word_ - 1, falling_in);
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
		vector<uint64_t> words{};
		uint64_t psum_ = 0;
		uint64_t size_ = 0;
	};

}