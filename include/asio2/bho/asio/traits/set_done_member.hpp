#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/traits/set_done_member.hpp>
#else
#include <boost/asio/traits/set_done_member.hpp>
#endif
