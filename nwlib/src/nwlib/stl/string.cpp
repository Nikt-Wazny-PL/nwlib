#include "string.h"

namespace nw::stl {

template class basic_string<char>;
template class basic_string<wchar_t>;
template class basic_string<char8_t>;
template class basic_string<char16_t>;
template class basic_string<char32_t>;

template class basic_arena_string<char>;
template class basic_arena_string<wchar_t>;
template class basic_arena_string<char8_t>;
template class basic_arena_string<char16_t>;
template class basic_arena_string<char32_t>;

}
