/*
 * File Name: TestHarness.cpp
 * Project Name: TestHarness
 * Required Files:
 * ----------
 * TestHarness.h
 * Cpp11-BlockingQueue.h
 * ----------
 * Created: Wednesday, 7th November 2018 8:12:16 pm
 * Author: Zidong Yu (zyu107@syr.edu)
 * Description: 
 * ----------
 * Implemented member functions in TestHarness class.
 * Defined the entry point for TestHarness package.
 * TestHarness server spawnes Client process and Child
 * processes by using the Process poll framework provided
 * by Professor, configuration data, like ip address, port
 * number, server name and file pathes, are sent by Testness 
 * server to each child processes. 
 * ----------
 * Last Modified: Saturday, 24th November 2018 7:30:48 pm
 * Modified By:
 * ----------
 */

#include "TestHarness.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../FileSystem/FileSystem.h"
#include "../Utilities/FileUtilities.h"
#include <chrono>
#include <thread>
#define TEST_TESTHARNESS

namespace Testing
{
//----< Constructor for Test Harness server>--------------------------
	TestHarness::TestHarness(EndPoint from)
		: comm_(from, "recvr"),from_(from)
	{
		comm_.start();
		start();
	}

//----< Spawn client process and child tester processes >------------------------------------------
	void TestHarness::start()
	{	std::string ChildPath = "../Debug/Child.exe";
		//Spawn Child Processes
		Process p1, p2;
		p1.application(ChildPath);p2.application(ChildPath);
		std::ostringstream os;
		os << from_.address << " " << from_.port << " " << "Child_1 " << "localhost " << "8081 " << "../SendFile ../SaveFile";
		std::string cmdLine_Child_1 = os.str();os.str("");
		os << from_.address << " " << from_.port << " " << "Child_2 " << "localhost " << "8082 " << "../SendFile ../SaveFile";
		std::string cmdLine_Child_2 = os.str();os.str("");
		p1.title("Child_1");p2.title("Child_2");
		p1.commandLine(cmdLine_Child_1);
		p2.commandLine(cmdLine_Child_2);
		p1.create();p2.create();
		std::function<void(void)> callback = []() { std::cout << "\n  --- child process exited with this message ---"; };
		p1.setCallBackProcessing(callback);
		p1.registerCallback();p2.setCallBackProcessing(callback);p2.registerCallback();
		std::cout << "\n    starting TestHarness receive thread listening on " << from_.toString();
		recvr = std::thread([&]() { recvMessages(); });
		recvr.detach();
		std::cout << "\n    starting TestHarness dispatch thread";
		dspat = std::thread([&]() { dispatchMessages(); });
		dspat.detach();
	}

	//----< waits for TestHarness to shutdown >------------------------
	/*
	*  - this is a place-holder for a more intellegent shutdown
	*  - not used in this demo
	*/
	void TestHarness::wait()
	{
		if (recvr.joinable())
			recvr.join();
		if (dspat.joinable())
			dspat.join();
	}

//----< Stop TestHarness server >------------------------
	void TestHarness::stop()
	{
		MsgPassingCommunication::Message stop;
		stop.command("TestHarnessStop");
		stop.to(from_);  // send to self
		stop.from(from_);
		comm_.postMessage(stop);
	}

//----< Send a message anywhere you want >------------------------
	void TestHarness::sendMsg(MsgPassingCommunication::Message msg)
	{
		comm_.postMessage(msg);
	}

//----< Revceive message >------------------------
	void TestHarness::recvMessages()
	{
		while (true)
		{
			MsgPassingCommunication::Message msg = comm_.getMessage();
			std::cout << "\n    TestHnRcv received msg: " + msg.name();
			if (msg.command() == "ready")
			{
				std::cout << "\n    TestHnRcv enques Ready msg: ";
				readyQ_.enQ(msg);
			}
			else if (msg.command() == "testReq")
			{
				std::cout << "\n    TestHnRcv enques Test Request msg: ";
				requestQ_.enQ(msg);
			}
			else if (msg.command() == "stop")
			{
				std::cout << "\n    TestHnRcv enques stop testing msg: ";
				requestQ_.enQ(msg);
			}
			else
			{
				std::cout << "\n    TestHarnessR - invalid message command: " << msg.command();
				std::cout << "\n	message name: " << msg.name();
			}
		}
	}


//----< deQ messages and send to testers >-------------------------
	void TestHarness::dispatchMessages()
	{
		while (true)
		{
			MsgPassingCommunication::Message trMsg = requestQ_.deQ();
			std::cout << "\n    TestHnDsp deQ msg: " + trMsg.command();

			MsgPassingCommunication::Message rdyMsg = readyQ_.deQ();
			std::cout << "\n    TestHnDsp deQ msg: " + rdyMsg.command();

			trMsg.to(rdyMsg.from());  // send request to ready child
			std::cout << "\n    TestHnDsp sending msg: " + trMsg.command();
			comm_.postMessage(trMsg);
		}
	}

//----< Setup file sending and receiving path >------------------------
	void TestHarness::setPath(std::string &send_path, std::string &save_path)
	{
		comm_.setSendFilePath(send_path);
		comm_.setSaveFilePath(save_path);
	}

//----< Get file send path >------------------------
	std::string TestHarness::getSendPath()
	{
		return comm_.getSendFilePath();
	}

//----< Get file receive path >------------------------
	std::string TestHarness::getSavePath()
	{
		return comm_.getSaveFilePath();
	}
}

#ifdef TEST_TESTHARNESS
int main()
{
	std::cout << "==============Demonstrating Req #3 and #10==============" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	
	EndPoint from("localhost",8888);//Set test harness's port to 8888
	Testing::TestHarness th(from);
	std::string send_path = "../SendFile";
	std::string save_path = "../SaveFile";
	th.setPath(send_path,save_path);

	std::cout << "===============================================" << std::endl;
	std::cout << "\n  TestHarness Info:" << std::endl;
	std::cout << "  Address: " << "localhost" << std::endl;
	std::cout << "  Port: " << 8888 << std::endl;
	std::cout << "  send path: " << send_path << std::endl;
	std::cout << "	receive path: " << save_path << std::endl;

	getchar();
	return 0;
}
#endif