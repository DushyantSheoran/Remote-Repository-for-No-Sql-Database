/////////////////////////////////////////////////////////////////////
// Display.cpp - Display utility for the NoSql Db				   //
// ver 1.0														   //
// Author : Dushyant Sheoran, CSE687   							   //
//			dsheoran@syr.edu +1(704)-957-2519					   //
/////////////////////////////////////////////////////////////////////


#include<iostream>
#include "Display.h"
//----------------------< test stub >---------------------------------
#ifdef Display_Test

DbCore<PayLoadType>  & makeTestDbDisplay()
{
	static DbCore<PayLoadType> db;
	DbElement<PayLoadType> elem1;
	elem1.name("Bandaru");
	elem1.descrip("Testing Key/Value Db");
	elem1.dateTime(DateTime().now());
	PayLoadType value1;
	value1.filePath = "C:/FileFolder";
	value1.catogries = { "Cpp","test","display" };
	elem1.payLoad(value1);
	elem1.children().clear();
	elem1.insertChildren("fileName.cpp");
	elem1.insertChildren("fileName.cs");

	DbElement<PayLoadType> elem2;
	elem2.name("Konchada");
	elem2.descrip("Testing in memeory Db");
	elem2.dateTime(DateTime().now());
	PayLoadType value2;
	value2.filePath = "C:/FileFolder";
	value2.catogries = { "C#","test","display" };
	elem2.payLoad(value2);

	DbElement<PayLoadType> elem3;
	elem3.name("Janga");
	elem3.descrip("Testing NoSql Db");
	elem3.dateTime(DateTime().now());
	PayLoadType value3;
	value3.filePath = "C:/FileFolder";
	value3.catogries = { "java","test","display" };
	elem3.payLoad(value3);

	DbElement<PayLoadType> elem4;
	elem4.name("Potti");
	elem4.descrip("Testing a Db");
	elem4.dateTime(DateTime().now());
	PayLoadType value4;
	value4.filePath = "C:/FileFolder";
	value4.catogries = { "Python","test","display" };
	elem4.payLoad(value4);

	db.insertRecord("fileName.cpp", elem1);
	db.insertRecord("fileName.cs", elem2);
	db.insertRecord("fileName.java", elem3);
	db.insertRecord("fileName.py", elem4);
	return db;
}
int main()
{
	DbCore<PayLoadType> db = makeTestDbDisplay();
	Display<PayLoadType> display1(db);
	display1.showAll();
	display1.show("fileName.cpp");

	return 0;
}
#endif 
