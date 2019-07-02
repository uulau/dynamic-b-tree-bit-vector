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

		static uint64_t fast_mod(uint64_t const num)
		{
			return num & 63;
		}

		static uint64_t fast_div(uint64_t const num)
		{
			return num >> 6;
		}

		static uint64_t fast_mul(uint64_t const num)
		{
			return num << 6;
		}

		explicit packed_vector(uint64_t const size = 0) {
			this->size_ = size;
			this->psum_ = 0;

			words = vector<uint64_t>(fast_div(size_) + (fast_mod(size_) != 0));

			assert(size_ / int_per_word_ <= words.size());
			assert((size_ / int_per_word_ == words.size()
				|| !(words[size_ / int_per_word_] >> ((size_ % int_per_word_) * width_)))
				&& "uninitialized non-zero values in the end of the vector");
		}

		explicit packed_vector(vector<uint64_t>&& _words, uint64_t const new_size) {
			this->words = _words;
			this->size_ = new_size;
			this->psum_ = psum(size_ - 1);

			assert(size_ / int_per_word_ <= words.size());
			assert((size_ / int_per_word_ == words.size()
				|| !(words[size_ / int_per_word_] >> ((size_ % int_per_word_) * width_)))
				&& "uninitialized non-zero values in the end of the vector");
		}

		~packed_vector() = default;

		/*
		 * high-level access to the vector. Supports assign, access,
		 * increment (++, +=), decrement (--, -=)
		 */
		pv_ref operator[](uint64_t const i) {

			return { *this, i };

		}

		bool at(uint64_t const i) const {
			assert(i < size_);

			return MASK & (words[fast_div(i)] >> fast_mod(i));
		}

		uint64_t psum() const {
			return psum_;
		}

		/*
		 * inclusive partial sum (i.e. up to element i included)
		 */
		uint64_t psum(uint64_t i) const {

			assert(i < size_);

			i++;

			uint64_t s = 0;
			uint64_t pos = 0;

			auto const max = fast_div(i);
			for (uint64_t j = 0; j < max; ++j) {
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
		uint64_t search(uint64_t x) const {

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
		uint64_t search_0(uint64_t x) const {

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
		uint64_t search_r(uint64_t x) const {

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
		bool contains(uint64_t x) const {

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
		bool contains_r(uint64_t x) const {

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
			push_back(x);
		}

		void remove(uint64_t i) {
			auto x = this->at(i);
			//shift ints left, from position i + 1 onwords
			shift_left(i);

			--size_;
			psum_ -= x;

			while (words.size() > size_ / int_per_word_ + extra_) {
				words.pop_back();
			}

			assert(size_ / int_per_word_ <= words.size());
			assert((size_ / int_per_word_ == words.size()
				|| !(words[size_ / int_per_word_] >> ((size_ % int_per_word_) * width_)))
				&& "uninitialized non-zero values in the end of the vector");
		}

		void insert(uint64_t i, uint64_t x) {
			if (i == size()) {
				push_back(x);
				return;
			}

			if (size_ + 1 > fast_mul(words.size())) {
				words.resize(words.size() + extra_, 0);
			}

			//shift right elements starting from number i
			shift_right(i);

			//insert x
			set<false>(i, x);

			psum_ += x;
			++size_;

			assert(size_ / int_per_word_ <= words.size());
			assert((size_ / int_per_word_ == words.size()
				|| !(words[size_ / int_per_word_] >> ((size_ % int_per_word_) * width_)))
				&& "uninitialized non-zero values in the end of the vector");
		}

		/*
		 * efficient push-back, implemented with a push-back on the underlying container
		 * the insertion of an element whose bit-size exceeds the current width causes a
		 * rebuild of the whole vector!
		 */
		void push_back(bool x) {
			assert(int_per_word_ == 64);
			assert(size_ <= words.size() * 64);

			//not enough space for the new element:
			//push back a new word
			if (fast_div(size_++) == words.size()) {
				words.push_back(0);
			}

			assert(size_ <= words.size() * 64);
			assert(!at(size_ - 1));

			if (x) {
				//insert x at the last position
				words[fast_div(size_ - 1)] |= static_cast<uint64_t>(1) << fast_mod(size_ - 1);
				psum_++;
			}

			assert(size_ / int_per_word_ <= words.size());
			assert((size_ / int_per_word_ == words.size()
				|| !(words[size_ / int_per_word_] >> ((size_ % int_per_word_) * width_)))
				&& "uninitialized non-zero values in the end of the vector");
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
			uint64_t tot_words = fast_div(size_) + (fast_mod(size_) != 0);

			assert(tot_words <= words.size());

			uint64_t nr_left_words = tot_words >> 1;

			assert(nr_left_words > 0);
			assert(tot_words - nr_left_words > 0);

			uint64_t nr_left_ints = fast_mul(nr_left_words);

			assert(size_ > nr_left_ints);
			uint64_t nr_right_ints = size_ - nr_left_ints;

			assert(words.begin() + nr_left_words + extra_ < words.end());
			vector<uint64_t> right_words(tot_words - nr_left_words + extra_, 0);
			std::copy(&words[nr_left_words], &words[tot_words], right_words.begin());
			words.resize(nr_left_words + extra_);
			std::fill(words.begin() + nr_left_words, words.end(), 0);
			words.shrink_to_fit();

			size_ = nr_left_ints;
			psum_ = psum(size_ - 1);

			auto right = new packed_vector(std::move(right_words), nr_right_ints);

			assert(size_ / int_per_word_ <= words.size());
			assert((size_ / int_per_word_ == words.size()
				|| !(words[size_ / int_per_word_] >> ((size_ % int_per_word_) * width_)))
				&& "uninitialized non-zero values in the end of the vector");

			return right;
		}

		/* set i-th element to x. updates psum */
		template <bool psum> void set(const uint64_t i, const bool x) {
			if constexpr (psum) {
				x ? psum_++ : psum_--;
			}

			const auto word_nr = fast_div(i);
			const uint8_t pos = fast_mod(i);

			words[word_nr] ^= (-(uint64_t)x ^ words[word_nr]) & (MASK << pos);
		}

		/*
		 * return total number of bits occupied in memory by this object instance
		 */
		uint64_t bit_size() const
		{
			return (sizeof(packed_vector) + words.capacity() * sizeof(ulint)) * 8;
		}

		uint64_t width() const {
			return width_;
		}

	private:
		//shift right of 1 position elements starting
		//from the i-th.
		//assumption: last element does not overflow!
		void shift_right(uint64_t i) {
			assert(i < size_);
			//number of integers that fit in a memory word
			assert(int_per_word_ > 0);
			assert(size_ + 1 <= fast_mul(words.size()));

			uint64_t current_word = fast_div(i);

			//integer that falls out from the right of current word
			uint64_t falling_out = 0;

			if (fast_mul(current_word) < i) {
				falling_out = (words[current_word] >> (int_per_word_ - 1));

				uint64_t falling_out_idx = std::min(fast_mul(current_word) + (int_per_word_ - 1), size_);

				for (uint64_t j = falling_out_idx; j > i; --j) {
					assert(j - 1 < size_);
					set<false>(j, at(j - 1));
				}

				current_word++;
			}

			//now for the remaining integers we can work blockwise

			uint64_t falling_out_temp;

			auto val = fast_div(size_);
			for (uint64_t j = current_word; j <= val; ++j) {

				assert(j < words.size());

				falling_out_temp = (words[j] >> (int_per_word_ - 1));

				words[j] <<= width_;

				assert(fast_mul(j) >= size_ || !at(fast_mul(j)));

				set<false>(fast_mul(j), falling_out);

				falling_out = falling_out_temp;
			}
		}

		//shift left of 1 position elements starting
		//from the (i + 1)-st.
		void shift_left(uint64_t i) {

			//number of integers that fit in a memory word
			assert(int_per_word_ > 0);

			if (i == 0) {
				set<false>(i, false);
				return;
			}

			// Divide by 64
			auto current_word = fast_div(i);

			//integer that falls in from the right of current word
			auto falling_in_idx = min(fast_mul(current_word + 1), size_ - 1);

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

		uint64_t sum(packed_vector& vec) const {
			uint64_t res = 0;

			for (uint64_t i = 0; i < vec.size(); ++i) {

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