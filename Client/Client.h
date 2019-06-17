/*
 * File Name: Client.h
 * Project Name: Client
 * Required Files:
 * ----------
 * Message.h
 * CommHeader.h
 * Sockets.h
 * LoggerHeader.h
 * ----------
 * Created: Thursday, 8th November 2018 8:39:05 pm
 * Author: Zidong Yu (zyu107@syr.edu)
 * Description: 
 * ----------
 * This header file declares the Client class, which 
 * is able to send test requests to TestHarness server
 * and receive test results from Child testers.
 * Client server is configured by TestHarness server,
 * configuration data are passed from TestHarness server 
 * to client server through the Process pool framework.
 * ----------
 * Last Modified: Saturday, 24th November 2018 7:16:15 pm
 * Modified By:
 * ----------
 */

#pragma once
#ifndef CLIENT_H
#define CLIENT_H
#endif
#include "../Message/Message.h"
#include "../MsgPassingComm/CommHeader.h"
#include "../Sockets/Sockets.h"
#include "../Logger/LoggerHeader.h"

using namespace MsgPassingCommunication;

namespace Testing
{///////////////////////////////////////////////////////////////////
	// Client class
	// - Sends Test Requests to TestHarness server.
	// - Set file receive path and send path.
	// - Receive test results from child testers.
	class ClientServer
	{
	public:
		ClientServer(EndPoint from, EndPoint to, std::string &recv_path, std::string &send_path);
		~ClientServer();
		void start();
		void wait();
		void sendMsgs(MsgPassingCommunication::Message &tq_msg);
		void setSavePath(std::string &save_path);
		void setSendPath(std::string &send_path);
		std::string getSavePath();
		std::string getSendPath();
		void getMsgs();
		void commStart();
		void commStop();
	private:
		std::thread recv;
		Comm clientComm_;
		EndPoint to_;
		EndPoint from_;
	};
}
