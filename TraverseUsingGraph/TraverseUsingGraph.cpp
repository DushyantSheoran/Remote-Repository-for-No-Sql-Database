// TraverseUsingGraph.cpp : Defines the entry point for the console application.
//
/////////////////////////////////////////////////////////////////////
// TraverseUsingGraph.cpp : Browsing  component for Software	   //
//							Repository Testbed					   //
// ver 1.0														   //
// Author : Dushyant Sheoran, CSE687   							   //
//			dsheoran@syr.edu +1(704)-957-2519					   //
/////////////////////////////////////////////////////////////////////

#include "../DBCore/DBCore.h"
#include "../PayLoad/PayLoad.h"
#include <iostream>
#include<string>
#include "../Persist/Persist.h"
#include "TraverseUsingGraph.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"

#include<iostream>
using namespace NoSqlDb;
using namespace RepositoryService;
using namespace FileSystem;

//------------------< Test stub >-----------------------------------
#ifdef TEST_GRAPH
int main()
{
	using Sptr1 = Sptr1<PayLoad>;
	DbCore<PayLoad> db;
	auto proc = [=](Sptr1 pNode, Key filename) { std::cout << filename << " ::" << pNode.descrip() << "\n"; };
	using P = decltype(proc);
	using V = DbElement<PayLoad>;
	TraverseUsingGraph<V, P, PayLoad> travers(db);


	Persist<PayLoad> persist(travers.persistDb());
	FileName f = Path::fileSpec("../Storage", "persistDb");
	persist.loadfromFile(f);
	std::cout << db.size();

	Sptr1 pNode = travers.find("test.testFile1.cs.1");

	std::cout << "\nTraverse the repository using a graph DS\n";
	std::cout << "FileName :: Description\n";
	std::cout << "Dependency walk for \n" << "test.testFile1.cs.1";
	Key fileNameBro = "test.testFile1.cs.1";
	iterable_queue<Key> dependents;
	dependents = travers.walker<iterable_queue<Key>>(DFS_q<P, PayLoad>, pNode, proc, travers.persistDb(), fileNameBro);
	return 0;
}
#endif

