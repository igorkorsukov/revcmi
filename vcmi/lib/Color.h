/*
 * Color.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include <rev/geometry/Color.h>

VCMI_LIB_NAMESPACE_BEGIN

using rev::geometry::ColorRGBA;

namespace vstd
{
template<typename Floating>
ColorRGBA lerp(const ColorRGBA & left, const ColorRGBA & right, const Floating & factor)
{
	return ColorRGBA(
		vstd::lerp(left.r, right.r, factor),
		vstd::lerp(left.g, right.g, factor),
		vstd::lerp(left.b, right.b, factor),
		vstd::lerp(left.a, right.a, factor)
	);
}
}

VCMI_LIB_NAMESPACE_END
