#include <stdint.h>

namespace dyn {
	template <class Container> class pv_reference {

	public:

		pv_reference(Container& c, uint64_t idx) : _pv(c), _idx(idx) {}

		operator uint64_t() {
			return _pv.at(_idx);
		}

		pv_reference const& operator=(uint64_t v) const {

			_pv.set(_idx, v);

			return *this;
		}

		pv_reference const& operator=(pv_reference& ref) const {

			_pv.set(_idx, uint64_t(ref));

			return *this;
		}

		//++pv[i]
		pv_reference const& operator++() const {

			_pv.increment(_idx, 1);

			return *this;

		}

		//pv[i]++
		pv_reference const operator++(int) const {

			pv_reference copy(*this);

			++(*this);

			return copy;

		}

		//--pv[i]
		pv_reference const& operator--() const {

			_pv.increment(_idx, 1, true);

			return *this;

		}

		//pv[i]--
		pv_reference const operator--(int) const {

			pv_reference copy(*this);

			--(*this);

			return copy;

		}

		pv_reference const& operator+=(uint64_t d) const {

			_pv.increment(_idx, d);

			return *this;

		}

		pv_reference const& operator-=(uint64_t d) const {

			_pv.increment(_idx, d, true);

			return *this;

		}

	private:

		Container& _pv;
		uint64_t _idx;

	};
}