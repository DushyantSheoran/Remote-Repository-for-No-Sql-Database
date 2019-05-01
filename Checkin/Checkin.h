/////////////////////////////////////////////////////////////////////////////////
// Checkin.h - Implements Check-in component of the Repository				   //
//  ver 1.0                                                                    //
//  Language:         Visual C#  2017                                          //
//  Platform:         Lenovo Z580 Windows 10                                   //
//  Application :     Software Repository TestBed , S18                        //
// Author : Dushyant Sheoran, CSE687   							     		   //
//			dsheoran@syr.edu +1(704)-957-2519					   			   //
/////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides class:
* - Chkeckin implements Repository checkin operations.
*   NoSqlDb designed and developed in project #1 will be used to store
*   Repository information such as dependency relation, chkin status, author
*
*
* Member functions:
* -------------------
* staggingPath  : This setup the stagging environment for the check-in component
* storagePath   : This sets up the storage environment for check-in
* copyFile		: This copies files from stagging to storage.
* persistRepos	: This persists the check-in operation through NoSqlDb
* loadMetadata	: This loads the persisted information at the end of the repository
* doCkeckin		: Check-in the file provided by the client
* addDependencies : Add the depencies of the file and stores the depdnecy relation
*					in NoSql database
*

* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* DateTime.h, DateTime.cpp
* Utilities.h, Utilities.cpp
* Version.h; Persist.h
* FileSystem --> static library
* XmlDocumnet --> static library
* Datetime --> static library
*
*
* Build Process:
*------------------
* CL /EHsc Checkin.cpp ../FileSystem/FileSystemDemo/FileSystem.cpp ../DateTime/DateTime.cpp
* ../XmlDocument/XmlDocument/XmlDocument.cpp ../XmlDocument/XmlElement/XmlElement.cpp
* ../XmlDocument/XmlParser/XmlParser.cpp ../XmlDocument/XmlElementParts/XmlElementParts.cpp
* ../XmlDocument/XmlElementParts/Tokenizer.cpp
*
*
* Maintenance History:
* --------------------
* ver 1.0 : Feb 2018
* - first release
*/

#pragma once
#include<iostream>
#include<string>
#include "../DBCore/DBCore.h"
#include "../Persist/Persist.h"
#include "../Version/Versioning.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../PayLoad/PayLoad.h"
#include <experimental/filesystem>
#include <cstdio>    
#include<Windows.h>
#include "../DBCore/Definitions.h"
#include "../Display/Display.h"
#include "../TraverseUsingGraph/TraverseUsingGraph.h"
//namespace fs = std::experimental::filesystem;



namespace RepositoryService {

	template<typename T>
	class Checkin {
	public:

		Checkin(NoSqlDb::DbCore<T> &db, Version v_,Children &cloPend) :db_(db), v(v_), closePending_(cloPend){};
		FullPath& staggingPath() { return staggingDir; }
		std::string staggingPath() const { return staggingDir; }
		void staggingPath(const FullPath& path) { staggingDir = path; }

		FullPath& storagePath() { return storageDir_; }
		FullPath storagePath() const { return storageDir_; }
		void storagePath(const FullPath& strpath) { storageDir_ = strpath; }

		bool copyFile(FileName destination, FileName source);
		bool persistRepos(const Xml &xml);

