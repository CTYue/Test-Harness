///////////////////////////////////////////////////////////////////////////
// Message.cpp - defines message structure used in communication channel //
// ver 1.3                                                               //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017          //
///////////////////////////////////////////////////////////////////////////

#include "Message.h"
#include <iostream>

using namespace MsgPassingCommunication;

//----< default constructor results in Message with no attributes >----

Message::Message() {}

//----< constructor accepting dst and src addresses >------------------

Message::Message(EndPoint to, EndPoint from)
{
  attributes_["to"] = to.toString();
  attributes_["from"] = from.toString();
}
//----< returns reference to Message attributes >----------------------

Message::Attributes& Message::attributes()
{
  return attributes_;
}
//----< adds or modifies an existing attribute >-----------------------

void Message::attribute(const Key& key, const Value& value)
{
  attributes_[key] = value;
}
//----< clears all attributes >----------------------------------------

void Message::clear()
{
  attributes_.clear();
}
//----< returns vector of attribute keys >-----------------------------

Message::Keys Message::keys()
{
  Keys keys;
  keys.reserve(attributes_.size());
  for (auto kv : attributes_)
  {
    keys.push_back(kv.first);
  }
  return keys;
}
//---< Check if the message has key or not >-----------------------------------

bool Message::containsKey(const Key& key)
{
  if (attributes_.find(key) != attributes_.end())
    return true;
  return false;
}
//----< remove attribute with this key >-------------------------------

bool Message::remove(const Key& key)
{
  if (containsKey(key))
  {
    attributes_.erase(key);
    return true;
  }
  return false;
}
//----< return value of specified key >--------------------------------

Message::Value Message::value(const Key& key)
{
  for (auto item : attributes_)
  {
    if (item.first == key)
      return item.second;
  }
  return "";
}

//----< return a set of DLLs included in the message >--------------------------------

std::vector<Message::Value> Message::dll_value(const Key& key)
{
	std::vector<Message::Value> result;
	for (auto item : attributes_)
	{
		if (item.first.find(key) != std::string::npos)
			result.push_back(item.second);
	}
		return result;
}


//----< Check if the message contains file or not >--------------------------------

bool Message::containFile()
{
	bool result = false;
	for (auto item : attributes_)
	{
		if (item.first.find("sendingFile") != std::string::npos)//file exits
			result |= true;
	}
	return result;
}

//----< get to attribute >---------------------------------------------

EndPoint Message::to()
{
  if (containsKey("to"))
  {
    return EndPoint::fromString(attributes_["to"]);
  }
  return EndPoint();
}
//----< set to attribute >---------------------------------------------

void Message::to(EndPoint ep)
{
  attributes_["to"] = ep.toString();
}
//----< get from attribute >-------------------------------------------

EndPoint Message::from()
{
  if (containsKey("from"))
  {
    return EndPoint::fromString(attributes_["from"]);
  }
  return EndPoint();
}
//----< set from attribute >-------------------------------------------

void Message::from(EndPoint ep)
{
  attributes_["from"] = ep.toString();
}
//----< get name attribute >-------------------------------------------

std::string Message::name()
{
  if (containsKey("name"))
  {
    return attributes_["name"];
  }
  return "";
}
//----< set name attribute >-------------------------------------------

void Message::name(const std::string& nm)
{
  attributes_["name"] = nm;
}
//----< get command attribute >----------------------------------------

std::string Message::command()
{
  if (containsKey("command"))
  {
    return attributes_["command"];
  }
  return "";
}
//----< set command attribute >----------------------------------------

void Message::command(const std::string& cmd)
{
  attributes_["command"] = cmd;
}
//----< get file name attribute >--------------------------------------

std::string Message::file()
{
  if (containsKey("sendingFile"))
  {
    return attributes_["sendingFile"];
  }
  return "";
}
//----< set file name attribute >--------------------------------------

void Message::file(const std::string& fl)
{
  attributes_["sendingFile"] = fl;
}


//----< get body length >----------------------------------------------

size_t Message::contentLength()
{
	if (containsKey("content-length"))
	{
		std::string lenStr = attributes_["content-length"];
		return Utilities::Converter<size_t>::toValue(lenStr);
	}
	return 0;
}
//----< set body length >----------------------------------------------

void Message::contentLength(size_t ln)
{
	attributes_["content-length"] = Utilities::Converter<size_t>::toString(ln);
}
//----< convert message to string representation >---------------------

std::string Message::toString()
{
  std::string temp;
  for (auto kv : attributes_)
  {
    temp += kv.first + ":" + kv.second + "\n";
  }
  return temp + "\n";
}

