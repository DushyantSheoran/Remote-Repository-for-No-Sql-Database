/////////////////////////////////////////////////////////////////////
// Checkin.cpp : Checkin component for Software Repository Testbed //
// ver 1.0														   //
// Author : Dushyant Sheoran, CSE687   							   //
//			dsheoran@syr.edu +1(704)-957-2519					   //
/////////////////////////////////////////////////////////////////////


#include "Checkin.h"
#include "../DBCore/DBCore.h"
#include "../PayLoad/PayLoad.h"
#include <iostream>
#include<string>
#include "../Persist/Persist.h"

using namespace NoSqlDb;
using namespace RepositoryService;

//-------< Test-stub >-----------------------------

#ifdef ChkinMain
int main()
{
	using VHolder = std::unordered_map<std::string, size_t>;
	DbCore<PayLoad> db;
	DbElement<PayLoad> dbelem;
	std::unordered_map<FileName, size_t> vHold;
	Version v(vHold);
	Children closePend;
	Checkin<PayLoad> c(db, v, closePend); 
	c.staggingPath("../Stagging");
	c.storagePath("../Storage");
	DbElement<PayLoad> dbele;
	DbElement<PayLoad> dbele5;
	dbele.name("Harika");
	PayLoad p("../Storage");
	p.categories().push_back("NoSqlDb");
	p.categories().push_back("Project1");
	p.isClosed("open");
	std::string f1 = "NosqlDb.DBCore.cpp";
	dbele.dateTime(DateTime().now());
	dbele.descrip("Chkin test sample");
	dbele.payLoad(p);
	c.doCkeckin(dbele, f1);
	//Closing the chkin
	dbele5 = dbele;
	dbele5.payLoad(p);
	c.doCkeckin(dbele5, f1);
	//Doing 3rd chkin
	std::string f3 = "test.testFile1.cs.1";
	DbElement<PayLoad> dbeleme3;
	PayLoad p2("../Storage");
	p2.categories().push_back("Test");
	p2.categories().push_back("Project2");
	p2.isClosed("open");
	dbeleme3.name("Harika");
	dbeleme3.dateTime(DateTime().now());
	dbeleme3.descrip("Chkin test sample");
	dbeleme3.payLoad(p2);
	c.doCkeckin(dbeleme3, f3);
	c.addDependencies("test.testFile1.cs.1", { "NosqlDb.DBCore.cpp.1" });
	c.addDependencies("NosqlDb.DBCore.cpp.1", { "test.testFile1.cs.1" });
	c.closeChkin("test.testFile1.cs.1");
	Persist<PayLoad> persist(c.toPersistChkin());
	Xml xml = persist.toXml();
	std::cout << xml;
	c.persistRepos(xml);
	Children depends = { "test.testFile1.cs.2","test.testFile1.cs.3" };
	c.doCkeckin(dbele, f1);
	getchar();
	return 0;
}
#endif



