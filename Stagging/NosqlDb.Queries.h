
/////////////////////////////////////////////////////////////////////////////////
// Queries.h -   Implement the query functions								  //
//  ver 1.0                                                                   //
//  Language:         Visual C#  2017                                         //
//  Platform:         Lenovo Z580 Windows 10                                  //
//  Application :     Key/Value DataBase , S18                                //
// Author : Dushyant Sheoran, CSE687   							              //
//			dsheoran@syr.edu +1(704)-957-2519					              //
////////////////////////////////////////////////////////////////////////////////

/*
* Package Operations:
* -------------------
* This package provides two classes:
* The condition class to register conditions for the regular expression string
* It holds a refernce to the DbElement
* Returns the condition object to provide cascading of functions
* Quries classes to demenstrate different types of queries
* -Interfaces:
---------------
* selectName :  Filter the keys based on the matching regular expression pattern
* selectDescr :  Filter the keys based on the matching regular expression pattern agains description
* selectedTime : Filter the keys between the mentioned intervals
* selectCAtegories : Queries against the catgories present in the payload part and returns keys
* selectfilePath :  Filter the keys between the filePath Pattern
* ChldrnSpcfdKey :  Filter the keys pattern matching the keys present in the children
* from			: Returns the keys of query on specified condition
* selct			: returns the keys on specified condition "Provides a compund AND"
* selectUnion	: Provides function for "Join" keys


* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* DateTime.h, DateTime.cpp
* Queries.h, Queries.cpp
*
* Maintenance History:
* --------------------
*
* ver 1.0 : 2 Feb 2018
* - first release
*/


#pragma once
#include "../DBCore/DBCore.h"
#include "../DateTime/DateTime.h"
#include "../DBCore/Definitions.h"

#include<iostream>
#include<string>
#include<vector>
#include<regex>
#include<stdarg.h>
#include<set>
#include<algorithm>
using namespace NoSqlDb;

namespace NoSqlDb {
	template<typename P>
	class Conditions {
		using regxPattern = const std::string&;
		using dbElement = DbElement<P>;
	public:
		Conditions() = default;
		bool match();
		void value(DbElement<P>& elem) { pdbelem_ = &elem; } // set pointer to element used during query

		Conditions &childrenPattern(regxPattern pattern0) { _chldPattrn = pattern0; return *this; };
		bool matchChildren();

		Conditions &namePattern(regxPattern pattern1) { _namePattrn = pattern1; return *this; };
		bool matchName();
		Conditions &descriptionPat(regxPattern pattern2) { _desPattrn = pattern2; return *this; };
		bool matchDescription();
		void children(Keys keys) { keys_ = keys; }
		void dbEleConstruction(DbElement<P> &pdbele) { pdbelem_ = &pdbele; }
		DbElement<P> dbEleConstruction() { return *pdbelem_; }

	private:
		DbElement<P>* pdbelem_ = nullptr;
		std::string _chldPattrn = "";
		std::string _namePattrn = "";
		std::string _desPattrn = "";
		Keys keys_;
	};


	template<typename P>
	bool Conditions<P>::match()
	{
		try
		{
			return matchName() && matchDescription() && matchChildren();
		}
		catch (std::exception& ex)
		{
			std::cout << "\n  exception thrown:\n  " << ex.what();
		}
		return false;
	}
	/*----< test element metadata for name match >---------------------*/

	template<typename P>
	bool Conditions<P>::matchName()
	{
		if (_namePattrn == "")
			return true;
		std::regex re(_namePattrn);
		return std::regex_search(pdbelem_->name(), re);
	}
	/*----< test metadata for description match >----------------------*/

	template<typename P>
	bool Conditions<P>::matchDescription()
	{
		if (_desPattrn == "")
			return true;
		std::regex re(_desPattrn);
		std::string desc = pdbelem_->descrip();
		return std::regex_search(desc, re);
	}

	template<typename P>
	bool Conditions<P>::matchChildren()
	{
		if (keys_.size() == 0)
			return true;
		Keys::iterator start = pdbelem_->children().begin();
		Keys::iterator end = pdbelem_->children().end();
		for (Key key : keys_)
		{
			if (std::find(start, end, key) == end)
				return false;
		}
		return true;
	}


	//------------< class interface for DbQueries >---------------------
	template<typename P>
	class DbQueries {
	public:
		using key = std::string;
		using dbKeyList = std::vector<std::string>;
		using childList = std::vector<std::string>;
		DbQueries(DbCore<P>& db_) :pdb_(db_) { keys_ = db_.keys(); }
		static void identity(std::ostream& out = std::cout);
		DbQueries& select(Conditions<P> &c);

