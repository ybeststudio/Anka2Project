#ifndef __MSL_RANDOM_H__
#define __MSL_RANDOM_H__
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 martysama0134 & IkarusDeveloper. All rights reserved.
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

#include <cstdint>
#include <chrono>
#include <random>
#include <type_traits>

namespace msl
{
	namespace details
	{
		inline std::default_random_engine& get_def_random_engine()
		{
			thread_local std::default_random_engine re;
			thread_local bool init = false;
			if (!init)
			{
				init = true;
				re.seed(static_cast<uint32_t>(time(nullptr)));
			}
			return re;
		}

		template <typename U, typename V = void> struct distribution_deduce;
		template <typename U> struct distribution_deduce<U, typename std::enable_if_t<std::is_integral_v<U>>>
		{
			using dist_type = std::uniform_int_distribution<U>;
		};
		template <typename U> struct distribution_deduce<U, typename std::enable_if_t<std::is_floating_point_v<U>>>
		{
			using dist_type = std::uniform_real_distribution<U>;
		};

		template <typename T>
		inline constexpr T max_val = (std::numeric_limits<T>::max)();

		template <typename T>
		inline constexpr T min_val = (std::numeric_limits<T>::min)();

		template <typename T>
		inline constexpr bool is_number_v = std::is_integral_v<T> || std::is_floating_point_v<T>;
	}

	//! @brief gen_random_number(min, max)
	template <class T> class gen_random_number
	{
	private:
		template <typename U, typename V = void> struct distribution_deduce;
		template <typename U> struct distribution_deduce<U, typename std::enable_if_t<std::is_integral_v<U>>>
		{
			using dist_type = std::uniform_int_distribution<U>;
		};
		template <typename U> struct distribution_deduce<U, typename std::enable_if_t<std::is_floating_point_v<U>>>
		{
			using dist_type = std::uniform_real_distribution<U>;
		};

	public:
		explicit gen_random_number(T max) : gen_random_number(0, max) {}
		gen_random_number(T min, T max) : m_dist(min, max) {}
		T operator()() { return m_dist(details::get_def_random_engine()); }

	private:
		typename distribution_deduce<T>::dist_type m_dist;
	};
	using gen_random_int = gen_random_number<int>;
	using gen_random_real = gen_random_number<double>;

	//! @brief random_int(min, max)
	template<typename T> std::enable_if_t<std::is_integral_v<T>, T>
	random_int(T min = details::min_val<T>, T max = details::max_val<T>)
	{
		std::uniform_int_distribution<T> d(min, max);
		return d(details::get_def_random_engine());
	}

	//! @brief random_real(min, max)
	template<typename T> std::enable_if_t<std::is_floating_point_v<T>, T>
	random_real(T min = details::min_val<T>, T max = details::max_val<T>)
	{
		std::uniform_real_distribution<T> d(min, max);
		return d(details::get_def_random_engine());
	}

	template<typename T, typename = std::enable_if_t<details::is_number_v<T>>>
	T random_number(T min = details::min_val<T>, T max = details::max_val<T>)
	{
		typename details::distribution_deduce<T>::dist_type dist(min, max);
		return dist(details::get_def_random_engine());
	}

	template<typename T>
	decltype(auto) random_from(const T& container) {
		using size_type = decltype(std::size(container));
		if(std::empty(container))
			return std::end(container);
		auto iter = std::begin(container);
		std::advance(iter, random_number<size_type>(0, std::size(container) - 1));
		return iter;
	}
} // namespace msl

#endif
