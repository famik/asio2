#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/experimental/as_tuple.hpp>
#else
#include <boost/asio/experimental/as_tuple.hpp>
#endif
