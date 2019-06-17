/////////////////////////////////////////////////////////////////////
// Comm.cpp - message-passing communication facility               //
// ver 2.6                                                         //
// Author: Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017//
// Modifier: Zidong Yu zyu107@syr.edu							   //
// Modification:												   //
// Added functions to support transfering multiple files contained //
// in test request message.										   //
/////////////////////////////////////////////////////////////////////

#include "Comm.h"
#include "../Logger/Logger.h"
#include "../Utilities/StringUtilities.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../FileSystem/FileSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <conio.h>

namespace MsgPassingCommunication
{
	Context::Context()
	{
		saveFilePath = "../SaveFile";  // default staging area for incoming files
		sendFilePath = "../SendFile";  // default staging area for outgoing files
	}
	Context ctx;
	//const std::streamsize BlockSize = 144;
	const std::streamsize BlockSize = 1024;
	//const std::streamsize BlockSize = 262144;
}

using namespace MsgPassingCommunication;
using namespace Sockets;

//----< constructor sets port >--------------------------------------

Receiver::Receiver(EndPoint ep, const std::string& name) : listener(ep.port), rcvrName(name)
{
	StaticLogger<1>::write("\n  -- starting Receiver");
}
//----< returns reference to receive queue >-------------------------

BlockingQueue<Message>* Receiver::queue()
{
	return &rcvQ;
}
//----< starts listener thread running callable object >-------------

template<typename CallableObject>
void Receiver::start(CallableObject& co)
{
	listener.start(co);
}
//----< stops listener thread >--------------------------------------

void Receiver::stop()
{
	listener.stop();
}
//----< retrieves received message >---------------------------------

Message Receiver::getMessage()
{
	StaticLogger<1>::write("\n  -- " + rcvrName + " deQing message");
	return rcvQ.deQ();
}
//----< constructor initializes endpoint object >--------------------

Sender::Sender(const std::string& name) : sndrName(name)
{
	lastEP = EndPoint();  // used to detect change in destination
}
//----< destructor waits for send thread to terminate >--------------

Sender::~Sender()
{
	if (sendThread.joinable())
		sendThread.join();
}
//----< starts send thread deQ, inspect, and send loop >-------------

void Sender::start()
{
	std::function <void()> threadProc = [&]() {
		while (true)
		{
			Message msg = sndQ.deQ();
			if (msg.containsKey("verbose"))
			{
				std::cout << "\n  Sending message:";
				msg.show();
				//::Beep(1000, 100);
			}
			if (msg.command() == "quit")
			{
				StaticLogger<1>::write("\n  -- send thread shutting down");
				return;
			}
			StaticLogger<1>::write("\n  -- " + sndrName + " send thread sending " + msg.name());
			std::string msgStr = msg.toString();

			if (msg.to().address != lastEP.address || msg.to().port != lastEP.port)
			{
				connecter.shutDown();
				//connecter.close();
				StaticLogger<1>::write("\n  -- attempting to connect to new endpoint: " + msg.to().toString());
				if (!connect(msg.to()))
				{
					StaticLogger<1>::write("\n can't connect");
					continue;
				}
				else
				{
					StaticLogger<1>::write("\n  connected to " + msg.to().toString());
				}
			}
			if(msg.containFile())
			{
				sendFile(msg);
			}
			else
			{
				bool sendRslt = connecter.send(msgStr.length(), (Socket::byte*)msgStr.c_str());
			}
		}
	};
	std::thread t(threadProc);
	sendThread = std::move(t);
}
//----< stops send thread by posting quit message >------------------

void Sender::stop()
{
	Message msg;
	msg.name("quit");
	msg.command("quit");
	postMessage(msg);
	connecter.shutDown();
}
//----< attempts to connect to endpoint ep >-------------------------

bool Sender::connect(EndPoint ep)
{
	lastEP = ep;
	return connecter.connect(ep.address, ep.port);
}
//----< posts message to send queue >--------------------------------

