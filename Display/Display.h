#pragma once
/////////////////////////////////////////////////////////////////////////////////
// Display.h - Implements Display Utility for the NoSql Database			  //
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
* This package provides a classe:
* -Interfaces:
---------------
* showAll	 :  Displays all elements present in the in-memory database
* showHeader :  To display the Header information before displaying the in-memeory db
* show		 :  To Dispaly a particular record
* showsummary : Depending on selection from the showAll
* showDetails : Depending on the selction of ShowALl desplys all the fields in detail
*
*Build Process:
----------------
*
* CL /EHsc Display.cpp ../FileSystem/FileSystemDemo/FileSystem.cpp ../DateTime/DateTime.cpp
*
*
* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* DateTime.h, DateTime.cpp
* Display.h, Display.cpp
*
* Maintenance History:
* --------------------
*
* ver 1.0 : 2 Feb 2018
* - first release
*/


#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "../DateTime/DateTime.h"
#include "../DBCore/DBCore.h"
#include "../Queries/Queries.h"
#include "../StringUtilities/StringUtilities.h"
#include "../PayLoad/PayLoad.h"
using namespace Utilities;
using namespace NoSqlDb;

using Key = std::string;
using Keys = std::vector<Key>;
using Name = std::string;
using Category = std::string;
using Children = std::vector<Key>;

/////////////////////////////////////////////////////////////////////
// Element class represents a data record in our NoSql database
// - in our NoSql database that is just the value in a key/value pair
// - it needs to store child data, something for you to implement
//

