#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/ssl/detail/handshake_op.hpp>
#else
#include <boost/asio/ssl/detail/handshake_op.hpp>
#endif
