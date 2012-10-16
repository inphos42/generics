#ifndef GENERICS_RECT_H
#define GENERICS_RECT_H

#include "point.h"

namespace generics {

	template< class coord_t, int DIMENSIONS, coord_t COORD_MIN, coord_t COORD_MAX >
	struct Rect {
		static constexpr coord_t MBRSIZE = 2 * DIMENSIONS;

		coord_t * bounds; // array of length 2*DIMENSIONS with the semantics of a touple of bounds [a_min .. a_max]

		inline Point< coord_t, DIMENSIONS > lowerBound() const {
			Point< coord_t, DIMENSIONS > result;

			for (int d = 0; d < DIMENSIONS; ++d)
				result[d] = bounds[d * 2];

			return result;
		}

		inline Point< coord_t, DIMENSIONS > upperBound() const {
			Point< coord_t, DIMENSIONS > result;

			for (int d = 0; d < DIMENSIONS; ++d)
				result[d] = bounds[d * 2 + 1];

			return result;
		}

		inline Point< coord_t, DIMENSIONS > center() const {
			Point< coord_t, DIMENSIONS > result;

			for (int d = 0; d < DIMENSIONS; ++d)
				result[d] = (bounds[d * 2 + 1] + bounds[d * 2]) / 2;

			return result;
		}

		inline bool overlapsRawRect(const coord_t * other) const {
			for (int d = 0; d < DIMENSIONS; ++d) {
				if (
					(bounds[d * 2] > other[d * 2 + 1]) || // left(A) > right(B)
					(bounds[d * 2 + 1] < other[d * 2])    // right(A) < left(B)
				)
					return false;
			}

			return true;
		}

		inline bool overlapsRawPoint(const coord_t * other) const {
			for (int d = 0; d < DIMENSIONS; ++d) {
				if (
					(bounds[d * 2] > other[d]) ||  // left(A) > coord(B)
					(bounds[d * 2 + 1] < other[d]) // right(A) < coord(B)
				)
					return false;
			}

			return true;
		}

		inline bool overlaps(const Rect & other) const {
			return overlapsRawRect(other.bounds);
		}

		inline bool overlaps(const Point< coord_t, DIMENSIONS > & point) const {
			return overlapsRawPoint(point.coords);
		}

		inline coord_t edgeLength(int dimension) const {
			coord_t result = bounds[dimension * 2 + 1] + 1 - bounds[dimension * 2];
			return result < 0 ? 0 : result;
		}

		inline coord_t area() const {
			coord_t result = 1;
			for (int d = 0; d < DIMENSIONS; ++d)
				result *= edgeLength(d);

			return result;
		}

		inline coord_t margin() const {
			coord_t result = 0;
			for  (int d = 0; d < DIMENSIONS; ++d)
				result += edgeLength(d);

			return result;
		}

		inline void nullify() {
			for (int d = 0 ; d < DIMENSIONS; ++d) {
				bounds[d * 2] = COORD_MAX;
				bounds[d * 2 + 1] = COORD_MIN;
			}
		}

		inline bool isNull() {
			for (int d = 0 ; d < DIMENSIONS; ++d) {
				if (bounds[d * 2 + 1] < bounds[d * 2])
					return true;
			}
			return false;
		}

		inline void enlarge(const Rect & other) {
			for (int d = 0; d < DIMENSIONS; ++d) {
				bounds[d * 2] = other.bounds[d * 2] < bounds[d * 2] ? other.bounds[d * 2] : bounds[d * 2]; // min(left(a), left(b))

				bounds[d * 2 + 1] = other.bounds[d * 2 + 1] > bounds[d * 2 + 1] ? other.bounds[d * 2 + 1] : bounds[d * 2 + 1]; // max(right(a), right(b))
			}
		}

		inline void enlarge(const Point< coord_t, DIMENSIONS > & other) {
			for (int d = 0; d < DIMENSIONS; ++d) {
				bounds[d * 2] = other[d] < bounds[d * 2] ? other[d] : bounds[d * 2]; // min(left(a), coord)

				bounds[d * 2 + 1] = other[d] > bounds[d * 2 + 1] ? other[d] : bounds[d * 2 + 1]; // max(right(a), coord)
			}
		}

		inline void reduce(const Rect & other) {
			for (int d = 0; d < DIMENSIONS; ++d) {
				bounds[d * 2] = other.bounds[d * 2] > bounds[d * 2] ? other.bounds[d * 2] : bounds[d * 2]; // max(left(a), left(b))

				bounds[d * 2 + 1] = other.bounds[d * 2 + 1] < bounds[d * 2 + 1] ? other.bounds[d * 2 + 1] : bounds[d * 2 + 1]; // min(right(a), right(b))
			}
		}

		inline static void shift(coord_t * rbounds, const Point< coord_t, DIMENSIONS > & vector) {
			for (int d = 0; d < DIMENSIONS; ++d) {
				rbounds[d * 2] += vector[d];
				rbounds[d * 2 + 1] += vector[d];
			}
		}

		inline static void shiftInv(coord_t * rbounds, const Point< coord_t, DIMENSIONS > & vector) {
			for (int d = 0; d < DIMENSIONS; ++d) {
				rbounds[d * 2] -= vector[d];
				rbounds[d * 2 + 1] -= vector[d];
			}
		}

		inline void shift(const Point< coord_t, DIMENSIONS > & vector) { shift(bounds, vector); }
		inline void shiftInv(const Point< coord_t, DIMENSIONS > & vector) { shift(bounds, vector); }

		inline void swap(Rect & other) {
			coord_t * h = bounds;
			bounds = other.bounds;
			other.bounds = h;
		}

		/** WARNING
		 * Size of raw MUST be equal to MBRSIZE otherwise behavior is undefined
		 */
		inline void takeDataFrom(coord_t raw[]) {
			delete[] bounds;
			bounds = raw;
		}

		Rect() : bounds(new coord_t[MBRSIZE]) {}

		Rect(const Rect & other) : bounds(new coord_t[MBRSIZE]) {
			for (int i = 0 ; i < MBRSIZE; ++i) bounds[i] = other.bounds[i];
		}

		explicit Rect(const coord_t * raw) : bounds(new coord_t[MBRSIZE]) {
			for (int i = 0 ; i < MBRSIZE; ++i) bounds[i] = raw[i];
		}

		explicit Rect(const Point< coord_t, DIMENSIONS > & lowerBound, const Point< coord_t, DIMENSIONS > & upperBound) : bounds(new coord_t[MBRSIZE]) {
			for (int i = 0 ; i < DIMENSIONS; ++i) {
				bounds[i * 2] = lowerBound[i];
				bounds[i * 2 + 1] = upperBound[i];
			}
		}

		~Rect() {
			delete[] bounds;
		}

		Rect & operator=(const Rect & other) {
			for (int i = 0 ; i < MBRSIZE; ++i) bounds[i] = other.bounds[i];
			return *this;
		}

		inline coord_t & operator[](int index) { return bounds[index]; }
		inline const coord_t & operator[](int index) const { return bounds[index]; }
	};

}

#endif
