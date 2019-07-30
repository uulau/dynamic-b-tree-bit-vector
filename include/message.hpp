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

		void set_index(const uint64_t val) {
			data = (val & ~0xF000000000000000) | (data & 0xF000000000000000);
			assert(get_index() == val);
		}

		uint64_t get_index() const {
			return data & ~0xF000000000000000;
		}

		void set_val(const bool val) {
			data = (data & ~(uint64_t(1) << 63)) | (uint64_t(val) << 63);
			assert(get_val() == val);
		}

		bool get_val() const {
			return data >> 63;
		}

		void set_type(const message_type type) {
			data = (data & ~0x6000000000000000) | (static_cast<uint64_t>(type) << 61);
			assert(get_type() == type);
		}

		message_type get_type() const {
			return static_cast<message_type>((data >> 61) & uint64_t(3));
		}

		void set_dirty(const bool val) {
			data = (data & ~0x800000000000000) | (uint64_t(val) << 60);
			assert(get_dirty() == val);
		}

		bool get_dirty() const {
			return (data >> 60) & uint64_t(1);
		}
	};

	struct i_buffer {
		uint64_t i0 : 15;
		uint64_t i1 : 15;
		uint64_t i2 : 15;
		uint64_t i3 : 15;
		bool val0 : 1;
		bool val1 : 1;
		bool val2 : 1;
		bool val3 : 1;
	};

	static message insert_message(const uint64_t index, const bool value)
	{
		message m;
		m.set_index(index);
		m.set_val(value);
		m.set_dirty(false);
		m.set_type(insert);
		assert(m.get_index() == index);
		assert(m.get_val() == value);
		assert(m.get_dirty() == false);
		assert(m.get_type() == message_type::insert);
		return m;
	}

	static message remove_message(const uint64_t index, const bool val)
	{
		message m;
		m.set_index(index);
		m.set_val(val);
		m.set_dirty(false);
		m.set_type(remove);
		assert(m.get_index() == index);
		assert(m.get_val() == val);
		assert(m.get_dirty() == false);
		assert(m.get_type() == remove);
		return m;
	}

	static message update_message(const uint64_t index, const bool value)
	{
		message m;
		m.set_index(index);
		m.set_val(value);
		m.set_dirty(false);
		m.set_type(update);
		assert(m.get_index() == index);
		assert(m.get_val() == value);
		assert(m.get_dirty() == false);
		assert(m.get_type() == update);
		return m;
	}

	static message rank_message(const uint64_t index)
	{
		message m;
		m.set_index(index);
		m.set_dirty(false);
		m.set_type(rank);
		assert(m.get_index() == index);
		assert(m.get_dirty() == false);
		assert(m.get_type() == rank);
		return m;
	}
}