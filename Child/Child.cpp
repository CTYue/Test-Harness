/*
 * File Name: Child.cpp
 * Project Name: Child
 * Required Files:
 * ----------
 * Child.h
 * Message.h
 * Cpp11-BlockingQueue.h
 * Sockets.h
 * DllLoader.h
 * ----------
 * Created: Sunday, 4th November 2018 8:29:34 pm
 * Author: Zidong Yu (zyu107@syr.edu)
 * Description: 
 * ----------
 * This file defines each member function in 
 * ChildTester class. The entry point of Child
 * package is also included.
 * Child tester's ip address and port are assigned
 * by TestHarness server. Parameters are passed from 
 * TestHarness server to Child tester through the
 * Process pool framework.
 * ----------
 * Last Modified: Saturday, 24th November 2018 7:16:00 pm
 * Modified By:
 * ----------
 */

#include "Child.h"
#include "../Message/Message.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Sockets/Sockets.h"
#include <iostream>
#include <string>
#include <functional>
#include <thread>
#define TEST_CHILD

using namespace MsgPassingCommunication;

//----< Constructor for Child Tester class, setup its ip address and port number,start Comm instance>--------------------------
Testing::ChildTester::ChildTester(std::string name, std::string &address, size_t &port, std::string &to_address, size_t &to_port) :name_(name), to_(to_address, to_port), from_(address, port), childComm_(from_, "Comm")
{
	childComm_.start();
	path_ = childComm_.getSaveFilePath();
}

//----< Start doTests thread, get messages from TestHarness server and executes tests inside each test driver >--------------------------
void Testing::ChildTester::start()
{
	std::cout << "\n      starting ChildTester thread listening on " + from_.toString();
	recv = std::thread([&]() { doTest(); });
	recv.detach();
}

//----< waits for Child tester to shutdown >------------------------
/*
*  - this is a place-holder for a more intellegent shutdown
*  - not used in this demo
*/
void Testing::ChildTester::wait()
{
	if (recv.joinable())
		recv.join();
}

//----< default destructor for Child tester class >--------------------------
Testing::ChildTester::~ChildTester()
{
	std::cout << "\n      ChildTester destroyed";
	log_file_.close();
	delete logger_factory;
	childComm_.stop();
}

//----< Set file receive path for Comm instance >--------------------------
void Testing::ChildTester::setSaveFilePath(std::string &save_path)
{
	childComm_.setSaveFilePath(save_path);
}

//----< Set file send path for Comm instance >--------------------------
void Testing::ChildTester::setSendFilePath(std::string &send_path)
{
	childComm_.setSendFilePath(send_path);
}


//----< Set test library path >--------------------------
void Testing::ChildTester::setPath(std::string& path)
{
	dllPath(path);
}

