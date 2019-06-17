#pragma once
////////////////////////////////////////////////////////////////////////////
// DllLoader.h : Test-Driver-DLL loader that uses ITests.h as protocole   //
// ver 2.0                                                                //
// Application : OOD F18 Project 2 Help                                   //
// Platform    : VS17 Community - Windows 10 Professional x64             //
// Modified    : Jim Fawcett, CSE687 - OOD, Fall 2018
// Author      : Ammar Salman, EECS Department, Syracuse University       //
//               313/788-4694, hoplite.90@hotmail.com                     //
// Modifier	   : Zidong Yu zyu107@syr.edu								  //
////////////////////////////////////////////////////////////////////////////
/*
*  Package description:
* ======================
*  This package loads Test-Driver DLLs that follow the ITests protocol
*  defined in ITests.h
*
*  The protocol forces all TestDrivers to have an implementation of ITests
*  interface which holds a collection of ITest pointers. This package will
*  attempt to load the DLL, get an instance of ITests* and retrieve the
*  std::vector<ITest*> contained within the DLL.
*
*  This header file contains a ILogger class which helps each test driver
*  creates its own instance of Logger. DllLoader class is also provided to
*  load and extract collection of tests from test libraries (DLLs).
*  Required files:
* =================
*  ITests.h
*  TestUtilities.h
*/

#include "ITests.h"
//#include "../Utilities/TestUtilities.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>

using namespace std;
using namespace Utilities;

class DllLoader
{
public:
	DllLoader();
	~DllLoader();
	bool loadAndExtract(const std::string& dll);
	const std::vector<Utilities::ITest*>& tests() const;
	void clear();  // unloads the DLL (if loaded) and clears all extracted tests

private:
	std::vector<Utilities::ITest*> extractedTests_;  // contains the set of tests extracted from the TestDriver DLL
	HMODULE hmod;  // handle to the loaded DLL
};

bool doTests(DllLoader& loader, Utilities::SingletonLoggerFactory<1, Utilities::NoLock>* pRes);
std::wstring toWstring(const std::string& s);
