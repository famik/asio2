#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/execution/detail/as_receiver.hpp>
#else
#include <boost/asio/execution/detail/as_receiver.hpp>
#endif
