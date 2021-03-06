#pragma once
/////////////////////////////////////////////////////////////////////////////////
// Persist.h - persist DbCore<P> to and from XML file						   //
//  ver 1.0                                                                   //
//  Language:         Visual C#  2017                                         //
//  Platform:         Lenovo Z580 Windows 10                                  //
//  Application :     Key/Value DataBase , S18                                //
// Author : Dushyant Sheoran, CSE687   							              //
//			dsheoran@syr.edu +1(704)-957-2519					              //
// Source		:     Jim Fawcett, CSE687 - Object Oriented Design, Spring 18 //
////////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines a single Persist class that:
*  - accepts a DbCore<P> instance when constructed
*  - persists its database to an XML string
*  - creates an instance of DbCore<P> from a persisted XML string
*
*  Required Files:
*  ---------------
*  Persist.h, Persist.cpp
*  DbCore.h, DbCore.cpp
*  Query.h, Query.cpp
*  PayLoad.h
*  XmlDocument.h, XmlDocument.cpp
*  XmlElement.h, XmlElement.cpp
* Build Process:
---------------------
*  CL /EHsc Persist.cpp  ../DateTime/DateTime.cpp ../XmlDocument/XmlDocument/XmlDocument.cpp 
* ../XmlDocument/XmlElement/XmlElement.cpp
* ../XmlDocument/XmlParser/XmlParser.cpp ../XmlDocument/XmlElementParts/XmlElementParts.cpp
* ../XmlDocument/XmlElementParts/Tokenizer.cpp
*
*
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 12 Feb 2018
*  - first release
*/

#include "../DbCore/DbCore.h"
#include "../Queries/Queries.h"
#include "../DateTime/DateTime.h"
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlElement/XmlElement.h"
#include "../StringUtilities/StringUtilities.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace Utilities;
namespace NoSqlDb
{
	using namespace XmlProcessing;
	using Xml = std::string;
	using Sptr = std::shared_ptr<AbstractXmlElement>;

	const bool augment = true;   // do augment
	const bool rebuild = false;  // don't augment

	/////////////////////////////////////////////////////////////////////
	// Persist<P> class
	// - persist DbCore<P> to XML string

	template<typename P>
	class Persist
	{
	public:
		Persist(DbCore<P>& db);
		virtual ~Persist() {}
		static void identify(std::ostream& out = std::cout);
		Persist<P>& shard(const Keys& keys);
		Persist<P>& addShardKey(const Key& key);
		Persist<P>& removeShard();
		Xml toXml();
		bool fromXml(const Xml& xml, bool augment = true);  // will clear and reload db if augment is false !!!
		bool loadfromFile(const std::string &filename);
	private:
		DbCore<P>& db_;
		Keys shardKeys_;
		bool containsKey(const Key& key);
		void toXmlRecord(Sptr pDb, const Key& key, DbElement<P>& dbElem);
	};
	//----< constructor >------------------------------------------------

	template<typename P>
	Persist<P>::Persist(DbCore<P>& db) : db_(db) {}

	//----< show file name >---------------------------------------------

	template<typename P>
	void Persist<P>::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
	//----< add key to shard collection >--------------------------------
	/*
	* - when sharding, a db record is persisted if, and only if, its key
		is contained in the shard collection
	*/
	template<typename P>
	Persist<P>& Persist<P>::addShardKey(const Key& key)
	{
		shardKeys_.push_back(key);
		return *this;
	}
	//----< does the shard key collection contain key? >-----------------

	template<typename P>
	bool Persist<P>::containsKey(const Key& key)
	{
		Keys::iterator start = shardKeys_.begin();
		Keys::iterator end = shardKeys_.end();
		return std::find(start, end, key) != end;
	}
	//----< set the shardKeys collection >-------------------------------

	template<typename P>
	Persist<P>& Persist<P>::shard(const Keys& keys)
	{
		shardKeys_ = keys;
		return *this;
	}
	//----< empty the shardKeys collection >-----------------------------

