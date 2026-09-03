#pragma once

#include "path.h"

namespace nw::fs {

inline bool exists(path path) { return path.exists(); }

inline bool is_absolute(path path) { return path.is_absolute(); }

inline bool create_file(path path, bool trunc = false) { return path.touch(trunc); }

inline bool create_directory(path path) { return path.mkdir(); }

inline bool create_directories(path path) { return path.mkdirs(); }

inline bool set_current_durectory(path new_cwd) { return path::set_cwd(new_cwd); }

inline path get_current_durectory() { return path::get_cwd(); }

}