void Sender::postMessage(Message msg)
{
	sndQ.enQ(msg);
}
//----< sends binary file >------------------------------------------
/*
*  - Note: sendFilePath has to match the full path to the staging area
*/
void showBlocks(Socket::byte* rwBuffer, std::streamsize blockSize)
{
	std::string db1(rwBuffer, blockSize);
	std::cout << "\n Sending File Block:";
	std::cout << "\n --------------------------------------\n";
	std::cout << db1;
	std::cout << "\n End of File Block";
	std::cout << "\n --------------------------------------\n";
}

bool Sender::sendSingleFile(Message msg)
{
	Socket::byte rwBuffer[BlockSize];

	if (!msg.containFile())
	{
		std::cout << "\n message does not contain any file.";
		return false;
	}
	std::vector<std::string> dlls = msg.dll_value("sendingFile");
	std::string fileSpec = ctx.sendFilePath + "/" + dlls[0];
	fileSpec = FileSystem::Path::getFullFileSpec(fileSpec);

	std::ifstream sendFile(fileSpec, std::ifstream::binary/*,_SH_DENYNO*/);
	if (!sendFile.good())
	{
		std::cout << "\n  can't open \"" << fileSpec << "\"";
		return false;
	}

	while (true)
	{
		if (msg.containsKey("verbose"))
		{
			std::ostringstream conv;
			conv << std::this_thread::get_id();
			std::string sid = ", thread id = " + conv.str();
			std::cout << "\n  sending   file block from \"" + msg.value("sendingFile") + "\"" + sid;
		}
		sendFile.read(rwBuffer, BlockSize);
		std::streamsize blockSize = sendFile.gcount();  // number of bytes read

		///////////////////////////////////////////////////////////////////////
		// uncomment the line below to view blocks read from file being sent
		// showBlocks(rwBuffer, blockSize);

		msg.contentLength(blockSize);
		std::string msgString = msg.toString();
		connecter.sendString(msgString);
		connecter.send(blockSize, rwBuffer);
		if (blockSize == 0)
			break;
	}
	sendFile.close();
	return true;
}

bool Sender::sendFile(Message msg)
{
	Socket::byte rwBuffer[BlockSize];

	if (!msg.containFile())
	{
		std::cout << "\n message does  not contain key sendingFile in sendFile(msg)";
		return false;
	}
	auto dlls = msg.dll_value("sendingFile");

	Message msg_file_trans;
	msg_file_trans.to(msg.to()); msg_file_trans.from(msg.from()); msg_file_trans.command(msg.command());
	msg_file_trans.name(msg.name()); msg_file_trans.attribute("author",msg.value("author"));
	for (int i = 0; i < dlls.size(); i++)
	{
		msg_file_trans.file(dlls[i]);
		sendSingleFile(msg_file_trans);
	}
	return true;
}

namespace MsgPassingCommunication
{
	//----< callable object posts incoming message to rcvQ >-------------
	/*
	*  This is ClientHandler for receiving messages and posting
	*  to the receive queue.
	*/
	class ClientHandler
	{
	public:
		//----< acquire reference to shared rcvQ >-------------------------

		ClientHandler(BlockingQueue<Message>* pQ, const std::string& name = "clientHandler") : pQ_(pQ), clientHandlerName(name)
		{
			std::cout << "\n--starting ClientHandler";
		}
		//----< shutdown message >-----------------------------------------

		~ClientHandler()
		{
			std::cout << "\n  ClientHandler destroyed";
		}
		//----< set BlockingQueue >----------------------------------------

		void setQueue(BlockingQueue<Message>* pQ)
		{
			pQ_ = pQ;
		}
		//----< frame message string by reading bytes from socket >--------

