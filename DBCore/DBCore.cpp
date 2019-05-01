/////////////////////////////////////////////////////////////////////
// DbCore.cpp - key/value pair in-memory database				   //
// ver 1.0														   //
// Author : Dushyant Sheoran, CSE687   							   //
//			dsheoran@syr.edu +1(704)-957-2519					   //
/////////////////////////////////////////////////////////////////////

#include "DbCore.h"
#include "../StringUtilities/StringUtilities.h"
#include <iostream>
#include <iomanip>
#include <functional>
#include<map>
using namespace NoSqlDb;

//---------------------< Test stub for DbCore >----------------------------

#ifdef Test_DB


int main()
{
	using fname = std::string;
	using category = std::vector<std::string>;
	DbCore<std::string> db;

	DbElement<std::string> demoElem;
	demoElem.name("Jim");
	demoElem.descrip("Instructor for CSE687");
	demoElem.dateTime(DateTime().now());
	std::map<std::string, std::vector<std::string>> m1, m2;
	std::vector<std::string> v1;
	demoElem.payLoad("fileName.cpp");
	db.insertRecord("Harika", demoElem);
	showElem(db["Harika"]);

	demoElem.name("Bandaru");
	demoElem.descrip("Intial name");
	demoElem.dateTime(DateTime().now());
	demoElem.payLoad("dispalyFile.cs");
	Utilities::Title("Demonstrating Requirement #4 Insertion record in database");
	std::cout << "\n" << "Return true on successful insertion false : Failure" << "\n" << "Case considered record exists";
	std::cout << "\n" << "Result-------->" << db.insertRecord("Harika1", demoElem);
	std::cout << "\n" << "Result ------->" << db.insertRecord("Harika", demoElem);

	Utilities::Title("Demonstrating Requirement #5 Addition of Relations");
	std::cout << "\n" << "Return true on success and false if related key not present";
	demoElem.payLoad("TestFile1");
	DbElement<std::string> dbele;
	dbele.name("Konchada");
	showElem(db["Harika"]);
	showDb(db);
	showDb(db);
	std::vector<std::string> newele;
	newele.push_back("Harika1");
	db.editingRelation("Harika", newele, true);
	showDb(db);
	return 0;
}

#endif 