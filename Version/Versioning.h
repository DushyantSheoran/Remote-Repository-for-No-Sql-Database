#pragma once
/////////////////////////////////////////////////////////////////////////////////
// Version.h - Implements Version component of the Repository				   //
//  ver 1.0                                                                    //
//  Language:         Visual C#  2017                                          //
//  Platform:         Lenovo Z580 Windows 10                                   //
//  Application :     Software Repository TestBed , S18                        //
// Author : Dushyant Sheoran, CSE687   							               //
//			dsheoran@syr.edu +1(704)-957-2519					               //
/////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides class:
* - Version implements Versioning operations.
*   Retrives all the version numbers of files in repository so will know the
*   latest version number.
*   Append the version number and generate a file which is key to the NoSqlDb
*   Can extract file by removing the version number helps during check-out process
*
* Member functions:
* -------------------
* getVersion    : Returns the version number of the given file
* hasVersion    : Checks whether the given file is already present in
*					repository or not
* removeVersion : Extreacts filename by removing version number
* addVersion	: Appends a version number to the file
* getLatestVersion	: Retrurns the most latest version of the given file in repo
*
*
* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* Utilities.h, Utilities.cpp
* Version.h; Persist.h
* FileSystem --> static library
*
*Build Process:
*---------------------
* CL /EHsc Version.cpp ../FileSystem/FileSystemDemo/FileSystem.cpp
*
* Maintenance History:
* --------------------
* ver 1.0 : Feb 2018
* - first release
*/

#include <iostream>
#include <string>
#include<unordered_map>
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
using namespace FileSystem;
namespace RepositoryService
{

	class Version
	{
	public:

		using FileName = std::string;
		using ChkinDirName = std::string;
		using StagingDirName = std::string;
		using versionNumber = size_t;
		using VersionKey = std::string;
		using VHolder = std::unordered_map<FileName, versionNumber>;
		Version(std::unordered_map<FileName, versionNumber> &vHolder);
		versionNumber getVersion(const FileName& file);
		bool hasVersion(const FileName& file);
		FileName removeVersion(FileName& file);
		FileName addVersion(FileName& file);
		versionNumber getLatestVersion(FileName& file);
		void restoreVersionsFromFiles(FileName &file);
		bool contains(const VersionKey &key);
		static void identity(std::ostream& out = std::cout);
	private:
		std::unordered_map<FileName, versionNumber> &versionHolder;

	};
	//------------< To fill in data in version Holder >-------
	void Version::restoreVersionsFromFiles(FileName& path)
	{
		versionHolder.clear();
		std::vector<FileName> files = Directory::getFiles(path, "*.*");
		for (auto file : files)
		{
			FileName filename_ = Path::getName(file);
			std::string file_nv = removeVersion(filename_);
			VersionKey key_ = file_nv;
			versionNumber versionnum = getVersion(file);
			if (contains(key_))
			{
				if (versionnum > versionHolder[key_])
					versionHolder[key_] = versionnum;
			}
			else {
				versionHolder[key_] = versionnum;
			}
		}
	}

	//----------------< To construct the Version holder >-------
	Version::Version(std::unordered_map<FileName, versionNumber> &vHolder):versionHolder(vHolder)
	{	
		versionHolder.clear();
		StagingDirName path = "../Storage/beforeRepoStart";
		restoreVersionsFromFiles(path);
	}
	inline void Version::identity(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
	//--------------------< Checks if the file has version number >-------
	bool Version::hasVersion(const FileName &file)
	{
		
		int pos = file.find_last_of('.');
		FileName end = file.substr(pos + 1);
		if (isdigit(end[0]))
		{
			return true;
		}
		return false;
	}
	//-------------< Remove version used for the code check-out and comparision >-----
	std::string Version::removeVersion(FileName &file)
	{
	
		int pos = file.find_last_of('.');
		std::string end = file.substr(pos + 1);
		if (isdigit(end[0]))
		{
			return file.substr(0, pos);
		}
		return file;
	}

	//--------------------< Add version number before chek-out >---------
	std::string Version::addVersion(FileName &file)
	{
		VersionKey key_ = removeVersion(file);
		versionNumber versionnum = 0;
		if (contains(key_))
			versionnum = versionHolder[key_];
		versionHolder[key_] = ++versionnum;
		int versionNum = static_cast<int>(versionnum);
		return file + "." + std::to_string(versionNum);
	}
	//-------------------< Get the version number of the given file>------
	Version::versionNumber Version::getVersion(const FileName &file)
	{
		versionNumber ver = 0;
		int pos = file.find_last_of(".");
		FileName verStr = file.substr(pos + 1);
		if (isdigit(verStr[0]))
		{
			return verStr[0] - '0';
		}
		return 0;
	}
	//-----------< To know the latest version of file in the repository>--------
	Version::versionNumber Version::getLatestVersion(FileName &file)
	{
		int temp = 0;
		file = removeVersion(file);
		VersionKey key = file;
		if (contains(key))
			temp = versionHolder[key];
		return temp;
	}
	//-------------------< Checks if present in version holder helper to hasVersion() >------
	bool Version::contains(const VersionKey &file)
	{
		auto itr = versionHolder.find(file);
		if (itr == versionHolder.end())
			return false;
		return true;
	}
}