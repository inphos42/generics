#ifndef GENERICS_DELTAENCODING_H
#define GENERICS_DELTAENCODING_H

namespace generics {

	template< typename T >
	inline void deltaUnpack(T * array, int size) {
		for (int d = 1; d < size; ++d)
			array[d] += array[d - 1];
	}

	template< typename T >
	inline void deltaUnpack(const T * from, T * to, int size) {
		to[0] = from[0];
		for (int d = 1; d < size; ++d)
			to[d] = from[d] + to[d - 1];
	}

	template< typename T >
	inline void deltaPack(const T * from, T * to, int size) {
		to[0] = from[0];
		for (int d = 1; d < size; ++d)
			to[d] = from[d] - from[d - 1];
	}

	template< typename T >
	inline void deltaPack(T * array, int size) {
		T prev = array[0], delta;
		for (int d = 1; d < size; ++d) {
			delta = array[d] - prev;
			prev = array[d];
			array[d] = delta;
		}
	}

}

#endif
