/////////////////////////////////////////////////////////////////////
// Client.cpp - creates TestRequest and stores in requestPath      //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  Client creates test request and stores in requestPath.
*  - This simulates what a Project #4 client does, e.g., creates
*    TestRequests and sends to TestHarness via a Comm channel.
*  Four demo test requests are provided in this file, each contains
*  a test library to be tested. They are generated in the form of 
*  TestRequest class first, and then converted to Message class in
*  order to be sent through Message Passing Comm framework.
*  ---------------
*  Required Files:
*  ---------------
*  TestRequest.h
*  FileUtilities.h
*  Client.h
*  ---------------
*  Maintenance History:
*  --------------------
*  ver 1.1 : 7 Nov  2018
*  - modified by Zidong Yu
*  ver 1.0 : 23 Oct 2018
*  - first release
*/
#ifndef WIN32_LEAN_AND_MEAN  // prevents duplicate includes of core parts of windows.h in winsock2.h
#define WIN32_LEAN_AND_MEAN
#endif
#include "../TestRequest/TestRequest.h"
#include "../Utilities/FileUtilities.h"
#include "Client.h"
#include <iostream>
#include <string>
#define CLIENT_TEST

using namespace MsgPassingCommunication;
//----< Constructor for ClientServer class,set its ip address and port, create Comm instance and set its file paths >--------------------------
	Testing::ClientServer::ClientServer(EndPoint from, EndPoint to, std::string &recv_path, std::string &send_path):clientComm_(from,"Comm")
	{
		to_ = to;
		from_ = from;
		setSendPath(send_path); setSavePath(recv_path);
		clientComm_.start();
	}
//----< Destructor for Client Server class >--------------------------
	Testing::ClientServer::~ClientServer()
	{
		std::cout << "\n  Client Comm shutting down...";
		clientComm_.stop();
		if (recv.joinable())
			recv.detach();
	}

//----< Start Comm instance >--------------------------
	void Testing::ClientServer::commStart()
	{
		clientComm_.start();
	}

//----< Stop Comm instance >--------------------------
	void Testing::ClientServer::commStop()
	{
		clientComm_.stop();
	}

//----< Send a message to TestHarness server >--------------------------
	void Testing::ClientServer::sendMsgs(MsgPassingCommunication::Message &tq_msg)
	{	
		tq_msg.from(from_);
		tq_msg.to(to_);
		std::cout << "--------------Demonstrating Req #6a--------------" << std::endl;
		std::cout << "      Sending test requests in the form of HTTP" << std::endl;
		std::cout << "		style messages.\n";
		std::cout << "-------------------------------------------------"<< std::endl;
		std::cout << "\n  Client sending msg: " << "[" << tq_msg.name() << "]" << " to TestHarness server.\n";
		std::cout << "  Message from: " << tq_msg.from().toString() << std::endl;
		std::cout << "  Message to: " << tq_msg.to().toString() << std::endl;
		std::cout << "  Message author: " << tq_msg.value("author") << std::endl;
		std::cout << "  Message time: " << tq_msg.value("time") << std::endl;
		std::cout << "  Message command: " << tq_msg.value("command") << std::endl;
		if (tq_msg.dlls_size() > 0)
		{
			std::vector<std::string> rtn_dlls = tq_msg.dll_name(); Testing::DateTime dt;
			for(auto item: rtn_dlls)
			{	//Send multiple dlls enumerately
				//Problem here!
				std::cout << "  Message sendingFile: " << item << std::endl;
				MsgPassingCommunication::Message temp; temp.name(tq_msg.name());
				temp.from(from_); temp.to(to_); temp.attribute("author", tq_msg.value("author"));
				temp.attribute("time", dt.now()); temp.file(item); temp.command("testReq");
				clientComm_.postMessage(temp); /*std::this_thread::sleep_for(std::chrono::milliseconds(1500));*/
			}
		}
		else
		{
			std::cout << "  Message sendingFile: " << tq_msg.value("sendingFile") << std::endl;
			clientComm_.postMessage(tq_msg);
		}
	}

//----< Set file receive path for Comm instance >--------------------------
	void Testing::ClientServer::setSavePath(std::string &save_path)
	{
		clientComm_.setSaveFilePath(save_path);
	}

//----< Set file send path for Comm instance >--------------------------
	void Testing::ClientServer::setSendPath(std::string &send_path)
	{
		clientComm_.setSendFilePath(send_path);
	}

//----< Get Comm file receive path>--------------------------
	std::string Testing::ClientServer::getSavePath()
	{
		return clientComm_.getSaveFilePath();
	}

//----< Get Comm file send path>--------------------------
	std::string Testing::ClientServer::getSendPath()
	{
		std::string rtn_value = clientComm_.getSendFilePath();
		return rtn_value;
	}

