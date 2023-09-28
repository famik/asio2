#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/detail/reactor_op_queue.hpp>
#else
#include <boost/asio/detail/reactor_op_queue.hpp>
#endif
