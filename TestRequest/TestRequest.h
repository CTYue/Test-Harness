#pragma once
/////////////////////////////////////////////////////////////////////
// TestRequest.h - Creates and parses Test Requests                //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  TestRequest class instances create and parse TestRequests.
*  Each TestRequest instance holds:
*  - name:    name of test request for display
*  - author:  author of test request
*  - date:    date test request was created
*  - one or more dll names
*
*  Required Files:
*  ---------------
*  TestRequest.h, TestRequest.cpp
*  Properties.h, Properties.cpp
*  DateTime.h, DateTime.cpp
*  StringUtilities.h, StringUtilities.cpp
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 23 Oct 2018
*  - first release
*/
#ifndef TESTREQUEST_H
#define TESTREQUEST_H
#include <iostream>
#include <string>
#include <vector>
#include "Properties.h"
#include "../Logger/DateTime.h"
#include "../Utilities/StringUtilities.h"
#include "../Utilities/Utilities.h"


namespace Testing
{
  using Name = std::string;
  using Dll = std::string;
  using Request = std::vector<Dll>;
  using Author = std::string;
  using Date = std::string;
  using namespace Utilities;

  class TestRequest 
  {
  public:
    Property<Name> name;
    Property<Request> request;//
    Property<Author> author;
    Property<Date> date;
    void addDll(const Dll& dll);
    std::string toString();
    TestRequest fromString(const std::string& trStr);
  };

	//----< add DLL name to request >----------------------------------

	inline void TestRequest::addDll(const Dll& dll)
	{
		request.valueRef().push_back(dll);
	}
	//----< serialize TestRequest to std::string >---------------------
	/*
	* - builds string holding comma separated list of TestRequest parts
	*/
	inline std::string TestRequest::toString()
	{
		std::string temp(name());
		temp += ", " + author() + ", " + date();
		for (auto dll : request())
		{
			temp += ", " + dll;
		}
		return temp;
	}
	//----< create TestRequest from serialized std::string >-----------
	/*
	* - Constructs TestRequest from a string previously created
	*   by a TestRequest instance.
	*/
	inline TestRequest TestRequest::fromString(const std::string& trStr)
	{
		TestRequest tr;
		std::vector<std::string> splits = Utilities::split(trStr);
		if (splits.size() < 3)
			return tr;
		tr.name(splits[0]);
		tr.author(splits[1]);
		tr.date(splits[2]);

		for (size_t i = 3; i < splits.size(); ++i)
		{
			tr.request.valueRef().push_back(splits[i]);
		}
		return tr;
	}
	//----< construction test for TestRequest >------------------------

}
#endif