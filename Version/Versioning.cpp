/////////////////////////////////////////////////////////////////////
// Version.cpp : Version component for Software Repository Testbed //
// ver 1.0														   //
// Author : Dushyant Sheoran, CSE687   							   //
//			dsheoran@syr.edu +1(704)-957-2519					   //
/////////////////////////////////////////////////////////////////////

#pragma once
#include<iostream>
#include<sstream>
#include<string>
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "Versioning.h"
#include <unordered_map>

using namespace RepositoryService;
using namespace FileSystem;

//---------------< Test-stub >------------------
#ifdef Version_Test
int main()
{
	std::unordered_map<std::string, size_t> vHold;
	Version v(vHold);

	Version::FileName f1 = "Help.cpp";
	f1 = v.addVersion(f1);
	std::cout << "After Adding version to file\t" << f1;
	std::string f2 = "testFile1.cs";
	f2 = v.addVersion(f2);
	std::cout << "\nAfter adding version to file\t" << f2;
	std::cout << "\n latest version\t" << v.getLatestVersion(f2);
	std::cout << "\n Remve version \t" << v.removeVersion(f1);

	return 0;
}

#endif 