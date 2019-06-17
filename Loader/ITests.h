#ifndef ITESTS_H
#define ITESTS_H
////////////////////////////////////////////////////////////////////////////
// ITests.h    : Protocol interface to load dlls in a unified manner      //
// ver 1.1                                                                //
//                                                                        //
// Application : OOD F18 Project 2 Help                                   //
// Platform    : VS17 Community - Windows 10 Professional x64             //
// Modified    : Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018  //
// Author      : Ammar Salman, EECS Department, Syracuse University       //
//               313/788-4694, hoplite.90@hotmail.com                     //
// Modifier    : Zidong Yu zyu107@syr.edu								  //
////////////////////////////////////////////////////////////////////////////
/*
*  Package description:
* ======================
*  This package defines two interfaces that allow DllLoader to load test
*  driver DLL files and extract the test information contained within.
*
*  The interfaces act as protocol in which DllLoader does NOT have to know
*  anything about the incoming DLL. DllLoader is only concerned with the
*  interface ITests which contains a collection of ITest pointers. 
*
*  If the loaded DLL contains definitions for the interfaces below indicating
*  it follows the protocole and hence the test(s) it contains will be loaded.
*  Otherwise, the DLL will be rejected.
*
*  This implies that any test driver DLL will have to follow this protocole
*  in order for DllLoader to accept its defined tests. See sample test
*  drivers: TestDriver1, TestDriver2
*
*  There are two interfaces defined here:
*  ITest  : the interface of which all tests must implement.
*  ITests : collection of ITest pointers. <== this is what DlLLoader is concerned with
*
*  ITest is similar to the interface developed in Project 1 as it provides
*  bool test() function which executes the test and returns the result. I
*  have added information about the test name and author in the interface
*  to make test information irrelivant to the DllLoader and TestHarness 
*  packages. This way, the test driver DLL contains all of the test info.
*
*  ITests is meant to allow a single DLL to contain multiple tests. This way, 
*  the test driver DLL can define a set of ITest derived classes and then return
*  all of them as a collection which can be loaded into DllLoader at once.
*  
*  
*  Usage:
* =================
*  The intention is to unify all TestDrivers into one form. Therefore, the
*  recommended way of making a test driver would be:
*  1) Define the Test classes (each implements ITest interface).
*  2) Define the TestCollection class which implements ITests. 
*  3) In TestCollection::tests() function, add all of the Test classes
*     defined in Step1 to the vector<ITest*> (see TestDriver1 and TestDriver2).
*  4) In get_ITests(): return new TestCollection
*
*
*  Required files:
* =================
*  LoggerHeader.h
*  IHostedResource.h
*  Maintainence History:
* =======================
*  ver 1.1 : 07 Oct 2018
*  - added means to pass Hosted Resource pointer to tests
*  ver 1.0 - first release
*/

// export interfaces functions for DllLoader to use
#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif
#include "../Logger/SingletonLoggerFactory.h"
#include <string>
#include <vector>

namespace Utilities
{
	// ITest interface - modified from project1 ITest to contain full info about the test
	class ITest {
	public:
		virtual DLL_DECL bool test() = 0;
		virtual DLL_DECL std::string name() = 0;
		virtual DLL_DECL std::string author() = 0;
		// provides means for host to pass a reference to its hosted resource
		virtual void acceptHostedResource(Utilities::SingletonLoggerFactory<1, Utilities::NoLock>* pRes) = 0;
	};

	// ITests interface - return a collection of ITest pointers
	class ITests {
	public:
		virtual DLL_DECL std::vector<ITest*> tests() = 0;
	};

	// different TestDriver DLLs will contain different implementations of get_ITests()
	// however, the interface is still the same which allows DllLoader to handle all of
	// the different TestDrivers the same way and extract their tests without knowing
	// ANY test-specific information.
	extern "C" {
		DLL_DECL ITests* get_ITests();
	}
}
#endif
