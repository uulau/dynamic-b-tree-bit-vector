#pragma once

#include "int_vector.hpp"
#include "rank_support_v5.hpp"
#include "select_support_mcl.hpp"
#include "util.hpp"
#include "message.hpp"
#include <vector>

using namespace sdsl;
using namespace std;
using namespace dyn;

namespace dyn {
	template<
		uint64_t bucket_width
	>
		class sdsl_bv {
		static_assert(bucket_width > 2);
		static_assert((bucket_width& (bucket_width - 1)) == 0);

		public:
			sdsl_bv() = delete;

			explicit sdsl_bv(const uint64_t message_count) {
				if (message_count <= 0) {
					throw invalid_argument("Buffer size limit must be a positive integer.");
				}

				bv = bit_vector();
				util::init_support(rs, &bv);
				util::init_support(ss, &bv);
				message_buffer = map<uint64_t, pair<uint64_t, vector<message>>>();
				message_max = message_count;
			}

			explicit sdsl_bv(uint32_t B, uint32_t B_LEAF, uint64_t message_count) : sdsl_bv(message_count) {
			}

			~sdsl_bv() = default;

			bool operator[](uint64_t i) const {
				return at(i);
			}

			bool at(uint64_t i) const {
				const auto bucket = bucket_div(i);

				auto it = message_buffer.find(bucket);

				if (it != message_buffer.end()) {
					for (const auto& message : it->second.second) {
						if (bucket_mul(bucket) + message.get_index() == i) {
							return message.get_val();
						}
					}
				}

				return bv[i];
			}

			void remove(const uint64_t i) {
				add_message(remove_message(i, at(i)));
			}

			void remove(const uint64_t i, const uint64_t val) {
				add_message(remove_message(i, val));
			}

			uint64_t size() const {
				uint64_t counter = 0;

				for (auto const& [index, val] : message_buffer) {
					for (auto const& message : val.second) {
						if (message.get_dirty()) {
							continue;
						}

						if (message.get_type() == message_type::insert) {
							++counter;
						}
						else if (message.get_type() == message_type::remove) {
							--counter;
						}
					}
				}

				return bv.size() + counter;
			}

			void insert(const uint64_t i, const bool x) {
				add_message(insert_message(i, x));
			}

			void set(const uint64_t i, const bool x) {
				const auto cur_val = at(i);

				if (cur_val == x)
				{
					return;
				}

				remove(i, x);
				insert(i, x);
			}

			void push_back(const bool x) {
				insert(size(), x);
			}

			uint64_t select(const uint64_t i) const {
				uint64_t inserts = 0;
				uint64_t highest_insert = 0;

				const auto initialized = bv.size() > 0;

				const auto original = initialized ? ss.select(i) : 0;

				for (auto const& [bucket, item] : message_buffer) {
					for (auto const& message : item.second) {
						if (message.get_dirty()) {
							continue;
						}

						const auto index = bucket ? message.get_index() * bucket_mul(bucket) : message.get_index();

						if ((!initialized || index <= original)
							&& message.get_val()) {
							++inserts;
							if (index > highest_insert)
							{
								highest_insert = index;
							}
						}

						if (inserts == i) {
							return highest_insert;
						}
					}
				}
				return ss.select(i - inserts);
			}

			uint64_t rank(const uint64_t i) const {
				// Total insertions
				uint64_t insertions = 0;
				// Insertions of 1's
				uint64_t sum = 0;

				for (auto const& [bucket, item] : message_buffer) {
					for (auto const& message : item.second) {
						if (message.get_dirty()) {
							continue;
						}

						// Won't affect rank anymore
						if (message.get_index() + bucket_mul(bucket) >= i) {
							continue;
						}

						++insertions;
						sum += message.get_val();

						// Enough insertions to answer query
						if (insertions == i) {
							return sum;
						}
					}
				}
				return rs.rank(i - insertions) + sum;
			}

