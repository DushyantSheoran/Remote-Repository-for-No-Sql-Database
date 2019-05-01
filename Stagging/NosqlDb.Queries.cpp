/////////////////////////////////////////////////////////////////////
// Queries.cpp - To test all the queries defined in Queries.h	   //
// ver 1.0														   //
// Author : Dushyant Sheoran, CSE687   							   //
//			dsheoran@syr.edu +1(704)-957-2519					   //
/////////////////////////////////////////////////////////////////////

//--------------< Test Stub >------------------------
#ifdef TEST_DBQUERIES



#include "Queries.h"
#include "../DBCore/DBCore.h"
#include "../DateTime/DateTime.h"
#include "../StringUtilities/StringUtilities.h"
#include "../PayLoad/PayLoad.h"
#include<iostream>
#include<algorithm>
#include<map>
using namespace std;
using namespace NoSqlDb;


int main()
{
	DbCore<PayLoad> db;
	DbElement<PayLoad> demoElem;


	demoElem.name("Jim");
	demoElem.descrip("Instructor for CSE687");
	demoElem.dateTime(DateTime().now());
	std::map<std::string, std::vector<std::string>> m1, m2;
	std::vector<std::string> v1;
	db.insertRecord("Harika", demoElem);
	showElem(db["Harika"]);

	demoElem.name("Bandaru");
	demoElem.descrip("Intial name");
	demoElem.dateTime(DateTime().now());

	Utilities::Title("Demonstrating Requirement #4 Insertion record in database");
	std::cout << "\n" << "Return true on successful insertion false : Failure" << "\n" << "Case considered record exists";
	std::cout << "\n" << "Result-------->" << db.insertRecord("Harika1", demoElem);
	std::cout << "\n" << "Result ------->" << db.insertRecord("Harika", demoElem);
	showHeader();
	showElem(db["Harika"]);
	showElem(db["Harika1"]);
	Utilities::Title("Demonstrating Requirement #5 Addition of Relations");
	std::cout << "\n" << "Return true on success and false if related key not present";
	std::vector<string> keysset;
	std::cout << "Result of value \n";
	DbQueries<PayLoad> dq(db);
	dq.keys();
	demoElem.name("Bandaru");
	demoElem.descrip("Intial name");
	demoElem.dateTime(DateTime().now());
	db.insertRecord("Harika3", demoElem);
	std::cout << "Updated keys";
	DbElement<PayLoad> dbPattern;
	dbPattern.name(".*an.*");
	dbPattern.descrip(".*st.*");
	Conditions<PayLoad> c2;
	c2.namePattern(".*an.*").descriptionPat(".*ia.*").childrenPattern(".*ia.*");
	Conditions<PayLoad> c3;
	c3.namePattern(".*an.*").descriptionPat(".*ia.*");
	DbQueries<PayLoad> dq5(db);
	dq5.select(c2).show();

}
#endif 

