#pragma once

#include "int_vector.hpp"
#include "rank_support_v5.hpp"
#include "util.hpp"
#include "message.hpp"
#include <vector>
#include "select_support_mcl.hpp"

using namespace sdsl;
using namespace std;

namespace dyn {
	class sdsl_bv {
	public:
		sdsl_bv() = delete;

		explicit sdsl_bv(uint64_t message_count = 128) {
			message_buffer = vector<message>();
			message_buffer.reserve(message_count);
		}

		explicit sdsl_bv(uint32_t B = 0, uint32_t B_LEAF = 0, uint64_t message_count = 128) {
			std::ignore = B;
			std::ignore = B_LEAF;
			message_buffer = vector<message>();
			message_buffer.reserve(message_count);
		}

		~sdsl_bv() = default;

		bool operator[](uint64_t i) const {
			return at(i);
		}

		bool at(uint64_t i) const {
			uint64_t original{ i };
			uint64_t value{ 0 };
			bool updated = false;
			for (const auto& message : message_buffer) {
				if (message.index <= i) {
					if (message.type == message_type::insert) {
						if (message.index == original) {
							value = message.value;
							updated = true;
						}
						else {
							return (*this)[i - 1];
						}
					}
					else if (message.type == message_type::remove) {
						return (*this)[i + 1];
					}
					else if (message.type == message_type::update) {
						if (message.index == original) {
							value = message.value;
							updated = true;
						}
					}
				}
			}

			if (updated) {
				return value;
			}

			return bv[i];
		}

		void remove(uint64_t i) {
			--message_count;
			add_message(remove_message(i));
		}

		uint64_t size() const {
			return message_count + bv.size();
		}

		void insert(uint64_t i, bool x) {
			++message_count;
			add_message(insert_message(i, x));
		}

		void set(uint64_t i, bool x) {
			add_message(update_message(i, x));
		}

		void push_back(bool x) {
			insert(size(), x);
		}

		uint64_t select(uint64_t i) {
			if (!ss.initialized() || message_buffer.size() > 0) {
				flush_messages();
				util::init_support(ss, &bv);
			}

			return ss(i + 1);
		}

		uint64_t rank(uint64_t i) {
			// TODO

			throw;
		}

	private:
		void flush_messages() {
			vector<char> vals;
			vals.reserve(bv.size() + message_count);

			for (const auto& x : bv) {
				vals.emplace_back(x);
			}

			for (const auto& message : message_buffer) {
				if (message.type == message_type::insert) {
					--message_count;
					vals.insert(vals.begin() + message.index, message.value);
				}
				else if (message.type == message_type::remove) {
					++message_count;
					vals.erase(vals.begin() + message.index);
				}
				else {
					vals[message.index] = message.value;
				}
			}

			bv = bit_vector(vals.size());

			for (auto i = 0; i < bv.size(); i++) {
				bv[i] = vals[i];
			}
			message_buffer.clear();
			util::init_support(rs, &bv);
			util::init_support(ss, &bv);
		}

		void update_messages(message& m)
		{
			// TODO
			throw;
		};

		void add_message(message message) {
			update_messages(message);

			message_buffer.emplace_back(message);

			if (message_buffer.size() == message_buffer.capacity()) {
				flush_messages();
			}
		}

		uint64_t message_count = 0;
		select_support_mcl<1, 1> ss;
		rank_support_v5<1, 1> rs;
		bit_vector bv;
		vector<message> message_buffer;
	};

}