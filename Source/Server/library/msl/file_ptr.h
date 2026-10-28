#ifndef __MSL_FILE_PTR_H__
#define __MSL_FILE_PTR_H__
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

#include <cstdio>
#include <string>
#include <cstring>
#include <vector>

namespace msl
{
class file_ptr
{
	std::FILE * m_ptr_{nullptr};

public:
	// constructor
	file_ptr() = default;
	explicit file_ptr(const std::string & fn, const char * mode = "r") : file_ptr(fn.c_str(), mode){};
	explicit file_ptr(const char * filename, const char * mode = "r") { open(filename, mode); }
#ifdef _WIN32
	explicit file_ptr(const std::wstring& fn, const wchar_t* mode = L"r") : file_ptr(fn.c_str(), mode) {};
	explicit file_ptr(const wchar_t * filename, const wchar_t * mode = L"r") { open(filename, mode); }
#endif
	explicit file_ptr(std::FILE * ptr) { m_ptr_ = ptr; }
	// move constructor
	file_ptr(file_ptr && fp) noexcept
	{
		reset(fp.m_ptr_);
		fp.m_ptr_ = nullptr;
	}
	file_ptr & operator=(file_ptr && fp) noexcept
	{
		reset(fp.m_ptr_);
		fp.m_ptr_ = nullptr;
		return *this;
	}
	// copy constructor
	file_ptr(const file_ptr &) = delete;
	file_ptr & operator=(const file_ptr &) = delete;
	// destructor
	~file_ptr() { reset(); }

	//! @brief *ptr
	std::FILE * operator*() const { return m_ptr_; }
	//! @brief if (!ptr)
	bool operator!() const { return !m_ptr_; }
	//! @brief ptr->elem
	std::FILE * operator->() const { return m_ptr_; }
	//! @brief if (ptr)
	explicit operator bool() const { return m_ptr_; }
#ifdef MSL_FILE_PTR_ENABLE_IMPLICIT_CONVERSION
	//! @brief implicit std::FILE ptr conversion
	operator std::FILE *() const { return m_ptr_; }
#endif

	//! @brief get the file ptr
	std::FILE * get() const { return m_ptr_; }

	//! @brief get the file ptr ref
	std::FILE *& get_ref() { return m_ptr_; }

	//! @brief get the file ptr pointer
	std::FILE ** get_ptr() { return &m_ptr_; }

	//! @brief close the file ptr and reset it
	void open(const std::string & fn, const char * mode = "r") { open(fn.c_str(), mode); }

	//! @brief close the file ptr and reset it
	void open(const char * filename, const char * mode = "r")
	{
#ifdef _WIN32
		fopen_s(&m_ptr_, filename, mode);
#else
		m_ptr_ = std::fopen(filename, mode);
#endif
	}

#ifdef _WIN32
	//! @brief close the file ptr and reset it
	void open(const std::wstring& fn, const wchar_t* mode = L"r") { open(fn.c_str(), mode); }

	//! @brief close the file ptr and reset it
	void open(const wchar_t* filename, const wchar_t* mode = L"r")
	{
		_wfopen_s(&m_ptr_, filename, mode);
	}

	//! @brief reset and reopen new file
	void reset(const std::wstring& fn, const wchar_t* mode = L"r") { reset(fn.c_str(), mode); }

	//! @brief reset and reopen new file
	void reset(const wchar_t* filename, const wchar_t* mode = L"r")
	{
		reset();
		open(filename, mode);
	}

#endif

	//! @brief alias of reset()
	void close() { reset(); }

	//! @brief swap two file_ptr
	void swap(file_ptr & fp) noexcept { std::swap(m_ptr_, fp.m_ptr_); }

	//! @brief reset and reopen new file
	void reset(const std::string & fn, const char * mode = "r") { reset(fn.c_str(), mode); }

	//! @brief reset and reopen new file
	void reset(const char * filename, const char * mode = "r")
	{
		reset();
		open(filename, mode);
	}