	template<typename P>
	Persist<P>& Persist<P>::removeShard()
	{
		shardKeys_.clear();
	}
	//----< persist database record to XML string >----------------------

	template<typename P>
	void Persist<P>::toXmlRecord(Sptr pDb, const Key& key, DbElement<P>& dbElem)
	{
		Sptr pRecord = makeTaggedElement("dbRecord");
		pDb->addChild(pRecord);
		Sptr pKey = makeTaggedElement("key", key);
		pRecord->addChild(pKey);

		Sptr pValue = makeTaggedElement("value");
		pRecord->addChild(pValue);
		Sptr pName = makeTaggedElement("name", dbElem.name());
		pValue->addChild(pName);
		Sptr pDescrip = makeTaggedElement("description", dbElem.descrip());
		pValue->addChild(pDescrip);

		Sptr pChildren = makeTaggedElement("children");
		pValue->addChild(pChildren);
		for (auto child : dbElem.children())
		{
			Sptr pChild = makeTaggedElement("child", child);
			pChildren->addChild(pChild);
		}

		Sptr pPayLoad = dbElem.payLoad().toXmlElement();
		pValue->addChild(pPayLoad);
	}
	//----< persist, possibly sharded, database to XML string >----------
	/*
	* - database is sharded if the shardKeys collection is non-empty
	*/
	template<typename P>
	Xml Persist<P>::toXml()
	{
		Sptr pDb = makeTaggedElement("db");
		pDb->addAttrib("type", "fromQuery");
		Sptr pDocElem = makeDocElement(pDb);
		XmlDocument xDoc(pDocElem);

		if (shardKeys_.size() > 0)
		{
			for (auto key : shardKeys_)
			{
				DbElement<P> elem = db_[key];
				toXmlRecord(pDb, key, elem);
			}
		}
		else
		{
			for (auto item : db_)
			{
				toXmlRecord(pDb, item.first, item.second);
			}
		}
		std::string xml = xDoc.toString();
		return xml;
	}
	//----< retrieve database from XML string >--------------------------
	/*
	* - Will clear db and reload if augment is false
	*/
	template<typename P>
	bool Persist<P>::fromXml(const Xml& xml, bool augment)
	{
		XmlProcessing::XmlDocument doc(xml);
		std::vector<Sptr> pRecords = doc.descendents("dbRecord").select();
		for (auto pRecord : pRecords)
		{
			Key key;
			DbElement<P> elem;
			P pl;
			std::vector<Sptr> pChildren = pRecord->children();
			for (auto pChild : pChildren)
			{
				if (pChild->tag() == "key")
					key = trim(pChild->children()[0]->value());
				else
				{
					std::vector<Sptr> pValueChildren = pChild->children();
					std::string valueOfTextNode;
					for (auto pValueChild : pValueChildren)
					{
						std::string tag = pValueChild->tag();
						if (pValueChild->children().size() > 0)
							valueOfTextNode = trim(pValueChild->children()[0]->value());
						else
							valueOfTextNode = "";
						if (tag == "name")
							elem.name(valueOfTextNode);
						else if (tag == "description")
							elem.descrip(valueOfTextNode);
						else if (tag == "dateTime")
							elem.dateTime(valueOfTextNode);
						else if (tag == "children")
						{
							for (auto pChild : pValueChild->children())
							{
								valueOfTextNode = trim(pChild->children()[0]->value());
								elem.children().push_back(valueOfTextNode);
							}
						}
						else if (tag == "payload")
						{
							pl = PayLoad::fromXmlElement(pValueChild);
							elem.payLoad(pl);
						}
					}
				}
				db_[key] = elem;
			}
		}
		return true;
	}
	template<typename P>
	bool Persist<P>::loadfromFile(const std::string &fileName)
	{
		std::ifstream in(fileName);
		if (!in.good())
		{
			std::cout << "\n  failed to open file";
			return false;
		}
		Xml xmlString;
		while (in.good())
		{
			char ch = in.get();

			if (!in.good())
				break;
			xmlString += ch;
		}
		in.close();

		fromXml(xmlString);
		return true;
	}
}
