#ifndef __MSL_RANGE_H__
#define __MSL_RANGE_H__
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

#include <algorithm>
#include <vector>

namespace msl
{
//! @brief range_iterator used for range
template <class T> class range_iterator
{
	T m_value_{};

public:
	explicit range_iterator(T value) : m_value_{value} {}
	T operator*() { return m_value_; }
	range_iterator<T> & operator++()
	{
		++m_value_;
		return *this;
	}
	range_iterator<T> & operator--()
	{
		--m_value_;
		return *this;
	}
	range_iterator<T> operator++(int)
	{
		++m_value_;
		return *this;
	}
	range_iterator<T> operator--(int)
	{
		--m_value_;
		return *this;
	}
	bool operator!=(range_iterator<T> & r) { return m_value_ != *r; }
};

//! @brief range mostly used in for-range (no memory allocation)
template <class T> class range
{
	range_iterator<T> m_min_{0};
	range_iterator<T> m_max_;

public:
	explicit range(T max) : m_max_{max} {}
	range(T min, T max) : m_min_{min}, m_max_{max} {}
	range_iterator<T> begin()
	{
		if (*m_min_ == *m_max_)
			return m_max_;
		return m_min_;
	}
	range_iterator<T> end() { return m_max_; }
};

using crange = range<char>;
using irange = range<int>;
using llrange = range<long long>;
using frange = range<float>;
using drange = range<double>;

//! @brief xrange mostly used in for-range (it uses std::vector with n=((max-min)/diff) elements)
template <class T> class xrange
{
	std::vector<T> m_vec_;

public:
	explicit xrange(T max) : xrange(0, max, 1) {}
	xrange(T min, T max) : xrange(min, max, 1) {}
	xrange(T min, T max, T diff) {
		init(min, max, diff);
	};
	auto begin() { return std::begin(m_vec_); }
	auto end() { return std::end(m_vec_); }

private:
	template<typename U=T>
	std::enable_if_t<std::is_floating_point_v<U>>
	init(T min, T max, T diff)
	{
		if (min >= max)
			return;
		if (diff <= 0)
			return;
		// 4x faster than emplace_back
		m_vec_.resize(static_cast<std::size_t>((max - min) / diff));
		for (auto & e : m_vec_)
		{
			e += min;
			min += diff;
		}
	}

	template<typename U=T>
	std::enable_if_t<std::is_integral_v<U>>
	init(T min, T max, T diff)
	{
		if (min >= max)
			return;
		if (diff <= 0)
			return;
		// 4x faster than emplace_back
		m_vec_.resize(static_cast<std::size_t>((max - min) / diff) + ((max - min) % diff ? 1 : 0));
		for (auto & e : m_vec_)
		{
			e += min;
			min += diff;
		}
	}
};

using xcrange = xrange<char>;
using xirange = xrange<int>;
using xllrange = xrange<long long>;
using xfrange = xrange<float>;
using xdrange = xrange<double>;
} // namespace msl
#endif
