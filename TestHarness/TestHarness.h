/*
 * File Name: TestHarness.h
 * Project Name: TestHarness
 * Required Files:
 * ----------
 *Cpp11-BlockingQueue.h
 *Message.h
 *Process.h
 *CommHeader.h
 *Sockets.h
 *Child.h
 * ----------
 * Created: Wednesday, 7th November 2018 9:38:43 pm
 * Author: Zidong Yu (zyu107@syr.edu)
 * Description: 
 * ----------
 * Defined the TestHarness class for
 * TestHarness package.
 * ----------
 * Last Modified: Saturday, 24th November 2018 7:31:05 pm
 * Modified By:
 * ----------
 */

#pragma once
#ifndef TESTHARNESS_H
#define TESTHARNESS_H
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Message/Message.h"
#include "Process.h"
#include "../MsgPassingComm/CommHeader.h"
#include "../Sockets/Sockets.h"
#include "../Child/Child.h"
#include <vector>

using namespace MsgPassingCommunication;
namespace Testing
{
	class TestHarness
	{
	public:
		TestHarness(EndPoint from);
		~TestHarness() = default;
		void start();
		void wait();
		void stop();
		void sendMsg(MsgPassingCommunication::Message msg);
		void recvMessages();
		void dispatchMessages();
		void setPath(std::string &send_path, std::string &save_path);
		std::string getSendPath();
		std::string getSavePath();
	private:
		BlockingQueue<MsgPassingCommunication::Message> readyQ_;
		BlockingQueue<MsgPassingCommunication::Message> requestQ_;
		std::thread recvr;
		std::thread dspat;
		Comm comm_;
		EndPoint to_;
		EndPoint from_;
		std::vector<Process> children_;
	};
}
#endif
