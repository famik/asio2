#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/experimental/impl/coro.hpp>
#else
#include <boost/asio/experimental/impl/coro.hpp>
#endif
