#pragma once

/**
  Math contains various mathematical functions that I find myself using but that are not 
  obvious in std

*/

#include <cmath>
#include <cstdlib>

namespace evil {

	namespace math {

		///random integer generator
		static int rand_int(int min, int max) {
			return min + (std::rand() % (int)(max - min + 1));
		}

		///random boolean generator
		static bool rand_bool() {
			return !!(std::rand() % 2);
		}

	}
}