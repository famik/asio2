#pragma once

#include <asio2/config.hpp>

#if defined(ASIO_STANDALONE) || defined(ASIO2_HEADER_ONLY)
#include <asio/detail/posix_tss_ptr.hpp>
#else
#include <boost/asio/detail/posix_tss_ptr.hpp>
#endif
