/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0															   //
// Author : Dushyant Sheoran, CSE687   							       //
//			dsheoran@syr.edu +1(704)-957-2519					       //
// Source : Jim Fawcett, CSE687 - Object Oriented Design, S2018        //
/////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*  Package contains one class, Server, that contains a Message-Passing Communication
*  facility. It processes each message by invoking an installed callable object
*  defined by the message's command key.
*
*  Message handling runs on a child thread, so the Server main thread is free to do
*  any necessary background processing (none, so far)
*  
*  Public Interface:
*--------------------
*  getFiles			:   Uses FileSystem Package to retrieve files 
*  getDirs			:	Uses FileSystem Package to retrieve Directories 
*  connectping		:	To server the connect requst by the client taken care by the socket package just
*						making a reply and sending to client.
*  fileRequest		:	To send a file-request on processing the check-in request.
*  checkinProcess   :   To check-in the file and equest the client comm to send the file to the server
*  chkoutProcess	:   To process the check-out with adding the dependencies of the file with metadata
*  fileCopy			:   Process the and copy the requested files by the client via communication channel
*  filestatus		:	To send the file status information
*  getFiles(lambada):	Invoked if the client sends a getFiles message mapped to the key getFiles in the server's dispatcher 
*  getDirs(lambada)	:	Invoked if the client sends a getDirs message mapped to the key getDirs in the server's dispatcher 
*  defaultFunction	:	To overcome erros when client unintenally press any key 
*  fileRecieved		:	Send file status on recieving the message
*  Requirement1HelperFunction : To show the requirement are meet by doing these things.
*  
* 
* Build Process:
* --------------
* devenv ServerPrototype.sln /rebuild debug
*
*
*  Required Files:
* -----------------
*  ServerPrototype.h, ServerPrototype.cpp
*  Comm.h, Comm.cpp, IComm.h
*  Message.h, Message.cpp
*  FileSystem.h, FileSystem.cpp
*  Utilities.h
*
*  Maintenance History:
* ----------------------
*  ver 1.0 : 3/27/2018
*  - first release
*/

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>
#include "../Utilities/Utilities.h"
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../Display/Display.h"
#include "../Queries/Queries.h"
#include "../StringUtilities/StringUtilities.h"

namespace MsgPassComm = MsgPassingCommunication;

using namespace NoSqlDb;
using namespace Repository;
using namespace FileSystem;
using namespace RepositoryService;
using Msg = MsgPassingCommunication::Message;

using repoNodes = iterable_queue<Key>;
std::queue<DbElement<PayLoad>> qu;
class ReposDbMaintain {
	bool formDbElement(Msg rcvmsg);
	bool clear();

private:
	DbCore<PayLoad> &db_;
	DbElement<PayLoad> &dbele_;
};
//-------< Uses FileSystem Package to retrieve files >-------------
Files Server::getFiles(const Repository::SearchPath& path)
{
	return Directory::getFiles(path);
}
//------------< Uses FileSystem Package to retrieve Directories >---------------------------
Dirs Server::getDirs(const Repository::SearchPath& path)
{
	return Directory::getDirectories(path);
}
template<typename T>
void show(const T& t, const std::string& msg)
{
	std::cout << "\n  " << msg.c_str();
	for (auto item : t)
	{
		std::cout << "\n    " << item.c_str();
	}
}
//------------------------< To show requirement #1 have been satisfied >--------------------------
bool Requirement1HelperFunction()
{
	Utilities::StringHelper::Title("       Requirement #1       ");
	std::cout << "\n  " << typeid(std::function<bool()>).name()
		<< ", declared in this function, "
		<< "\n  is only valid for C++11 and later versions.";
	Utilities::StringHelper::Title("\n         Requirement #1b         ");
	std::cout << "\n  A visual examination of all the submitted code "
		<< "will show only\n  use of streams and operators new and delete.";

	return true;
}
//----------------------< A function in server dispatcher that handles if the message key is connect >----------------
std::function<Msg(Server &,Msg)> connectping = [](Server &s,Msg msg)
{
	Utilities::StringHelper::Title("\n       Requireemnt # 5 Asynchronous Communication      ");
	std::cout << "\n  Recieved a connect message from Client\n";
	Utilities::StringHelper::Title("        Requiremment #5b    \"HTTP STYLE MESSAGES\"");
	msg.show();
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("Connection-set");
	return reply;
};
//-------------------< Server request a file to the client >--------------------------------