		NoSqlDb::DbCore<T>& loadMetadata(FileName file, PersitsPath storepath);
		bool doCkeckin(const DbElement<T> &elem, FileName key_);
		bool addDependencies(FileName file, Children dependents);
		void displayNotAddedDependents(const Children& c);
		bool closeChkin(const Key& filename);
		bool mutualDependent(const Key & filename);
		bool getDependencies(const Key & key_, const Key& filename);
		bool chkopenchkins(const Key &key_);
		NoSqlDb::DbCore<T>& toPersistChkin() { return db_; }
		void closeAllClosePending(const Key& filename);
		bool getallFilesStagging();
		Children& closePending() { return closePending_; }
		Children closePending() const { return closePending_; }
		void closePending(const Children& closePend) { closePending_ = closePend; }
		void showRepoStructure();
		bool showFailure(std::string,std::string);
	private:
		NoSqlDb::DbCore<T> &db_;
		FullPath staggingDir;
		FullPath storageDir_;
		Version v;
		iterable_queue<Key> dependents;
		Children files;
		Children &closePending_;
	};

	
	template<typename T>
	bool Checkin<T>::getallFilesStagging()
	{
		files = Directory::getFiles(staggingDir, "*.*");
		return true;
	}
	//------< loadMEtadata should load the persisted data to Db given file location >-----
	template<typename T>
	NoSqlDb::DbCore<T> & Checkin<T>::loadMetadata(FileName file, PersitsPath Dbstore)
	{
		std::string fName = FileSystem::Path::fileSpec(Dbstore, file);
		Persist<T> persist(db_);
		if (persist.loadfromFile(fName))
			return db_;
	}

	//-----------------< Copies files from stagging environment to the storage environemnt >------
	template<typename T>
	bool Checkin<T>::copyFile(FileName destination, FileName source)
	{
		std::vector<std::string> splittedName = Utilities::split(source, '.');
		if (!Directory::exists(Path::getPath(splittedName[0]))) {}
		Directory::create(storagePath() + '/' + splittedName[0]);
		if (destination.length() == 0 || source.length() == 0)
			return false;
		if (((storagePath()).length()) == 0 || ((staggingPath()).length()) == 0)
			return false;
		try {
			std::string dest1 = FileSystem::Path::fileSpec(storagePath()+'/'+splittedName[0], destination);
			std::string src1 = FileSystem::Path::fileSpec(staggingPath(), source);
			return FileSystem::File::copy(src1, dest1, false);
		}
		catch (std::exception &ex) {
			std::cout << "\n    exception thrown:";
			std::cout << "\n    " << ex.what();
			std::cout << "\nfailed <<<<\n";
			return false;
		}

	}
	template<typename T> 
	bool Checkin<T>::showFailure(std::string key1_,std::string name) {
		Display<T> dis(db_);
		Utilities::title("\nTthe Check-in failed because of Violation of Single OwnerShip Policy see below for details");
		dis.show(key1_, false);
		std::cout << "Author trying to check-in is::" << name;
		return true;
	}
	
	//----------< Check-in function of the checkin component >--------
	template<typename T>
	bool Checkin<T>::doCkeckin(const DbElement<T> &elem, FileName key_)
	{
		getallFilesStagging();
		if (std::find(files.begin(), files.end(), key_) == files.end())
			return false;
		int ver = v.getLatestVersion(key_);
		Display<T> dis(db_);
		FileName versioned;
		FileName key1_;
		key1_ = key_ + "." + std::to_string(ver);
		if (db_.contains(key1_))
		{
			if (db_[key1_].name().compare(elem.name()) != 0)
			{
				showFailure(key1_,elem.name());
				return false;
			}
			T p1 = db_[key1_].payLoad();
			if ((p1.isClosed().compare("closed")) == 0 || ((p1.isClosed().compare("ClosePending")) == 0)) {
				versioned = v.addVersion(key_);
				db_.insertRecord(versioned, elem);
			}
			else
			{
				versioned = key1_;
				db_.updateRecord(key1_, elem);
			}	
		}
		else
		{
			versioned = v.addVersion(key_);
			db_.insertRecord(versioned, elem);
		}
		if (elem.payLoad().isClosed().compare("closed") == 0)
		{
			if (!closeChkin(versioned))
			{
				dis.show(versioned, false);
				db_.deletedbRecord(versioned);
				std::cout << "The above Check-in failed as trying to close the open dependents";
				return false;
			}
		}
		if (copyFile(versioned, key_))
		{
			dis.show(versioned, false);
			return true;
		}
		else
			return false;
	}