		void show(std::ostream& out = std::cout);


		template<typename CallObj>
		DbQueries& select(CallObj callObj);

		template<typename CallObj>
		DbQueries& select1(CallObj callObj);

		DbQueries &selectname(const std::string pattern1);
		DbQueries &selectDescription(const std::string pattern2);
		DbQueries &selectdateTime(DateTime dt1, DateTime dt2 = (DateTime().now()));
		DbQueries &selectCategories(const std::string &patrn);
		DbQueries &selectfilePath(const std::string &fPath);
		DbQueries &ChldrnSpcfdKey(const std::string keys);

		DbQueries &selectChildrn(const std::string &ptrn1);
		DbQueries &from(dbKeyList &keys);
		DbQueries &selectUnion(std::vector<std::string> set1, std::vector<std::string> set2);

		DbQueries &selectUnionVa(int va_args, ...);

		bool valuofKey(const std::string & k);

		DbQueries &selectIntersection(int n_args, ...);

		dbKeyList& keys() { return keys_; }
		dbKeyList keys() const { return keys_; }
		void keys(const dbKeyList& keys) { keys_ = keys; }
		void keys(dbKeyList & keys) { keys_ = keys; }

		DbCore<P> & database() { return pdb_; }
	private:
		DbCore<P> &pdb_;
		dbKeyList keys_;
	};
	template<typename P>
	inline void DbQueries<P>::identity(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}

	//----------------< to get all keys of the query result >-----------------------
	template<typename P>
	DbQueries<P> & DbQueries<P>::from(dbKeyList &keysq)
	{
		keys(keysq);
		return *this;
	}
	//-----------< a display function >----------------------------------
	template<typename P>
	void DbQueries<P>::show(std::ostream& out = std::cout)
	{

		out << "\n";
		for (auto key : keys_)
		{
			out << key << "\n";
		}
		return;
	}
	template<typename P>
	DbQueries<P>& DbQueries<P>::select(Conditions<P> &c) {
		Keys newKeys;
		for (auto item : pdb_)
		{
			c.value(item.second);
			if (c.match())
				newKeys.push_back(item.first);
		}
		keys_ = newKeys;
		return *this;
		return *this;
	}
	//----------------< select keys theat matches names by pattern >---------------------
	template<typename P>
	DbQueries<P> &DbQueries<P>::selectname(const std::string pattern1)
	{
		if (pattern1 == "")
			return *this;
		std::regex regx(pattern1);
		dbKeyList updatedkeys;
		DbCore<P> db = database();
		for (auto &it : keys_)
		{
			DbElement<P> dbele = db[it];
			if (std::regex_match(static_cast<std::string>(dbele.name()), regx))
			{
				updatedkeys.push_back(it);
			}
		}
		keys(updatedkeys);
		return *this;
	}

	//--------------< seclect keys if they have the given pattern in the description >------------
	template<typename P>
	DbQueries<P> &DbQueries<P>::selectDescription(const std::string pattern2)
	{
		if (pattern2 == "")
			return *this;
		std::regex regx(pattern2);
		dbKeyList updatedkeys;
		DbCore<P> db = database();
		for (auto &it : keys_)
		{

			if (std::regex_match(static_cast<std::string>(db[it].descrip()), regx))
			{
				updatedkeys.push_back(it);
			}
		}

		keys(updatedkeys);
		return *this;
	}
	//----------------------< selct all keys if the have date and time in the given interval >-------------
	template<typename P>
	DbQueries<P> &DbQueries<P>::selectdateTime(DateTime dt1, DateTime dt2 = DateTime().now()) {
		dbKeyList updatedkeys;
		DbCore<P> db = database();
		std::cout << "\n    date interval match Query for dates between:";
		std::cout << "\n    " << dt1.time() << " and " << dt2.time();
		for (auto &it : keys_)
		{
			if (dt1.time() <= db[it].dateTime().time() && db[it].dateTime().time() <= dt2.time())
				updatedkeys.push_back(it);
		}
		if (updatedkeys.size() <= 0)
			std::cout << "\n    didn't find match for DateTime interval";
		keys(updatedkeys);
		return *this;

	}
	//-----------------< select keys if they have following categories >---------
	template<typename P>
	DbQueries<P> &DbQueries<P>::selectCategories(const std::string &patrn)
	{
		DbCore<P> db = database();
		dbKeyList updatedkeys;
		std::regex regx(patrn);
		for (auto catekey : keys_)
		{
			int flag = 0;
			PayLoad insStruct = db[catekey].payLoad();
			if (insStruct.filePath != "")
			{
				for (auto cateMat : insStruct.catogries)
				{
					if (std::regex_match(static_cast<std::string>(cateMat), regx))
						flag = 1;
				}
			}
			if (flag)
				updatedkeys.push_back(catekey);
		}
		keys(updatedkeys);
		return *this;
	}

