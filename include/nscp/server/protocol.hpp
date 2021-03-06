#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ssl/context.hpp>

#include <socket/socket_helpers.hpp>
#include <socket/server.hpp>

#include "handler.hpp"
#include "parser.hpp"

namespace nscp {
	using boost::asio::ip::tcp;

	//
	// Connection states:
	// on_accept
	// on_connect	-> connected	wants_data = true
	// on_read		-> got_req.		has_data = true
	// on_write		-> connected	wants_data = true

	static const int socket_bufer_size = 8096;
	struct read_protocol : public boost::noncopyable {
		static const bool debug_trace = false;

		typedef std::vector<char> outbound_buffer_type;

		typedef boost::shared_ptr<nscp::server::handler> handler_type;
		outbound_buffer_type data_;
		nscp::server::digester parser_;
		handler_type handler_;
		typedef boost::array<char, socket_bufer_size>::iterator iterator_type;

		enum state {
			none,
			connected,
			got_request,
			done
		};

		state current_state_;

		static boost::shared_ptr<read_protocol> create(socket_helpers::connection_info info, handler_type handler) {
			return boost::shared_ptr<read_protocol>(new read_protocol(info, handler));
		}

		read_protocol(socket_helpers::connection_info info, handler_type handler) 
			: info_(info)
			, handler_(handler)
			, current_state_(none)
		{}

		inline void set_state(state new_state) {
			current_state_ = new_state;
			if (new_state == connected)
				parser_.reset();
		}

		bool on_accept(boost::asio::ip::tcp::socket& socket, int count) {
			std::list<std::string> errors;
			std::string s = socket.remote_endpoint().address().to_string();
			if (info_.allowed_hosts.is_allowed(socket.remote_endpoint().address(), errors)) {
				log_debug(__FILE__, __LINE__, "Accepting connection from: " + s);
				return true;
			} else {
				BOOST_FOREACH(const std::string &e, errors) {
					log_error(__FILE__, __LINE__, e);
				}
				log_error(__FILE__, __LINE__, "Rejected connection from: " + s);
				return false;
			}
		}

		bool on_connect() {
			set_state(connected);
			return true;
		}

		bool wants_data() {
			return current_state_ == connected;
		}
		bool has_data() {
			return current_state_ == got_request;
		}

		bool on_read(char *begin, char *end) {
			while (begin != end) {
				bool result;
				iterator_type old_begin = begin;
				boost::tie(result, begin) = parser_.digest(begin, end);
				if (result) {
					nscp::packet response;
					try {
						response = handler_->process(parser_.get_packet());
					} catch (const std::exception &e) {
						response = handler_->create_error("Exception processing request: " + utf8::utf8_from_native(e.what()));
					} catch (...) {
						response = handler_->create_error("Exception processing request");
					}

					data_ = response.write();
					set_state(got_request);
					return true;
				}
			}
			return true;
		}
		void on_write() {
			set_state(connected);
		}
		outbound_buffer_type get_outbound() const {
			return data_;
		}

		socket_helpers::connection_info info_;

		socket_helpers::connection_info get_info() const {
			return info_;
		}

		void log_debug(std::string file, int line, std::string msg) const {
			handler_->log_debug("nscp", file, line, msg);
		}
		void log_error(std::string file, int line, std::string msg) const {
			handler_->log_error("nscp", file, line, msg);
		}
	};

	namespace server {
		typedef socket_helpers::server::server<read_protocol, socket_bufer_size> server;
	}

} // namespace nscp
