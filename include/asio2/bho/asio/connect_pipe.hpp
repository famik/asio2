#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/connect_pipe.hpp>
#else
#include <boost/asio/connect_pipe.hpp>
#endif
