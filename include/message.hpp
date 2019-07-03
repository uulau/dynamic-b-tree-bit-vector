#pragma once

namespace dyn {
	enum message_type {
		insert, remove, query, update, rank, select
	};

	struct message {
		uint64_t index;
		message_type type;
		bool value;
	};

	static message const insert_message(uint64_t index, bool value) {
		return message{ index, message_type::insert, value };
	}

	static message const remove_message(uint64_t index) {
		return message{ index, message_type::remove, 0 };
	}

	static message const query_message(uint64_t index) {
		return message{ index, message_type::query, 0 };
	}

	static message const update_message(uint64_t index, bool value) {
		return message{ index, message_type::update, value };
	}

	static message const rank_message(uint64_t index) {
		return message{ index, message_type::rank, 0 };
	}

	static message const select_message(uint64_t index) {
		return message{ index, message_type::select, 0 };
	}
}