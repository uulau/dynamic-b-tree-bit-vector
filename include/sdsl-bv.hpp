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
	class sdsl_bv {
	public:
		sdsl_bv() = delete;

		explicit sdsl_bv(uint64_t message_count, uint64_t bucket_w = 2) {
			if (!message_count) {
				throw invalid_argument("Message buffer size must be greater than zero.");
			}

			if (!bucket_w) {
				throw invalid_argument("Bucket size must be greater than zero.");
			}

			if (bucket_w % 2 != 0) {
				throw invalid_argument("Bucket size must be a power of two.");
			}

			bv = bit_vector();
			util::init_support(rs, &bv);
			util::init_support(ss, &bv);
			message_buffer = map<uint64_t, pair<uint64_t, vector<message>>>();
			message_max = message_count;
			bucket_width = bucket_w;
		}

		explicit sdsl_bv(uint32_t B, uint32_t B_LEAF, uint64_t message_count) : sdsl_bv(message_count) {
		}

		~sdsl_bv() = default;

		bool operator[](uint64_t i) const {
			return at(i);
		}

		bool at(uint64_t i) const {
			uint64_t bucket = i / bucket_width;

			auto it = message_buffer.find(bucket);

			if (it != message_buffer.end()) {
				for (const auto& message : it->second.second) {
					if ((bucket * bucket_width) + message.get_index() == i) {
						return message.get_val();
					}
				}
			}

			return bv[i];
		}

		void remove(uint64_t i) {
			add_message(remove_message(i, at(i)));
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

		void insert(uint64_t i, bool x) {
			add_message(insert_message(i, x));
		}

		void set(uint64_t i, bool x) {
			remove(i);
			insert(i, x);
		}

		void push_back(bool x) {
			insert(size(), x);
		}

		uint64_t select(uint64_t i) const {
			uint64_t inserts = 0;

			for (auto const& [bucket, item] : message_buffer) {
				for (auto const& message : item.second) {
					if (message.get_dirty()) {
						continue;
					}

					if (message.get_index() * (bucket * bucket_width) <= i && message.get_val()) {
						++inserts;
					}

					if (inserts == i) {
						return message.get_index() * (bucket * bucket_width) + 1;
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
					if (message.get_index() + (bucket * bucket_width) >= i) {
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
					temp[message.get_index() + (bucket * bucket_width)] = true;
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
					temp[message.get_index() + (bucket * bucket_width)] = message.get_val();
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
			auto bucket = m.get_index() / bucket_width;
			auto index = m.get_index() % bucket_width;

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
					auto increment = curr_mes.get_index() >= copy.get_index() ? message_buffer[bucket].first + 1 : message_buffer[bucket].first;
					curr_mes.set_index(curr_mes.get_index() + increment);

					// Overflow, set index to actual value and readd, mark old as dirty
					if (curr_mes.get_index() >= bucket_width) {
						curr_mes.set_index(curr_mes.get_index() + (bucket * bucket_width));
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

		uint64_t message_max;
		uint64_t bucket_width = 4;
		uint64_t messages = 0;
		map<uint64_t, pair<uint64_t, vector<message>>> message_buffer;
		rank_support_v5<1, 1> rs;
		select_support_mcl<1, 1> ss;
		bit_vector bv;
	};
}