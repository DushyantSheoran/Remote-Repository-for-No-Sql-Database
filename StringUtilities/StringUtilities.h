#pragma once
#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H
///////////////////////////////////////////////////////////////////////
// StringUtilities.h - small, generally useful, helper classes       //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2017                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides functions:
* - Title(text)           display title
* - title(text)           display subtitle
* - putline(n)            display n newlines
* - trim(str)             remove leading and trailing whitespace
* - split(str, 'delim')   break string into vector of strings separated by delim char
* - showSplit(vector)     display splits
*
* Required Files:
* ---------------
*   StringUtilities.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 12 Jan 2018
* - first release
* - refactored from earlier Utilities.h
*
* Notes:
* ------
* - Designed to provide all functionality in header file.
* - Implementation file only needed for test and demo.
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>
#include <locale>
#include<algorithm>

namespace Utilities
{


	template<typename T>
	class Convert
	{
	public:
		virtual ~Convert<T>() {}
		static std::string toString(const T& t);
		static T fromString(const std::string& str);
		static std::string id(const T& t);
	};
	/*----< serialize t to a std::string >-----------------------------*/
	/*
	* Assumes that T defines an insertion operator.
	*/
	template<typename T>
	std::string Convert<T>::toString(const T& t)
	{
		std::ostringstream out;
		out.precision(8);
		out << *const_cast<T*>(&t);
		return out.str();
	}
	/*----< create an instance of T from a std::string >---------------*/
	/*
	* Assumes that T defines an extraction operator.
	* To succeed str must be created from Convert<T>::toString(const T& t).
	*/
	template<typename T>
	T Convert<T>::fromString(const std::string& str)
	{
		std::istringstream in(str);
		in.precision(8);
		T value;
		in >> value;    // istringstream extraction provides the conversion
		return value;
	}
	/*----< create an id string from an instance of T using RTTI >----*/

	template<typename T>
	std::string Convert<T>::id(const T& t)
	{
		return typeid(t).name();
	}
	/*----< specialization for std::strings >------------------------*/

	template<>
	inline std::string Convert<std::string>::toString(const std::string& str)
	{
		return str;
	}

	template<>
	inline std::string Convert<std::string>::fromString(const std::string& str)
	{
		return str;
	}
	/////////////////////////////////////////////////////////////////////
	// String Helper functions

	//----< display underlined title >-----------------------------------

	inline void Title(const std::string& text, std::ostream& out = std::cout, char underline = '=')
	{
		out << "\n  " << text;
		out << "\n " << std::string(text.size() + 2, underline);
	}
	//----< display underlined subtitle >--------------------------------

	static inline int isspaceHelper(int ch)
	{
		std::locale loc;
		return std::isspace(ch, loc);
	}
	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),std::not1(std::ptr_fun<int, int>(isspaceHelper))));
		s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());

		return s;
	}
	static inline std::string trimnewLine(std::string &s)
	{
		s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
		return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(),std::not1(std::ptr_fun<int, int>(isspaceHelper))).base(), s.end());
		s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
		return s;
	}

	inline void title(const std::string& text, std::ostream& out = std::cout, char underline = '-')
	{
		out << "\n  " << text;
		out << "\n " << std::string(text.size() + 2, underline)<<"\n";
	}
	inline std::string trim(std::string &s) {
		std::string result= ltrim(rtrim(s));
		return trimnewLine(result);
	}



	//----< display j newlines >-----------------------------------------

	inline void putline(size_t j = 1, std::ostream& out = std::cout)
	{
		for (size_t i = 0; i < j; ++i)
			out << "\n";
	}
	/*--- remove whitespace from front and back of string argument ---*/
	/*
	*  - does not remove newlines
	*/
	template <typename T>
	inline std::basic_string<T> trim(const std::basic_string<T>& toTrim)
	{
		if (toTrim.size() == 0)
			return toTrim;
		std::basic_string<T> temp;
		std::locale loc;
		typename std::basic_string<T>::const_iterator iter = toTrim.begin();
		while (isspace(*iter, loc) && *iter != '\n')
		{
			if (++iter == toTrim.end())
			{
				break;
			}
		}
		for (; iter != toTrim.end(); ++iter)
		{
			temp += *iter;
		}
		typename std::basic_string<T>::reverse_iterator riter;
		size_t pos = temp.size();
		for (riter = temp.rbegin(); riter != temp.rend(); ++riter)
		{
			--pos;
			if (!isspace(*riter, loc) || *riter == '\n')
			{
				break;
			}
		}
		if (0 <= pos && pos < temp.size())
			temp.erase(++pos);
		return trimnewLine(temp);
		
	}
	

	/*--- split sentinel separated strings into a vector of trimmed strings ---*/

	template <typename T>
	inline std::vector<std::basic_string<T>> split(const std::basic_string<T>& toSplit, T splitOn = ',')
	{
		std::vector<std::basic_string<T>> splits;
		std::basic_string<T> temp;
		typename std::basic_string<T>::const_iterator iter;
		for (iter = toSplit.begin(); iter != toSplit.end(); ++iter)
		{
			if (*iter != splitOn)
			{
				temp += *iter;
			}
			else
			{
				splits.push_back(trim(temp));
				temp.clear();
			}
		}
		if (temp.length() > 0)
			splits.push_back(trim(temp));
		return splits;
	}
	/*--- show collection of string splits ------------------------------------*/

	template <typename T>
	inline void showSplits(const std::vector<std::basic_string<T>>& splits, std::ostream& out = std::cout)
	{
		out << "\n";
		for (auto item : splits)
		{
			if (item == "\n")
				out << "\n--" << "newline";
			else
				out << "\n--" << item;
		}
		out << "\n";
	}
}
#endif
