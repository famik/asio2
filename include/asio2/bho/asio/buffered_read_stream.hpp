#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/buffered_read_stream.hpp>
#else
#include <boost/asio/buffered_read_stream.hpp>
#endif
