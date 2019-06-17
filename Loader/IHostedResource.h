/////////////////////////////////////////////////////////////////////
// IHostedResource.h - Allow tests to use Host facilities          //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
// Modifier: Zidong Yu zyu107@syr.edu							   //
// Description: This file provides IHostedResource interface which //
// helps host pass resources to client.In this project, we will	   //
// logger facility from Executive package to each test driver	   //
// library.														   //
/////////////////////////////////////////////////////////////////////

#include <string>

/////////////////////////////////////////////////////////////////////
// IHostedResource interface
// - implemented by Host and used by tests
// - see modifications to ITests interface to support this
struct IHostedResource
{
  virtual ~IHostedResource() {}
  virtual void say() = 0;
  virtual void message(const std::string& msg) = 0;
};