		std::string readMsg(Socket& socket)
		{
			std::string temp, msgString;
			while (socket.validState())
			{
				temp = socket.recvString('\n');  // read attribute
				msgString += temp;
				if (temp.length() < 2)           // if empty line we are done
					break;
			}
			return msgString;
		}
		//----< receive file blocks >--------------------------------------
		/*
		*  - expects msg to contain file and contentLength attributes
		*  - expects to be connected to appropriate destination
		*  - these requirements are established in Sender::start()
		*/
		void showBlocks(Socket::byte* rwBuffer, std::streamsize blockSize)
		{
			std::string db1(rwBuffer, blockSize);
			std::cout << "\n Receiving File Block:";
			std::cout << "\n --------------------------------------\n";
			std::cout << db1;
			std::cout << "\n End of File Block";
			std::cout << "\n --------------------------------------\n";
		}

		bool receiveFile(Message msg)
		{
			Socket::byte rwBuffer[BlockSize];

			std::string fileName = msg.value("sendingFile");
			std::string fileSpec = ctx.saveFilePath + "\\" + fileName;
			fileSpec = FileSystem::Path::getFullFileSpec(fileSpec);

			if (msg.containsKey("verbose"))
			{
				std::cout << "\n  in receiveFile fileSpec = \"" + fileSpec + "\"";
			}

			std::ofstream saveStream(fileSpec, std::ofstream::binary);
			if (!saveStream.good())
			{
				std::cout << "\n  can't open save file \"" + fileSpec + "\"";
				return false;
			}

			while (true)
			{
				if (msg.containsKey("verbose"))
				{
					std::ostringstream conv;
					conv << std::this_thread::get_id();
					std::string sid = ", thread id = " + conv.str();
					std::cout << "\n  receiving file block from \"" + msg.value("sendingFile") + "\"" + sid;
					std::cout << "\n  content length = " + Utilities::Converter<std::streamsize>::toString(msg.contentLength());
				}

				std::streamsize blockSize = msg.contentLength();
				if (blockSize == 0)
				{
					std::cout << "\n--leaving receiveFile";
					break;
				}
				Socket::byte terminator;
				pSocket->recv(1, &terminator);  // blank line marking end of message header

				pSocket->recv(blockSize, rwBuffer);
				saveStream.write(rwBuffer, blockSize);

				///////////////////////////////////////////////////////////////////
				// uncomment line below to view blocks being retrieved from socket
				//
				// showBlocks(rwBuffer, blockSize);

				std::string msgString = readMsg(*pSocket);  // read next message header

				if (msgString.length() == 0)   // connection with sender lost
				{
					std::cout << "\n--leaving receiveFile";
					break;
				}

				msg = Message::fromString(msgString);

				if (msg.contentLength() == 0)  // receiving complete
				{
					std::cout << "\n--leaving receiveFile";
					break;
				}
			}
			//saveStream.flush();
			saveStream.close();
			pQ_->enQ(msg);  // posting receive completed
			return true;
		}
		//----< reads messages from socket and enQs in rcvQ >--------------

		void operator()(Socket socket)
		{
			pSocket = &socket;
			//while (socket.validState())
			while (true)
			{
				std::string msgString = readMsg(socket);
				if (msgString.length() == 0)
				{
					//std::cout << "\n  invalid message";
					break;
				}
				Message msg = Message::fromString(msgString);
				if (msg.containsKey("verbose"))
				{
					std::cout << "\n  receiving message: \"" + msg.value("command") + "\"";
				}
				if (msg.containsKey("sendingFile"))
				{
					receiveFile(msg);
				}
				else
				{
					pQ_->enQ(msg);
				}
				StaticLogger<1>::write("\n  -- " + clientHandlerName + " RecvThread read message: " + msg.name());
				if (msg.command() == "quit")
					break;
			}
			StaticLogger<1>::write("\n  -- terminating ClientHandler thread");
		}
	private:
		BlockingQueue<Message>* pQ_;
		std::string clientHandlerName;
		Socket* pSocket = nullptr;
	};
}
Comm::Comm(EndPoint ep, const std::string& name) : rcvr(ep, name), sndr(name), commName(name) {}

Context* Comm::getContext() { return &ctx; }

