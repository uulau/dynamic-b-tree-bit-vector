#pragma once

namespace dyn {
	enum message_type {
		insert, remove, query, update
	};

	struct message {
		message_type type;
		uint64_t index;
		uint64_t value;
		bool upsert_increment;
	};

	const message insert_message(uint64_t index, uint64_t value) {
		return message{ message_type::insert , index, value };

	}

	const message remove_message(uint64_t index) {
		return message{ message_type::remove , index };
	}

	const message query_message(uint64_t index) {
		return message{ message_type::query , index };

	}

	const message update_message(uint64_t index, uint64_t value, uint64_t, bool increment) {
		return message{ message_type::update, index, value, increment };
	}
}