std::function<Msg(Server &,Msg)> fileRequest =[](Server &s,Msg rcvmsg)
{
	Msg reply;
	Utilities::StringHelper::Title("\n    Requirement #3 and #6 Recieved Check-in and Server sending \"File DownLoad\" Request as a result Client \"Upload Files\"");
	std::cout << "\n Sending File Request as part of Check-in\n";
	reply.to(rcvmsg.from());
	reply.from(rcvmsg.to());
	reply.attribute("MessageType", "Server sending File Request");
	reply.attribute("fileName", rcvmsg.value("fileName"));
	reply.command("sendFile");
	reply.show();
	return reply;
};
//-----------------< Helper function >----------------------
bool clear(DbElement<PayLoad> &dbele)
{
	dbele.name("");
	dbele.descrip("");
	dbele.children().clear();
	PayLoad p;
	p.isClosed("open");
	dbele.payLoad(p);
	return true;
}
//-------< stack overflow >-----------------------
std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}
//-------< Parse vector into a string >----------
std::string parseVaector(const std::vector<std::string> &vecStr)
{
	std::string parseString="";
	for (auto val : vecStr)
		parseString = parseString + val + "+";
	return parseString;
}
//------------< Form DbElement >-----------------
bool Server::formDBElement(Msg rcvmsg)
{	
	clear(dbele_);
	dbele_.name(rcvmsg.value("author"));
	dbele_.descrip(rcvmsg.value("description"));
	PayLoad p = dbele_.payLoad();
	Children cats;
	cats = split(rcvmsg.value("catogries"),'+');
	p.categories(cats);
	Children deps;
	deps = split(rcvmsg.value("dependencies"),'+');
	dbele_.children(deps);
	p.value(rcvmsg.value("path"));
	p.isClosed(rcvmsg.value("status"));
	dbele_.payLoad(p);
	dbele_.dateTime(DateTime().now());
	Key fileName = rcvmsg.value("fileName");
	for(auto i :deps)
	std::cout << i;
	qu.push(dbele_);
	return true;
}
//-------< Make entry in database >-------
bool Server::makeDbEntry(const DbElement<PayLoad> &dbelem)
{
	return true;
}


//-------< Invoked when client sends a check-in message this a function in server dispatcher for key "check-in" >-----

std::function<Msg(Server &,Msg)> checkinProcess = [](Server &s,Msg msg)
{
	Msg reply;
	Utilities::StringHelper::Title("\n         Requirement #2c Check-in message      ");
	std::cout << "\n  Recieved a check-in message from Client\n";
	msg.show();
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("check-in_status", "Recieved Check-in Message Success");
	reply.attribute("fileName", "NosqlDb.Persist.cpp.1");
	reply.attribute("statuschckin", "Success");
	s.formDBElement(msg);
	reply.command("dummy");
	return reply;
};
//----------------< Form message Attributes for sending reply >----------------------
Msg Server::formMessageAttributes(Msg msg)
{
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	using Sptr1 = Sptr1<PayLoad>;
	auto proc = [=](Sptr1 pNode, Key filename) { std::cout << ""; };
	using P = decltype(proc);
	using V = DbElement<PayLoad>;
	iterable_queue<Key> dependts;
	std::vector<std::string> getdepes;
	if (msg.value("Dependents").compare("yes") == 0) {

	TraverseUsingGraph<V, P, PayLoad> travers(_db);
	FileName fileName = msg.value("fileName");
	Utilities::title( "Checkout file--> "+fileName+" and its dependency chain");
	Sptr1 pNode = travers.find(fileName);
	//DFS on the NoSqlDb graph structure to get the dependents relation
	dependts = travers.walker<iterable_queue<Key>>(DFS_q<P, PayLoad>, pNode, proc, travers.persistDb(), fileName);
	dependts.push(msg.value("fileName"));
	checkoutHelper(dependts);
	
	for (auto d : dependts) {
		Msg reply2;
		reply2 = reply;
		reply.command("FileDownload");
		reply2.attribute("saveFilePath", "../../../ClientStorage");
		reply2.attribute("sendFilePath", "../CheckoutStorage");
		getdepes.push_back(d);
		reply2.file(v.removeVersion(d));
		postMessage(reply2);
	}
	std::string dep = parseVaector(getdepes);
	reply.attribute("Dependecy", dep);

	}
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("check-out_status", "Recieved Check-out Message Success");
	reply.messageText("Check-out reply");
	Key key_ = msg.value("fileName");
	PayLoad p = _db[key_].payLoad();
	reply.attribute("name", _db[key_].name());
	reply.attribute("Description",_db[key_].descrip());
	reply.attribute("FilePath",p.value());
	std::string cat = parseVaector(p.categories());
	reply.attribute("category", cat);	
	reply.attribute("fileName",key_);
	reply.command("check-out");
	return reply;
}
//-------------< Check-out Helper >------------------------
void Server::checkoutHelper(repoNodes &chkoutFiles)
{
	Checkout<PayLoad> chkout(_db, v);
	chkout.storagePath("../Storage");
	chkout.chkoutPath("../CheckoutStorage");
	chkout.doCheckoutgiven(chkoutFiles);
	return;
}
//--------< Invoked when client sends a check-out message mapped to server dispatcher for the key "check-out" >------
std::function<Msg(Server &,Msg)> chkoutProcess = [](Server &s,Msg msg)
{
	Utilities::StringHelper::Title("\n    Requirement #2c Recieved a check-out message");
	std::cout << "\nRecieved a check-out message from Client\n";

	Msg reply = s.formMessageAttributes(msg);	
	
	repoNodes que;
	if (msg.value("Dependents").compare("no") == 0) {
		Utilities::title("Checkout file--> " +msg.value("fileName") + "  without dependents");
		Msg reply2;
		reply2.to(msg.from());
		reply2.from(msg.to());
		reply2.attribute("saveFilePath", "../../../ClientStorage");
		reply2.attribute("sendFilePath", "../CheckoutStorage");
		int pos = msg.value("fileName").find_last_of('.');
		std::string end = msg.value("fileName").substr(pos + 1);
		if (isdigit(end[0]))
		{
			reply2.attribute("file", msg.value("fileName").substr(0, pos));
		}

		s.postMessage(reply2);
		que.push(msg.value("fileName"));
		s.checkoutHelper(que);
	}
	msg.show();
	
	return reply;
};
//--------< Invoked on echo if the server sends to itself as it has comm "both client and sever end points" >------
std::function<Msg(Server &,Msg)> echo = [](Server &s,Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	return reply;
};