void Comm::start()
{
	BlockingQueue<Message>* pQ = rcvr.queue();
	pCh_ = new ClientHandler(pQ, commName);
	rcvr.start(*pCh_);
	sndr.start();
}

void Comm::stop()
{
	rcvr.stop();
	sndr.stop();
	delete pCh_;
	pCh_ = nullptr;
}

void Comm::postMessage(Message msg)
{
	sndr.postMessage(msg);
}

Message Comm::getMessage()
{
	return rcvr.getMessage();
}

std::string Comm::name()
{
	return commName;
}

std::string Comm::setSendFilePath(const std::string& relPath)
{
	std::string sendFilePath = FileSystem::Path::getFullFileSpec(relPath);
	FileSystem::Directory::create(sendFilePath);
	ctx.sendFilePath = sendFilePath;
	return sendFilePath;
}

std::string Comm::setSaveFilePath(const std::string& relPath)
{
	std::string saveFilePath = FileSystem::Path::getFullFileSpec(relPath);
	FileSystem::Directory::create(saveFilePath);
	ctx.saveFilePath = saveFilePath;
	return saveFilePath;
}

std::string Comm::getSendFilePath()
{
	return ctx.sendFilePath;
}

std::string Comm::getSaveFilePath()
{
	return ctx.saveFilePath;
}

//----< test stub >--------------------------------------------------

#ifdef TEST_COMM

/////////////////////////////////////////////////////////////////////
// Test #1 - Demonstrates Sender and Receiver operations

void DemoSndrRcvr(const std::string& machineName)
{
	Utilities::title("Demonstrating Sender and Receiver classes");

	SocketSystem ss;
	EndPoint ep1;
	ep1.port = 9091;
	ep1.address = "localhost";
	Receiver rcvr1(ep1);
	BlockingQueue<Message>* pQ1 = rcvr1.queue();

	ClientHandler ch1(pQ1);
	rcvr1.start(ch1);

	EndPoint ep2;
	ep2.port = 9092;
	ep2.address = "localhost";
	Receiver rcvr2(ep2);
	BlockingQueue<Message>* pQ2 = rcvr2.queue();

	ClientHandler ch2(pQ2);
	rcvr2.start(ch2);

	Sender sndr;
	sndr.start();
	bool connected = sndr.connect(ep1);
	Message msg;
	msg.name("msg #1");
	msg.to(ep1);
	msg.from(msg.to());
	msg.command("do it");
	msg.attribute("bodyAttrib", "zzz");
	StaticLogger<1>::flush();
	std::cout << "\n  sndr in main posting message:  " << msg.name();
	sndr.postMessage(msg);

	msg.name("msg #2");
	msg.to(EndPoint(machineName, 9092));
	StaticLogger<1>::flush();
	std::cout << "\n  sndr in main posting message:  " << msg.name();
	sndr.postMessage(msg);

	Message rcvdMsg = rcvr1.getMessage();  // blocks until message arrives
	StaticLogger<1>::flush();
	std::cout << "\n  rcvr1 in main received message: " << rcvdMsg.name();
	rcvdMsg.show();

	rcvdMsg = rcvr2.getMessage();  // blocks until message arrives
	StaticLogger<1>::flush();
	std::cout << "\n  rcvr2 in main received message: " << rcvdMsg.name();
	rcvdMsg.show();

	Utilities::title("Sending message to EndPoint that doesn't exist");

	msg.name("msg #3");
	msg.to(EndPoint("DoesNotExist", 1111));  // Unknown endpoint - should fail
	StaticLogger<1>::flush();
	std::cout << "\n  sndr in main posting message:  " << msg.name();
	msg.show();
	sndr.postMessage(msg);                   // will never reach rcvr

	msg.name("msg #4");
	msg.to(EndPoint("localhost", 9091));
	StaticLogger<1>::flush();
	std::cout << "\n  sndr in main posting message:  " << msg.name();
	sndr.postMessage(msg);                  // this should succeed
	StaticLogger<1>::flush();
	rcvdMsg = rcvr1.getMessage();
	std::cout << "\n  rcvr1 in main received message: " << rcvdMsg.name();
	rcvdMsg.show();

	rcvr1.stop();
	rcvr2.stop();
	sndr.stop();
	StaticLogger<1>::flush();

	std::cout << "\n  press enter to quit DemoSndrRcvr";
	_getche();
	std::cout << "\n";
}

