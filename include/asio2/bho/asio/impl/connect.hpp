#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/impl/connect.hpp>
#else
#include <boost/asio/impl/connect.hpp>
#endif
