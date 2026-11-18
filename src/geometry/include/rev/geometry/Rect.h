/*
 * Rect.h — shared rectangle type for revcmi (extracted from VCMI)
 *
 * License: GNU General Public License v2.0 or later
 */
#pragma once

#include "Point.h"

#include <algorithm>

namespace rev::geometry
{

/// Rectangle class, which have a position and a size
class Rect
{
public:
	int x;
	int y;
	int w;
	int h;

	Rect()
	{
		x = y = w = h = -1;
	}
	Rect(int X, int Y, int W, int H)
	{
		x = X;
		y = Y;
		w = W;
		h = H;
	}
	Rect(const Point & position, const Point & size)
	{
		x = position.x;
		y = position.y;
		w = size.x;
		h = size.y;
	}
	Rect(const Rect& r) = default;

	static Rect createCentered( const Point & around, const Point & dimensions )
	{
		return Rect(around - dimensions/2, dimensions);
	}

	static Rect createCentered( const Rect & rect, const Point & dimensions )
	{
		return createCentered(rect.center(), dimensions);
	}

	static Rect createAround(const Rect &r, int width)
	{
		return Rect(r.x - width, r.y - width, r.w + width * 2, r.h + width * 2);
	}

	bool isInside(int qx, int qy) const
	{
		if (qx > x && qx<x+w && qy>y && qy<y+h)
			return true;
		return false;
	}
	bool isInside(const Point & q) const
	{
		return isInside(q.x,q.y);
	}
	int top() const
	{
		return y;
	}
	int bottom() const
	{
		return y+h;
	}
	int left() const
	{
		return x;
	}
	int right() const
	{
		return x+w;
	}

	Point topLeft() const
	{
		return Point(x,y);
	}
	Point topRight() const
	{
		return Point(x+w,y);
	}
	Point bottomLeft() const
	{
		return Point(x,y+h);
	}
	Point bottomRight() const
	{
		return Point(x+w,y+h);
	}
	Point center() const
	{
		return Point(x+w/2,y+h/2);
	}
	Point dimensions() const
	{
		return Point(w,h);
	}

	Rect resize(const int size) const
	{
		return Rect(x-size,y-size,w+2*size,h+2*size);
	}

	void moveTo(const Point & dest)
	{
		x = dest.x;
		y = dest.y;
	}

	Rect operator+(const Point &p) const
	{
		return Rect(x+p.x,y+p.y,w,h);
	}

	Rect operator-(const Point &p) const
	{
		return Rect(x-p.x,y-p.y,w,h);
	}

	template<typename T>
	Rect operator*(const T &mul) const
	{
		return Rect(x*mul, y*mul, w*mul, h*mul);
	}


	Rect& operator=(const Rect &p)
	{
		x = p.x;
		y = p.y;
		w = p.w;
		h = p.h;
		return *this;
	}

	Rect& operator+=(const Point &p)
	{
		x += p.x;
		y += p.y;
		return *this;
	}

	Rect& operator-=(const Point &p)
	{
		x -= p.x;
		y -= p.y;
		return *this;
	}

	bool operator == (const Rect & other)
	{
		return x == other.x && y == other.y && w == other.w && h == other.h;
	}

	/// returns distance from this rect to point, or 0 if inside
	int distanceTo(const Point & target) const
	{
		int distanceX = std::max({left() - target.x, 0, target.x - right()});
		int distanceY = std::max({top() - target.y, 0, target.y - bottom()});

		return Point(distanceX, distanceY).length();
	}

	/// returns true if this rect intersects with another rect
	bool intersectionTest(const Rect & other) const
	{
		// this rect is above other rect
		if(this->bottom() < other.top())
			return false;

		// this rect is below other rect
		if(this->top() > other.bottom() )
			return false;

		// this rect is to the left of other rect
		if(this->right() < other.left())
			return false;

		// this rect is to the right of other rect
		if(this->left() > other.right())
			return false;

		return true;
	}

	/// returns true if this rect intersects with line specified by two points
	/// Algorithm to test whether line segment between points line1-line2 will intersect with
	/// rectangle specified by top-left and bottom-right points
	/// Note that in order to avoid floating point rounding errors algorithm uses integers with no divisions
	bool intersectionTest(const Point & line1, const Point & line2) const
	{
		// check whether segment is located to the left of our rect
		if (line1.x < left() && line2.x < left())
			return false;

		// check whether segment is located to the right of our rect
		if (line1.x > right() && line2.x > right())
			return false;

		// check whether segment is located on top of our rect
		if (line1.y < top() && line2.y < top())
			return false;

		// check whether segment is located below of our rect
		if (line1.y > bottom() && line2.y > bottom())
			return false;

		Point vector { line2.x - line1.x, line2.y - line1.y};

		// compute position of corners relative to our line
		int tlTest = vector.y*topLeft().x - vector.x*topLeft().y + (line2.x*line1.y-line1.x*line2.y);
		int trTest = vector.y*bottomRight().x - vector.x*topLeft().y + (line2.x*line1.y-line1.x*line2.y);
		int blTest = vector.y*topLeft().x - vector.x*bottomRight().y + (line2.x*line1.y-line1.x*line2.y);
		int brTest = vector.y*bottomRight().x - vector.x*bottomRight().y + (line2.x*line1.y-line1.x*line2.y);

		// if all points are on the left of our line then there is no intersection
		if ( tlTest > 0 && trTest > 0 && blTest > 0 && brTest > 0 )
			return false;

		// if all points are on the right of our line then there is no intersection
		if ( tlTest < 0 && trTest < 0 && blTest < 0 && brTest < 0 )
			return false;

		// if all previous checks failed, this means that there is an intersection between line and AABB
		return true;
	}

	/// Returns rect that represents intersection of two rects
	Rect intersect(const Rect & other) const
	{
		if(intersectionTest(other))
		{
			Point topLeft{
				std::max(this->left(), other.left()),
				std::max(this->top(), other.top())
			};

			Point bottomRight{
				std::min(this->right(), other.right()),
				std::min(this->bottom(), other.bottom())
			};

			return Rect(topLeft, bottomRight - topLeft);
		}
		else
		{
			return Rect();
		}
	}

	/// Returns rect union - rect that covers both this rect and provided rect
	Rect include(const Rect & other) const
	{
		Point topLeft{
			std::min(this->left(), other.left()),
			std::min(this->top(), other.top())
		};

		Point bottomRight{
			std::max(this->right(), other.right()),
			std::max(this->bottom(), other.bottom())
		};

		return Rect(topLeft, bottomRight - topLeft);
	}

	template <typename Handler>
	void serialize(Handler &h)
	{
		h & x;
		h & y;
		h & w;
		h & this->h;
	}
};

}