/////////////////////////////////////////////////////////////////////
// Test #2 - Demonstrates Comm class using a single thread
//           sending and receiving messages from two Comm
//           instances.

void DemoCommClass(const std::string& machineName)
{
	Utilities::title("Demonstrating Comm class");

	SocketSystem ss;

	EndPoint ep1("localhost", 9191);
	Comm comm1(ep1, "comm1");
	comm1.start();

	EndPoint ep2("localhost", 9192);
	Comm comm2(ep2, "comm2");
	comm2.start();

	// send msg from comm1 to comm1
	Message msg;
	msg.name("msg #1");
	msg.to(ep1);
	msg.from(ep1);
	StaticLogger<1>::flush();
	std::cout << "\n  comm1 in main posting message:   " << msg.name();
	comm1.postMessage(msg);
	msg = comm1.getMessage();
	StaticLogger<1>::flush();
	std::cout << "\n  comm1 in main received message:  " << msg.name();
	msg.show();

	// send msg from comm1 to comm2
	msg.name("msg #2");
	msg.from(ep1);
	msg.to(ep2);
	StaticLogger<1>::flush();
	std::cout << "\n  comm1 in main posting message:  " << msg.name();
	comm1.postMessage(msg);
	msg = comm2.getMessage();
	StaticLogger<1>::flush();
	std::cout << "\n  comm2 in main received message: " << msg.name();
	msg.show();

	// send msg from comm2 to comm1
	msg.name("msg #3");
	msg.to(ep1);
	msg.from(ep2);
	StaticLogger<1>::flush();
	std::cout << "\n  comm2 in main posting message:  " << msg.name();
	comm2.postMessage(msg);
	msg = comm1.getMessage();
	StaticLogger<1>::flush();
	std::cout << "\n  comm1 in main received message: " << msg.name();
	msg.show();

	// send msg from comm2 to comm2
	msg.name("msg #4");
	msg.from(ep2);
	msg.to(ep2);
	StaticLogger<1>::flush();
	std::cout << "\n  comm2 in main posting message:  " << msg.name();
	comm2.postMessage(msg);
	msg = comm2.getMessage();
	StaticLogger<1>::flush();
	std::cout << "\n  comm2 in main received message: " << msg.name();
	msg.show();

	comm1.stop();
	comm2.stop();
	StaticLogger<1>::flush();
	std::cout << "\n  press enter to quit DemoComm";
	_getche();
}
/////////////////////////////////////////////////////////////////////
// Test #3 - Demonstrate server with two concurrent clients
//           sending and receiving messages

//----< handler for first concurrent client >------------------------

void ThreadProcClnt1()
{
	std::cout << Utilities::stitle("Client #1 thread starting");
	Comm comm(EndPoint("localhost", 9891), "client1Comm");
	comm.start();
	EndPoint serverEP("localhost", 9890);
	EndPoint clientEP("localhost", 9891);

	Message fileMsg(serverEP, clientEP);
	fileMsg.name("fileSender");
	///////////////////////////////////////////////////
	// This works too:
	//fileMsg.attribute("sendingFile", "Comm.cpp");
	fileMsg.file("TestDriver1.dll");
	//fileMsg.file("FileSystem.h");
	fileMsg.attribute("verbose", "blah");
	std::cout << "\n  " + comm.name() + " posting:  " + fileMsg.file();
	comm.postMessage(fileMsg);

	size_t IMax = 3;
	for (size_t i = 0; i < IMax; ++i)
	{
		Message msg(serverEP, clientEP);
		msg.name("client #1 : msg #" + Utilities::Converter<size_t>::toString(i));
		std::cout << "\n  " + comm.name() + " posting:  " + msg.name();
		comm.postMessage(msg);
		Message rply = comm.getMessage();
		std::cout << "\n  " + comm.name() + " received: " + rply.name();
		::Sleep(100);
	}
	::Sleep(200);

	Message stop;
	stop.name("stop");
	stop.to(serverEP);
	stop.command("stop");
	comm.postMessage(stop);
}
//----< handler for 2nd concurrent client >--------------------------

