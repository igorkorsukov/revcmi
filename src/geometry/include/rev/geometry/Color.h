/*
 * Color.h — shared RGBA color type for revcmi (extracted from VCMI)
 *
 * License: GNU General Public License v2.0 or later
 */
#pragma once

#include <cstdint>

namespace rev::geometry
{

/// An object that represents RGBA color
class ColorRGBA
{
public:
	enum : uint8_t
	{
		ALPHA_OPAQUE = 255,
		ALPHA_TRANSPARENT = 0,
	};

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	constexpr ColorRGBA()
		:r(0)
		,g(0)
		,b(0)
		,a(0)
	{
	}

	constexpr ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		: r(r)
		, g(g)
		, b(b)
		, a(a)
	{}

	constexpr ColorRGBA(uint8_t r, uint8_t g, uint8_t b)
		: r(r)
		, g(g)
		, b(b)
		, a(ALPHA_OPAQUE)
	{}

	template <typename Handler>
	void serialize(Handler &h)
	{
		h & r;
		h & g;
		h & b;
		h & a;
	}

	bool operator==(ColorRGBA const& rhs) const
	{
		return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
	}

	bool operator<(const ColorRGBA& rhs) const
	{
		auto mean_lhs = (r + g + b + a) / 4.0;
		auto mean_rhs = (rhs.r + rhs.g + rhs.b + rhs.a) / 4.0;
		return mean_lhs < mean_rhs;
	}

};

}
