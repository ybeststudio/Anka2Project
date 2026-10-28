#include "stdafx.h"

#ifdef __AUTO_HUNT__
float GetDistanceNew(const TPixelPosition& PixelPosition, const TPixelPosition& c_rPixelPosition)
{
	const float fdx = PixelPosition.x - c_rPixelPosition.x;
	const float fdy = PixelPosition.y - c_rPixelPosition.y;
	return sqrtf((fdx * fdx) + (fdy * fdy));
}
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp>
void split_argument(const char* argument, std::vector<std::string>& vecArgs, const char* arg)
{
	boost::split(vecArgs, argument, boost::is_any_of(arg), boost::token_compress_on);
}
#endif