void ThreadProcClnt2()
{
	std::cout << Utilities::stitle("Client #2 thread starting");
	Comm comm(EndPoint("localhost", 9892), "client2Comm");
	comm.start();
	EndPoint serverEP("localhost", 9890);
	EndPoint clientEP("localhost", 9892);

	Message fileMsg(serverEP, clientEP);
	fileMsg.value("sendingFile");
	fileMsg.name("fileSender");
	fileMsg.file("comm.cpp");
	fileMsg.attribute("verbose", "blah");
	std::cout << "\n  " + comm.name() + " posting: " + fileMsg.file();
	comm.postMessage(fileMsg);

	size_t IMax = 3;
	for (size_t i = 0; i < IMax; ++i)
	{
		Message msg(serverEP, clientEP);
		msg.name("client #2 : msg #" + Utilities::Converter<size_t>::toString(i));
		std::cout << "\n  " + comm.name() + " posting:  " + msg.name();
		comm.postMessage(msg);
		Message rply = comm.getMessage();
		std::cout << "\n  " + comm.name() + " received: " + rply.name();
	}
	::Sleep(200);
}
//----< server demonstrates two-way asynchronous communication >-----
/*
*  - One server receiving messages and sending replies to
*    two concurrent clients.
*/
void DemoClientServer()
{
	Utilities::title("Demonstrating Client-Server - one server with two concurrent clients");

	//SocketSystem ss;

	EndPoint serverEP("localhost", 9890);
	EndPoint clientEP("localhost", 9891);
	Comm comm(serverEP, "serverComm");
	std::string path = comm.setSaveFilePath("../SaveFiles");
	std::cout << "\n  files will be saved in \"" + path + "\"";
	path = comm.setSendFilePath("../SendFiles");
	std::cout << "\n  files will be sent from \"" + path + "\"";
	comm.start();
	std::thread t1(ThreadProcClnt1);
	t1.detach();
	std::thread t2(ThreadProcClnt2);
	t2.detach();

	Message msg, rply;
	rply.name("reply");
	size_t count = 0;
	while (true)
	{
		// display each incoming message

		msg = comm.getMessage();
		std::cout << "\n  " + comm.name() + " received message: " << msg.name();

		if (msg.containsKey("sendingFile"))  // is this a file message?
		{
			// contentLength() returns zero if all the file blocks have been sent

			if (msg.contentLength() == 0)
				std::cout << "\n  " + comm.name() + " received file \"" + msg.file() + "\" from " + msg.name();
		}
		else  // non-file message
		{
			rply.to(msg.from());
			rply.from(serverEP);
			rply.name("server reply #" + Utilities::Converter<size_t>::toString(++count) + " to " + msg.from().toString());

			comm.postMessage(rply);
			if (msg.command() == "stop")
			{
				break;
			}
		}
	}

	comm.stop();
	StaticLogger<1>::flush();
	std::cout << "\n  press enter to quit DemoClientServer";
	_getche();
}

Cosmetic cosmetic;

int main()
{
	Utilities::Title("Demo of Message-Passing Communication");
	Utilities::putline();

	///////////////////////////////////////////////////////////////////
	// remove comments on lines below to show many of the gory details
	//
	//StaticLogger<1>::attach(&std::cout);
	//StaticLogger<1>::start();

	///////////////////////////////////////////////////////////////////
	// if you uncomment the lines below, you will run all demos

	//DemoSndrRcvr("Odin");  // replace "Odin" with your machine name or localhost
	//DemoCommClass("Odin");
	DemoClientServer();

	return 0;
}
#endif
