#pragma once

/**
  Math contains various mathematical functions that I find myself using but that are not 
  obvious in std

*/

#include <cmath>
#include <cstdlib>
#include <utility>

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

		static int round_int(double arg) {
			return (int)std::lround(arg);
		}
	}

	namespace geom {

		template<typename T> struct Rect;//forward

		/** A 2D vector - a point
		*/
		template<typename T>
		struct Vec2D{
			T x;
			T y;

			Vec2D():x{ 0 }, y{ 0 } {}
			Vec2D(T x0, T y0) :x{ x0 }, y{ y0 } {}

			///copy constructor
			Vec2D(const Vec2D& b) {
				x = b.x;
				y = b.y;
			}

			///move constructor
			Vec2D(Vec2D&& b) {
				x = b.x;
				y = b.y;
			}

			///copy assignment
			Vec2D& operator=(const Vec2D& b) {

				if(this != &rhs) {
					x = b.x;
					y = b.y;
				}
				return *this;
			}
			///move assignment
			Vec2D& operator=(Vec2D&& b) {

				if(this != &b) {
					x = b.x;
					y = b.y;
				}
				return *this;
			};

			///vector style +=
			Vec2D& operator+=(const Vec2D& b) {
				x += b.x;
				y += b.y;
				return *this;
			};
			
			///vector style -=
			Vec2D& operator-=(const Vec2D& b) {
				x -= b.x;
				y -= b.y;
				return *this;
			};

			///scalar multiplication assignment rhs
			Vec2D& operator*=(T s) {
				x *= s;
				y *= s;
				return *this;
			};

			///scalar division assignment rhs
			Vec2D& operator/=(T s) {
				x /= s;
				y /= s;
				return *this;
			};

			///negative
			Vec2D operator-() const {
				return Vec2d<T>(-x,-y);		 
			};

			///trivial
			Vec2D operator+() const {
				return Vec2d<T>(x,y);
			};

			///vector add
			Vec2D operator+(const Vec2D &b) const {
				return Vec2d<T>(x+b.x,y+b.y);
			};

			///vector subtract
			Vec2D operator-(const Vec2D &b) const {
				return Vec2d<T>(x-b.x, y-b.y);
			};

			///scalar multiply rhs
			Vec2D operator*(T s) const {
				return Vec2d<T>(s*b.x,s*b.y);
			};

			///scalar division rhs
			Vec2D operator/(T s) const {
				return Vec2d<T>(b.x/s,b.y/s);
			};
			
			///vector equality
			bool operator==(const Vec2D &b) const {
				return (x==b.x && y == b.y)
			};

			///vector inequality
			bool operator!=(const Vec2D &b) const {
				return  (x != b.x || y != b.y)
			};	

			bool inside(const Rect<T> &b)const;
			bool outside(const Rect<T> &b)const;
		};

		///scalar multiply lhs
		template <typename T>
		Vec2D<T> operator*(T s, Vec2D<T> const& b) {
			return b * s;
		}

		template<typename T>
		struct Rect {

			Vec2D<T> tl;
			Vec2D<T> br;

			Rect() :tl{0,0},br{0,0} {}
			Rect(T x0, T y0, T w, T h) :tl{ x0,y0 }, br{ x0 + w,y0 + h } {}

			///copy constructor
			Rect(const Rect& b) {
				tl = b.tl;
				br = b.br;
			}

			///move constructor
			Rect(Rect&& b) {
				tl = std::move(b.tl);
				br = std::move(b.br);
			}

			///copy assignment
			Rect& operator=(const Rect& b) {

				if(this != &rhs) {
					tl = b.tl;
					br = b.br;
				}
				return *this;
			}

			///move assignment
			Rect& operator=(Rect&& b) {

				if(this != &b) {
					tl = std::move(b.tl);
					br = std::move(b.br);
				}
				return *this;
			};

			///rectangle equality
			bool operator==(const Rect &b) const {
				return (tl == b.tl && br == b.br)
			};

			///rectangle inequality
			bool operator!=(const Rect &b) const {
				return  (tl != b.tl || br != b.br)
			};

			///this contains b
			bool contains(const Rect &b) {
				return contains(b.tl) && contains(b.br);
			};

			///this is inside of b
			bool inside(const Rect &b) {
				return b.contains(*this);
			};

			///this is outside of b
			bool outside(const Rect &b) {
				return ((b.br.x < tl.x) ||
					(b.tl.x > br.x) ||
					(b.br.y < tl.y) ||
					(b.tl.y > br.y));
			};

			///this overlaps b
			bool overlaps(const Rect &b) {
				return !outside(b) && !inside(b);
			};

			//top left position
			void setX(T x){
				T w = br.x - tl.x;
				tl.x=x; 
				br.x = x + w;
			}
			T x()const { return tl.x; }

			//top left position
			void setY(T y) {
				T h = br.y - tl.y;
				tl.y = y;
				br.y = y + h;
			}
			T y()const { return tl.y; }

			//the width
			T w()const { return br.x - tl.x; }
			void setW(T w) {
				br.x = tl.x+w;
			}

			//the height
			T h()const { return br.y - tl.y; }
			void setH(T h) {
				br.y = tl.y + h;
			}

			T left()const { return tl.x; }
			T right()const { return br.x; }
			T top()const { return tl.y; }
			T bottom()const { return br.y; }

			//the center of this
			Vec2D<T> center()const;

			//this contains b
			bool contains(const Vec2D<T> &b)const;
		};
	
		template <typename T> bool Vec2D<T>::inside(const Rect<T> &b)const {
			return b.contains(*this);
		};

		template <typename T> bool Vec2D<T>::outside(const Rect<T> &b)const {
			return !inside(b);
		};

		template <typename T> Vec2D<T> Rect<T>::center()const { return Vec2D<T>( (tl + br) / 2); }

		template <typename T> bool Rect<T>::contains(const Vec2D<T> &b)const {

			return  ((tl.x <= b.x && b.x <= br.x) && (tl.y <= b.y && b.y <= br.y));
		};

	}
}

