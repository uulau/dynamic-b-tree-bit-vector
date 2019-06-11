#pragma once

namespace dyn {
	template <class Container> class spsi_reference {

	public:
		spsi_reference(Container& c, uint64_t idx) : _spsi(c), _idx(idx) {}

		operator uint64_t() {
			return _spsi.at(_idx);
		}

		spsi_reference const& operator=(uint64_t v) const {

			_spsi.set(_idx, v);

			return *this;
		}

		spsi_reference const& operator=(spsi_reference& ref) const {

			_spsi.set(_idx, uint64_t(ref));

			return *this;
		}

		//++spsi[i]
		spsi_reference const& operator++() const {

			_spsi.increment(_idx, 1);

			return *this;

		}

		//spsi[i]++
		spsi_reference const operator++(int) const {

			spsi_reference copy(*this);

			++(*this);

			return copy;

		}

		//--spsi[i]
		spsi_reference const& operator--() const {

			_spsi.decrement(_idx, 1);

			return *this;

		}

		//spsi[i]--
		spsi_reference const operator--(int) const {

			spsi_reference copy(*this);

			--(*this);

			return copy;

		}

		spsi_reference const& operator+=(uint64_t d) const {

			_spsi.increment(_idx, d);

			return *this;

		}

		spsi_reference const& operator-=(uint64_t d) const {

			_spsi.decrement(_idx, d);

			return *this;

		}

	private:
		Container& _spsi;
		uint64_t _idx;
	};
}