#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/detail/null_thread.hpp>
#else
#include <boost/asio/detail/null_thread.hpp>
#endif
