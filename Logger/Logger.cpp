#pragma once
/////////////////////////////////////////////////////////////////////
// Logger.cpp - log text messages to std::ostream                  //
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
#ifndef WIN32_LEAN_AND_MEAN  // prevents duplicate includes of core parts of windows.h in winsock2.h
#define WIN32_LEAN_AND_MEAN
#endif#include <functional>
#include <Windows.h>
#include "Logger.h"
//#include "../Utilities/StringUtilities.h"
#include "../Utilities/Utilities.h"


//----< send text message to std::ostream >--------------------------

void Logger::write(const std::string& msg)
{
  if(_ThreadRunning)
    _queue.enQ(msg);
}
//----< flush ostream buffer >---------------------------------------

void Logger::flush()
{
  if (_ThreadRunning)
  {
    while (_queue.size() > 0)  // wait for logger queue to empty
      ::Sleep(50);
	for(auto item : streams)
		item->flush();
  }
}

//----<  >---------------------------------------
void Logger::title(const std::string& msg, char underline)
{
  std::string temp = "\n  " + msg + "\n " + std::string(msg.size() + 2, underline);
  write(temp);
}

//----< add a new stream to logger>---------------------
void Logger::add(std::ostream* stream) 
{ 
  streams.push_back(stream);
}
//----< start logging >----------------------------------------------

void Logger::start()
{
  if (_ThreadRunning)
    return;
  _ThreadRunning = true;
  std::function<void()> tp = [=]() {
    while (true)
    {
      std::string msg = _queue.deQ();
      if (msg == "quit")
      {
        _ThreadRunning = false;
        break;
      }
	  for(auto item: streams)
		  *item << msg;
    }
  };
  std::thread thr(tp);
  thr.detach();
}
//----< stop logging >-----------------------------------------------

void Logger::stop(const std::string& msg)
{
  if (_ThreadRunning)
  {
    if(msg != "")
      write(msg);
    write("quit");  // request thread to stop
    while (_ThreadRunning)
      /* wait for thread to stop*/
      ;
  }
}
//----< stop logging thread >----------------------------------------

Logger::~Logger()
{
  stop(); 
}

//#define TEST_LOGGER

#ifdef TEST_LOGGER


int main()
{
  //Util::Title("Testing Logger Class");
  Logger log;
  std::ofstream myfile;
  myfile.open("../Test_logger.txt");


  log.add(&myfile);
  log.add(&std::cout);
  log.start();
  log.title("Testing Logger Class", '=');
  log.write("\n  2318718658");
  log.write("\n  2348789999999");
  log.write("\n  fajkhruijn2f");
  log.write("\n  won'ssflkjoeti get logged - stopped");
  log.write("\n  starting sssss");
  log.write("\n  and stopping again");


  //myfile.close();

  getchar();
}

#endif
