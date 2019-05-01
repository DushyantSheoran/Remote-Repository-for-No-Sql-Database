#pragma once
/////////////////////////////////////////////////////////////////////////////////
// Definitions.h -    Declares common usings				   				   //
//  ver 1.0                                                                   //
//  Language:         Visual C#  2017                                         //
//  Platform:         Lenovo Z580 Windows 10                                  //
//  Application :     Key/Value DataBase , S18                                //
//  Author      :     Harika Bandaru, Syracuse University                     //
//                    hbandaru@syr.edu (936)-242-5972)						  //
// Source		:     Jim Fawcett, CSE687 - Object Oriented Design, Spring 18 //
////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
namespace NoSqlDb
{

	using Key = std::string;
	using Children = std::vector<Key>;
	using Keys = std::vector<Key>;
	using Parents = std::vector<Key>;
	using FileName = std::string;
	using PersitsPath = std::string;
	using FullPath = std::string;
	using categry = std::string;
	
	const bool showKey = true;
	const bool doNotShowKey = false;
}

