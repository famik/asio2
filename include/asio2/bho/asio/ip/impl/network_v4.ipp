#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/ip/impl/network_v4.ipp>
#else
#include <boost/asio/ip/impl/network_v4.ipp>
#endif
