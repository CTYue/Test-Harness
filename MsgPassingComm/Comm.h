#pragma once
/////////////////////////////////////////////////////////////////////
// Comm.h - message-passing communication facility                 //
// ver 2.6                                                         //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017    //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines Sender and Receiver classes.
*  - Sender uses a SocketConnecter and supports connecting to multiple
*    sequential endpoints and posting messages.
*  - Receiver uses a SocketListener which returns a Socket on connection.
*  It also defines a Comm class
*  - Comm simply composes a Sender and a Receiver, exposing methods:
*    postMessage(Message) and getMessage()
*
*  Required Files:
*  ---------------
*  Comm.h, Comm.cpp,
*  Sockets.h, Sockets.cpp,
*  Message.h, Message.cpp,
*  Utilities.h, Utilities.cpp
*
*  Maintenance History:
*  --------------------
*  ver 2.6 : 11 Nov 2018
*  - Finally found and fixed the file sending bug.  I had made the rwBuffer array, used to
*    transfer bytes between files and sockets, global, intending to fix that later, but
*    forgot about it.  Interactions between file sending and receiving in that one buffer
*    was the root of the problem.  These are the fixes:
*    - removed the global buffer, rwBuffer at the beginning of Comm.cpp, replaced with local
*      rwBuffers in Sender::sendFile and ClientHandler::receiveFile.
*    - tested chunking files with block sizes of 144, 1024, 10240, and 262144, all of which
*      now work correctly.
*    - added Sender::showBlocks and ClientHandler::showBlocks to help find the problem.
*    - made some minor cosmetic changes to Sender::sendFile and ClientHandler::receiveFile.
*    - cleaned up a small memory leak by deleting a ClientHandler instance used as a
*      prototype for the socket listener to create threads to handle incoming messages.
*  ver 2.5 : 09 Nov 2018
*  - Avoided effect of bug sending binary files with the following changes:
*    - changed Context::blocksize from 1024 to 262144
*    - std::ios::binary to std::ios::in | std::ios::binary in Sender::sendFile(...)
*      in Comm.cpp
*    - std::ios::binary to std::ios::in | std::ios::binary in ClientHandler::receiveFile(...)
*      in Comm.cpp
*    - removed redundant content-length test from DemoClientServer() in Comm.cpp
*    The first change avoided, but didn't fix the bug.  The rest simply clean up the code a bit.
*  ver 2.4 : 14 Oct 2018
*  - converted to use new Utilities
*  ver 2.3 : 07 Apr 2018
*  - changed message attribute for sending files from "file" to "sendingFile"
*  ver 2.2 : 27 Mar 2018
*  - added interface IComm and object factory (static method in IComm)
*  - Comm now implements the IComm interface
*  - No change to code other that derive from interface and implement its create method
*  ver 2.1 : 25 Mar 2018
*  - added declaration of SocketSystem in Comm class
*  ver 2.0 : 07 Oct 2017
*  - added sendFile(...) as private member of Sender
*  - added receiveFile() as member of ClientHandler
*  ver 1.0 : 03 Oct 2017
*  - first release
*/

#include "../Message/Message.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Sockets/Sockets.h"
#include "IComm.h"
#include <string>
#include <thread>

using namespace Sockets;

namespace MsgPassingCommunication
{
	///////////////////////////////////////////////////////////////////
	// Communication Context

	using Path = std::string;

	struct Context
	{
		Context();
		Path sendFilePath;
		Path saveFilePath;
	};

	//extern Context ctx;

	///////////////////////////////////////////////////////////////////
	// Receiver class

	class Receiver
	{
	public:
		Receiver(EndPoint ep, const std::string& name = "Receiver");
		template<typename CallableObject>
		void start(CallableObject& co);
		void stop();
		Message getMessage();
		BlockingQueue<Message>* queue();
	private:
		BlockingQueue<Message> rcvQ;
		SocketListener listener;
		std::string rcvrName;
	};

	///////////////////////////////////////////////////////////////////
	// Sender class

	class Sender
	{
	public:
		Sender(const std::string& name = "Sender");
		~Sender();
		void start();
		void stop();
		bool connect(EndPoint ep);
		void postMessage(Message msg);

	private:
		bool sendSingleFile(Message msg);
		bool sendFile(Message msg);
		BlockingQueue<Message> sndQ;
		SocketConnecter connecter;
		std::thread sendThread;
		EndPoint lastEP;
		std::string sndrName;
	};

	class ClientHandler;

	class Comm : public IComm
	{
	public:
		Comm(EndPoint ep, const std::string& name = "Comm");
		Context* getContext();
		void start();
		void stop();
		void postMessage(Message msg);
		Message getMessage();
		std::string name();
		std::string setSendFilePath(const std::string& relPath);
		std::string setSaveFilePath(const std::string& relPath);
		std::string getSendFilePath();
		std::string getSaveFilePath();
	private:
		Sender sndr;
		Receiver rcvr;
		std::string commName;
		Sockets::SocketSystem socksys_;
		ClientHandler* pCh_ = nullptr;
	};

	inline IComm* IComm::create(const std::string& machineAddress, size_t port)
	{
		std::cout << "\n  creating an instance of Comm on the native heap";
		EndPoint ep(machineAddress, port);
		IComm* pComm = new Comm(ep, "created Comm");
		return pComm;
	}
}