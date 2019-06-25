#pragma once

namespace dyn {
	enum message_type {
		insert, remove, query, update
	};

	struct message {
		message_type type;
		uint64_t index;
		bool value;
	};

	const message insert_message(uint64_t index, bool value) {
		return message{ message_type::insert, index, value };
	}

	const message remove_message(uint64_t index) {
		return message{ message_type::remove, index, 0 };
	}

	const message query_message(uint64_t index) {
		return message{ message_type::query, index, 0 };
	}

	const message update_message(uint64_t index, bool value) {
		return message{ message_type::update, index, value };
	}
}