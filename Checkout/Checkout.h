#pragma once
/////////////////////////////////////////////////////////////////////////////////
// Checkin.h - Implements Check-out component of the Repository				   //
//  ver 1.0                                                                    //
//  Language:         Visual C#  2017                                          //
//  Platform:         Lenovo Z580 Windows 10                                   //
//  Application :     Software Repository TestBed , S18                        //
// Author : Dushyant Sheoran, CSE687   							               //
//			dsheoran@syr.edu +1(704)-957-2519					   			   //
/////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides class:
* - Checkout implements Repository check-out operations.
*   NoSqlDb designed and developed in project #1 will be used to store
*   Repository information such as dependency relation, chkin status, author
*   Here we can walk through the dependency relationship in two ways either
*   through the graph or through the recursive walk
*
*
* Member functions:
* -------------------
* chkoutPath    : This setup the check-out environment for the check-out component
* storagePath   : This sets up the storage environment of check-in
* copyFile		: This copies files from stagging to storage.
* extractFileName : Extracts filename by removing the version number.
* doCheckoutgiven : Uses graph walk to get all the dependency information of
*					a given key as the NoSqlDb is a graph representation of the
*					repository
* doCkecout		: Check-out the file and its dependents to location
*				  provided by the client
* getDependencies : Recursive walk through all the nodes of the given key and
*					thier sub-nodes
*
* Build Process:
*-------------------
*  CL /EHsc Checkput.cpp ../FileSystem/FileSystemDemo/FileSystem.cpp ../DateTime/DateTime.cpp
* ../XmlDocument/XmlDocument/XmlDocument.cpp ../XmlDocument/XmlElement/XmlElement.cpp
* ../XmlDocument/XmlParser/XmlParser.cpp ../XmlDocument/XmlElementParts/XmlElementParts.cpp
* ../XmlDocument/XmlElementParts/Tokenizer.cpp
*
*
* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* Utilities.h, Utilities.cpp
* Version.h; Persist.h
* TraverseUsingGraph.cpp, TraverseUsingGraph.h
* FileSystem --> static library
* XmlDocumnet --> static library
* Datetime --> static library
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Feb 2018
* - first release
*/

#include "../DBCore/Definitions.h"
#include "../DBCore/DBCore.h"
#include "../Version/Versioning.h"
#include "../TraverseUsingGraph/TraverseUsingGraph.h"
//------------< Class declaration >------------------------------------
using namespace NoSqlDb;
namespace RepositoryService {

	template<typename T>
	class Checkout {
	public:
		Checkout(DbCore<T> &db, Version v_) :db_(db), v(v_) {};
		FileName extractFileName(Key key_);
		bool getDependencies(Key key_);
		bool doCheckout(FileName file);

		FullPath& chkoutPath() { return chkoutPath_; }
		FullPath chkoutPath() const { return chkoutPath_; }
		void chkoutPath(const FullPath& strpath) { chkoutPath_ = strpath; }

		FullPath& storagePath() { return storagePath_; }
		FullPath storagePath() const { return storagePath_; }
		void storagePath(const FullPath& chkinpath) { storagePath_ = chkinpath; }

		bool copyFile(FileName destination, FileName source);
		DbCore<T>& fromPersistChkin() { return db_; }
		bool doCheckoutgiven(iterable_queue<Key> itrqueue);


	private:
		DbCore<T> &db_;
		FullPath chkoutPath_;
		FullPath storagePath_;
		Version v;
		Children dependents;
	};
	//-----------------< Extract filename by removing version number >-------------
	template<typename T>
	FileName Checkout<T>::extractFileName(Key key_)
	{
		return v.removeVersion(key_);
	}
	//------------------< Copy File from chkin environment to chkout environment >---------
	template<typename T>
	bool Checkout<T>::copyFile(FileName destination, FileName source)
	{
		std::vector<std::string> splittedName = Utilities::split(source, '.');
		
		if (destination.length() == 0 || source.length() == 0)
			return false;
		if (((storagePath()).length()) == 0 || ((chkoutPath()).length()) == 0)
			return false;
		std::string dest1 = Path::fileSpec(chkoutPath(), destination);
		std::string src1 = Path::fileSpec(storagePath()+'/'+splittedName[0], source);
		return FileSystem::File::copy(src1, dest1, false);
	}
	//-----------< Recursive call to get dependency information >---------
	template<typename T>
	bool Checkout<T>::getDependencies(Key key_)
	{
		if (!db_[key_].children().size() == 0)
			return true;
		for (auto child : db_[key_].children())
		{
			dependents.push_back(child);
			getDependencies(child);
		}
		return true;
	}
	//------------------< Chkout the file and its dependencies >---------
	template<typename T>
	bool Checkout<T>::doCheckout(FileName file)
	{

		if (!db_.contains(file))
			return false;
		dependents.clear();
		dependents.push_back(file);
		bool result = getDependencies(file);
		for (auto chkoutfile : dependents)
		{
			FileName dest = extractFileName(chkoutfile);
			copyFile(dest, chkoutfile);
		}
		return result;
	}
	//-------------------< Walktrough the NoSqlDb graph to get dependecy information>----
	template<typename T>
	bool Checkout<T>::doCheckoutgiven(iterable_queue<Key> filename)
	{
		Key filenam = filename.front();
		if (!db_.contains(filenam))
			return false;
		for (auto file : filename)
		{
			FileName dest = extractFileName(file);
			copyFile(dest, file);
		}
		return true;
	}
}

