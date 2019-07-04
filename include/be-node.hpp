#pragma once

#include "message.hpp"
#include <vector>
#include <iostream>

namespace dyn {
	template <class leaf_type> class be_node {
	public:
		explicit be_node(const be_node& n) {
			message_buffer = vector<message>(n.message_buffer);

			subtree_sizes = { n.subtree_sizes };
			subtree_psums = { n.subtree_psums };

			if (n.has_leaves_) {

				leaves = vector<leaf_type*>(n.nr_children, NULL);

				for (uint64_t i = 0; i < n.nr_children; ++i) {

					leaves[i] = new leaf_type(*n.leaves[i]);

				}

			}
			else {

				children = vector<be_node*>(n.nr_children, NULL);

				for (uint64_t i = 0; i < n.nr_children; ++i) {

					children[i] = new be_node(*n.children[i]);
					children[i]->overwrite_parent(this);

				}

			}

			rank_ = n.rank_; 		//rank of this node among its siblings

			nr_children = n.nr_children; 	//number of subtrees

			has_leaves_ = n.has_leaves_;		//if true, leaves array is nonempty && children is empty

			message_buffer = n.message_buffer;

			this->B = n.B;

			this->B_LEAF = n.B_LEAF;
		}

		/*
		 * create new root node. This node has only 1 (empty) child, which is a leaf.
		 */
		explicit be_node(uint32_t B, uint32_t B_LEAF, uint64_t message_count) {
			init_message_buffer(message_count);

			subtree_sizes = vector<uint64_t>(2 * B + 2);
			subtree_psums = vector<uint64_t>(2 * B + 2);

			nr_children = 1;
			has_leaves_ = true;

			leaves = vector<leaf_type*>(1);
			leaves[0] = new leaf_type();

			this->B = B;
			this->B_LEAF = B_LEAF;
		}

		/*
		 * create new node given some children (other internal nodes),the parent, && the rank of this
		 * node among its siblings
		 */
		explicit be_node(uint32_t B, uint32_t B_LEAF, std::vector<be_node*>& c, int64_t message_count, be_node* P = NULL, uint32_t rank = 0) {

			this->rank_ = rank;
			this->parent = P;

			init_message_buffer(message_count);

			subtree_sizes = vector<uint64_t>(2 * B + 2);
			subtree_psums = vector<uint64_t>(2 * B + 2);

			uint64_t si = 0;
			uint64_t ps = 0;

			assert(c.size() <= 2 * B + 2);

			for (uint32_t i = 0; i < c.size(); ++i) {

				si += c[i]->size();
				ps += c[i]->psum();

				subtree_sizes[i] = si;
				subtree_psums[i] = ps;

			}

			nr_children = c.size();
			has_leaves_ = false;

			children = vector<be_node*>(c);

			uint32_t r = 0;
			for (auto cc : children) {

				cc->overwrite_rank(r++);
				cc->overwrite_parent(this);

			}
			this->B = B;
			this->B_LEAF = B_LEAF;
		}

		/*
		 * create new node given some children (leaves),the parent, && the rank of this
		 * node among its siblings
		 */
		explicit be_node(uint32_t B, uint32_t B_LEAF, std::vector<leaf_type*>& c, int64_t message_count, be_node* P = NULL, uint32_t rank = 0) {

			this->rank_ = rank;
			this->parent = P;

			subtree_sizes = vector<uint64_t>(2 * B + 2);
			subtree_psums = vector<uint64_t>(2 * B + 2);

			assert(c.size() <= 2 * B + 2);

			uint64_t si = 0;
			uint64_t ps = 0;

			for (uint32_t i = 0; i < c.size(); ++i) {

				si += c[i]->size();
				ps += c[i]->psum();

				subtree_sizes[i] = si;
				subtree_psums[i] = ps;

			}

			nr_children = c.size();
			has_leaves_ = true;

			init_message_buffer(message_count);

			leaves = vector<leaf_type*>(c);
			this->B = B;
			this->B_LEAF = B_LEAF;
		}

		void free_mem() {
			if (has_leaves()) {
				free_leaves();
			}
			else {
				free_children();
			}
		}

		/*
		 * return i-th integer in the subtree rooted in this node
		 */
		bool at(uint64_t const i, bool updated = false, bool val = false) {
			for (const auto& message : message_buffer) {
				if (message.get_index() == i) {
					if (message.get_type() != message_type::remove) {
						updated = true;
						val = message.get_val();
						continue;
					}
					else {
						return at(i + 1, false, false);
					}
				}

				if (message.get_index() < i) {
					if (message.get_type() == message_type::insert) {
						return at(i - 1, false, false);
					}
					else if (message.get_type() == message_type::remove) {
						return at(i + 1, false, false);
					}
				}
			}

			if (has_leaves() && updated) {
				return val;
			}

			assert(i < size());

			uint32_t j = subtree_size_bound<true>(i);

			uint64_t previous_size = (j == 0 ? 0 : subtree_sizes[j - 1]);

			assert(i >= previous_size);

			if (has_leaves()) {
				assert(j < leaves.size());
				assert(j < nr_children);
				assert(leaves[j] != NULL);
				assert(i - previous_size < leaves[j]->size());

				return leaves[j]->at(i - previous_size);
			}

			//else: recurse on children
			return children[j]->at(i - previous_size, updated, val);
		}

		/*
		 * returns sum up to i-th integer included
		 */
		uint64_t psum(uint64_t i) {

			assert(i < size());

			uint32_t j = subtree_size_bound<true>(i);

			//size/psum stored in previous counter
			uint64_t previous_size = (j == 0 ? 0 : subtree_sizes[j - 1]);
			uint64_t previous_psum = (j == 0 ? 0 : subtree_psums[j - 1]);

			assert(i >= previous_size);

			//i-th element is in the j-th children

			//if children are leaves, extract psum from j-th leaf
			if (has_leaves()) {

				return previous_psum + leaves[j]->psum(i - previous_size);

			}

			//else: recurse on children
			return previous_psum + children[j]->psum(i - previous_size);

		}

		/*
		 * returns smallest i such that I_0 + ... + I_i >= x
		 */
		uint64_t search(uint64_t x) {

			assert(x <= psum());

			uint32_t j = 0;

			while (subtree_psums[j] < x || (subtree_psums[j] == 0 && x == 0)) {

				j++;
				assert(j < subtree_psums.size());

			}

			//size/psum stored in previous counter
			uint64_t previous_size = (j == 0 ? 0 : subtree_sizes[j - 1]);
			uint64_t previous_psum = (j == 0 ? 0 : subtree_psums[j - 1]);

			assert(x > previous_psum || (previous_psum == 0 && x == 0));

			//i-th element is in the j-th children

			//if children are leaves, extract psum from j-th leaf
			if (has_leaves()) {

				return previous_size + leaves[j]->search(x - previous_psum);

			}

			//else: recurse on children
			return previous_size + children[j]->search(x - previous_psum);

		}

		/*
		 * This function corresponds to select_0 on bitvectors,
		 * && works only if all integers are 0 || 1 (causes a failed
		 * assertion otherwise!)
		 *
		 * returns smallest i such that the number of zeros before position
		 * i (included) is == x. x must be > 0
		 */
		uint64_t search_0(uint64_t x) {

			assert(x <= size() - psum());
			assert(x > 0);

			uint32_t j = 0;

			while ((subtree_sizes[j] - subtree_psums[j]) < x) {

				j++;
				assert(j < subtree_psums.size());

			}

			//size/psum stored in previous counter
			uint64_t previous_size = (j == 0 ? 0 : subtree_sizes[j - 1]);
			uint64_t previous_psum = (j == 0 ? 0 : subtree_psums[j - 1]);
			uint64_t previous_zeros = previous_size - previous_psum;

			assert(x > previous_zeros);

			//i-th element is in the j-th children

			if (has_leaves()) {

				return previous_size + leaves[j]->search_0(x - previous_zeros);

			}

			//else: recurse on children
			return previous_size + children[j]->search_0(x - previous_zeros);

		}

		void increment(uint64_t i, bool delta) {
			create_message(update_message(i, delta));
		}

		be_node* insert(const uint64_t i, bool x) {
			return create_message(insert_message(i, x));
		}

		be_node* remove(uint64_t i) {
			return create_message(remove_message(i));
		}

		uint64_t size() {
			return subtree_sizes[nr_children - 1] + size_total;
		}

		uint64_t bit_size() const {
			uint64_t bs = 8 * sizeof(b_node);

			bs += subtree_sizes.capacity() * sizeof(uint64_t) * 8;

			bs += subtree_psums.capacity() * sizeof(uint64_t) * 8;

			bs += children.capacity() * sizeof(b_node*) * 8;

			bs += leaves.capacity() * sizeof(leaf_type*) * 8;

			bs += message_buffer.size() * sizeof(message) * 8;

			bs += 2 * sizeof(uint64_t) * 8;

			if (has_leaves()) {

				for (ulint i = 0; i < nr_children; ++i) {
					assert(leaves[i] != NULL);
					bs += leaves[i]->bit_size();

				}

			}
			else {

				for (ulint i = 0; i < nr_children; ++i) {

					assert(children[i] != NULL);
					bs += children[i]->bit_size();

				}

			}

			return bs;

		}

	private:
		uint32_t B;

		uint32_t B_LEAF;

		be_node() = delete;

		uint64_t psum() {
			return subtree_psums[nr_children - 1] + sum_total;
		}

		uint32_t rank() { return rank_; }

		void overwrite_rank(uint32_t r) { rank_ = r; }

		uint64_t size_no_messages() {
			assert(nr_children > 0);
			assert(nr_children - 1 < subtree_sizes.size());

			return subtree_sizes[nr_children - 1];
		}

		const be_node* get_parent() const {
			return parent;
		}

		bool message_buffer_is_full() const
		{
			return message_buffer.size() == message_buffer.capacity();
		}

		void increment_rank() {
			rank_++;
		}

		bool has_leaves() const {
			return has_leaves_;
		}

		bool is_root() const {
			return parent == nullptr;
		}

		bool is_full() const
		{
			assert(nr_children <= 2 * B + 2);
			return nr_children == (2 * B + 2);
		}

		/*
		* increment || decrement i-th integer by delta
		*/
		void increment_item(uint64_t i, bool delta) {
			assert(i < size());

			uint32_t j = subtree_size_bound<true>(i);

			//size stored in previous counter
			uint64_t previous_size = (j == 0 ? 0 : subtree_sizes[j - 1]);

			assert(i >= previous_size);

			//i-th element is in the j-th children

			//if children are leaves, increment in the j-th leaf
			if (has_leaves()) {

				assert(j < nr_children);
				assert(j < leaves.size());
				assert(leaves[j] != NULL);
				leaves[j]->increment(i - previous_size, delta);

			}
			else {

				//else: recurse on children
				assert(j < nr_children);
				assert(j < children.size());
				assert(children[j] != NULL);
				children[j]->increment(i - previous_size, delta);

			}

			//after the increment, modify psum counters
			for (uint32_t k = j; k < nr_children; ++k) {

				//check for under/overflows
				assert(delta || (subtree_psums[k] <= (~uint64_t(0)) - delta));
				//assert((!delta) || (delta <= subtree_psums[k]));

				subtree_psums[k] = (delta ? subtree_psums[k] + delta : subtree_psums[k] - delta);

			}
		}

		/*
		* true iff this node can lose
		* a child && remain above the min.
		* number of children, B + 1
		* || this node is the root
		*/
		bool can_lose() const {
			return (nr_children >= (B + 2) || (is_root()));
		}

		bool leaf_can_lose(leaf_type* leaf) const {
			return (leaf->size() >= (B_LEAF + 1));
		}

		be_node* insert_item(const uint64_t i, bool x) {
			assert(i <= size());
			assert(is_root() || !parent->is_full());

			be_node* new_root = nullptr;
			be_node* right = nullptr;

			if (is_full()) {

				right = split();

				assert(!is_full());
				assert(!right->is_full());

				//insert recursively
				if (i < size()) {

					insert_without_split(i, x);

				}
				else {

					assert(right != NULL);
					right->insert_without_split(i - size(), x);

				}

				//if this is the root, create new root
				if (is_root()) {

					vector<be_node*> vn{ this, right };

					new_root = new be_node(B, B_LEAF, vn, message_buffer.capacity());
					assert(!new_root->is_full());

					this->overwrite_parent(new_root);
					right->overwrite_parent(new_root);

				}
				else {

					//else: pass the info to parent
					assert(rank() < parent->number_of_children());
					parent->new_children(rank(), this, right);

					be_node* n = parent;

					while (n && n->is_full()) {
						auto r = n->split();
						if (n->is_root()) {
							vector<be_node*> vn{ n, r };
							new_root = new be_node(B, B_LEAF, vn, message_buffer.capacity());
							assert(!new_root->is_full());
							n->overwrite_parent(new_root);
							r->overwrite_parent(new_root);
							break;
						}
						else {
							assert(n->rank() < n->parent->number_of_children());
							n->parent->new_children(n->rank(), n, r);
						}
						n = n->parent;
					}
				}

			}
			else {

				insert_without_split(i, x);

			}

			//if !root, do !return anything.
			return new_root;
		}

		void free_leaves() {
			for (uint32_t i = 0; i < nr_children; ++i) {
				delete leaves[i];
			}
		}

		void free_children() {
			for (uint32_t i = 0; i < nr_children; ++i) {
				children[i]->free_mem();
			}
			for (uint32_t i = 0; i < nr_children; ++i) {
				delete children[i];
			}
		}

		/*
		 * remove the integer at position i.
		* If the root changes, return the new root.
		*
		* new root could be different than current root if current root
		* has only one non-leaf child.
		*
		*/
		be_node* remove_item(uint64_t i) {
			assert(i < size_no_messages());
			//assert(is_root() || parent->can_lose());

			be_node* p = this;

			while (p->parent) {
				auto rank = p->rank();
				p = p->parent;
				uint64_t sum = 0;
				for (uint64_t a = 0; a < rank; a++) {
					sum += p->subtree_sizes[a];
				}
				i += sum;
				if (!p->can_lose()) {
					p->ensure_can_lose(i);
				}
			}

			be_node* x = this;

			if (!x->can_lose()) {
				ensure_can_lose(i);
			}

			//At this point, x can afford to lose a child.
			//Find the child from which to remove
			assert(x->can_lose());
			assert(this->can_lose());
			assert(i < this->size_no_messages());

			uint32_t j = subtree_size_bound<true>(i);

			//size stored in previous counter
			uint64_t previous_size = (j == 0 ? 0 : subtree_sizes[j - 1]);

			i = i - previous_size;
			assert(i >= 0);
			remove_leaf_index(i, j);
			be_node* new_root = nullptr;

			if (!p->has_leaves()) {
				//if root has only one child, make that child the root
				if (p->nr_children == 1) {
					new_root = p->children[0];
					new_root->parent = nullptr;
				}
			}
			return new_root;
		}

		be_node* create_message(const message& m) {
			be_node* new_root = nullptr;

			if (!has_leaves()) {
				new_root = add_message(m);
			}
			else {
				new_root = add_to_leaf(m);
			}

			if (!new_root) {
				update_counters<true>();

			}
			else {
				new_root->update_counters();
			}

			return new_root;
		}

		template <bool upper> uint32_t subtree_size_bound(uint32_t i) {
			uint32_t j = 0;

			if constexpr (upper) {
				while (subtree_sizes[j] <= i) {

					j++;
					assert(j < subtree_sizes.size());

				}
			}
			else {
				while (subtree_sizes[j] < i) {

					j++;
					assert(j < subtree_sizes.size());

				}
			}

			return j;
		}

		void init_message_buffer(uint64_t message_count) {
			message_buffer = vector<message>();
			message_buffer.reserve(message_count);
		}

		void overwrite_parent(be_node* P) {
			parent = P;
		}

		uint32_t number_of_children() {
			return nr_children;
		}

		be_node* add_to_leaf(const message& m) {
			be_node* new_root = nullptr;
			switch (m.get_type())
			{
			case message_type::insert:
			{
				new_root = insert_item(m.get_index(), m.get_val());
				break;
			}
			case message_type::remove:
			{
				new_root = remove_item(m.get_index());
				break;
			}
			case message_type::update:
			{
				increment_item(m.get_index(), m.get_val());
				break;
			}
			default: throw;
			}
			return new_root;
		}

		be_node* flush_messages() {
			be_node* new_root = nullptr;
			while (!message_buffer.empty()) {
				auto message = message_buffer[0];

				uint32_t j = message.get_type() == message_type::insert
					? subtree_size_bound<false>(message.get_index())
					: subtree_size_bound<true>(message.get_index());

				auto previous_size = (j == 0 ? 0 : subtree_sizes[j - 1]);

				auto i = message.get_index() - previous_size;

				erase_message(message_buffer[0]);

				auto messages = message_buffer.size();

				message.set_index(i);

				new_root = children[j]->create_message(message);

				if (messages != message_buffer.size()) {
					break;
				}
			}
			return new_root;
		}

		be_node* add_message(const message m) {
			switch (m.get_type()) {
			case message_type::insert: {
				size_total++;
				sum_total += m.get_val();
				message_buffer.emplace_back(m);
				break;
			}
			case message_type::remove: {
				size_total--;
				sum_total -= m.get_val();
				message_buffer.emplace_back(m);
				break;
			}
			case message_type::update: {
				m.get_val() ? sum_total++ : sum_total--;
				message_buffer.emplace_back(m);
				break;
			}
			default: throw;
			}

			if (message_buffer_is_full()) {
				return flush_messages();
			}

			return nullptr;
		}

		void erase_message(const message& m) {
			switch (m.get_type()) {
			case message_type::insert: {
				size_total--;
				sum_total -= m.get_val();
				message_buffer.erase(message_buffer.begin());
				break;
			}
			case message_type::remove: {
				size_total++;
				sum_total += m.get_val();
				message_buffer.erase(message_buffer.begin());
				break;
			}
			case message_type::update: {
				m.get_val() ? sum_total-- : sum_total++;
				message_buffer.erase(message_buffer.begin());
				break;
			}
			default: throw;
			}
		}

		template <bool single = false> void update_counters() {
			uint64_t ps = 0;
			uint64_t si = 0;

			for (uint32_t k = 0; k < nr_children; ++k) {

				if (has_leaves()) {

					assert(leaves[k] != NULL);
					ps += leaves[k]->psum();
					si += leaves[k]->size();

				}
				else {

					assert(children[k] != NULL);
					ps += children[k]->psum();
					si += children[k]->size();
					if constexpr (!single) {
						children[k]->update_counters();
					}
				}

				subtree_psums[k] = ps;
				subtree_sizes[k] = si;

			}
		}

		/*
		 * new element between elements i && i+1
		 */
		void new_children(uint32_t i, be_node* left, be_node* right) {

			assert(i < nr_children);
			assert(!is_full()); 		//this node must !be full!
			assert(!has_leaves()); 	//this procedure can be called only on nodes
			//whise children are !leaves

			//size/psum stored in previous counter
			uint64_t previous_size = (i == 0 ? 0 : subtree_sizes[i - 1]);
			uint64_t previous_psum = (i == 0 ? 0 : subtree_psums[i - 1]);

			//first of all, move forward counters i+1, i+2, ...
			for (uint32_t j = subtree_sizes.size() - 1; j > i; j--) {

				//node is !full so overwriting subtree_sizes[subtree_sizes.size()-1] is safe
				subtree_sizes[j] = subtree_sizes[j - 1];
				subtree_psums[j] = subtree_psums[j - 1];

			}

			subtree_sizes[i] = previous_size + left->size();
			subtree_psums[i] = previous_psum + left->psum();

			//number of children increases by 1
			nr_children++;

			//temporary copy children
			vector<be_node*> temp(children);

			//reset children
			children = vector<be_node*>(nr_children);
			uint32_t k = 0;//index in children

			for (uint32_t j = 0; j < nr_children - 1; ++j) {

				if (i == j) {

					//insert left && right, ignore child i
					assert(k < nr_children);
					children[k++] = left;
					assert(k < nr_children);
					children[k++] = right;

				}
				else {

					//insert child i
					assert(k < nr_children);
					assert(temp[j] != NULL);
					children[k++] = temp[j];

				}

			}

			assert(k == nr_children);

			//children i && i+1 are new; we have now to increase the rank
			//of children i+2,...

			for (uint32_t j = i + 2; j < nr_children; j++) {

				children[j]->increment_rank();
				assert(children[j]->rank() < nr_children);

			}

		}

		void new_children(uint32_t i, leaf_type* left, leaf_type* right) {

			assert(i < nr_children);
			assert(!is_full()); //this node must !be full!
			assert(has_leaves());

			//treat this case separately
			if (nr_children == 1) {

				subtree_sizes[0] = left->size();
				subtree_sizes[1] = left->size() + right->size();

				subtree_psums[0] = left->psum();
				subtree_psums[1] = left->psum() + right->psum();

				leaves = vector<leaf_type*>{ left, right };

				nr_children++;

				return;

			}

			//size/psum stored in previous counter
			uint64_t previous_size = (i == 0 ? 0 : subtree_sizes[i - 1]);
			uint64_t previous_psum = (i == 0 ? 0 : subtree_psums[i - 1]);

			//first of all, move forward counters i+1, i+2, ...
			for (uint32_t j = nr_children; j > i; j--) {

				//node is !full so overwriting subtree_sizes[subtree_sizes.size()-1] is safe
				subtree_sizes[j] = subtree_sizes[j - 1];
				subtree_psums[j] = subtree_psums[j - 1];

			}

			subtree_sizes[i] = previous_size + left->size();
			subtree_psums[i] = previous_psum + left->psum();

			//number of children increases by 1
			nr_children++;

			//temporary copy leaves
			vector<leaf_type*> temp(leaves);

			//reset leaves
			leaves = vector<leaf_type*>(nr_children);
			uint32_t k = 0;//index in leaves

			for (uint32_t j = 0; j < nr_children - 1; ++j) {
				if (i == j) {

					//insert left && right, ignore child i
					assert(k < nr_children);
					leaves[k++] = left;
					assert(k < nr_children);
					leaves[k++] = right;

				}
				else {

					//insert child i
					assert(k < nr_children);
					leaves[k++] = temp[j];

				}

			}

		}

		/*
		 * insert in a node, where we know that this node is !full
		 */
		void insert_without_split(uint64_t i, uint64_t x) {

			assert(!is_full());
			assert(i <= size());

			uint32_t j = nr_children - 1;

			//if i==size, then insert in last children
			if (i < size()) {

				j = 0;

				while (subtree_sizes[j] <= i) {

					j++;
					assert(j < subtree_sizes.size());

				}

			}

			//size stored in previous counter
			uint64_t previous_size = (j == 0 ? 0 : subtree_sizes[j - 1]);

			assert(i >= previous_size);

			//i-th element is in the j-th children
			uint64_t insert_pos = i - previous_size;

			if (!has_leaves()) {

				assert(!is_full());
				assert(insert_pos <= children[j]->size());
				assert(children[j]->get_parent() == this);
				children[j]->insert(insert_pos, x);

			}
			else {

				if (leaf_is_full(leaves[j])) {

					//if leaf full, split it

					leaf_type* right = leaves[j]->split();
					leaf_type* left = leaves[j];

					assert(!leaf_is_full(leaves[j]));

					//insert new children in this node
					this->new_children(j, left, right);

					//insert in the correct leaf half
					if (insert_pos < left->size()) {

						left->insert(insert_pos, x);

					}
					else {

						right->insert(insert_pos - left->size(), x);

					}

				}
				else {

					leaves[j]->insert(insert_pos, x);

				}

			}

			/*
			 * we inserted an integer in some children, && number of
			 * children may have increased. re-compute counters
			 */
			assert(!has_leaves() || nr_children <= leaves.size());
			assert(has_leaves() || nr_children <= children.size());
			assert(nr_children <= subtree_psums.size());
			assert(nr_children <= subtree_sizes.size());
			update_counters();
		}

		/*
		 * splits this (full) node into 2 nodes with B keys each.
		 * The left node is this node, && we return the right node
		 */
		be_node* split() {

			assert(nr_children == 2 * B + 2);

			be_node* right = nullptr;

			if (has_leaves()) {

				vector<leaf_type*> right_children_l(nr_children - nr_children / 2);

				uint64_t k = 0;

				for (uint32_t i = nr_children / 2; i < nr_children; ++i) {
					right_children_l[k++] = leaves[i];
				}

				assert(k == right_children_l.size());

				right = new be_node(B, B_LEAF, right_children_l, message_buffer.capacity(), parent, rank() + 1);
				leaves.erase(leaves.begin() + nr_children / 2, leaves.end());

			}
			else {

				vector<be_node*> right_children_n(nr_children - nr_children / 2);

				uint64_t k = 0;

				for (uint32_t i = nr_children / 2; i < nr_children; ++i) {
					right_children_n[k++] = children[i];
				}

				assert(k == right_children_n.size());

				right = new be_node(B, B_LEAF, right_children_n, message_buffer.capacity(), parent, rank() + 1);

				children.erase(children.begin() + nr_children / 2, children.end());

			}

			nr_children = nr_children / 2;

			auto count = message_buffer.size();
			vector<message> mb(move(message_buffer));
			message_buffer.reserve(count);
			size_total = 0;
			sum_total = 0;

			for (auto& message : mb) {
				if (message.get_type() == message_type::insert) {
					if (message.get_index() <= size()) {
						add_message(message);
					}
					else {
						message.set_index(message.get_index() - size());
						right->add_message(message);
					}
				}
				else {
					if (message.get_index() < size()) {
						add_message(message);
					}
					else {
						message.set_index(message.get_index() - size());
						right->add_message(message);
					}
				}

			}

			return right;
		}

		bool leaf_is_full(leaf_type* l) {

			assert(l->size() <= 2 * B_LEAF);
			return (l->size() == 2 * B_LEAF);

		}

		/*
		* in the following 2 vectors, the first nr_subtrees+1 elements refer to the
		* nr_subtrees subtrees
		*/
		std::vector<uint64_t> subtree_sizes;
		std::vector<uint64_t> subtree_psums;

		std::vector<message> message_buffer;

		std::vector<be_node*> children;
		std::vector<leaf_type*> leaves;

		be_node* parent = nullptr; 		//NULL for root
		uint32_t rank_ = 0; 		//rank of this node among its siblings

		uint32_t nr_children = 0; 	//number of subtrees

		bool has_leaves_ = false;	//if true, leaves array is nonempty && children is empty

		int64_t size_total = 0;
		int64_t sum_total = 0;

		void remove_leaf_index(uint64_t i, uint64_t j) {
			assert(this->leaves.size() == nr_children);
			assert(this->can_lose());

			//remove from the leaf directly, ensuring
			//it remains of size at least B_LEAF
			leaf_type* x = this->leaves[j];
			if (!(leaf_can_lose(x) || (this->leaves.size() == 1))) {
				//Need to ensure that x
				//can lose a child && still have
				//the min # of children

				leaf_type* y; //an adjacent sibling of x
				bool y_is_prev; //is y the previous sibling?
				assert(this->leaves.size() > 0);
				if (j > 0) {
					y = this->leaves[j - 1];
					y_is_prev = true;
				}
				else {
					assert(j + 1 < this->leaves.size());
					y = this->leaves[j + 1];
					y_is_prev = false;
				}

				if (leaf_can_lose(y)) {
					//steal a child of y,
					//and give it to x
					uint64_t z; //the child 
					if (y_is_prev) {
						//y is the previous sibling of x
						z = y->at(y->size() - 1);

						//update y
						y->remove(y->size() - 1);

						//update x
						x->insert(0, z);

						i = i + 1; //update the removal position to reflect x's new child z

						//update x->parent subtree info
						this->subtree_sizes[j - 1] -= 1;
						this->subtree_psums[j - 1] -= z;


						assert(this->subtree_sizes[j] == this->subtree_sizes[j - 1] + x->size());
						assert(this->subtree_psums[j] == this->subtree_psums[j - 1] + x->psum());

					}
					else {
						//y is the next sibling of x
						z = y->at(0);

						//update y
						y->remove(0);

						//update x
						x->insert(x->size(), z);

						//update x->parent subtree info
						this->subtree_sizes[j] += 1;
						this->subtree_psums[j] += z;

						assert(this->subtree_sizes[j + 1] == this->subtree_sizes[j] + y->size());
						assert(this->subtree_psums[j + 1] == this->subtree_psums[j] + y->psum());
					}
				}
				else {
					//y can!lose a child
					//means: neither x nor y can lose a child
					//so: merge x,y into single node of size
					//2B_LEAF 

					if (y_is_prev) {
						for (size_t ii = 0; ii < y->size(); ++ii) {
							x->insert(0, y->at(y->size() - 1 - ii));
						}

						assert(x->size() == 2 * B_LEAF);

						--j; //update the location of x in this's list of children
						//update removal position to be wrt yx (if y is prev)
						i = i + y->size();
					}
					else {
						for (size_t ii = 0; ii < y->size(); ++ii) {
							x->insert(x->size(), y->at(ii));
						}
					}
					//update parent (i.e. this)
					--(this->nr_children);

					for (size_t t = j; t < this->nr_children; ++t) {
						this->subtree_sizes[t] = this->subtree_sizes[t + 1];
						this->subtree_psums[t] = this->subtree_psums[t + 1];
					}

					if (y_is_prev) {
						assert(0 <= j);
						assert(j < this->leaves.size());
						this->leaves.erase(this->leaves.begin() + j);
					}
					else {
						assert(0 <= j + 1);
						assert(j + 1 < this->leaves.size());
						this->leaves.erase(this->leaves.begin() + j + 1);
					}

				}
			} //end if !x->can_lose()

			//the leaf x is large enough for safe removal
			//i has been computed wrt x

			assert(i < x->size());

			uint64_t z = x->at(i);
			x->remove(i);

			//update satellite data
			//requires traversal back up to root

			while (j < this->nr_children) {
				--subtree_sizes[j];
				subtree_psums[j] -= z;
				++j;
			}

			be_node* tmp_parent = this->parent;
			be_node* tmp_child = this;
			while (tmp_parent != NULL) {
				uint32_t r = tmp_child->rank_;
				uint32_t nc = tmp_parent->nr_children;
				while (r < nc) {
					--(tmp_parent->subtree_sizes[r]);

					tmp_parent->subtree_psums[r] -= z;
					++r;
				}

				tmp_child = tmp_parent;
				tmp_parent = tmp_child->parent;
			}
		}

		void ensure_can_lose(uint64_t i) {
			auto x = this;
			//Need to ensure that *x
			//can lose a child && still have
			//the min # of children

			if (x != x->parent->children[x->rank()]) {
				uint32_t real_r = 0;
				while (real_r < x->parent->nr_children) {
					if (x->parent->children[real_r] == x) {
						cerr << real_r << ' ' << x->rank() << endl;
						cerr << x->has_leaves() << endl;
						break;
					}
					++real_r;
				}
			}
			assert(x == x->parent->children[x->rank()]);
			be_node* y; //an adjacent sibling of x
			bool y_is_prev; //is y the previous sibling?
			if (rank() > 0) {
				y = x->parent->children[rank() - 1];
				y_is_prev = true;
			}
			else {
				y = x->parent->children[rank() + 1];
				y_is_prev = false;
			}

			if (y->can_lose()) {
				if (!x->has_leaves()) {
					//steal a child of y,
					//and give it to x
					be_node* z; //the child
					if (y_is_prev) {
						//y is the previous sibling of x
						z = y->children.back();

						assert(z->rank() == (y->nr_children - 1));
						//update z
						z->overwrite_parent(x);
						z->overwrite_rank(0);

						//update y
						--(y->nr_children);
						(y->children).pop_back();

						//update x
						++(x->nr_children);
						(x->children).insert((x->children).begin(), z);

						i = i + z->size(); //update the removal position to reflect x's new child z

						uint64_t si = 0;
						uint64_t ps = 0;
						for (size_t j = 0; j < x->nr_children; ++j) {
							si += (x->children)[j]->size();
							ps += (x->children)[j]->psum();

							(x->subtree_sizes)[j] = si;
							(x->subtree_psums)[j] = ps;
						}

						//update ranks of x's children
						uint32_t r = 0;
						for (auto cc : x->children) {
							cc->overwrite_rank(r);
							++r;
						}

						//update x->parent subtree info

						x->parent->subtree_sizes[x->rank() - 1] -= z->size();
						x->parent->subtree_psums[x->rank() - 1] -= z->psum();


					}
					else {
						//y is the next sibling of x
						z = y->children.front();

						assert(z->rank() == 0);
						//update z
						z->overwrite_parent(x);
						z->overwrite_rank(x->nr_children);

						//update y
						--(y->nr_children);
						(y->children).erase(y->children.begin());
						uint64_t si = 0;
						uint64_t ps = 0;
						for (size_t j = 0; j < y->nr_children; ++j) {
							si += (y->children)[j]->size();
							ps += (y->children)[j]->psum();

							(y->subtree_sizes)[j] = si;
							(y->subtree_psums)[j] = ps;
						}
						//update ranks of y's children
						uint32_t r = 0;
						for (auto cc : y->children) {
							cc->overwrite_rank(r);
							++r;
						}

						//update x
						++(x->nr_children);
						(x->children).insert((x->children).end(), z);
						x->subtree_sizes[x->nr_children - 1] = x->subtree_sizes[x->nr_children - 2] + z->size();
						x->subtree_psums[x->nr_children - 1] = x->subtree_psums[x->nr_children - 2] + z->psum();


						//update x->parent subtree info
						x->parent->subtree_sizes[x->rank()] += z->size();
						x->parent->subtree_psums[x->rank()] += z->psum();
					}
				}
				else {     //x has leaves
				//steal a child of y,
				//and give it to x
					leaf_type* z; //the child
					if (y_is_prev) {
						//y is the previous sibling of x
						z = y->leaves.back();

						//update y
						--(y->nr_children);
						(y->leaves).pop_back();

						//update x
						++(x->nr_children);
						(x->leaves).insert((x->leaves).begin(), z);

						i = i + z->size(); //update the removal position to reflect x's new child z

						uint64_t si = 0;
						uint64_t ps = 0;
						for (size_t j = 0; j < x->nr_children; ++j) {
							si += (x->leaves)[j]->size();
							ps += (x->leaves)[j]->psum();

							(x->subtree_sizes)[j] = si;
							(x->subtree_psums)[j] = ps;
						}

						//update x->parent subtree info
						x->parent->subtree_sizes[x->rank() - 1] -= z->size();
						x->parent->subtree_psums[x->rank() - 1] -= z->psum();


					}
					else {
						//y is the next sibling of x
						z = y->leaves.front();

						//update y
						--(y->nr_children);
						(y->leaves).erase(y->leaves.begin());
						uint64_t si = 0;
						uint64_t ps = 0;
						for (size_t j = 0; j < y->nr_children; ++j) {
							si += (y->leaves)[j]->size();
							ps += (y->leaves)[j]->psum();

							(y->subtree_sizes)[j] = si;
							(y->subtree_psums)[j] = ps;
						}

						//update x
						++(x->nr_children);
						(x->leaves).insert((x->leaves).end(), z);
						x->subtree_sizes[x->nr_children - 1] = x->subtree_sizes[x->nr_children - 2] + z->size();
						x->subtree_psums[x->nr_children - 1] = x->subtree_psums[x->nr_children - 2] + z->psum();

						x->parent->subtree_sizes[x->rank()] += z->size();
						x->parent->subtree_psums[x->rank()] += z->psum();
					}
				}
			}
			else {
				//y can!lose a child
				//means: neither x nor y can lose a child
				//so: merge x,y into single node of size
				//2B + 2
				assert(x->nr_children == B + 1);
				assert(y->nr_children == B + 1);
				be_node* prev;
				be_node* next;
				if (y_is_prev) {
					prev = y;
					next = x;
				}
				else {
					prev = x;
					next = y;
				}
				be_node* xy;
				if (!x->has_leaves()) {
					vector< be_node* > cc(prev->children.begin(), prev->children.end());
					cc.insert(cc.end(), next->children.begin(), next->children.end());

					assert(cc.size() == 2 * B + 2);
					xy = new be_node(B, B_LEAF, cc, message_buffer.capacity(), prev->parent, prev->rank());
				}
				else {
					assert(prev->nr_children == prev->leaves.size());
					assert(next->nr_children == next->leaves.size());
					vector< leaf_type* > cc(prev->leaves.begin(), prev->leaves.end());
					cc.insert(cc.end(), next->leaves.begin(), next->leaves.end());

					if (cc.size() > 2 * B + 2) {
						cerr << endl;
						cerr << prev->nr_children << ' ' << prev->leaves.size() << endl;
						cerr << next->nr_children << ' ' << next->leaves.size() << endl;
						cerr << cc.size() << ' ' << 2 * B + 2 << endl;
					}

					assert(cc.size() == 2 * B + 2);
					xy = new be_node(B, B_LEAF, cc, message_buffer.capacity(), prev->parent, prev->rank());
				}

				//update xy->parent
				if (xy->parent != NULL) {
					--(xy->parent->nr_children);
					xy->parent->children.erase(xy->parent->children.begin() + xy->rank() + 1);
					xy->parent->children[xy->rank()] = x; //x will be overwritten by xy
					for (size_t j = xy->rank(); j < xy->parent->nr_children; ++j) {
						xy->parent->subtree_sizes[j] = xy->parent->subtree_sizes[j + 1];
						xy->parent->subtree_psums[j] = xy->parent->subtree_psums[j + 1];
						xy->parent->children[j]->overwrite_rank(j);
					}
				}

				//update removal position to be wrt yx (if y is prev)
				if (y == prev) {
					i = i + y->size();
				}

				//overwrite x to have xy's data
				x->subtree_sizes = xy->subtree_sizes;
				x->subtree_psums = xy->subtree_psums;
				x->children = xy->children;
				x->leaves = xy->leaves;
				x->rank_ = xy->rank_;
				x->nr_children = xy->nr_children;
				x->has_leaves_ = xy->has_leaves_;

				uint32_t r = 0;
				if (!x->has_leaves()) {
					for (auto cc : x->children) {
						cc->overwrite_parent(x);
						cc->overwrite_rank(r++);
					}
				}

				delete xy;
				//y has been merged into x, so needs to be de-allocated.
				delete y;
			}
		}
	};
}