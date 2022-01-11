/*
 * COPYRIGHT (C) 2017-2021, zhllxt
 *
 * author   : zhllxt
 * email    : 37792738@qq.com
 * 
 * Distributed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * (See accompanying file LICENSE or see <http://www.gnu.org/licenses/>)
 */

#ifndef __ASIO2_SERVER_HPP__
#define __ASIO2_SERVER_HPP__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <asio2/base/detail/push_options.hpp>

#include <cstdint>
#include <memory>
#include <chrono>
#include <functional>
#include <atomic>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <type_traits>

#include <asio2/3rd/asio.hpp>
#include <asio2/3rd/magic_enum.hpp>

#include <asio2/base/iopool.hpp>
#include <asio2/base/error.hpp>
#include <asio2/base/log.hpp>
#include <asio2/base/listener.hpp>
#include <asio2/base/session_mgr.hpp>
#include <asio2/base/define.hpp>

#include <asio2/base/detail/object.hpp>
#include <asio2/base/detail/allocator.hpp>
#include <asio2/base/detail/util.hpp>
#include <asio2/base/detail/buffer_wrap.hpp>
#include <asio2/base/detail/condition_wrap.hpp>

#include <asio2/base/component/user_data_cp.hpp>
#include <asio2/base/component/user_timer_cp.hpp>
#include <asio2/base/component/post_cp.hpp>
#include <asio2/base/component/async_event_cp.hpp>

namespace asio2::detail
{
	ASIO2_CLASS_FORWARD_DECLARE_BASE;

