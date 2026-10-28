#ifndef __MSL_CAST_H__
#define __MSL_CAST_H__
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2018 martysama0134. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cmath>
#include <exception>
namespace msl
{
//! @brief msl::truncate_error struct
struct truncate_error : std::exception
{
	const char * what() const noexcept override { return "MSL::Truncate Cast Exception"; }
};
//! @brief msl::truncate_cast truncate with no check
template <class T, class U> constexpr T truncate_cast(U u)
{
	return static_cast<T>(std::trunc(u));
}
//! @brief msl::truncate: truncate and throw msl::truncate_error if check fails
template <class T, class U> constexpr T truncate(U u)
{
	auto t = truncate_cast<T>(u);
	if (static_cast<U>(t) != std::trunc(u))
		throw truncate_error();
	return t;
}
//! @brief msl::integral_cast: from floating points to integral types only and throw msl::truncate_error if check fails (slight faster than truncate)
template <class T, class U> constexpr T integral_cast(U u)
{
	auto t = static_cast<T>(u);
	if (static_cast<U>(t) != std::trunc(u))
		throw truncate_error();
	return t;
}
//! @brief more meaningful cast between dynamic_cast and static_cast
template <class T, class U> constexpr T inherit_cast(U && u) noexcept
{
	return static_cast<T>(std::forward<U>(u));
}
} // namespace msl
#endif
