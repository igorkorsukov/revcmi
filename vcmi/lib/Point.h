/*
 * Point.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include <rev/geometry/Point.h>

VCMI_LIB_NAMESPACE_BEGIN

// Forward-declared here for compatibility: many headers historically got int3
// visibility by including Point.h / Rect.h (Point used to declare Point(const int3&)).
class int3;

using rev::geometry::Point;

VCMI_LIB_NAMESPACE_END
