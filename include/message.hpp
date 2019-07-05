#pragma once

namespace dyn {
	enum message_type {
		insert = 0, remove = 1, update = 3, rank = 4
	};

	struct message {
		uint64_t data = 0;

		message() = default;

		message(const message& m) {
			data = m.data;
		}

		void set_index(uint64_t val) {
			data = (val & ~0xF000000000000000) | (data & 0xF000000000000000);
		}

		uint64_t get_index() const {
			return data & ~0xF000000000000000;
		}

		void set_val(bool val) {
			data = (data & ~(uint64_t(1) << 63)) | (uint64_t(val) << 63);
		}

		bool get_val() const {
			return data >> 63;
		}

		void set_type(message_type type) {
			data = (data & ~0x6000000000000000) | (static_cast<uint64_t>(type) << 61);
		}

		message_type get_type() const {
			return static_cast<message_type>((data >> 61) & uint64_t(3));
		}

		void set_dirty(bool val) {
			data = (data & ~0x800000000000000) | (uint64_t(val) << 60);
		}

		bool get_dirty() {
			return (data >> 60) & uint64_t(1);
		}
	};

	inline static message const insert_message(uint64_t index, bool value) {
		message m;
		m.set_index(index);
		m.set_val(value);
		m.set_dirty(false);
		m.set_type(message_type::insert);
		return m;
	}

	inline static message const remove_message(uint64_t index, bool val) {
		message m;
		m.set_index(index);
		m.set_val(val);
		m.set_dirty(false);
		m.set_type(message_type::remove);
		return m;
	}

	inline static message const update_message(uint64_t index, bool value) {
		message m;
		m.set_index(index);
		m.set_val(value);
		m.set_dirty(false);
		m.set_type(message_type::update);
		return m;
	}

	inline static message const rank_message(uint64_t index) {
		message m;
		m.set_index(index);
		m.set_dirty(false);
		m.set_type(message_type::rank);
		return m;
	}
}