//------< Invoked if the client sends file request mapped to the key filesCopy >----------------
std::function < Msg(Server &,Msg)> fileCopy = [](Server &s,Msg msg)
{
	Utilities::StringHelper::Title("\n    Requirement #2b Recieved a SendFile Request message");
	std::cout << "\nRecieved a FileRequest during check-out from Client\n";
	msg.show();
	Msg reply;
	reply.command("file-status");
	std::unordered_map<std::string, std::string> attr = msg.getattributes();
	reply.to(msg.from());
	reply.from(msg.to());
	reply.file(msg.value("fileNeeded"));
	reply.attribute("sendFilePath", msg.value("sendFilePath"));
	reply.attribute("saveFilePath", msg.value("saveFilePath"));
	reply.messageText("File transfer success");
	return reply;
};

//---------------< response to SendFile function>-----------------
std::function <Msg(Server &,Msg)> DisplayDataBase = [](Server &s,Msg msg)
{
	Msg reply;
	Utilities::StringHelper::Title("\n           Repository DataBase After Check-ins       ");
	
	DbCore<PayLoad> sam = s.dbStr();
	Display<PayLoad> dis(sam);
	return reply;
};

//---------------< response to SendFile function>-----------------
std::function <Msg(Msg)> filestatus = [](Msg msg)
{
	Msg reply;
	Utilities::StringHelper::Title("\n           Requirement #2b        ");
	std::cout << "Recieved a File a file with Status From Client end";
	msg.show();
	return reply;
};
//-----------< Invoked if the client sends a getFiles message mapped to the key getFiles in the server's dispatcher >--
std::function<Msg(Server &,Msg)> getFiles = [](Server &s,Msg msg) {
	Msg reply;
	Utilities::StringHelper::Title("\n    Requirement #2c SendList of Files on Request");
	std::cout << "\nRecieved a getFiles request from Client as part of Browse\n";
	msg.show();
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFiles");
	reply.attribute("status", "Received Successfully");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Files files = Server::getFiles(searchPath);
		size_t count = 0;
		for (auto item : files)
		{
			std::string countStr = Utilities::Converter<size_t>::toString(++count);
			reply.attribute("file" + countStr, item);
		}
	}
	else
	{
		std::cout << "\n  getFiles message did not define a path attribute";
	}
	return reply;
};

