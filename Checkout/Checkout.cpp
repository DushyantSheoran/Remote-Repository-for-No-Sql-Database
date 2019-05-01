///////////////////////////////////////////////////////////////////////
// Checkout.cpp : Chekout component for Software Repository Testbed  //
// ver 1.0														     //
// Author : Dushyant Sheoran, CSE687   							     //
//			dsheoran@syr.edu +1(704)-957-2519					     //
///////////////////////////////////////////////////////////////////////
#include "../DBCore/DBCore.h"
#include "../PayLoad/PayLoad.h"
#include <iostream>
#include<string>
#include "../Persist/Persist.h"
#include "Checkout.h"
using namespace NoSqlDb;
using namespace RepositoryService;
//-------------------< test-stub >---------------
#ifdef Chkout_Test
int main()
{
	DbCore<PayLoad> db;
	std::unordered_map<FileName, size_t> vHold;
	Version v(vHold);
	Checkout<PayLoad> chkout(db, v);

	Persist<PayLoad> persist(chkout.fromPersistChkin());
	chkout.storagePath("../Storage");
	chkout.chkoutPath("../CheckoutStorage");
	FileName f = Path::fileSpec(chkout.storagePath(), "persistDb");
	persist.loadfromFile(f);
	std::cout << db.size();
	FileName chkoutfile = "test.testFile1.cs.1";
	chkout.doCheckout(chkoutfile);

	return 0;
}
#endif 
