
#if _WIN32

#include "nwlib/fs/path.h"

#include <Windows.h>
#include <fileapi.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <processenv.h>
#include <winnt.h>

#include <utility>

namespace nw::fs {

bool path::is_absolute() const
{
	if (m_length < 3uz)
		return false;

	char* const buffer = m_buffer.as<char>();

	const bool has_win32_prefix = (
		(buffer[0] >= 'A' && buffer[0] <= 'Z') || 
		(buffer[0] >= 'a' && buffer[0] <= 'z')
	);

	if (has_win32_prefix && buffer[1] == ':' && (buffer[2] == '\\' || buffer[2] == '/'))
		return true;

	if (buffer[0] == '\\' && buffer[1] == '\\')
		return true;

	return false;
}

bool path::make_absolute()
{
	if (is_absolute())
		return true;

	const DWORD required = GetFullPathNameA(
		m_buffer.as<char>(),
		0,
		nullptr,
		nullptr
	);

	if (required == 0)
		return false;

	mem::buffer new_buffer(required * sizeof(char));

	const DWORD new_length = GetFullPathNameA(
		m_buffer.as<char>(),
		required + 1uz,
		new_buffer.as<char>(),
		nullptr
	);

	if (new_length == 0 || new_length > required)
		return false;

	m_buffer = std::move(new_buffer);
	m_length = new_length;
	return true;
}

bool path::exists() const
{
    return GetFileAttributesA(m_buffer.as<char>()) != INVALID_FILE_ATTRIBUTES;
}

bool path::touch(bool truncate) const
{
	if (m_length == 0)
		return false;

	const LPCSTR filepath        = m_buffer.as<char>();
	const DWORD  desired_access  = GENERIC_READ | (truncate ? GENERIC_WRITE : 0);
	const DWORD  share_accss     = FILE_SHARE_READ;
	const DWORD  creation_flags  = truncate ? CREATE_ALWAYS : CREATE_NEW;
	const DWORD  file_attributes = FILE_ATTRIBUTE_NORMAL;
	const HANDLE handle = CreateFileA(filepath, desired_access, share_accss, nullptr, creation_flags, file_attributes, nullptr);

	if (handle == INVALID_HANDLE_VALUE)
		return false;

	CloseHandle(handle);
	return true;
}

bool path::mkdirs() const
{
	if (m_length == 0uz)
		return false;

	if (exists())
		return true;

	const LPCSTR buffer = m_buffer.as<char>();

	for (size_t i = 0uz; i < m_length; ++i)
	{
		if ((buffer[i] != '\\' && buffer[i] != '/') || i < 3uz)
			continue;

		const path parent_path = subpath(0, i);
		if (!parent_path.exists() && !parent_path.mkdir())
			return false;
	}

	return exists() || mkdir();
}

bool path::mkdir() const
{
	if (m_length == 0uz || exists())
		return false;

	return CreateDirectoryA(m_buffer.as<char>(), nullptr) != FALSE;
}

path path::get_cwd()
{
	const DWORD cwd_length = GetCurrentDirectoryA(0, nullptr);
	if (cwd_length)
		return path();

	path cwd {};
	cwd.m_buffer = mem::buffer((cwd_length + 1) * sizeof(char));
	cwd.m_length = cwd_length;
	
	GetCurrentDirectoryA(cwd.get_length(), cwd.m_buffer.as<char>());
	return cwd;
}

bool path::set_cwd(path path)
{
	if (path.get_length() > 0uz && path.exists())
		return false;

	return SetCurrentDirectoryA(path.m_buffer.as<char>()) != FALSE;
}

}

#endif