		private:
			void flush_messages() {
				bit_vector temp(size());

				// Mark insertion points
				for (auto const& [bucket, item] : message_buffer) {
					for (auto const& message : item.second) {
						if (message.get_dirty()) {
							continue;
						}
						temp[message.get_index() + bucket_mul(bucket)] = true;
					}
				}

				// Loop through current bitvector, incrementing indices which have message insertions before them
				uint64_t last_insertion = 0;
				uint64_t i = 0;
				for (const auto& val : bv) {
					if (!temp[i]) {
						temp[i] = val;
						last_insertion = i;
					}
					else {
						auto counter = i + 1;
						while (temp[counter] != 0 || counter <= last_insertion) {
							++counter;
						}
						temp[counter] = val;
						last_insertion = counter;
					}
					++i;
				}

				// Set actual message values
				for (auto const& [bucket, item] : message_buffer) {
					for (auto const& message : item.second) {
						if (message.get_dirty()) {
							continue;
						}
						temp[message.get_index() + bucket_mul(bucket)] = message.get_val();
					}
				}

				// Clear buffer and set message count to 0
				message_buffer.clear();
				messages = 0;

				bv = bit_vector(std::move(temp));

				// Create static support structures for the new bit vector
				util::init_support(rs, &bv);
				util::init_support(ss, &bv);
			}

			void add_message(const message& m) {
				// Calculate bucket and index inside bucket
				auto bucket = bucket_div(m.get_index());
				const auto index = bucket_mod(m.get_index());

				// Copy for modifying
				message copy = m;

				// Set index to value inside bucket
				copy.set_index(index);

				// Try to find bucket
				auto it = message_buffer.find(bucket);

				if (it != message_buffer.end()) {
					// Bucket found, increment existing message indices by modifier and push greater indices
					// than current message to the right by one
					for (auto& curr_mes : message_buffer[bucket].second) {
						const auto increment = curr_mes.get_index() >= copy.get_index() ? message_buffer[bucket].first + 1 : message_buffer[bucket].first;
						curr_mes.set_index(curr_mes.get_index() + increment);

						// Overflow, set index to actual value and readd, mark old as dirty
						if (curr_mes.get_index() >= bucket_width) {
							curr_mes.set_index(curr_mes.get_index() + bucket_mul(bucket));
							auto new_mes = curr_mes;
							// Will create new buckets, if needed
							add_message(new_mes);
							curr_mes.set_dirty(true);
						}

						// Increment higher bucket modifiers
						for (auto& [index, val] : message_buffer) {
							if (index > bucket) {
								++(val.first);
							}
						}
					}
					// Clear modifier
					message_buffer[bucket].first = 0;
					// Add message to bucket
					it->second.second.push_back(copy);
				}
				else {
					// Message not found, create new bucket and a modifier
					message_buffer.insert({ bucket, { 0, { copy } } });
				}

				// Increment message count
				++messages;

				// Flush if buffer full
				if (messages == message_max) {
					flush_messages();
				}
			}

			static constexpr uint64_t ilog2(uint64_t n) {
				uint64_t i = 0;
				for (uint64_t k = sizeof(uint64_t) * CHAR_BIT; 0 < (k /= 2);) {
					if (n >= uint64_t(1) << k)
					{
						i += k;
						n >>= k;
					}
				}
				return i;
			}

			static constexpr uint64_t bucket_log = ilog2(bucket_width);

			static constexpr uint64_t bucket_mul(uint64_t num) {
				return num << bucket_log;
			}

			static constexpr uint64_t bucket_div(uint64_t num) {
				return num >> bucket_log;
			}

			static constexpr uint64_t bucket_mod(uint64_t num) {
				return num & (bucket_width - 1);
			}

			uint64_t messages = 0;
			uint64_t message_max;
			map<uint64_t, pair<uint64_t, vector<message>>> message_buffer;
			rank_support_v5<1, 1> rs;
			select_support_mcl<1, 1> ss;
			bit_vector bv;
	};
}