	template<class derived_t, class session_t>
	class server_impl_t
		: public object_t       <derived_t>
		, public iopool_cp
		, public user_data_cp   <derived_t>
		, public user_timer_cp  <derived_t>
		, public post_cp        <derived_t>
		, public async_event_cp <derived_t>
	{
		ASIO2_CLASS_FRIEND_DECLARE_BASE;

	public:
		using super = object_t     <derived_t>;
		using self  = server_impl_t<derived_t, session_t>;

		using key_type = std::size_t;

	public:
		/**
		 * @constructor
		 */
		template<class ThreadCountOrScheduler>
		explicit server_impl_t(ThreadCountOrScheduler&& tcos)
			: object_t     <derived_t>()
			, iopool_cp               (std::forward<ThreadCountOrScheduler>(tcos))
			, user_data_cp <derived_t>()
			, user_timer_cp<derived_t>()
			, post_cp      <derived_t>()
			, rallocator_()
			, wallocator_()
			, listener_  ()
			, io_        (iopool_cp::_get_io(0))
			, sessions_  (io_, this->state_)
		{
		}

		/**
		 * @destructor
		 */
		~server_impl_t()
		{
		}

		/**
		 * @function : start the server
		 */
		inline bool start() noexcept
		{
			return true;
		}

		/**
		 * @function : stop the server
		 */
		inline void stop()
		{
			ASIO2_ASSERT(this->io_.strand().running_in_this_thread());

			if (!this->io_.strand().running_in_this_thread())
			{
				this->derived().post([this]() mutable
				{
					this->stop();
				});
				return;
			}

			// close user custom timers
			this->stop_all_timers();

			// close all posted timed tasks
			this->stop_all_timed_tasks();

			// close all async_events
			this->notify_all_events();

			// destroy user data, maybe the user data is self shared_ptr, 
			// if don't destroy it, will cause loop refrence.
			this->user_data_.reset();
		}

		/**
		 * @function : check whether the server is started 
		 */
		inline bool is_started() const noexcept
		{
			return (this->state_ == state_t::started);
		}

		/**
		 * @function : check whether the server is stopped
		 */
		inline bool is_stopped() const noexcept
		{
			return (this->state_ == state_t::stopped);
		}

		/**
		 * @function : get this object hash key
		 */
		inline key_type hash_key() const noexcept
		{
			return reinterpret_cast<key_type>(this);
		}

		/**
		 * @function : Asynchronous send data for each session
		 * supporting multi data formats,see asio::buffer(...) in /asio/buffer.hpp
		 * You can call this function on the communication thread and anywhere,it's multi thread safed.
		 * PodType * : async_send("abc");
		 * PodType (&data)[N] : double m[10]; async_send(m);
		 * std::array<PodType, N> : std::array<int,10> m; async_send(m);
		 * std::vector<PodType, Allocator> : std::vector<float> m; async_send(m);
		 * std::basic_string<Elem, Traits, Allocator> : std::string m; async_send(m);
		 */
		template<class T>
		inline derived_t & async_send(const T& data)
		{
			this->sessions_.for_each([&data](std::shared_ptr<session_t>& session_ptr) mutable
			{
				session_ptr->async_send(data);
			});
			return this->derived();
		}

		/**
		 * @function : Asynchronous send data for each session
		 * You can call this function on the communication thread and anywhere,it's multi thread safed.
		 * PodType * : async_send("abc");
		 */
		template<class CharT, class Traits = std::char_traits<CharT>>
		inline typename std::enable_if_t<
			std::is_same_v<detail::remove_cvref_t<CharT>, char> ||
			std::is_same_v<detail::remove_cvref_t<CharT>, wchar_t> ||
			std::is_same_v<detail::remove_cvref_t<CharT>, char16_t> ||
			std::is_same_v<detail::remove_cvref_t<CharT>, char32_t>, derived_t&> async_send(CharT * s)
		{
			return this->async_send(s, s ? Traits::length(s) : 0);
		}

		/**
		 * @function : Asynchronous send data for each session
		 * You can call this function on the communication thread and anywhere,it's multi thread safed.
		 * PodType (&data)[N] : double m[10]; async_send(m,5);
		 */
		template<class CharT, class SizeT>
		inline typename std::enable_if_t<std::is_integral_v<detail::remove_cvref_t<SizeT>>, derived_t&>
			async_send(CharT* s, SizeT count)
		{
			if (s)
			{
				this->sessions_.for_each([s, count](std::shared_ptr<session_t>& session_ptr) mutable
				{
					session_ptr->async_send(s, count);
				});
			}
			return this->derived();
		}

	public:
		/**
		 * @function : get the acceptor refrence,derived classes must override this function
		 */
		inline auto & acceptor() noexcept { return this->derived().acceptor(); }

		/**
		 * @function : get the listen address
		 */
		inline std::string listen_address()
		{
			try
			{
				return this->acceptor().local_endpoint().address().to_string();
			}
			catch (system_error & e) { set_last_error(e); }
			return std::string();
		}

		/**
		 * @function : get the listen port
		 */
		inline unsigned short listen_port()
		{
			try
			{
				return this->acceptor().local_endpoint().port();
			}
			catch (system_error & e) { set_last_error(e); }
			return static_cast<unsigned short>(0);
		}


		/**
		 * @function : get connected session count
		 */
		inline std::size_t session_count() noexcept { return this->sessions_.size(); }

		/**
		 * @function :
		 * @param    : fn - The handler to be called for each session.
		 * Function signature :
		 * void(std::shared_ptr<asio2::xxx_session>& session_ptr)
		 */
		template<class Fun>
		inline derived_t & foreach_session(Fun&& fn)
		{
			this->sessions_.for_each(std::forward<Fun>(fn));
			return this->derived();
		}

		/**
		 * @function : find the session by session's hash key
		 */
		template<class KeyType>
		inline std::shared_ptr<session_t> find_session(const KeyType& key)
		{
			return this->sessions_.find(key);
		}

		/**
		 * @function : find the session by user custom role
		 * @param    : fn - The handler to be called when search the session.
		 * Function signature :
		 * bool(std::shared_ptr<asio2::xxx_session>& session_ptr)
		 * @return   : std::shared_ptr<asio2::xxx_session>
		 */
		template<class Fun>
		inline std::shared_ptr<session_t> find_session_if(Fun&& fn)
		{
			return std::shared_ptr<session_t>(this->sessions_.find_if(std::forward<Fun>(fn)));
		}

		/**
		 * @function : get the io object refrence
		 */
		inline io_t & io() noexcept { return this->io_; }

	protected:
		/**
		 * @function : get the recv/read allocator object refrence
		 */
		inline auto & rallocator() noexcept { return this->rallocator_; }
		/**
		 * @function : get the send/write/post allocator object refrence
		 */
		inline auto & wallocator() noexcept { return this->wallocator_; }

		inline session_mgr_t<session_t> & sessions() noexcept { return this->sessions_; }
		inline listener_t               & listener() noexcept { return this->listener_; }
		inline std::atomic<state_t>     & state   () noexcept { return this->state_;    }

		inline constexpr static bool is_session() noexcept { return false; }
		inline constexpr static bool is_client () noexcept { return false; }
		inline constexpr static bool is_server () noexcept { return true ; }

	protected:
		// The memory to use for handler-based custom memory allocation. used for acceptor.
		handler_memory<>                            rallocator_;

		/// The memory to use for handler-based custom memory allocation. used fo send/write/post.
		handler_memory<size_op<>, std::true_type>   wallocator_;

		/// listener
		listener_t                                  listener_;

		/// The io (include io_context and strand) used to handle the accept event.
		io_t                                      & io_;

		/// state
		std::atomic<state_t>                        state_ = state_t::stopped;

		/// session_mgr
		session_mgr_t<session_t>                    sessions_;

		/// use this to ensure that server stop only after all sessions are closed
		std::shared_ptr<void>                       counter_ptr_;
	};
}

#include <asio2/base/detail/pop_options.hpp>

#endif // !__ASIO2_SERVER_HPP__
