#pragma once

#include "int_vector.hpp"
#include "rank_support_v5.hpp"
#include "util.hpp"
#include "message.hpp"
#include <vector>
#include "select_support_mcl.hpp"

using namespace sdsl;
using namespace std;
using namespace dyn;

namespace dyn {
	class sdsl_bv {
	public:
		sdsl_bv() = delete;

		explicit sdsl_bv(uint64_t message_count = 128) {
			message_buffer = vector<message>();
			message_buffer.reserve(message_count);
		}

		explicit sdsl_bv(uint32_t B = 0, uint32_t B_LEAF = 0, uint64_t message_count = 128) {
			message_buffer = vector<message>();
			message_buffer.reserve(message_count);
		}

		~sdsl_bv() = default;

		bool operator[](uint64_t i) const {
			return at(i);
		}

		bool at(uint64_t i) const {
			int64_t offset = 0;
			for (auto y = message_buffer.rbegin(); y != message_buffer.rend(); ++y) {
				const auto message = *y;

				if (message.get_index() > i) {
					continue;
				}

				if (message.get_type() == message_type::insert) {
					if (message.get_index() == i) {
						return message.get_val();
					}
					else {
						--offset;
					}
				}
				else {
					++offset;
				}
			}

			return bv[i + offset];
		}

		void remove(uint64_t i) {
			add_message(remove_message(i, at(i)));
		}

		uint64_t size() const {
			return bv.size() + message_buffer.size();
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

		uint64_t select(uint64_t i) {
			return 0;
		}

		uint64_t rank(const uint64_t i) {
			uint64_t sum = 0;
			uint64_t mcount = 0;

			for (auto it = message_buffer.rbegin(); it != message_buffer.rend(); ++it) {
				if (it->get_index() < i) {
					++mcount;
					sum += it->get_val();

					if (mcount == i) {
						break;
					}
				}
				else {
					break;
				}
			}

			if (rs.initialized()) {
				sum += rs.rank(i - mcount);
			}

			return sum;
		}

	private:
		void flush_messages() {
			auto inserts = message_buffer.size();

			auto temp = vector(inserts + bv.size(), -1);

			for (auto& it = message_buffer.rbegin(); it != message_buffer.rend(); ++it) {
				auto index = it->get_index();

				while (temp[index] != -1) {
					++index;
				}

				temp[index] = it->get_val();
			}

			for (auto i = 0; i < bv.size(); ++i) {
				auto index = i;
				while (temp[index] != -1) {
					++index;
				}
				temp[index] = bv[i];
			}

			bv = bit_vector(temp.size());

			for (int i = 0; i < temp.size(); ++i) {
				bv[i] = temp[i];
			}

			util::init_support(ss, &bv);
			util::init_support(rs, &bv);

			message_buffer.clear();
		}

		void add_message(const message& m) {
			assert(!m.get_dirty());

			if (message_buffer.empty()) {
				message_buffer.emplace_back(m);
			}
			else {
				for (auto it = message_buffer.begin(); it != message_buffer.end(); ++it) {
					message& current_message = *it;
					if (current_message.get_index() < m.get_index()) {
						message_buffer.insert(it, m);
						break;
					}
					else {
						it->set_index(it->get_index() + 1);
					}
				}
			}

			if (message_buffer.size() == message_buffer.capacity()) {
				flush_messages();
			}
		}

		select_support_mcl<1, 1> ss;
		rank_support_v5<1, 1> rs;
		bit_vector bv;
		vector<message> message_buffer;
	};
}