// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#pragma once

#include "msvc.hpp"
#include <cassert>
#include <algorithm>
#include <vector>
#include <cstdint>

namespace dyn {
	class packed_vector {
	public:
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

			words = std::vector<uint64_t>(fast_div(size_) + (fast_mod(size_) != 0));

			assert(size_ / int_per_word_ <= words.size());
			assert((size_ / int_per_word_ == words.size()
				|| !(words[size_ / int_per_word_] >> ((size_ % int_per_word_) * width_)))
				&& "uninitialized non-zero values in the end of the vector");
		}

		explicit packed_vector(std::vector<uint64_t>&& _words, uint64_t const new_size) {
			this->words = _words;
			this->size_ = new_size;
			this->psum_ = psum(size_ - 1);

			assert(size_ / int_per_word_ <= words.size());
			assert((size_ / int_per_word_ == words.size()
				|| !(words[size_ / int_per_word_] >> ((size_ % int_per_word_) * width_)))
				&& "uninitialized non-zero values in the end of the vector");
		}

		~packed_vector() = default;

		bool at(uint64_t i) const {
			assert(i < size());

			auto index = i;

			if (buffer_index != 0xFFFFFFFFFFFFFFFF) {
				if (buffer2_index != 0xFFFFFFFFFFFFFFFF) {
					auto i2 = buffer2_index;

					if (buffer_index <= i2) {
						++i2;
					}

					if (i2 <= i) {
						if (i2 == i) {
							return buffer2_val;
						}
						else {
							++index;
						}
					}
				}
				if (buffer_index <= i) {
					if (buffer_index == i) {
						return buffer_val;
					}
					else {
						++index;
					}
				}
			}

			return MASK & (words[fast_div(index)] >> fast_mod(index));
		}

		uint64_t psum() const {
			return psum_;
		}

