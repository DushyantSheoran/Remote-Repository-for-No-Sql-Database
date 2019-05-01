
/////////////////////////////////////////////////////////////////////////////////
// DbCore.h - Implements NoSql database				   						   //
//  ver 1.0                                                                   //
//  Language:         Visual C#  2017                                         //
//  Platform:         Lenovo Z580 Windows 10                                  //
//  Application :     Key/Value DataBase , S18                                //
// Author : Dushyant Sheoran, CSE687   							              //
//			dsheoran@syr.edu +1(704)-957-2519					              //
// Source		:     Jim Fawcett, CSE687 - Object Oriented Design, Spring 18 //
////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides two classes:
* - DbCore implements core NoSql database operations.
*   DbElement provides the value part of our key-value database.
*   It contains fields for name, description, date, child collection
*   and a payload field of the template type which contains filePath and category information
*	The package also provides functions for displaying:
* - set of all database keys
* - database elements
* - all records in the database

* Build Process:
---------------------
*  CL /EHsc Checkin.cpp  ../DateTime/DateTime.cpp ../StringUtilitites/StringUtilities.cpp
*
*
* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* DateTime.h, DateTime.cpp
* Utilities.h, Utilities.cpp
*
* Maintenance History:
* --------------------
* ver 1.1 : 19 Jan 2018
* - added code to throw exception in index operators if input key is not in database
* ver 1.0 : 10 Jan 2018
* - first release
*/

#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "../DateTime/DateTime.h"
#include<vector>pa#include<iterator>
#include "Definitions.h"
namespace NoSqlDb
{
	/////////////////////////////////////////////////////////////////////
	// DbElement class
	// - provides the value part of a NoSql key-value database

	template<typename T>
	class DbElement
	{
	public:
		// methods to get and set DbElement fields

		std::string& name() { return name_; }
		std::string name() const { return name_; }
		void name(const std::string& name) { name_ = name; }

		std::string& descrip() { return descrip_; }
		std::string descrip() const { return descrip_; }
		void descrip(const std::string& name) { descrip_ = name; }

		DateTime& dateTime() { return dateTime_; }
		DateTime dateTime() const { return dateTime_; }
		void dateTime(const DateTime& dateTime) { dateTime_ = dateTime; }

		Children& children() { return children_; }
		Children children() const { return children_; }
		void children(const Children& children) { children_ = children; }
		void insertChildren(const Key k);
		T& payLoad() { return payLoad_; }
		T payLoad() const { return payLoad_; }
		void payLoad(const T& payLoad) { payLoad_ = payLoad; }
		bool removeChildKey(const Key& key);
		bool containsChildKey(const Key& key);
	private:
		std::string name_;
		std::string descrip_;
		DateTime dateTime_;
		Children children_;
		T payLoad_;
	};

	/////////////////////////////////////////////////////////////////////
	// DbCore class
	// - provides core NoSql db operations
	// - does not provide editing, querying, or persistance operations

	template <typename T>
	class DbCore
	{
	public:
		using DbStore = std::unordered_map<Key, DbElement<T>>;
		using iterator = typename DbStore::iterator;

		// methods to access database elements

		Keys keys();
		bool contains(const Key& key);
		size_t size();
		void throwOnIndexNotFound(bool doThrow) { doThrow_ = doThrow; }
		DbElement<T>& operator[](const Key& key);
		DbElement<T> operator[](const Key& key) const;
		typename iterator begin() { return dbStore_.begin(); }
		typename iterator end() { return dbStore_.end(); }

		void keys(const Keys& keys) { keys_ = keys; }
		void keys(Keys & keys) { keys_ = keys; }
		Keys keys1() { return keys_; }
		// methods to get and set the private database hash-map storage

		DbStore& dbStore() { return dbStore_; }
		DbStore dbStore() const { return dbStore_; }
		void dbStore(const DbStore& dbStore) { dbStore_ = dbStore; }
		bool deletedbRecord(const Key k);

