#pragma once

#include <string>
#include <boost/program_options.hpp>
#include <protobuf/plugin.pb.h>

namespace compat {
	void log_args(const Plugin::QueryRequestMessage::Request &request);
	void addAllNumeric(boost::program_options::options_description &desc, const std::string suffix = "");
	void addShowAll(boost::program_options::options_description &desc);
	void do_matchFirstNumeric(const boost::program_options::variables_map &vm, const std::string key, std::string &target, const std::string var, const std::string bound, const std::string op);
	void matchFirstNumeric(const boost::program_options::variables_map &vm, const std::string upper, const std::string lower, std::string &warn, std::string &crit, const std::string suffix = "");
	void matchShowAll(const boost::program_options::variables_map &vm, Plugin::QueryRequestMessage::Request &request, std::string prefix = "${status}: ");

	inline void inline_addarg(Plugin::QueryRequestMessage::Request &request, const std::string &str) {
		if (!str.empty())
			request.add_arguments(str);
	}
}