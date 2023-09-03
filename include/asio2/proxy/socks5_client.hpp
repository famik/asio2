/*
 * Copyright (c) 2017-2023 zhllxt
 *
 * author   : zhllxt
 * email    : 37792738@qq.com
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __ASIO2_SOCKS5_CLIENT_HPP__
#define __ASIO2_SOCKS5_CLIENT_HPP__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <asio2/base/detail/push_options.hpp>

#include <asio2/tcp/tcp_client.hpp>

namespace asio2::detail
{
	ASIO2_CLASS_FORWARD_DECLARE_BASE;
	ASIO2_CLASS_FORWARD_DECLARE_TCP_BASE;
	ASIO2_CLASS_FORWARD_DECLARE_TCP_CLIENT;

	template<class, class> class socks5_session_impl_t;

	template<class derived_t, class args_t = template_args_tcp_client>
	class socks5_client_impl_t
		: public tcp_client_impl_t <derived_t, args_t>
	{
		ASIO2_CLASS_FRIEND_DECLARE_BASE;
		ASIO2_CLASS_FRIEND_DECLARE_TCP_BASE;
		ASIO2_CLASS_FRIEND_DECLARE_TCP_CLIENT;

		template<class, class> friend class socks5_session_impl_t;

	public:
		using super = tcp_client_impl_t   <derived_t, args_t>;
		using self  = socks5_client_impl_t<derived_t, args_t>;

		using args_type   = args_t;
		using buffer_type = typename args_t::buffer_t;

	public:
		/**
		 * @brief constructor
		 */
		template<class... Args>
		explicit socks5_client_impl_t(Args&&... args)
			: super(std::forward<Args>(args)...)
		{
		}

		/**
		 * @brief destructor
		 */
		~socks5_client_impl_t()
		{
			this->stop();
		}

		/**
		 * @brief destroy the content of all member variables, this is used for solve the memory leaks.
		 * After this function is called, this class object cannot be used again.
		 */
		inline void destroy()
		{
			derived_t& derive = this->derived();

			derive.co_timer_.reset();

			super::destroy();
		}

		/**
		 * @brief async start the client, asynchronous connect to server.
		 * @param host - A string identifying a location. May be a descriptive name or
		 * a numeric address string.
		 * @param port - A string identifying the requested service. This may be a
		 * descriptive name or a numeric string corresponding to a port number.
		 */
		template<typename String, typename StrOrInt, typename CompletionToken>
		auto async_start(String&& host, StrOrInt&& port, CompletionToken&& token)
		{
			derived_t& derive = this->derived();

			bool f = super::async_start(std::forward<String>(host), std::forward<StrOrInt>(port));

			derive.co_timer_->expires_after(f ?
				std::chrono::steady_clock::duration::max() :
				std::chrono::steady_clock::duration::zero());

			return asio::async_compose<CompletionToken, void(asio::error_code)>(
				detail::wait_timer_op{*(derive.co_timer_)},
				token, derive.socket());
		}

	protected:
		template<typename C, typename DeferEvent>
		inline void _handle_connect(
			const error_code& ec,
			std::shared_ptr<derived_t> this_ptr, std::shared_ptr<ecs_t<C>> ecs, DeferEvent chain)
		{
			detail::cancel_timer(*(this->derived().co_timer_));

			super::_handle_connect(ec, std::move(this_ptr), std::move(ecs), std::move(chain));
		}

	protected:
		std::shared_ptr<asio::steady_timer> co_timer_ = std::make_shared<asio::steady_timer>(io_->context());
	};
}

namespace asio2
{
	using socks5_client_args = detail::template_args_tcp_client;

	template<class derived_t, class args_t>
	using socks5_client_impl_t = detail::socks5_client_impl_t<derived_t, args_t>;

	/**
	 * @brief socks5 tcp client
	 * @throws constructor maybe throw exception "Too many open files" (exception code : 24)
	 * asio::error::no_descriptors - Too many open files
	 */
	template<class derived_t>
	class socks5_client_t : public detail::socks5_client_impl_t<derived_t, detail::template_args_tcp_client>
	{
	public:
		using detail::socks5_client_impl_t<derived_t, detail::template_args_tcp_client>::socks5_client_impl_t;
	};

	/**
	 * @brief socks5 tcp client
	 * @throws constructor maybe throw exception "Too many open files" (exception code : 24)
	 * asio::error::no_descriptors - Too many open files
	 */
	class socks5_client : public socks5_client_t<socks5_client>
	{
	public:
		using socks5_client_t<socks5_client>::socks5_client_t;
	};
}

#include <asio2/base/detail/pop_options.hpp>

#endif // !__ASIO2_SOCKS5_CLIENT_HPP__