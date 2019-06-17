#pragma once
#ifndef TESTER_H
#define TESTER_H
#endif
/////////////////////////////////////////////////////////////////////
// Tester.h - Prototype for Test Harness child Tester              //
// Author: Zidong Yu  zyu107@syr.edu						       //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines one class, Tester:
*  - uses DllLoader to load libraries and extract tests.
*  - provides pointer to its member logger to each test, then
*    executes it, using the infrastructure from TestUtilities.
*
*  Required Files:
*  ---------------
*  Tester.h, Tester.cpp
*  DllLoader.h, DllLoader.cpp
*  ISingletonLogger.h, SingletonLogger.h
*  ITest.h, TestUtilities.h
*  Properties.h, Properties.cpp
*  FileUtilities.h, FileUtilities.cpp
*  FileSystem.h, FileSystem.cpp
*  StringUtilities.h
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 21 Oct 2018
*  - first release
*
*/
#include "../MsgPassingComm/CommHeader.h"
#include "../TestRequest/TestRequest.h"
#include "../TestRequest/Properties.h"
#include "../Loader/DllLoader.h"
#include <iostream>
#include <vector>
#include <string>

using namespace MsgPassingCommunication;

namespace Testing
{
	///////////////////////////////////////////////////////////////////
	// Child Tester class
	// - Loads TestRequests from requestPath.
	//   Each TestRequest provides names of Dlls to load and test
	// - Loads Test Dynamic Link Libraries (Dlls) from libPath.
	// - Executes tests defined in each Dll.
	// - Each tests logs ouputs using Tester class's logger
	using Requests = std::vector<TestRequest>;
	using LoggerFactory = Utilities::SingletonLoggerFactory<1, Utilities::NoLock>;
	class ChildTester
	{
	public:
		ChildTester(std::string name, std::string &address, size_t &port, std::string &to_address, size_t &to_port);
		~ChildTester();
		void start();
		void wait();
		void doTest();
		void setPath(std::string& path);
		void sendMsg(MsgPassingCommunication::Message &msg);
		void setSaveFilePath(std::string &save_path);
		void setSendFilePath(std::string &send_path);
		Property<Requests> requests;
		Property<std::string> dllPath;
	private:
		std::thread recv;
		DllLoader dllLoader_;
		std::string name_;
		std::string path_;
		LoggerFactory* logger_factory;
		std::ofstream log_file_;
		EndPoint from_;
		EndPoint to_;//TestHarness address
		Comm childComm_;
	};



}
