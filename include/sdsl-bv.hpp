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

				if (message.get_dirty() || message.get_index() > i) {
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
			uint64_t message_count = 0;

			for (const auto& message : message_buffer) {
				switch (message.get_type()) {
					if (message.get_dirty()) {
						continue;
					}

				case message_type::insert: {
					++message_count;
					break;
				}
				case message_type::remove: {
					--message_count;
					break;
				}
				}
			}
			return message_count + bv.size();
		}

		template <message_type mtype> uint64_t messages() {
			uint64_t message_count = 0;

			for (const auto& message : message_buffer) {
				if (message.get_dirty()) {
					continue;
				}

				if constexpr (mtype == message_type::insert) {
					if (message.get_type() == message_type::insert) {
						++message_count;
					}
				}
				else {
					if (message.get_type() == message_type::remove) {
						++message_count;
					}
				}
			}

			return message_count;
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
			if (!ss.initialized() || message_buffer.size() > 0) {
				flush_messages();
				util::init_support(ss, &bv);
			}

			return ss(i + 1);
		}

		uint64_t rank(const uint64_t i) {
			auto static_rank = rs.initialized() ? rs.rank(i) : 0;

			return static_rank;
		}

	private:
		void flush_messages() {
			auto inserts = messages<message_type::insert>();
			vector<char> vals;
			vals.reserve(bv.size() + inserts);

			for (const auto& x : bv) {
				vals.emplace_back(x);
			}

			for (auto begin = message_buffer.rbegin(); begin != message_buffer.rend(); begin++) {
				auto message = *begin;

				if (message.get_dirty()) {
					continue;
				}

				if (message.get_index() >= vals.size()) {
					vals.resize(vals.size() + (message.get_index() + 1 - vals.size()), -1);
				}

				switch (message.get_type()) {
				case message_type::insert: {
					if (vals[message.get_index()] == -1) {
						vals[message.get_index()] = message.get_val();
					}
					else {
						vals.insert(vals.begin() + message.get_index(), message.get_val());
					}
					break;
				}

				case message_type::remove: {
					vals.erase(vals.begin() + message.get_index());
					break;
				}
				default: throw;
				}
			}

			message_buffer.clear();

			bv = bit_vector(vals.size());

			for (auto i = 0; i < bv.size(); i++) {
				assert(vals[i] != -1);
				bv[i] = vals[i];
			}

			util::init_support(rs, &bv);
			util::init_support(ss, &bv);
		}

		void add_message(const message& m) {
			assert(!m.get_dirty());

			message_buffer.emplace_back(m);

			if (message_buffer.size() > 1) {
				message& latest_message = message_buffer[message_buffer.size() - 1];
				auto latest_index = latest_message.get_index();

				for (auto y = message_buffer.rbegin() + 1; y != message_buffer.rend(); ++y)
				{
					message& current_message = *y;

					if (current_message.get_dirty()) {
						continue;
					}

					switch (latest_message.get_type())
					{
					case message_type::insert:
					{
						if (latest_message.get_index() > current_message.get_index()) {
							latest_message.set_index(latest_message.get_index() - 1);
						}
						else if (latest_message.get_index() < current_message.get_index()) {
							current_message.set_index(current_message.get_index() + 1);
						}
						break;
					}

					case message_type::remove:
					{
						switch (current_message.get_type()) {
						case message_type::insert: {
							if (latest_message.get_index() == current_message.get_index()) {
								current_message.set_dirty(true);
								latest_message.set_dirty(true);
							}
							else if (latest_message.get_index() < current_message.get_index()) {
								current_message.set_index(current_message.get_index() - 1);
							}
							break;
						}
						case message_type::remove: {
							if (latest_message.get_index() > current_message.get_index()) {
								latest_message.set_index(latest_message.get_index() + 1);
							}
							else if (latest_message.get_index() < current_message.get_index()) {
								current_message.set_index(current_message.get_index() - 1);
							}
							break;
						}
						}
						break;
					}
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