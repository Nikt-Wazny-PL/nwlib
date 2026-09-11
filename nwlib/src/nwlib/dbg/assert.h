#pragma once

namespace nw::dbg {

void assert(bool condition, const char* condition_str, const char* fmt = "", ...);

}

#define NWDBG_ASSERT(cond, ...) ::nw::dbg::assert(cond, #cond, __VA_ARGS__)
