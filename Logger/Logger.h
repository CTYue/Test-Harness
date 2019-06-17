#ifndef LOGGER_H
#define LOGGER_H
#endif
#pragma once
/////////////////////////////////////////////////////////////////////
// Logger.h - log text messages to std::ostream                    //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Application: Several Projects, CSE687 - Object Oriented Design  //
// Author:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package supports logging for multiple concurrent clients to a
* single std::ostream.  It does this be enqueuing messages in a
* blocking queue and dequeuing with a single thread that writes to
* the std::ostream.
*
* Build Process:
* --------------
* Required Files: Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
*
* Build Command: devenv logger.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 22 Feb 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"

using Streams = std::vector<std::ostream*>;

class Logger
{
public:
  Logger() {}
  void add(std::ostream* stream);
  void start();
  void stop(const std::string& msg = "");
  void write(const std::string& msg);
  void flush();
  void title(const std::string& msg, char underline = '-');
  int stream_numbers()
  {
	 return streams.size();
  }
  ~Logger();
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
private:
  std::thread* _pThr;
  Streams streams;
  BlockingQueue<std::string> _queue;
  bool _ThreadRunning = false;
};


template<int i>
class StaticLogger
{
public:
	static void add(std::ostream* pOut) { _logger.add(pOut); }
	static void start() { _logger.start(); }
	static void stop(const std::string& msg = "") { _logger.stop(msg); }
	static void write(const std::string& msg) { _logger.write(msg); }
	static void flush() { _logger.flush(); }
	static void title(const std::string& msg, char underline = '-') { _logger.title(msg, underline); }
	static Logger& instance() { return _logger; }
	StaticLogger(const StaticLogger&) = delete;
	StaticLogger& operator=(const StaticLogger&) = delete;
private:
	static Logger _logger;
};

template<int i>
Logger StaticLogger<i>::_logger;

struct Cosmetic
{
	~Cosmetic() { std::cout << "\n\n"; }
};