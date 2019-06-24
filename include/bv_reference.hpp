#pragma once

template <class Container> class bv_reference {

public:

	bv_reference(Container& c, uint64_t idx) : _bv(c), _idx(idx) {}

	operator bool() {
		return _bv.at(_idx);
	}

	bv_reference const& operator=(bool v) const {

		_bv.set(_idx, v);

		return *this;
	}

	bv_reference const& operator=(bv_reference& ref) const {

		_bv.set(_idx, bool(ref));

		return *this;
	}

private:

	Container& _bv;
	uint64_t _idx;

};