//----< Get messages, if the message is a test request, execute tests in its attaching test driver >--------------------------
void Testing::ChildTester::doTest()
{	auto logger_ = logger_factory->getInstance();bool test_res = true;logger_->setTerminator("");std::string logFileSpec = "../" + name_ + "_log.txt";
	log_file_.open(logFileSpec, std::ofstream::out | std::ofstream::trunc);log_file_.close();
	log_file_.open(logFileSpec, std::ios::app);logger_->addStream(&log_file_);
	logger_->write("\n==================Demonstrate Req #7 #8 and #9====================");
	logger_->write("\n------------------------------------------------");
	Testing::DateTime dt;logger_->write("\n      Testing time: " + dt.now());logger_->write("\n      Log path: ../");
	MsgPassingCommunication::Message ready;ready.from(from_);ready.to(to_);ready.command("ready");
	logger_->write("\n      Tester sending msg: " + ready.command()); childComm_.postMessage(ready);bool result = true;
	while (true){	MsgPassingCommunication::Message msg = childComm_.getMessage();
		logger_->write("\n-------------------------------------------------");
		logger_->write("\n      Tester " + from_.toString() + " received msg: " + msg.name());
		logger_->write("\n------------------Message info-------------------");logger_->write("\n      Message name: " + msg.name());
		logger_->write("\n      Message command: " + msg.value("command"));logger_->write("\n      Message author: " + msg.value("author"));
		logger_->write("\n      Message from: " + msg.from().toString()); std::vector<std::string> dlls = msg.dll_value("sendingFile");
		logger_->write("\n      Message file: "); 
		for (std::string item : dlls) logger_->write("                      " + item + "\n");logger_->write("\n");
		if (msg.command() == "testReq"){
			logger_->write("\n------------------Test task starts-------------------");
			for (int i=0;i<dlls.size();i++)
			{	std::string fileSpec = path_ + "\\" + dlls[i]; dllLoader_.clear(); bool load_check = dllLoader_.loadAndExtract(fileSpec);
				if (!load_check) { logger_->write("\n      Can't load " + fileSpec + ".\n"); test_res = false; }
				else {
					logger_->write("\n      Sucessfully loaded " + fileSpec);
					logger_->write("\n      Execute tests extracted from " + fileSpec);
					logger_->write("\n-----------------------Test Result----------------------");
					test_res = doTests(dllLoader_, logger_factory); result &= test_res;
					logger_->write("\n--------------------------------------------------------");
					if (test_res) logger_->write("\n    All tests passed in test driver library: " + fileSpec + "\n");
					else { logger_->write("\n    At least one test failed in test driver library: " + fileSpec + "\n"); }
					logger_->write("\n--------------------------------------------------------");
					logger_->write("\n    Detailed log file for each test library is named: log_" + msg.name() + ".txt\n");
					logger_->write("\n    Test report summary is named: [Child_name]_log.txt");
					logger_->write("\n    Only test report summary will be sent back to client in the"); logger_->write("\n    form of Message.");
				}
			}
			logger_->write("\n--------------------------------------------------------\n");logger_->write("       Send logging result back to client...\n");
			MsgPassingCommunication::Message log_res_msg;log_res_msg.command("result");DateTime dt; log_res_msg.name(msg.name());
			log_res_msg.from(from_); log_res_msg.to(msg.from()); std::string res; if (result) res = "Passed"; else res = "Failed";
			log_res_msg.attribute("result", res);log_res_msg.attribute("author", msg.value("author"));log_res_msg.attribute("dll", dlls[0]);
			log_res_msg.attribute("time", dt.now());log_res_msg.attribute("log", logFileSpec);
			logger_->write("\n------------------------Test Ends-----------------------\n");log_file_.flush();
			childComm_.postMessage(log_res_msg);
		childComm_.postMessage(ready);
	}
	}
}

//----< Send a message to anywhere you want >--------------------------
void Testing::ChildTester::sendMsg(MsgPassingCommunication::Message &msg)
{
	childComm_.postMessage(msg);
}


#ifdef TEST_CHILD
int main(int argc, char *argv[])
{
	/*	Test Stub	*/
	std::cout << "\n  Demonstrating Child Tester Handles Test Request";
	std::cout << "\n ===============================================";
	
	if (argc < 7)
	{
		std::cout << "\nInvalid parameters..." << std::endl;
		return 0;
	}

	//Handle parameters sent by Testharness server
	std::string addr_to = argv[0];
	size_t port_to = stoi(argv[1]);
	std::string child_name = argv[2];
	std::string addr_from = argv[3];
	size_t port_from = stoi(argv[4]);
	std::string send_path = argv[5];
	std::string receive_path = argv[6];

	std::cout << "\n  Child tester Info:" << std::endl;
	std::cout << "  child_name: " << argv[2] << std::endl;
	std::cout << "  addr_from: " << argv[3] << std::endl;
	std::cout << "  port_from: " << stoi(argv[4]) << std::endl;
	std::cout << "  addr_to: " << argv[0] << std::endl;
	std::cout << "  port_to: " << stoi(argv[1]) << std::endl;
	std::cout << "  send path: " << argv[5] << std::endl;
	std::cout << "	receive path: " << argv[6] << std::endl;
	//Create a Child Tester instance and then setup it.
	Testing::ChildTester ct(child_name,addr_from,port_from,addr_to,port_to);
	ct.start();
	ct.setPath(receive_path);

	getchar();
}
#endif