//-----------< Invoked if the client sends a getDirs message mapped to the key getDirs in the server's dispatcher >--
std::function<Msg(Server &,Msg)> getDirs = [](Server &s,Msg msg) {
	Msg reply;
	Utilities::StringHelper::Title("\n    Requirement #2c SendList of directories on Request");
	std::cout << "\nRecieved a getDirs request from Client as part of Browse\n";
	msg.show();
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getDirs");
	reply.attribute("status", "Recieved Directories Successfully");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Files dirs = Server::getDirs(searchPath);
		size_t count = 0;
		for (auto item : dirs)
		{
			if (item != ".." && item != ".")
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("dir" + countStr, item);
			}
		}
	}
	else
	{
		std::cout << "\n  getDirs message did not define a path attribute";
	}
	return reply;
};
//--------------------< To overcome erros when client unintenally press any key >-------
std::function<Msg(Server &,Msg)> defaultFunction = [](Server &s,Msg msg)
{
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("noFunction");
	return reply;
};
//----------------< Walktrugh Graph >--------------------
std::vector<std::string> Server::walkthroughGraph(Keys &keys)
{
	
	using Sptr1 = Sptr1<PayLoad>;
	std::vector<std::string> keyDescPair;
	auto proc = [&](Sptr1 pNode, Key filename) { std::string formFormat = filename + ":" + pNode.descrip();
	keyDescPair.push_back(formFormat);
	};
	using P = decltype(proc);
	using V = DbElement<PayLoad>;
	TraverseUsingGraph<V, P, PayLoad> travers(_db);
	for (auto filename : keys)
	{
		Sptr1 pNode1 = travers.find(filename);
		
		travers.walker<iterable_queue<Key>>(DFS_q<P, PayLoad>, pNode1, proc, travers.persistDb(), filename, 1);
	}

	return keyDescPair;
}
//---------------< Category search Helper >-----------------
std::vector<std::string> Server::catHelper(std::string expression)
{
	categry cat = expression;
	DbQueries<PayLoad> qu(_db);
	std::vector<std::string> res;
	auto hasCategory = [&cat](DbElement<PayLoad> elem) {
		return (elem.payLoad()).hasCategory(cat);

	};
	res = walkthroughGraph(qu.select1(hasCategory).keys());
	res.erase(unique(res.begin(), res.end()), res.end());
	return res;
}
//----------< Version search Helper >----------------
std::vector<std::string> Server::verHelper(std::string expression)
{
	categry f1 = expression;
	DbQueries<PayLoad> qu(_db);
	std::vector<std::string> res;
	auto hasVersion = [&f1](Key key) {
		int pos = key.find_last_of('.');
		std::string end = key.substr(pos + 1);
		return end[0] == f1[0] ? true : false;
	};
	res = walkthroughGraph(qu.select(hasVersion).keys());
	res.erase(unique(res.begin(), res.end()), res.end()); 
	return res;
}
//-------------< File search Query Helper >------------
std::vector<std::string> Server::fileHelper(std::string expression)
{
	std::string f1 = expression;
	DbQueries<PayLoad> qu(_db);
	std::vector<std::string> res;
	auto hasFile = [&f1](Key key) {
		return key.compare(f1) == 0 ? true : false;
	};
	res = walkthroughGraph(qu.select(hasFile).keys());
	res.erase(unique(res.begin(), res.end()), res.end()); 
	return res;
}
//----------------------< Status Helper >----------------------
std::vector<std::string> Server::statHelper(std::string expression)
{
	std::string status = expression;
	DbQueries<PayLoad> qu(_db);
	std::vector<std::string> res;
	auto openStatus = [&status](DbElement<PayLoad> elem) {
		PayLoad p = elem.payLoad();
		return p.isClosed().compare(status) == 0 ? true : false;
	};
	res = walkthroughGraph(qu.select1(openStatus).keys());
	res.erase(unique(res.begin(), res.end()), res.end()); 
	return res;
}

