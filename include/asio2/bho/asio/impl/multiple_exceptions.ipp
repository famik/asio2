#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/impl/multiple_exceptions.ipp>
#else
#include <boost/asio/impl/multiple_exceptions.ipp>
#endif