	//---------< Add dependencies checking if the file is closed or not if not closed not 
	//			adding to dependency reltion>--------
	template<typename T>
	bool Checkin<T>::addDependencies(FileName file, Children dependents)
	{
		Children notAdded;
		if (!db_.contains(file))
			return false;
		for (auto depe : dependents)
		{
			if (!db_.contains(depe))
				notAdded.push_back(depe);
			else
				db_[file].children().push_back(depe);
		}
		dependents.clear();
		if (notAdded.size() > 0)
			displayNotAddedDependents(notAdded);
		return !(notAdded.size() > 0);
	}
	//---------------< Helper function for addition of depedencies> ----------
	template<typename T>
	void Checkin<T>::displayNotAddedDependents(const Children &notDepen)
	{

		std::cout << "\nFollwing Dependencies not added because they are not in repo\n";
		for (auto child : notDepen)
			std::cout << child << "\n";
	}
	//------< Persisting the repo operations into file in Xml format at the end>-----
	template<typename T>
	bool Checkin<T>::persistRepos(const Xml& xml)
	{
		FileName file = FileSystem::Path::fileSpec(storagePath(), "persistDb");

		std::ofstream out(file);
		try {
			if (!out.good())
				return false;
		}
		catch (std::exception const &e)
		{
			std::cout << "Caught an exception regarding the file" << e.what() << "\n";
		}
		out << xml;
		out.close();
		return true;
	}
	template<typename T>
	bool Checkin<T>::getDependencies(const Key& key_, const Key& filename)
	{

		using Sptr1 = DbElement<T>;
		auto proc = [=](DbElement<T>, Key) { std::cout << ""; };
		using P = decltype(proc);
		using V = DbElement<T>;

		TraverseUsingGraph<V, P, T> travers(db_);
		Sptr1 pNode = travers.find(key_);
		dependents = travers.walker<iterable_queue<Key>>(DFS_q<P, T>, pNode, proc, db_, filename);
		return true;
	}

	//-------------------< Closing chheckin by taking care of mutual dependents >-----------
	template<typename T>
	bool Checkin<T>::mutualDependent(const Key & filename)
	{
		if (!db_.contains(filename))
			return false;
		if (std::find(dependents.begin(), dependents.end(), filename) != dependents.end())
			return true;
		else
			return false;
	}
	template<typename T>
	bool Checkin<T>::chkopenchkins(const Key &filename)
	{
		iterable_queue<Key> empty;
		std::swap(dependents, empty);
		getDependencies(filename, "");
		for (auto depe : dependents)
		{
			if (depe.compare(filename) == 0)
				continue;
			if (!((db_[depe].payLoad().isClosed()).compare("open")))
				return true;
				
		}
		return false;
	}
	template<typename T>
	void Checkin<T>::showRepoStructure()
	{
		Utilities::title("     Repository Structure     ");
		Display<T> dis(db_);
		dis.showAll();
	}
	template<typename T>
	void Checkin<T>::closeAllClosePending(const Key& filename)
	{
		Children closePend = closePending();
		Display<T> dis(db_);
		bool chkflag;
		if (closePend.size() == 0)return;
		for (auto pend : closePend)
		{
			getDependencies(pend, "");
			chkflag = false;
			for (auto dep : dependents)
			{
				if (((db_[dep].payLoad()).isClosed()).compare("open")==0)
				{
					chkflag = true; break;
				}
			}
			if (!chkflag) {
				db_[pend].payLoad().isClosed("closed");
				dis.show(pend, false);
			}
		}
		showRepoStructure();
		return;
	}
	template<typename T>
	bool Checkin<T>::closeChkin(const Key& filename)
	{
		T payload;
		bool mutualDependents = false, openChkins = false;

		if (db_.contains(filename))
		{
			payload = db_[filename].payLoad();
			if (chkopenchkins(filename))
			{
				if (mutualDependent(filename)) {
					payload.isClosed("ClosePending");
					std::cout << "\n" << "Please change status of check-in to closed after all mutual"
						<< " dependents are closed";
					closePending_.push_back(filename);
					db_[filename].payLoad(payload);
					return true;
				}
				else
					return false;
			}
			
			payload.isClosed("closed");
			db_[filename].payLoad(payload);
			closeAllClosePending(filename);
			return true;
		}
		return false;
	}
}