		bool editMetadata(const Key& k, const DbElement<T>& el);
		bool editPayLoad(const Key &k, const T &p);
		bool editingRelation(const Key &k, std::vector<std::string> child, bool addDelete);
		bool insertRecord(const Key &k, const DbElement<T> &el);
		bool updateRecord(const Key &k, const DbElement<T> &el);
		Parents parents(const Key& key);
	
	private:
		DbStore dbStore_;
		bool doThrow_ = false;
		Keys keys_;
	};

	/////////////////////////////////////////////////////////////////////
	// DbCore<T> methods

	//----< does db contain this key? >----------------------------------

	template<typename T>
	void DbElement<T>::insertChildren(const Key key)
	{
		children().push_back(key);
	}
	//---------------< to avoid adding keys if not present check if key contains >------------
	template<typename T>
	bool DbCore<T>::contains(const Key& key)
	{
		iterator iter = dbStore_.find(key);
		return iter != dbStore_.end();
	}
	//--------------< Insert a new record to the already present database >---------------------
	template<typename T>
	bool DbCore<T>::insertRecord(const Key& key, const DbElement<T> &el)
	{
		if (contains(key))
			return false;
		else
			dbStore_[key] = el;
		return true;
	}

	//----------------< Update a record with new payload or DbElement instance >-----------
	template<typename T>
	bool DbCore<T>::updateRecord(const Key& key, const DbElement<T> &el)
	{
		if (contains(key)) {
			dbStore_[key] = el;
			return true;
		}
		return false;
	}
	template<typename T>
	bool DbElement<T>::containsChildKey(const Key& key)
	{
		Keys::iterator start = children_.begin();
		Keys::iterator end = children_.end();
		return std::find(start, end, key) != end;
	}
	//-----------< Remove a key if added by mistake >-------------------
	template<typename T>
	bool DbElement<T>::removeChildKey(const Key& key)
	{
		size_t keyCount = children_.size();
		std::vector<Key>::iterator start = children_.begin();
		std::vector<Key>::iterator end = children_.end();
		std::vector<Key>::iterator found = std::find(start, end, key);
		if (found == end)
			return false;
		found = children_.erase(found);
		return keyCount == children_.size() + 1;
	}

	//----< find all parents of record index by key >--------------------

	template<typename T>
	Parents DbCore<T>::parents(const Key& key)
	{
		Parents parents;
		for (auto item : dbStore_)
		{
			if (item.second.containsChildKey(key))
				parents.push_back(item.first);
		}
		return parents;
	}

	//------------------< Delete a record from database >----------------------------------
	template<typename T>
	bool DbCore<T>::deletedbRecord(const Key key)
	{
		size_t numErased = dbStore_.erase(key);
		Parents parents = this->parents(key);
		for (auto dbKey : parents)
		{
			dbStore_[dbKey].removeChildKey(key);
		}
		return numErased > 0;

	}
	//----------------< Utility function for Requirement #5 >---------------------
	bool isSubsetOrEqual(std::vector<std::string> const& keys, std::vector<std::string> const& child) {
		for (auto const& av : keys) {
			if (std::find(child.begin(), child.end(), av) == child.end())
				return false;
		}
		return true;
	}


	/* Add relation only if keys are present in the dbStore to avoid creation of dangling pointers*/
	template<typename T>
	bool DbCore<T>::editingRelation(const Key &k, std::vector<std::string> relatedkeys, bool addDelete)
	{
		std::vector<std::string> keysDb;
		std::vector<std::string> already = dbStore_[k].children();
		std::vector<std::string>::iterator addrelation;
		if (addDelete) {

			for (auto &addrel : relatedkeys)
			{
				if (!contains(addrel)) {
					addrelation = std::find(already.begin(), already.end(), addrel);
					if (addrelation != already.end())
						already.erase(addrelation);
					continue;
				}
				if (!(std::find(already.begin(), already.end(), addrel) != already.end()))
					already.push_back(addrel);
			}

		}
		else
		{
			for (auto &rel : relatedkeys)
			{
				addrelation = std::find(already.begin(), already.end(), rel);
				if (addrelation != already.end())
					already.erase(addrelation);

			}
		}
		dbStore_[k].children(already);
		return true;
	}



