#ifndef __MSL_UTILS_H__
#define __MSL_UTILS_H__
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

#include <string>
#include <vector>

namespace msl
{
//! @brief string_split split a string into a vector by providing a single delim character
template <class T = std::vector<std::string>> T string_split(const std::string & str, char tok = ' ')
{
	T vec;
	std::size_t prev = 0;
	auto cur = str.find(tok);
	while (cur != std::string::npos)
	{
		vec.emplace_back(str.substr(prev, cur - prev));
		prev = cur + 1;
		cur = str.find(tok, prev);
	}
	vec.emplace_back(str.substr(prev, cur - prev));
	return vec;
}

//! @brief string_split split a string into a vector by providing the delim string
template <class T = std::vector<std::string>> T string_split(const std::string & str, const std::string & tok = " ")
{
	T vec;
	std::size_t prev = 0;
	auto cur = str.find(tok);
	while (cur != std::string::npos)
	{
		vec.emplace_back(str.substr(prev, cur - prev));
		prev = cur + tok.size();
		cur = str.find(tok, prev);
	}
	vec.emplace_back(str.substr(prev, cur - prev));
	return vec;
}

//! @brief string_split_any split a string into a vector by providing any of the single delim characters
template <class T = std::vector<std::string>> T string_split_any(const std::string & str, const std::string & toks = " ")
{
	T vec;
	std::size_t prev = 0;
	auto cur = str.find_first_of(toks);
	while (cur != std::string::npos)
	{
		vec.emplace_back(str.substr(prev, cur - prev));
		prev = cur + 1;
		cur = str.find_first_of(toks, prev);
	}
	vec.emplace_back(str.substr(prev, cur - prev));
	return vec;
}

//! @brief string_join join a vector into a string by uniting them with tok char
template <class T = std::vector<std::string>> std::string string_join(const T & vec, const char tok = ' ')
{
	std::string str;
	for (auto & elem : vec)
	{
		if (!str.empty())
			str += tok;
		str += elem;
	}
	return str;
}

//! @brief string_join join a vector into a string by uniting them with tok string
template <class T = std::vector<std::string>> std::string string_join(const T & vec, const std::string & tok = " ")
{
	std::string str;
	for (auto & elem : vec)
	{
		if (!str.empty())
			str += tok;
		str += elem;
	}
	return str;
}

//! @brief string_replace_in_place replace all char instances of 'from' to 'to' from the input string
inline void string_replace_in_place(std::string & str, const char from, const char to)
{
	for (auto & c : str)
	{
		if (c == from)
			c = to;
	}
}

//! @brief string_replace replace all char instances of 'from' to 'to' into a new output string
inline std::string string_replace(std::string str, const char from, const char to)
{
	string_replace_in_place(str, from, to);
	return str;
}

//! @brief string_replace_in_place replace all string instances of 'from' to 'to' from the input string
inline void string_replace_in_place(std::string & str, const std::string & from, const std::string & to)
{
	if (from.empty())
		return;
	std::size_t pos = 0;
	while ((pos = str.find(from, pos)) != std::string::npos)
	{
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
}

//! @brief string_replace replace all string instances of 'from' to 'to' into a new output string
inline std::string string_replace(std::string str, const std::string & from, const std::string & to)
{
	string_replace_in_place(str, from, to);
	return str;
}

//! @brief whitespaces returns a string containing the default ascii spaces
inline const char * whitespaces()
{
	return " \f\n\r\t\v";
}

//! @brief ltrim_in_place left trims in place 'str' of any of the 'chars' characters
inline std::string & ltrim_in_place(std::string & str, const char * chars = whitespaces())
{
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

//! @brief rtrim_in_place right trims in place 'str' of any of the 'chars' characters
inline std::string & rtrim_in_place(std::string & str, const char * chars = whitespaces())
{
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

//! @brief ltrim left trims by copy 'str' of any of the 'chars' characters
inline std::string ltrim(std::string str, const char * chars = whitespaces())
{
	return ltrim_in_place(str, chars);
}

//! @brief rtrim right trims by copy 'str' of any of the 'chars' characters
inline std::string rtrim(std::string str, const char * chars = whitespaces())
{
	return rtrim_in_place(str, chars);
}

//! @brief trim_in_place right and left trims in place 'str' of any of the 'chars' characters
inline std::string & trim_in_place(std::string & str, const char * chars = whitespaces())
{
	return ltrim_in_place(rtrim_in_place(str, chars), chars);
}

//! @brief trim right and left trims by copy 'str' of any of the 'chars' characters
inline std::string trim(std::string str, const char * chars = whitespaces())
{
	return trim_in_place(str, chars);
}

//! @brief refill for c arrays (set default value)
template <class _Ty, std::size_t _Size> constexpr void refill(_Ty (&_Array)[_Size]) noexcept
{
	std::fill(std::begin(_Array), std::end(_Array), _Ty());
}

//! @brief refill for c arrays (set custom value)
template <class _Ty, std::size_t _Size> constexpr void refill(_Ty (&_Array)[_Size], _Ty _Elem) noexcept
{
	std::fill(std::begin(_Array), std::end(_Array), _Elem);
}

//! @brief refill for std containers (set default value)
template <class _Container> constexpr void refill(_Container & _Cont)
{
	std::fill(std::begin(_Cont), std::end(_Cont), typename _Container::value_type());
}

//! @brief refill for std containers (set custom value)
template <class _Container, class _Ty> constexpr void refill(_Container & _Cont, _Ty _Elem)
{
	std::fill(std::begin(_Cont), std::end(_Cont), _Elem);
}
} // namespace msl
#endif
