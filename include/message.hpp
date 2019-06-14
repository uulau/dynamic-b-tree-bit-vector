#pragma once
#include <cstdint>

namespace dyn {
	enum message_type {
		insert, remove, query, update
	};

	constexpr int64_t message_bitsize() {
		return sizeof(message_type) * 8 + sizeof(uint64_t) * 8 + sizeof(uint64_t) * 8;
	}

	struct message {
		message_type type;
		uint64_t index;
		uint64_t value;
	};

	message& insert_message(int64_t index, int64_t value) {
		auto val = new message();
		val->type = message_type::insert;
		val->index = index;
		val->value = value;
		return *val;
	}

	message& remove_message(int64_t index) {
		auto val = new message();
		val->type = message_type::remove;
		val->index = index;
		val->value = NULL;
		return *val;
	}

	message& query_message(int64_t index) {
		auto val = new message();
		val->type = message_type::query;
		val->index = index;
		val->value = NULL;
		return *val;
	}

	message& update_message(int64_t index, int64_t value) {
		auto val = new message();
		val->type = message_type::update;
		val->index = index;
		val->value = value;
		return *val;
	}
}