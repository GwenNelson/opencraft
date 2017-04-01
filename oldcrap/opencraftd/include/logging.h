#pragma once

#include <event2/event.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#define LOG BOOST_LOG_TRIVIAL


void configure_logging(std::string logfile, bool debug_mode);