//----< extracts name from attribute string >--------------------------

Message::Key Message::attribName(const Attribute& attrib)
{
  size_t pos = attrib.find_first_of(':');
  if (pos == attrib.length())
    return "";
  return attrib.substr(0, pos);
}
//----< extracts value from attribute string >-------------------------

Message::Value Message::attribValue(const Attribute& attrib)
{
  size_t pos = attrib.find_first_of(':');
  if (pos == attrib.length())
    return "";
  return attrib.substr(pos + 1, attrib.length() - pos);
}

//----< return test library numbers contained in each message >-----------

size_t Message::dlls_size()
{
	return dlls_.size();
}

//----< return test library names contained in each message >-----------

std::vector<std::string> Message::dll_name()
{
	return dlls_;
}


//----< creates message from message representation string >-----------

Message Message::fromString(const std::string src)

{
	Message msg;
	std::vector<std::string> splits = Utilities::split(src, '\n');
	for (Attribute attr : splits)
	{
		if (attribName(attr) != "")
			msg.attributes()[attribName(attr)] = attribValue(attr);
	}
	return msg;
}

//----< creates message from an existing Test Request >-----------
void Message::fromTestRequest(Testing::TestRequest &src)
{
	std::string tq_content = src.toString();
  std::vector<std::string> splits = Utilities::split(tq_content,',');
  if (splits.size() < 4)
  {
	  std::cout << "Invalid test request: add at least one test library." << std::endl;
	  return;
  }
  attributes_["name"] = splits[0];
  attributes_["author"] = splits[1];
  attributes_["time"] = splits[2];
  attributes_["command"] = "testReq";
  if (splits.size() == 4)
	  attributes_["sendingFile"] = splits[3];
  if (splits.size() > 4)
  {
	  for (int i = 3; i < splits.size(); i++)
	  {
		  dlls_.push_back(splits[i]);
		  std::cout << "dll name: " << splits[i] << " pushed." << std::endl;
	  }
  }
}


//----< displays message on std::ostream >-----------------------------
/*
*  - adds beginning newline and removes trailing newline
*  - by default stream is std::cout
*  - can be replaced by std::ostringstream to get display string
*/
std::ostream& Message::show(std::ostream& out)
{
  std::string temp = toString();  // convert this message to string
  size_t pos = temp.find_last_of("\n\n");
  if (pos < temp.size())
  {
    temp[pos] = '\0';  // remove last newline
  }
  out << "\n" << temp; // prepend newline
  return out;
}
//----< test stub >----------------------------------------------------
//#define TEST_MESSAGE

#ifdef TEST_MESSAGE

int main()
{
  Utilities::Title("Testing Message Class");

  Utilities::title("testing endpoints");
  EndPoint ep("localhost", 8080);
  std::cout << "\n  address = " << ep.address;
  std::cout << "\n  port = " << ep.port;
  std::string epStr = ep.toString();
  std::cout << "\n  " << epStr;

  EndPoint newEp = EndPoint::fromString(epStr);
  std::cout << "\n  " << newEp.toString();
  Utilities::putline();

  Utilities::title("testing messages");
  Utilities::putline();

  Utilities::title("creating message from Message::methods");
  Message msg;
  msg.name("msg#1");
  msg.to(EndPoint("localhost", 8080));
  msg.from(EndPoint("localhost", 8081));
  msg.command("doIt");
  msg.contentLength(42);
  msg.file("someFile");
  msg.show();

  Utilities::title("testing Message msg = fromString(msg.toString())");
  Message newMsg = Message::fromString(msg.toString());
  newMsg.show();

  Utilities::title("retrieving attributes from message");
  std::cout << "\n  msg name          : " << newMsg.name();
  std::cout << "\n  msg command       : " << newMsg.command();
  std::cout << "\n  msg to            : " << newMsg.to().toString();
  std::cout << "\n  msg from          : " << newMsg.from().toString();
  std::cout << "\n  msg file          : " << newMsg.file();
  std::cout << "\n  msg content-Length: " << newMsg.contentLength();
  Utilities::putline();

  Utilities::title("adding custom attribute");
  newMsg.attribute("customName", "customValue");
  newMsg.show();

  Utilities::title("testing assignment");
  Message srcMsg;
  srcMsg.name("srcMsg");
  srcMsg.attribute("foobar", "feebar");
  srcMsg.show();
  std::cout << "\n  assigning srcMsg to msg #1";
  newMsg = srcMsg;
  newMsg.show();

  std::cout << "\n\n";
  return 0;
}
#endif
