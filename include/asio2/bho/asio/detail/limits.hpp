#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/detail/limits.hpp>
#else
#include <boost/asio/detail/limits.hpp>
#endif
