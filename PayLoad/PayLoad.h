#ifndef PAYLOAD_H
#define PAYLOAD_H
/////////////////////////////////////////////////////////////////////////////////
// Checkin.h - Implements an application specific data						   //
//  ver 1.0                                                                    //
//  Language:         Visual C#  2017                                          //
//  Platform:         Lenovo Z580 Windows 10                                   //
//  Application :     Software Repository TestBed , S18                        //
// Author : Dushyant Sheoran, CSE687   							               //
//			dsheoran@syr.edu +1(704)-957-2519					               //
// Source		:     Jim Fawcett, CSE687 - Object Oriented Design, Spring 18  //
////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides class:
* - aplication specific payload part specifies FilePath ; Categories ;
*	check-in status for the application
* Member functions:
* -------------------
* isClosed	    : This represents whether the check-in is closed or not.
* toXmlElement  : Forms an Xml representation for the given data
*
*Build Process:
-------------------
* CL /EHsc PayLoad.cpp
* ../XmlDocument/XmlDocument/XmlDocument.cpp ../XmlDocument/XmlElement/XmlElement.cpp
* ../XmlDocument/XmlParser/XmlParser.cpp ../XmlDocument/XmlElementParts/XmlElementParts.cpp
* ../XmlDocument/XmlElementParts/Tokenizer.cpp ../StringUtilities/StringUtilities.cpp
*
* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* Utilities.h, Utilities.cpp
* XmlDocumnet --> static library
*
*
* Maintenance History:
* --------------------
* ver 1.0 :  Feb 2018
* - first release
*/

#include <string>
#include <vector>
#include <iostream>
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlElement/XmlElement.h"
#include "../StringUtilities/StringUtilities.h"
#include "../DbCore/DbCore.h"
#include "IPayLoad.h"
using namespace Utilities;

///////////////////////////////////////////////////////////////////////
// PayLoad class provides:
// - a std::string value which, in Project #2, will hold a file path
// - a vector of string categories, which for Project #2, will be 
//   Repository categories
// - methods used by Persist<PayLoad>:
//   - Sptr toXmlElement();
//   - static PayLoad fromXmlElement(Sptr elem);



namespace NoSqlDb
{
	class PayLoad : public IPayLoad<PayLoad>
	{
	public:
		PayLoad() = default;
		PayLoad(const std::string& val, const std::string& isClose = "open", bool isTraversed = false) : filePath(val), isClosed_(isClose), isTraversed_(isTraversed) {}
		static void identify(std::ostream& out = std::cout);
		PayLoad& operator=(const std::string& val)
		{
			filePath = val;
			return *this;
		}
		operator std::string() { return filePath; }

		std::string value() const { return filePath; }
		std::string& value() { return filePath; }
		void value(const std::string& value) { filePath = value; }

		void categories(const Children& catg) { catogries = catg; }
		std::vector<std::string>& categories() { return catogries; }
		std::vector<std::string> categories() const { return catogries; }

		std::string isClosed() { return isClosed_; }
		std::string isClosed() const { return isClosed_; }
		void isClosed(const std::string &closed) { isClosed_ = closed; }
		bool isTraversed() { return isTraversed_; }
		void isTraversed(bool travers) { isTraversed_ = travers; }

		std::string  BoolToString(bool b);

		bool hasCategory(const std::string& cat)
		{
			return std::find(categories().begin(), categories().end(), cat) != categories().end();
		}

		Sptr toXmlElement();
		static PayLoad fromXmlElement(Sptr elem);

		static void showPayLoadHeaders(std::ostream& out = std::cout);
		static void showElementPayLoad(NoSqlDb::DbElement<PayLoad>& elem, std::ostream& out = std::cout);
		static void showDb(NoSqlDb::DbCore<PayLoad>& db, std::ostream& out = std::cout);
	private:
		std::string filePath;
		std::vector<std::string> catogries;
		std::string isClosed_;
		bool isTraversed_;
	};

	inline std::string PayLoad::BoolToString(bool b)
	{
		return b ? "true" : "false";
	}

	//----< show file name >---------------------------------------------

	inline void PayLoad::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
	//----< create XmlElement that represents PayLoad instance >---------
	/*
	* - Required by Persist<PayLoad>
	*/
	inline Sptr PayLoad::toXmlElement()
	{
		Sptr sPtr = XmlProcessing::makeTaggedElement("payload");
		XmlProcessing::XmlDocument doc(makeDocElement(sPtr));
		Sptr sPtrVal = XmlProcessing::makeTaggedElement("value", filePath);
		sPtr->addChild(sPtrVal);
		Sptr sPtrCats = XmlProcessing::makeTaggedElement("categories");
		sPtr->addChild(sPtrCats);
		for (auto cat : catogries)
		{
			Sptr sPtrCat = XmlProcessing::makeTaggedElement("category", cat);
			sPtrCats->addChild(sPtrCat);
		}
		Sptr sptrStatus = XmlProcessing::makeTaggedElement("chkinstatus", isClosed_);
		sPtr->addChild(sptrStatus);
		Sptr sptetraver = XmlProcessing::makeTaggedElement("traverstatus", BoolToString(isTraversed_));
		sPtr->addChild(sptetraver);
		return sPtr;
	}
	//----< create PayLoad instance from XmlElement >--------------------
	/*
	* - Required by Persist<PayLoad>
	*/
	inline PayLoad PayLoad::fromXmlElement(Sptr pElem)
	{
		PayLoad pl;
		for (auto pChild : pElem->children())
		{
			std::string val;
			std::string tag = pChild->tag();
			if (pChild->children().size() > 0)
				val = trim(pChild->children()[0]->value());
			else
				val = "";
			if (tag == "value")
			{
				pl.value(val);
			}
			if (tag == "categories")
			{
				std::vector<Sptr> pCategories = pChild->children();
				for (auto pCat : pCategories)
				{
					pl.categories().push_back(trim(pCat->children()[0]->value()));
				}
			}
			if (tag == "chkinstatus")
			{
				pl.isClosed(val);
			}
			if (tag == "traverstatus")
			{
				if (val == "true")
					pl.isTraversed(true);
				else
					pl.isTraversed(false);
			}
		}
		return pl;
	}
	/////////////////////////////////////////////////////////////////////
	// PayLoad display functions

	inline void PayLoad::showPayLoadHeaders(std::ostream& out)
	{
		out << "\n  ";
		out << std::setw(10) << std::left << "Name";
		out << std::setw(45) << std::left << "Payload Value";
		out << std::setw(25) << std::left << "Categories";
		out << "\n  ";
		out << std::setw(10) << std::left << "--------";
		out << std::setw(40) << std::left << "--------------------------------------";
		out << std::setw(25) << std::left << "-----------------------";
	}

	//----------------< Display functon >-----------------
	inline void PayLoad::showElementPayLoad(NoSqlDb::DbElement<PayLoad>& elem, std::ostream& out)
	{
		out << "\n  ";
		out << std::setw(10) << std::left << elem.name().substr(0, 8);
		out << std::setw(40) << std::left << elem.payLoad().value().substr(0, 38);
		for (auto cat : elem.payLoad().categories())
		{
			out << cat << " ";
		}
	}
	//-------< Display function >------------
	inline void PayLoad::showDb(NoSqlDb::DbCore<PayLoad>& db, std::ostream& out)
	{
		showPayLoadHeaders(out);
		for (auto item : db)
		{
			NoSqlDb::DbElement<PayLoad> temp = item.second;
			PayLoad::showElementPayLoad(temp, out);
		}
	}
}
#endif