#include "uuid.h"
#include "random.h"

namespace nw::utils {

static thread_local random s_random {};

uuid& uuid::regenerate()
{
	for (auto& word : m_words)
		word = s_random.get<word_t>();

	return *this;
}

}
