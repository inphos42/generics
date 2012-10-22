#ifndef GENERICS_POINT_H
#define GENERICS_POINT_H

#include <cmath>

namespace generics {
	template< class coord_t, int DIMENSIONS >
	inline coord_t manhattanDist(coord_t a[], coord_t b[]) {
		coord_t result = 0;
		for (int d = 0; d < DIMENSIONS; ++d)
			result += a[d] < b[d] ? b[d] - a[d] : a[d] - b[0];
		return result;
	}

	template< class coord_t, int DIMENSIONS, class result_t = double >
	inline result_t euklidDist(coord_t a[], coord_t b[]) {
		result_t result = 0;
		for (int d = 0; d < DIMENSIONS; ++d)
			result += (b[d] - a[d]) * (b[d] - a[d]);
		return ::sqrt(result);
	}

	template< class coord_t, int DIMENSIONS >
	struct Point {
		coord_t * coords;

		inline void swap(Point & other) {
			coord_t * h = coords;
			coords = other.coords;
			other.coords = h;
		}

		inline coord_t & operator[](int index) { return coords[index]; }
		inline const coord_t & operator[](int index) const { return coords[index]; }

		Point & operator=(const Point & other) {
			for (int i = 0 ; i < DIMENSIONS; ++i) coords[i] = other.coords[i];
			return *this;
		}

		template< class o_coord_t >
		Point & operator=(const Point< o_coord_t, DIMENSIONS > & other) {
			for (int i = 0 ; i < DIMENSIONS; ++i) coords[i] = other.coords[i];
			return *this;
		}

		Point & operator=(coord_t value) {
			for (int i = 0 ; i < DIMENSIONS; ++i) coords[i] = value;
			return *this;
		}

		template< typename o_coord_t >
		inline Point & operator+=(const Point< o_coord_t, DIMENSIONS > & other) {
			for (int i = 0 ; i < DIMENSIONS; ++i) coords[i] += other.coords[i];
			return *this;
		}

		template< typename o_coord_t >
		inline Point & operator-=(const Point< o_coord_t, DIMENSIONS > & other) {
			for (int i = 0 ; i < DIMENSIONS; ++i) coords[i] -= other.coords[i];
			return *this;
		}

		inline Point & operator*=(coord_t value) {
			for (int i = 0 ; i < DIMENSIONS; ++i) coords[i] *= value;
			return *this;
		}

		template< typename o_coord_t >
		inline bool operator==(const Point< o_coord_t, DIMENSIONS > & other) {
			for (int i = 0 ; i < DIMENSIONS; ++i) {
				if (other[i] != coords[i])
					return false;
			}
			return true;
		}

		template< typename o_coord_t >
		inline bool operator!=(const Point< o_coord_t, DIMENSIONS > & other) {
			for (int i = 0 ; i < DIMENSIONS; ++i) {
				if (other[i] != coords[i])
					return true;
			}
			return false;
		}

		inline void negate() {
			for (int i = 0 ; i < DIMENSIONS; ++i) coords[i] = -coords[i];
		}

		template< typename o_coord_t >
		inline coord_t manhattanDist(const Point< o_coord_t, DIMENSIONS > & other) const {
			return generics::manhattanDist< coord_t, DIMENSIONS >(coords, other.coords);
		}

		template< typename o_coord_t >
		inline coord_t euklidDist(const Point< o_coord_t, DIMENSIONS > & other) const {
			return generics::euklidDist< coord_t, DIMENSIONS >(coords, other.coords);
		}

		Point() : coords(new coord_t[DIMENSIONS]) {}
		Point(const Point & other) : coords(new coord_t[DIMENSIONS]) {
			for (int i = 0 ; i < DIMENSIONS; ++i) coords[i] = other.coords[i];
		}
		Point(coord_t uniform) : coords(new coord_t[DIMENSIONS]) {
			for (int i = 0 ; i < DIMENSIONS; ++i) coords[i] = uniform;
		}

		~Point() { delete[] coords; }
	};

	template< typename coord_t, int DIMENSIONS, typename b_coord_t = coord_t >
	inline Point< coord_t, DIMENSIONS > operator+(const Point< coord_t, DIMENSIONS > & a, const Point< b_coord_t, DIMENSIONS > & b) {
		Point< coord_t, DIMENSIONS >  result;
		for (int i = 0 ; i < DIMENSIONS; ++i) result[i] = a[i] + b[i];

		return result;
	}

	template< typename coord_t, int DIMENSIONS, typename b_coord_t =coord_t >
	inline Point< coord_t, DIMENSIONS > operator-(const Point< coord_t, DIMENSIONS > & a, const Point< b_coord_t, DIMENSIONS > & b) {
		Point< coord_t, DIMENSIONS > result;
		for (int i = 0 ; i < DIMENSIONS; ++i) result[i] = a[i] - b[i];

		return result;
	}

	template< typename coord_t, int DIMENSIONS, typename b_coord_t  >
	inline Point< coord_t, DIMENSIONS > operator*(const Point< coord_t, DIMENSIONS > & a, b_coord_t b) {
		Point< coord_t, DIMENSIONS >  result;
		for (int i = 0 ; i < DIMENSIONS; ++i) result[i] = a[i] * b;

		return result;
	}

	template< typename coord_t, int DIMENSIONS, typename b_coord_t >
	inline Point< coord_t, DIMENSIONS > operator*(b_coord_t b, const Point< coord_t, DIMENSIONS > & a) {
		return operator*< coord_t, DIMENSIONS, b_coord_t >(a, b);
	}

}

#endif