//-----------------< Query Helper >--------------------
Msg Server::queyHelper(Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	reply.attribute("type", msg.value("type"));
	std::vector<std::string> res;
	std::string strVec;
	
	if (!(msg.value("type").compare("Category")))
	{
		res = catHelper(msg.value("regularExpres"));
	}
	if (!msg.value("type").compare("Open"))
	{
		res = statHelper("open");
		
	}
	if (!msg.value("type").compare("Closed"))
	{
		res = statHelper("closed");
	}

	if (!msg.value("type").compare("HasFile"))
	{
		std::string f1 = msg.value("regularExpres");
		res = fileHelper(f1);
	}
	if (!msg.value("type").compare("Version"))
	{
		std::string f1 = msg.value("regularExpres");
		res = verHelper(f1);
	}
	
		strVec = parseVaector(res);
		reply.attribute("queryresult", strVec);

	
	return reply;

}
//------------------< Process Custom Query >-------
std::function<Msg(Server &, Msg)> processCustomQuery = [](Server &s, Msg msg)
{
	Msg reply;
	
	reply = s.queyHelper(msg);
	reply.attribute("status", "success");
	return reply;
};
//-----------------< Finale Message to handle Download by Repo Server >--------------------
std::function<Msg(Server &, Msg)> finalMessage = [](Server &s, Msg msg)
{
	Utilities::StringHelper::Title("\nFile Downloaded Successfully");
	msg.show();
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	Checkin<PayLoad> chkin = s.CheckinGet();
	if (qu.size() > 0)
	{
		if(chkin.doCkeckin(qu.front(), msg.value("file")))
			reply.attribute("statuschckin", "Success");
		else
			reply.attribute("statuschckin", "Failed");
		qu.pop();
	}
	reply.attribute("check-in_status", "Shows Failed or Scuess . Tested for failure case 1 Ownership 2 OpenDepes");
	reply.attribute("fileName",msg.value("file"));
	s.formDBElement(msg);
	reply.command("check-in");
	return reply;
};
//----------------< Form the DBMessage >------------------
Msg Server::formDbMsg(Msg msg)
{
	std::string filename = msg.value("fileName");
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	DbElement<PayLoad> dbele = _db[filename];
	reply.attribute("Author", dbele.name());
	reply.attribute("Description", dbele.descrip());
	PayLoad p = dbele.payLoad();
	std::string categry = parseVaector(p.categories());
	reply.attribute("category", categry);
	std::string depe = parseVaector(dbele.children());
	reply.attribute("Dependecy", depe);
	reply.attribute("status", p.isClosed());
	reply.attribute("fileName", filename);
	return reply;

}
//--------------< File Content >------------------
std::function<Msg(Server &, Msg)> fileContent = [](Server &s, Msg msg)
{
	Utilities::StringHelper::Title("\nFile Double Click Request for File Content");
	msg.show();
	std::vector<std::string> splittedName = Utilities::split(msg.value("fileName"), '.');
	Msg reply = s.formDbMsg(msg);
	reply.command("fileContent");
	reply.attribute("path", "../../../Storage/" + splittedName[0]);
	return reply;
};

//-------------------< File recieved status >----------------------------
std::function<Msg(Msg)> fileRecieved = [](Msg msg)
{
	Utilities::StringHelper::Title("Recived a file from Client");
	msg.show();
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	return reply;
};

//-------------< Initially Repository Structure >---------
bool Server::testR3c()
{
	Utilities::title("\nIntital Repository Structure before running the application");
	Display<PayLoad> dis(_db);
	dis.showAll();
	return true;
}

//---------< Repository server entry point >------------
int main()
{
	DbCore<PayLoad> db;
	DbElement<PayLoad> dbelem;
	std::unordered_map<FileName, size_t> vHold;
	Version v(vHold);
	Children closePend;
	Checkin<PayLoad> ch(db, v,closePend);
	ch.staggingPath("../CheckinStorage");
	ch.storagePath("../Storage");
	Persist<PayLoad> persist(ch.toPersistChkin());
	FileName file = Path::fileSpec(ch.staggingPath(), "persistDb");
	persist.loadfromFile(file);
	Utilities::StringHelper::Title("Repository Server Prototype");
	Requirement1HelperFunction();
	Server server(serverEndPoint, "ServerPrototype",db, dbelem,ch,v);
	server.start();
	server.addMsgProc("echo", echo);
	
	server.addMsgProc("connect", connectping);
	server.addMsgProc("check-in", checkinProcess);
	server.addMsgProc("check-out", chkoutProcess);
	server.addMsgProc("defaultFunction", defaultFunction);
	server.addMsgProc("filesCopy", fileCopy);
	server.addMsgProc("fileRequest", fileRequest);
	server.addMsgProc("finalMessage", finalMessage);
	server.testR3c();
	server.addMsgProc("getFiles", getFiles);
	server.addMsgProc("getDirs", getDirs);
	server.addMsgProc("customQuery", processCustomQuery);
	server.addMsgProc("fileContent", fileContent);
	server.processMessages();

	Msg msg(serverEndPoint, serverEndPoint);  
	msg.name("msgToSelf");
	msg.command("echo");
	msg.attribute("verbose", "show me");
	server.postMessage(msg);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::cin.get();
	std::cout << "\n";
	return 0;
}