template<typename PayLoad>
//--------------------< Interface to the Display Utility class >---------------
class Display
{
public:
	enum Style { summary, detailed };
	Display(DbCore<PayLoad> &db) : db_(db) {}
	Children children;
	void showAll(Style style = summary, size_t fieldwidth = 22, std::ostream& out = std::cout);
	void showHeader(Style style = summary, size_t fieldwidth = 22, std::ostream& out = std::cout);
	void showHeadersing(Key k, Style style = summary, size_t fieldwidth = 22, std::ostream& out = std::cout);
	void show(Key key, bool optheader = true, Style style = summary, size_t fieldwidth = 22, std::ostream& out = std::cout);
private:
	void showSummary(Key key, size_t fieldwidth, std::ostream& out);
	void showDetails(Key key, size_t fieldwidth, std::ostream& out);
	DbCore<PayLoad> &db_;
	int maxchldsi;
	int maxcat;
};
//-------------------------< Shows the header information of the database>-----------
template<typename PayLoad>
void Display<PayLoad>::showHeader(Style style, size_t fieldwidth, std::ostream& out)
{
	size_t numChildren = 0;
	size_t catgrs = 0;
	size_t sizech = 0;
	Keys keys = db_.keys();
	PayLoad data;
	for (Key k : keys)
	{
		if (db_.contains(k))
		{
			data = db_[k].payLoad();
			if (catgrs < data.categories().size())
				catgrs = data.categories().size();
			if (sizech < db_[k].children().size())
				sizech = db_[k].children().size();
		}
	}
	int save = out.setf(std::ios::left, std::ios::adjustfield);
	out << "\n";
	out << "  ";
	out << std::setw(fieldwidth) << std::string("Key").substr(0, fieldwidth) << "  ";
	out << std::setw(fieldwidth) << std::string("Name").substr(0, fieldwidth) << "  ";
	if (sizech > 0)
	{
		for (size_t i = 0; i < sizech; ++i)
			out << std::setw(fieldwidth) << std::string("Child").substr(0, fieldwidth) << "  ";
	}
	if (catgrs > 0)
	{
		for (size_t i = 0; i < catgrs; ++i)
			out << std::setw(fieldwidth) << std::string("category").substr(0, fieldwidth) << "  ";
	}
	out << std::setw(fieldwidth) << std::string("Closed").substr(0, fieldwidth) << " ";
	out << "\n  ";
	for (size_t i = 0; i < sizech + catgrs + 3; ++i)
		out << std::setw(fieldwidth) << std::string("-----------").substr(0, fieldwidth) << "  ";

	out << "\n";
	maxchldsi = sizech;
	maxcat = catgrs;
	out.setf(save);

}
//--------------------< shows the summary of the records in the in-memory database> -----
template<typename PayLoad>
void Display<PayLoad>::showSummary(Key key, size_t fieldwidth, std::ostream& out)
{
	PayLoad data;
	if (!db_.contains(key))
		return;

	DbElement<PayLoad> elem = db_[key];
	int chld = elem.children().size();

	data = elem.payLoad();
	int catsi = data.categories().size();
	int save = out.setf(std::ios::left, std::ios::adjustfield);
	out << "  ";
	out << std::setw(fieldwidth) << trimnewLine(key).substr(0, fieldwidth) << "  ";
	out << std::setw(fieldwidth) << static_cast<std::string>(trimnewLine(elem.name())).substr(0, fieldwidth) << "  ";
	//	out << std::setw(fieldwidth) << static_cast<std::string>(elem.descrip()).substr(0, fieldwidth) << " ";

	for (auto k : elem.children())
		out << std::setw(fieldwidth) << static_cast<std::string>(trimnewLine(k)).substr(0, fieldwidth) << "  ";
	while (chld < maxchldsi) {
		out << std::setw(fieldwidth) << static_cast<std::string>(" ").substr(0, fieldwidth) << "  "; chld++;
	}

	if (data.categories().size() > 0)
	{
		for (auto k : data.categories())
			out << std::setw(fieldwidth) << static_cast<std::string>(trimnewLine(k)).substr(0, fieldwidth) << "  ";
	}
	while (catsi < maxcat) {
		out << std::setw(fieldwidth) << static_cast<std::string>(" ").substr(0, fieldwidth) << "  "; chld++;
	}

	out << std::setw(fieldwidth) << static_cast<std::string>(data.isClosed()) << " ";
	out << "\n";
	out.setf(save);
}
//-------< Detail information of each record present in the inmemory database>-------
template<typename PayLoad>
void Display<PayLoad>::showDetails(Key key, size_t fieldwidth, std::ostream& out)
{
	if (!db_.contains(key))
		return;
	PayLoad data;
	DbElement<PayLoad> elem = db_[key];
	data = elem.payLoad();
	int save = out.setf(std::ios::left, std::ios::adjustfield);
	out << "  ";
	out << "key      : " << trimnewLine(key) << "\n  ";

	out << "Name : " << static_cast<std::string>(elem.name()) << "\n  ";
	out << "Description     : " << elem.descrip() << "\n  ";
	out << "Children :\n    ";
	if (elem.children().size() > 0)
	{
		for (Key k : elem.children())
			out << std::setw(fieldwidth) << static_cast<std::string>(k).substr(0, fieldwidth) << " ";
	}
	out << "\nCategories:\n   ";
	if (data.categories().size() > 0)
	{
		for (Key k : data.categories())
			out << std::setw(fieldwidth) << static_cast<std::string>(k).substr(0, fieldwidth) << " ";
	}
	out << std::setw(fieldwidth) << static_cast<std::string>(data.isClosed()) << " ";
	out << "\n";
	out.setf(save);
}
//---------------< To make choice whether to diplay the sumary or the detail>---------
template<typename PayLoad>
void Display<PayLoad>::showAll(Style style, size_t fieldwidth, std::ostream& out)
{
	Keys keys = db_.keys();
	if (style == Display<PayLoad>::Style::summary)
	{
		showHeader(style, fieldwidth, out);
		for (Key k : keys)
			showSummary(k, fieldwidth, out);
	}
	else
	{
		for (Key k : keys)
			showDetails(k, fieldwidth, out);
	}

}
template<typename PayLoad>
void Display<PayLoad>::showHeadersing(Key k, Style style, size_t fieldwidth, std::ostream& out)
{
	PayLoad data = db_[k].payLoad();
	int save = out.setf(std::ios::left, std::ios::adjustfield);
	out << "\n";
	out << "  ";
	out << std::setw(fieldwidth) << std::string("Key").substr(0, fieldwidth) << "  ";
	out << std::setw(fieldwidth) << std::string("Name").substr(0, fieldwidth) << "  ";
	for (size_t i = 0; i < db_[k].children().size(); ++i)
		out << std::setw(fieldwidth) << std::string("Child").substr(0, fieldwidth) << "  ";
	for (size_t i = 0; i < data.categories().size(); ++i)
		out << std::setw(fieldwidth) << std::string("category").substr(0, fieldwidth) << "  ";

	out << std::setw(fieldwidth) << std::string("Chk-in_Status").substr(0, fieldwidth) << " ";
	out << "\n  ";
	for (size_t i = 0; i < (db_[k].children().size() + data.categories().size() + 3); ++i)
		out << std::setw(fieldwidth) << std::string("-----------").substr(0, fieldwidth) << "  ";

	out << "\n";
	out.setf(save);

}


//--------------------------------< to the record present in in-memeory db>=------
template<typename PayLoad>
void Display<PayLoad>::show(Key key, bool hdoptional, Style style, size_t fieldwidth, std::ostream& out)
{
	if (!db_.contains(key))
		return;
	if (!hdoptional)
		showHeadersing(key);
	if (style == Display<PayLoad>::Style::summary)
		showSummary(key, 22, out);
	else
		showDetails(key, 22, out);
}



