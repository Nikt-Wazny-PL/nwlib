#pragma once

#include "../mem/buffer.h"

#include <cstddef>
#include <cstdint>

namespace nw::fs {

class path
{
public:
	path() = default;
	path(const char* cstring, size_t length = SIZE_MAX);
	path(const path& other); // creates a view
	path(path&& other) noexcept = default;

	size_t get_length()   const { return m_length; }
	size_t get_capacity() const { return m_buffer.get_capacity(); }
	bool   is_view()      const { return !m_buffer.has_ownership(); }

	path& make_owned();

	path subpath(size_t offset = 0uz, size_t length = SIZE_MAX) const; // returns a view
	path subpath_owned(size_t offset = 0uz, size_t length = SIZE_MAX) const { return subpath(offset, length).make_owned(); }

	bool is_absolute() const;
	bool make_absolute();
	
	bool exists() const;

	bool touch(bool truncate = false) const;

	bool mkdirs() const;
	bool mkdir() const;

	static path get_cwd();
	static bool set_cwd(path path);

	path& operator=(const path& other); // makes a view
	path& operator=(path&& other) noexcept = default;

	bool operator==(const path& other) const;
	bool operator!=(const path& other) const;

private:
	mem::buffer m_buffer {};
	size_t      m_length {};
};

}
