#include "path.h"

#include <algorithm>
#include <cstddef>
#include <cstring>

namespace nw::fs {

path::path(const char* cstring, size_t length)
	: m_buffer(std::min(length, strlen(cstring)) + 1)
	, m_length(std::min(length, strlen(cstring)) + 0)
{
	std::memcpy(m_buffer.get_memory(), cstring, m_buffer.get_capacity());
	m_buffer.as<char>()[length] = '\0';
}

path::path(const path& other)
	: path(other.subpath()) {}

path& path::make_owned()
{
	if (is_view())
		return *this;

	mem::buffer new_buffer(get_capacity());
	std::memcpy(new_buffer.get_memory(), m_buffer.get_memory(), get_capacity());

	m_buffer = std::move(new_buffer);

	return *this;
}

path path::subpath(size_t offset, size_t length) const
{
	char*  const offset_buffer = m_buffer.as<char>() + offset;
	size_t const buffer_size   = m_length - offset;

	if (length > buffer_size)
		return {};

	return path(offset_buffer, length);
}

path& path::operator=(const path& other)
{
	if (this != &other)
	{
		m_length = other.get_length();
		
		if (m_buffer.get_capacity() <= m_length)
			m_buffer = mem::buffer(other.get_length() + 1);
		else if (is_view())
			make_owned();

		std::memcpy(m_buffer.get_memory(), other.m_buffer.get_memory(), m_length * sizeof(char));
		m_buffer.as<char>()[m_length] = '\0';
	}

	return *this;
}

bool path::operator==(const path& other) const
{
	return 
		m_length == other.get_length()
		&& strcmp(m_buffer.as<char>(), other.m_buffer.as<char>()) == 0;
}

bool path::operator!=(const path& other) const
{
	return 
		m_length != other.get_length()
		|| strcmp(m_buffer.as<char>(), other.m_buffer.as<char>()) != 0;
}

}