	//-----------------< to return keys if filepath matches >--------------------
	template<typename P>
	DbQueries<P> &DbQueries<P>::selectfilePath(const std::string &ptrn)
	{
		DbCore<P> db = database();
		dbKeyList updatedkeys;
		std::regex regx(ptrn);
		keys(updatedkeys);
		return *this;
	}
	//----------------------< select keys if they have children based on given pattern >-----------------
	template<typename P>
	DbQueries<P> &DbQueries<P>::selectChildrn(const std::string &ptrn)
	{
		DbCore<P> db = database();
		dbKeyList updatedkeys;
		std::regex regx(ptrn);
		for (auto &it : keys_)
		{
			int flag = 0;
			for (auto &cat : db[it].children())
			{
				if (std::regex_match(static_cast<std::string>(cat), regx))
					flag = 1;
			}
			if (flag)
				updatedkeys.push_back(it);
		}
		keys(updatedkeys);
		return *this;
	}
	template<typename P>
	bool DbQueries<P>::valuofKey(const std::string & key1)
	{
		if (!pdb_.contains(key1))
			return false;
		return true;
	}
	//-------------------< Get the keys of children of specified keys >------------------
	template<typename P>
	DbQueries<P> &DbQueries<P>::ChldrnSpcfdKey(const std::string keyspfd)
	{
		DbCore<P> db = database();
		DbCore<P>::Keys chldks;
		if (!db.contains(keyspfd))
			return *this;
		DbElement<P>::Children  chldrn = db[keyspfd].children();
		for (auto chldkey : chldrn)
		{
			//Checking if the child is present in Db or Deleted to avoid calling on null value
			if (db.contains(chldkey))
			{
				chldks.push_back(chldkey);
			}
		}
		keys(chldks);
		return *this;
	}
	//-----------------< This works for union of 2 set of keys >---------------------------
	template<typename P>
	DbQueries<P> & DbQueries<P>::selectUnion(std::vector<std::string> set1, std::vector<std::string> set2)
	{
		std::vector<std::string> updatedkeys;
		std::set<std::string> comnkeys;
		for (auto k : set1)
			comnkeys.insert((comnkeys.end()), k);
		for (auto k : set2)
			comnkeys.insert((comnkeys.end()), k);
		updatedkeys.assign(comnkeys.begin(), comnkeys.end());
		keys(updatedkeys);
		return *this;
	}

	//-------------<Works for union of n- set of keys where n is mentioned in "n_args">--------
	template<typename P>
	DbQueries<P> & DbQueries<P>::selectUnionVa(int n_args, ...)
	{
		va_list ap;
		va_start(ap, n_args);
		std::vector<std::string> updatedkeys;
		std::set<std::string> comnkeys;

		for (int i = 0; i < n_args; i++)
		{
			std::vector<std::string> ketset = va_arg(ap, std::vector<std::string>);
			for (auto k : ketset)
				comnkeys.insert((comnkeys.end()), k);

		}
		updatedkeys.assign(comnkeys.begin(), comnkeys.end());
		keys(updatedkeys);

		return *this;
	}
	template<typename P>
	DbQueries<P> & DbQueries<P>::selectIntersection(int n_args, ...)
	{
		va_list ap;
		va_start(ap, n_args);
		std::vector<std::string> keyset1;
		std::vector<std::string> keyset2;
		std::vector<std::string> common;
		for (int i = 0; i < n_args; i += 2)
		{
			keyset1 = va_arg(ap, std::vector<std::string>);
			keyset2 = va_arg(ap, std::vector<std::string>);
			std::set_intersection(keyset1.begin(), keyset1.end(), keyset2.begin(), keyset2.end(), std::back_inserter(common));
		}
		keys(common);
		return *this;
	}
	/*
	*  - CallObj is defined by the application to return results from
	*    application's payload.  See test stub for example uses.
	*/
	template<typename P>
	template<typename CallObj>
	DbQueries<P>& DbQueries<P>::select(CallObj callObj)
	{
		Keys newKeys;
		for (auto item : pdb_)
		{
			if (callObj(item.first))
				newKeys.push_back(item.first);
		}
		keys_ = newKeys;
		return *this;
	}
	template<typename P>
	template<typename CallObj>
	DbQueries<P> &DbQueries<P>::select1(CallObj callObj)
	{
		Keys newKeys;
		for (auto item : pdb_)
		{
			if (callObj(item.second))
				newKeys.push_back(item.first);
		}
		keys_ = newKeys;
		return *this;
	}

}