		/*
		 * inclusive partial sum (i.e. up to element i included)
		 */
		uint64_t psum(uint64_t i) const {

			assert(i < size_);

			++i;

			uint64_t add_val = 0;
			auto index = i;

			if (buffer_index != 0xFFFFFFFFFFFFFFFF) {
				if (buffer2_index != 0xFFFFFFFFFFFFFFFF) {
					auto i2 = buffer2_index;

					if (buffer_index <= i2) {
						++i2;
					}

					if (i2 < i) {
						add_val += buffer2_val;
						--index;
					}
				}
				if (buffer_index <= i) {
					if (buffer_index < i) {
						add_val += buffer_val;
						--index;
					}
				}
			}

			uint64_t s = 0;
			uint64_t pos = 0;

			auto const max = fast_div(index);
			for (uint64_t j = 0; j < max; ++j) {
				s += __builtin_popcountll(words[j]);
				pos += 64;
			}

			auto const mod = fast_mod(index);
			if (mod) {
				s += __builtin_popcountll(words[max] & ((uint64_t(1) << mod) - 1));
			}

			return s + add_val;
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

			auto div = fast_div(size_);
			for (uint64_t j = 0; j < div && s < x; ++j) {
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

			auto div = fast_div(size_);
			for (uint64_t j = 0; j < div && s < x; ++j) {
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

			auto div = fast_div(size_);
			for (uint64_t j = 0; j < div && s < x; ++j) {
				pop = uint64_t(64) + __builtin_popcountll(words[j]);
				pos += 64;
				s += pop;
			}

			pos -= fast_mul(pos > 0);
			s -= pop;

			for (; pos < size_ && s < x; ++pos) {

				s += (uint64_t(1) + at(pos));

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
				s += (at(j) + uint64_t(1));
			}

			return s == x;
		}

		void increment(uint64_t i, bool val, bool subtract = false) {

			assert(i < size_);

			if (subtract)
			{
				set<false>(i, false);
				return;
			}

			set<false>(i, val);
			val ? psum_++ : psum_--;
		}

		void append(uint64_t x) {
			push_back(x);
		}

		void remove(uint64_t i) {
			assert(i < size_);
			auto x = this->at(i);
			//shift ints left, from position i + 1 onwords
			shift_left(i);

			--size_;
			psum_ -= x;

			auto div = fast_div(size_);
			while (words.size() > div + extra_) {
				words.pop_back();
			}

			assert(size_ / int_per_word_ <= words.size());
			assert((size_ / int_per_word_ == words.size()
				|| !(words[size_ / int_per_word_] >> ((size_ % int_per_word_) * width_)))
				&& "uninitialized non-zero values in the end of the vector");
		}

		void insert(uint64_t i, uint64_t x) {
			// First empty
			if (buffer_index == 0xFFFFFFFFFFFFFFFF) {
				buffer_index = i;
				buffer_val = x;
			}
			// First filled, second empty
			else if (buffer2_index == 0xFFFFFFFFFFFFFFFF) {
				buffer2_index = buffer_index;
				buffer2_val = buffer_val;
				buffer_index = i;
				buffer_val = x;
			}
			// Both full
			else {
				insert_proper();
				buffer_index = i;
				buffer_val = x;
				buffer2_index = 0xFFFFFFFFFFFFFFFF;
			}
		}

		void insert_proper() {
			if (size_ + 2 > fast_mul(words.size())) {
				words.reserve(words.size() + extra_);
				words.resize(words.size() + extra_, 0);
			}

			auto first_word = fast_div(buffer_index);
			auto second_word = fast_div(buffer2_index);
			auto first_index = fast_mod(buffer_index);
			auto second_index = fast_mod(buffer2_index);
			auto first_word_start_index = fast_mul(first_word);
			auto second_word_start_index = fast_mul(second_word);

			// Apply first insertion, store number falling out
			//if (first_word_start_index < buffer_index) {
			auto first_falling_out = (words[first_word] >> 63) & uint64_t(1);
			uint64_t word = words[first_word];
			uint64_t one_mask = (uint64_t(1) << first_index) - 1;
			uint64_t zero_mask = ~one_mask;
			uint64_t unchanged = word & one_mask;
			word <<= 1;
			words[first_word] = (word & zero_mask) | unchanged;
			//}

			// Apply second insertion, store number falling out
			//if (second_word_start_index < buffer2_index) {
			auto second_falling_out = (words[second_word] >> 63) & uint64_t(1);
			word = words[second_word];
			one_mask = (uint64_t(1) << second_index) - 1;
			zero_mask = ~one_mask;
			unchanged = word & one_mask;
			word <<= 1;
			words[second_word] = (word & zero_mask) | unchanged;
			//}

			// Store to const for compiler auto-vectorization (definetely, maybe)
			auto const words_s = words.size();
			uint64_t falling_out_temp1;
			uint64_t falling_out_temp2;
			auto lesser = std::min(first_word, second_word);
			auto greater = std::max(first_word, second_word);
			for (auto i = lesser; i < words_s; ++i) {
				auto first_bypass = first_word < i;
				auto second_bypass = second_word < i;

				// Passed both
				if (first_bypass && second_bypass) {
					falling_out_temp1 = (words[i] >> 63) & uint64_t(1);
					falling_out_temp2 = (words[i] >> 62) & uint64_t(1);

					words[i] = (words[i] << 2) | (first_falling_out << 1) | second_falling_out;

					first_falling_out = falling_out_temp1;
					second_falling_out = falling_out_temp2;
				}
				// Passed first
				else if (first_bypass) {
					falling_out_temp1 = (words[i] >> 63) & uint64_t(1);

					words[i] = (words[i] << 1) | first_falling_out;

					first_falling_out = falling_out_temp1;
				}
				// Passed second
				else if (second_bypass) {
					falling_out_temp1 = (words[i] >> 63) & uint64_t(1);

					words[i] = (words[i] << 1) | second_falling_out;

					second_falling_out = falling_out_temp1;
				}
				// Must've passed at least one
			}
			set<false>(buffer2_index, buffer2_val);
			set<false>(buffer_index, buffer_val);
			psum_ += buffer_val + buffer2_val;
			size_ += 2;
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
			auto size = size_;
			if (buffer_index != 0xFFFFFFFFFFFFFFFF) ++size;
			if (buffer2_index != 0xFFFFFFFFFFFFFFFF) ++size;
			return size;
		}

		/*
		 * split content of this vector into 2 packed blocks:
		 * Left part remains in this block, right part in the
		 * new returned block
		 */
		packed_vector* split() {
			insert_proper();

			buffer_index = 0xFFFFFFFFFFFFFFFF;
			buffer2_index = 0xFFFFFFFFFFFFFFFF;

			uint64_t tot_words = fast_div(size_) + (fast_mod(size_) != 0);

			assert(tot_words <= words.size());

			uint64_t nr_left_words = tot_words >> 1;

			assert(nr_left_words > 0);
			assert(tot_words - nr_left_words > 0);

			uint64_t nr_left_ints = fast_mul(nr_left_words);

			assert(size_ > nr_left_ints);
			uint64_t nr_right_ints = size_ - nr_left_ints;

			assert(words.begin() + nr_left_words + extra_ < words.end());
			assert(words.begin() + tot_words <= words.end());
			std::vector<uint64_t> right_words(tot_words - nr_left_words + extra_, 0);
			std::copy(words.begin() + nr_left_words, words.begin() + tot_words, right_words.begin());
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
			const auto pos = fast_mod(i);

			words[word_nr] = (words[word_nr] & ~(MASK << pos)) | (uint64_t(x) << pos);
		}

		/*
		 * return total number of bits occupied in memory by this object instance
		 */
		uint64_t bit_size() const
		{
			return (sizeof(packed_vector) + words.capacity() * sizeof(uint64_t)) * 8;
		}

		uint64_t width() const {
			return width_;
		}

		void insert_word(uint64_t i, uint64_t word, uint8_t width, uint8_t n) {
			assert(i <= size());
			assert(n);
			assert(n * width <= sizeof(word) * 8);
			assert(width * n == 64 || (word >> width * n) == 0);

			if (n == 1) {
				// only one integer to insert
				insert(i, word);

			}
			else if (width == 1 && width_ == 1 && n == 64) {
				// insert 64 bits packed into a word
				uint64_t pos = size_ / 64;
				uint8_t offset = size_ - pos * 64;

				if (!offset) {
					words.insert(words.begin() + pos, word);
				}
				else {
					assert(pos + 1 < words.size());

					words.insert(words.begin() + pos, words[pos + 1]);

					words[pos] &= ((1llu << offset) - 1);
					words[pos] |= word << offset;

					words[pos + 1] &= ~((1llu << offset) - 1);
					words[pos + 1] &= word >> (64 - offset);
				}

				size_ += n;
				psum_ += __builtin_popcountll(word);

			}
			else {
				const uint64_t mask = (1llu << width) - 1;
				while (n--) {
					insert(i++, word & mask);
					word >>= width;
				}
			}
		}

	private:
		void shift_right(uint64_t i, uint64_t current_word) {
			assert(i < size());
			//number of integers that fit in a memory word
			assert(int_per_word_ > 0);
			assert(size_ + 1 <= fast_mul(words.size()));

			uint64_t index = fast_mod(i);

			//integer that falls out from the right of current word
			uint64_t falling_out = 0;

			auto val = fast_mul(current_word);
			if (val < i) {
				falling_out = (words[current_word] >> (int_per_word_ - 1) * width_) & uint64_t(1);
				uint64_t word = words[current_word];
				uint64_t one_mask = (uint64_t(1) << index) - 1;
				uint64_t zero_mask = ~one_mask;
				uint64_t unchanged = word & one_mask;
				word <<= 1;
				words[current_word] = (word & zero_mask) | unchanged;
				current_word++;
			}

			//now for the remaining integers we can work blockwise

			uint64_t falling_out_temp;

			//val = fast_div(size_);
			const auto s = words.size();
			for (uint64_t j = current_word; j < s; ++j) {

				assert(j < words.size());

				falling_out_temp = (words[j] >> (int_per_word_ - 1)) & uint64_t(1);

				words[j] = (words[j] << 1) | falling_out;

				//assert(fast_mul(j) >= size_ || !at(fast_mul(j)));

				//set<false>(fast_mul(j), falling_out, j);
				falling_out = falling_out_temp;
			}
		}

		// 	//now for the remaining integers we can work blockwise

		// 	uint64_t falling_out_temp;

		// 	val = fast_div(size_);
		// 	for (uint64_t j = current_word; j <= val; ++j) {

		// 		assert(j < words.size());

		// 		falling_out_temp = (words[j] >> (int_per_word_ - 1)) & uint64_t(1);

		// 		words[j] <<= 1;

		// 		assert(fast_mul(j) >= size_ || !at(fast_mul(j)));

		// 		set<false>(fast_mul(j), falling_out);

		// 		falling_out = falling_out_temp;
		// 	}
		// }

		//shift left of 1 position elements starting
		//from the (i + 1)-st.
		void shift_left(const uint64_t i) {

			//number of integers that fit in a memory word
			assert(int_per_word_ > 0);
			assert(i < size_);

			if (i == (size_ - 1)) {
				set<false>(i, false);
				return;
			}

			uint64_t current_word = fast_div(i);

			//integer that falls in from the right of current word
			uint64_t falling_in_idx;

			if (fast_mul(current_word) < i) {
				falling_in_idx = std::min(fast_mul(current_word + 1), size_ - 1);

				for (uint64_t j = i; j < falling_in_idx; ++j) {
					assert(j + 1 < size_);
					set<false>(j, at(j + 1));
				}

				if (falling_in_idx == size_ - 1) {
					set<false>(size_ - 1, 0);
				}
				current_word++;
			}

			//now for the remaining integers we can work blockwise
			for (uint64_t j = current_word; fast_mul(j) < size_; ++j) {
				words[j] >>= 1;
				const auto fval = fast_mul(j + 1);
				falling_in_idx = fval < size_ ? at(fval) : 0;
				set<false>(fast_mul(j) + int_per_word_ - 1, falling_in_idx);
			}

		}

		uint64_t sum(packed_vector& vec) const {
			uint64_t res = 0;
			for (uint64_t i = 0; i < vec.size(); ++i) {
				res += vec.at(i);
			}
			return res;
		}

		static constexpr uint8_t width_ = 1;
		static constexpr uint8_t int_per_word_ = 64;
		static constexpr uint64_t MASK = 1;
		static constexpr uint8_t extra_ = 2;
		std::vector<uint64_t> words{};
		uint64_t psum_ = 0;
		uint64_t size_ = 0;
		uint64_t buffer_index = 0xFFFFFFFFFFFFFFFF;
		bool buffer_val;
		uint64_t buffer2_index = 0xFFFFFFFFFFFFFFFF;
		bool buffer2_val;
	};

}