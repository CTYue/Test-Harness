#pragma once
#ifndef PROPERTIES_H
#define PROPERTIES_H
/////////////////////////////////////////////////////////////////////
// Properties.h - provides getter and setter methods               //
// ver 1.1                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package provides a single Property<T> class that provides
*  getter and setter methods and backing store T t_ using the property
*  name.
*  - Here's an example:
*      Property<int> intProp;
*      intProp(42)        Sets its internal store to 42
*      intProp()          returns the internal store by value
*      intProp.valueRef() returns the internal store by reference
*  - So one declaration creates three methods with simple syntax, using
*    the property's instance name.
*  - The getter and setter methods are virtual, so you can override
*    them to provide logic that constraints getting and setting.
*
*  Required Files:
*  ---------------
*  Properties.h
*
*  Maintenance History:
*  --------------------
*  ver 1.1 : 23 Oct 2018
*  - added valueRef() to return store by reference - see note
*  ver 1.0 : 11 Oct 2018
*  - first release
*/
namespace Utilities
{
	template <typename T>
	class Property
	{
	public:
		virtual ~Property() {}

		//----< setter assigns the state of t to the internal store >----

		virtual void operator()(const T& t)
		{
			// derived classes put constraining logic here
			t_ = t;
		}
		//----< getter returns the value of the internal store >---------
		/*
		*  - Return by value is used so that clients cannot change the
		*    internal store without using setter.
		*/
		virtual T operator()()
		{
			// derived classes put constraining logic here
			return t_;
		}
		//----< retuns reference to internal value >---------------------
		/*
		*  - use with caution, since caller can change internal store
		*    without using setter.
		*/
		T& valueRef()
		{
			return t_;
		}
	private:
		T t_;
	};
}
#endif