	//! @brief reset and reassign new ownership
	void reset(std::FILE * ptr)
	{
		reset();
		m_ptr_ = ptr;
	}

	//! @brief close the file and reset the ptr
	void reset()
	{
		if (m_ptr_)
		{
			std::fclose(m_ptr_);
			m_ptr_ = nullptr;
		}
	}

	//! @brief release the file ptr
	std::FILE * release()
	{
		const auto ptr = m_ptr_;
		m_ptr_ = nullptr;
		return ptr;
	}

	//! @brief return whether or not the file is open
	bool is_open() const { return m_ptr_; }

	//! @brief return the file size whether from the current position or from beginning
	std::size_t size(bool from_current = false) const
	{
		const auto cur = tell(); // get current pos
		seek(0, SEEK_END); // go to EOF
		const auto filesize = tell(); // get filesize
		seek(cur, SEEK_SET); // go to current pos
		return from_current ? filesize - cur : filesize;
	}
	//! @brief return the file size from the current position; alias of size(true)
	std::size_t remain_size() const { return size(true); }

	//! @brief write into the file from stream
	template<class... Args>
	void write(const char* _Format, Args&&... args) const { std::fprintf(m_ptr_, _Format, std::forward<Args>(args)...); }
	//! @brief write into the file from byte vector
	std::size_t write(const std::vector<char> & vec) const { return std::fwrite(vec.data(), vec.size(), 1, m_ptr_); }
	//! @brief write into the file from c array
	std::size_t write(const void * buf, size_t size) const { return std::fwrite(buf, size, 1, m_ptr_); }

	//! @brief write into the file from string
	std::size_t string_write(const std::string & str) const { return std::fwrite(str.data(), str.size(), 1, m_ptr_); }
	//! @brief write into the file from zstring
	std::size_t string_write(const char * str) const { return std::fwrite(str, std::strlen(str), 1, m_ptr_); }

#ifdef _WIN32
	//! @brief write into the file from wstring
	void string_write(const std::wstring& str) const { std::fwrite(str.data(), str.size(), 1, m_ptr_); }
	//! @brief write into the file from wchar
	void string_write(const wchar_t* str) const { std::fwrite(str, std::wcslen(str), 1, m_ptr_); }
#endif

	//! @brief read the file from the current position as byte stream returning a vector
	std::vector<char> read(std::size_t n = 0) const
	{
		if (n == 0) // 0 implies reading the whole remaining file
			n = this->remain_size();
		std::vector<char> buf(n);
		std::fread(buf.data(), 1, buf.size(), m_ptr_);
		return buf;
	}

	//! @brief read the file as std::fread as does
	std::size_t fread(void* buf, std::size_t n) const
	{
		return std::fread(buf, 1, n, m_ptr_);
	}

	//! @brief read the file from the current position as byte stream using a buffer
	std::size_t read(void * buf, std::size_t n = 0) const
	{
		if (n == 0) // 0 implies reading the whole remaining file
			n = this->remain_size();
		return std::fread(buf, 1, n, m_ptr_);
	}

	//! @brief tell the file
	long tell() const
	{
		return std::ftell(m_ptr_);
	}

	//! @brief seek the file
	void seek(long offset, int origin = SEEK_SET) const
	{
		std::fseek(m_ptr_, offset, origin);
	}

	//! @brief read the file from the current position returning null-terminated string
	std::string string_read(const std::size_t n = 0) const
	{
		auto vec = this->read(n);
		if (!vec.empty() && vec[vec.size() - 1] != '\0') // append EOS at the end of vector
			vec.emplace_back('\0');
		return std::string(vec.begin(), vec.end()); // convert vector to string
	}

	//! @brief read the file from the current position using a null-terminated string buffer
	void string_read(char buf[], const std::size_t n = 0) const
	{
		this->read(buf, n);
		if (buf[n - 1] != '\0')
			buf[n - 1] = '\0';
	}

	//! @brief move unwritten memory buffer data to the target file.
    void flush() const
    {
        std::fflush(m_ptr_);
    }
};
} // namespace msl
#endif