	//-------------< edit metadata of the DbElement  >----------
	template<typename T>
	bool DbCore<T>::editMetadata(const Key& key, const DbElement<T>& el)
	{
		if (contains(key)) {
			DbElement<T>& el1 = dbStore_[key];
			if (el.name() != "")
				el1.name(el.name());
			if (el.descrip() != "")
				el1.descrip(el.descrip());
			return true;
		}
		std::cout << "\n" << "A record does not exist to modify" << "\n";
		return false;
	}

	//---------< edit the value part or the PayLoad part of the record >-------------
	template<typename T>
	bool DbCore<T>::editPayLoad(const Key &key, const T &p)
	{
		if (contains(key))
		{
			dbStore_[key].payLoad(p);
			return true;

		}
		return false;
		std::cout << "Key is not present to edit the PayLoad in Database";
	}
	//----< returns current key set for db >-----------------------------

	template<typename T>
	typename Keys DbCore<T>::keys()
	{
		Keys dbKeys;
		DbStore& dbs = dbStore();
		size_t size = dbs.size();
		dbKeys.reserve(size);
		for (auto item : dbs)
		{
			dbKeys.push_back(item.first);
		}
		return dbKeys;
	}
	//----< return number of db elements >-------------------------------

	template<typename T>
	size_t DbCore<T>::size()
	{
		return dbStore_.size();
	}
	//----< extracts value from db with key >----------------------------
	/*
	*  - indexes non-const db objects
	*/
	template<typename T>
	DbElement<T>& DbCore<T>::operator[](const Key& key)
	{
		if (!contains(key))
		{
			if (doThrow_)
				throw(std::exception("key does not exist in db"));
			else
				return (dbStore_[key] = DbElement<T>());
		}
		return dbStore_[key];
	}
	//----< extracts value from db with key >----------------------------
	/*
	*  - indexes const db objects
	*/
	template<typename T>
	DbElement<T> DbCore<T>::operator[](const Key& key) const
	{
		if (!contains(key))
		{
			throw(std::exception("key does not exist in db"));
		}
		return dbStore_[key];
	}
	/////////////////////////////////////////////////////////////////////
	// display functions

	//----< display database key set >-----------------------------------

	template<typename T>
	void showKeys(DbCore<T>& db, std::ostream& out = std::cout)
	{
		out << "\n  ";
		for (auto key : db.keys())
		{
			out << key << " ";
		}
	}
	//----< show record header items >-----------------------------------

	inline void showHeader(std::ostream& out = std::cout)
	{
		out << "\n  ";
		out << std::setw(26) << std::left << "DateTime";
		out << std::setw(10) << std::left << "Name";
		out << std::setw(25) << std::left << "Description";
		out << std::setw(25) << std::left << "Payload";
		out << "\n  ";
		out << std::setw(26) << std::left << "------------------------";
		out << std::setw(10) << std::left << "--------";
		out << std::setw(25) << std::left << "-----------------------";
		out << std::setw(25) << std::left << "-----------------------";
	}
	//----< display DbElements >-----------------------------------------

	template<typename T>
	void showElem(const DbElement<T>& el, std::ostream& out = std::cout)
	{
		out << "\n  ";

		out << std::setw(26) << std::left << std::string(el.dateTime().time());
		out << std::setw(10) << std::left << el.name();
		out << std::setw(25) << std::left << el.descrip();
		T m1;
		m1 = el.payLoad();
		std::vector<std::string> children;
		children = el.children();
		if (children.size() > 0)
		{
			out << "\n    child keys: ";
			for (auto key : children)
			{
				out << " " << key;
			}
		}
	}

	//----< display all records in database >----------------------------

	template<typename T>
	void showDb(const DbCore<T>& db, std::ostream& out = std::cout)
	{
		showHeader(out);
		typename DbCore<T>::DbStore dbs = db.dbStore();
		for (auto item : dbs)
		{
			std::cout << item.first;
			showElem(item.second, out);
		}
	}
}

