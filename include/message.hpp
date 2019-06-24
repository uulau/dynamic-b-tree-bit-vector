#pragma once

namespace dyn {
	enum message_type {
		insert, remove, query, update
	};

	struct message {
		message_type type;
		uint64_t index;
		bool value;
		bool upsert_subtract;
	};

	const message insert_message(uint64_t index, bool value) {
		return message{ message_type::insert, index, value, false };
	}

	const message remove_message(uint64_t index) {
		return message{ message_type::remove, index, 0, false };
	}

	const message query_message(uint64_t index) {
		return message{ message_type::query, index, 0, false };

	}

	const message update_message(uint64_t index, bool value, bool increment) {
		return message{ message_type::update, index, value, increment };
	}
}