//----< Get messages>--------------------------
	void Testing::ClientServer::getMsgs()
	{
		MsgPassingCommunication::Message msg;
		while (true)
		{
			msg = clientComm_.getMessage();
			std::cout << "\n  Client recvd  message \"" << msg.name() << "\"";
			if (msg.command() == "result")
			{
				//logging results sent by child to client.
				std::cout << "\n-----------Demonstrating Req #5b and #6b---------" << std::endl;
				std::cout << "      Child testers send Logging Result to Client" << std::endl;
				std::cout << "      Client server displays test results.	 " << std::endl;
				std::cout << "-------------------------------------------------" << std::endl;
				std::cout << "   Client received test result from: " << msg.from().toString() << std::endl;
				std::cout << "   Test Request name: " << msg.value("name") << std::endl;
				std::cout << "	 Test result: " << msg.value("result") << std::endl;
				std::cout << "	 Test author: " << msg.value("author") << std::endl;
				std::cout << "	 Test log: " << msg.value("log") << std::endl;
				std::cout << "	 Test Date and time: " << msg.value("time") << std::endl;
			}
			else if (msg.command() == "stop")
			{
				std::cout << "   Client received stop command from: " << msg.from().toString() << std::endl;
				break;
			}
			else
			{
				std::cout << "\n  Client received invalid message from: " << msg.from().toString() << std::endl;
				std::cout << "	Message name:  " << msg.name() << std::endl;
				std::cout << "  Message command: " << msg.command() << std::endl;
				std::cout << "  Message author: " << msg.value("author") << std::endl;
				std::cout << "  Message Date and time: " << msg.value("time") << std::endl;
			}
		}
	}

//----< waits for Client server to shutdown >------------------------
/*
*  - this is a place-holder for a more intellegent shutdown
*  - not used in this demo
*/	
	void Testing::ClientServer::wait()
	{
		if (recv.joinable())
			recv.join();
	}

//----< Start listening thread, get messages.>--------------------------
	void Testing::ClientServer::start()
	{
		std::cout << "\n  starting Client's listen thread listening on " + from_.toString();
		recv = std::thread([&]() { getMsgs(); });
		recv.detach();
	}

#ifdef CLIENT_TEST
int main(int argc, char *argv[])
{
	if (argc < 7)
	{
		std::cout << "Invalid parameters..." << std::endl;
		return 0;
	}
	std::cout << "==============Demonstrating Req #6==============" << std::endl;
	std::cout << "      Review requirement #6: Shall provide a  " << std::endl;
	std::cout << "      Client process which sends test requests " << std::endl;
	std::cout << "      in the form of Comm messages, and displays" << std::endl;
	std::cout << "      test results in the form of Comm messages." << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	//Handle parameters sent by TestHarness server.
	std::string addr_to = argv[0];
	size_t port_to = std::stoi(argv[1]);
	std::string child_name = argv[2];
	std::string addr_from = argv[3];
	size_t port_from = std::stoi(argv[4]);
	std::string send_path = argv[5];
	std::string receive_path = argv[6];

	std::cout << "\n  Client Info: " << std::endl;
	std::cout << "  client_name: " << argv[2] << std::endl;
	std::cout << "  addr_from: " << argv[3] << std::endl;
	std::cout << "  port_from: " << std::stoi(argv[4]) << std::endl;
	std::cout << "  addr_to: " << argv[0] << std::endl;
	std::cout << "  port_to: " << std::stoi(argv[1]) << std::endl;
	std::cout << "  send path: " << argv[5] << std::endl;
	std::cout << "  receive path: " << argv[6] << std::endl;

	EndPoint from(addr_from, port_from);
	EndPoint to(addr_to, port_to);

	Testing::ClientServer client(from, to, receive_path, send_path);
	client.start();
	//Send Test Requests
	Testing::TestRequest tq1;
	tq1.author("Zidong");
	Testing::DateTime dt;
	tq1.date(dt.now());
	tq1.name("Test_Request_1");
	tq1.addDll("TestDriver1.dll");
	tq1.addDll("TestDriver2.dll");
	tq1.addDll("TestDriver3.dll");
	tq1.addDll("TestDriver4.dll");
	std::cout << std::endl << "TestRequest: " << tq1.toString() << std::endl;
	//Convert test request to message
	Message tq_msg;
	tq_msg.fromTestRequest(tq1);
	tq_msg.command("testReq");
	client.sendMsgs(tq_msg);

	Testing::TestRequest tq2;
	tq2.author("Zidong");
	Testing::DateTime dt2;
	tq2.date(dt2.now());
	tq2.name("Test_Request_2");
	tq2.addDll("TestDriver2.dll");
	//Convert test request to message
	Message tq_msg_2;
	tq_msg_2.fromTestRequest(tq2);
	tq_msg_2.command("testReq");
	client.sendMsgs(tq_msg_2);

	Testing::TestRequest tq3;
	tq3.author("Zidong");
	Testing::DateTime dt3;
	tq3.date(dt3.now());
	tq3.name("Test_Request_3");
	tq3.addDll("TestDriver3.dll");
	//Convert test request to message
	Message tq_msg_3;
	tq_msg_3.fromTestRequest(tq3);
	tq_msg_3.command("testReq");
	client.sendMsgs(tq_msg_3);

	Testing::TestRequest tq4;
	tq4.author("Zidong");
	Testing::DateTime dt4;
	tq4.date(dt4.now());
	tq4.name("Test_Request_4");
	tq4.addDll("TestDriver4.dll");
	//Convert test request to message
	Message tq_msg_4;
	tq_msg_4.fromTestRequest(tq4);
	tq_msg_4.command("testReq");
	client.sendMsgs(tq_msg_4);

	getchar();
}
#endif