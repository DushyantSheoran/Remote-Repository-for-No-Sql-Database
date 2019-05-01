#pragma once
///////////////////////////////////////////////////////////////////////
// ServerPrototype.h - Console App that processes incoming messages  //
// ver 1.0															 //
// Author : Dushyant Sheoran, CSE687   							     //
//			dsheoran@syr.edu +1(704)-957-2519					     //
// Source : Jim Fawcett, CSE687 - Object Oriented Design, S2018      //
///////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*  Package contains one class, Server, that contains a Message-Passing Communication
*  facility. It processes each message by invoking an installed callable object
*  defined by the message's command key.
*
*  Message handling runs on a child thread, so the Server main thread is free to do
*  any necessary background processing (none, so far).
* 
* Public Interfaces
* -------------------
* start				: start server's instance of Comm 
* stop				: stop Comm instance
* addMsgProc		: add ServerProc callable object to server's dispatcher 
* processMessages	: start processing messages on child thread
* postMessage		: pass message to Comm for sending
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
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include "../Messages/Messages.h"
#include "../MsgPassingComm/Comm.h"
#include <windows.h>
#include <tchar.h>
#include "../DBCore/Definitions.h"
#include "../DBCore/DBCore.h"
#include "../PayLoad/PayLoad.h"
#include "../Checkin/Checkin.h"
#include "../Version/Versioning.h"
#include "../Checkout/Checkout.h"


//-----------<Repository server >----------------------------
namespace Repository
{
	using File = std::string;
	using Files = std::vector<File>;
	using Dir = std::string;
	using Dirs = std::vector<Dir>;
	using SearchPath = std::string;
	using Key = std::string;
	using Msg = MsgPassingCommunication::Message;
	
	const SearchPath storageRoot = "../Storage";  // root for all server file storage
	const MsgPassingCommunication::EndPoint serverEndPoint("localhost", 8080);  // listening endpoint

	class Server
	{
	public:
		using ServerProc = std::function<Msg(Server &, Msg)>;
		using MsgDispatcher = std::unordered_map<Key, ServerProc>;

		Server(MsgPassingCommunication::EndPoint ep, const std::string& name,NoSqlDb::DbCore<NoSqlDb::PayLoad> &db, NoSqlDb::DbElement<NoSqlDb::PayLoad> &dbele_, RepositoryService::Checkin<NoSqlDb::PayLoad> chk, RepositoryService::Version v);
		Server() = default;
		void start();
		void stop();
		void addMsgProc(Key key, ServerProc proc);
		void processMessages();
		void postMessage(MsgPassingCommunication::Message msg);
		MsgPassingCommunication::Message getMessage();
		static Dirs getDirs(const SearchPath& path = storageRoot);
		static Files getFiles(const SearchPath& path = storageRoot);
		 bool formDBElement(MsgPassingCommunication::Message msg);
		 MsgPassingCommunication::Message formMessageAttributes(MsgPassingCommunication::Message msg);
		static bool makeDbEntry(const NoSqlDb::DbElement<NoSqlDb::PayLoad> &dblem);
		Server *sendServerobj() { return this; }
		RepositoryService::Checkin<NoSqlDb::PayLoad> CheckinGet() { return chkin; };
		NoSqlDb::DbElement<NoSqlDb::PayLoad> dbEleme() { return dbele_; };
		NoSqlDb::DbCore<NoSqlDb::PayLoad> dbStr() { return _db; }
		void checkoutHelper(RepositoryService::iterable_queue<Key> &choutFiles);
		Msg queyHelper(Msg msg);
		std::vector<std::string> walkthroughGraph(Keys &keys);
		Msg formDbMsg(Msg msg);
		std::vector<std::string> compoundQuery(Msg msg);
		bool testR3c();
		std::vector<std::string> catHelper(std::string);
		std::vector<std::string> verHelper(std::string);
		std::vector<std::string> fileHelper(std::string);
		std::vector<std::string> statHelper(std::string);
	private:
		MsgPassingCommunication::Comm comm_;
		MsgDispatcher dispatcher_;
		std::thread msgProcThrd_;
		NoSqlDb::DbCore<NoSqlDb::PayLoad> &_db;
		NoSqlDb::DbElement<NoSqlDb::PayLoad> &dbele_;
		RepositoryService::Checkin<NoSqlDb::PayLoad> chkin;
		RepositoryService::Version v;
	};
	//----< initialize server endpoint and give server a name >----------

	inline Server::Server(MsgPassingCommunication::EndPoint ep, const std::string& name,NoSqlDb::DbCore<NoSqlDb::PayLoad>& db, NoSqlDb::DbElement<NoSqlDb::PayLoad> &dbele, RepositoryService::Checkin<NoSqlDb::PayLoad> chkn, RepositoryService::Version v)
		: comm_(ep, name), _db(db),dbele_(dbele),chkin(chkn),v(v) {}

	//----< start server's instance of Comm >----------------------------

	inline void Server::start()
	{
		comm_.start();
	}
	//----< stop Comm instance >-----------------------------------------

	inline void Server::stop()
	{
		if (msgProcThrd_.joinable())
			msgProcThrd_.join();
		comm_.stop();
	}
	//----< pass message to Comm for sending >---------------------------

	inline void Server::postMessage(MsgPassingCommunication::Message msg)
	{
		comm_.postMessage(msg);
	}
	//----< get message from Comm >--------------------------------------

	inline MsgPassingCommunication::Message Server::getMessage()
	{
		Msg msg = comm_.getMessage();
		return msg;
	}
	//----< add ServerProc callable object to server's dispatcher >------

	inline void Server::addMsgProc(Key key, ServerProc proc)
	{
		dispatcher_[key] = proc;
	}
	//----< start processing messages on child thread >------------------

	inline void Server::processMessages()
	{
		auto proc = [&]()
		{
			if (dispatcher_.size() == 0)
			{
				std::cout << "\n  no server procs to call";
				return;
			}
			while (true)
			{
				Msg msg = getMessage();
				Msg reply;
				Msg reply2;
				Msg quitmessage;
				if (msg.containsKey("verbose"))
				{
					std::cout << "\n";
				}
				if (msg.command() == "serverQuit")
				{
					break;
				}
				if (msg.command() == "check-in")
				{
					reply2 = dispatcher_["fileRequest"](*this,msg);
					postMessage(reply2);
				}
				if ((dispatcher_.find(msg.command()) != (dispatcher_.end())))
					reply = dispatcher_[msg.command()](*this,msg);
				else
					dispatcher_["defaultFunction"](*this,msg);
				if (msg.to().port != msg.from().port)  // avoid infinite message loop
				{
					//To avoid reply infinite loop
					if (reply.to().port != msg.to().port) {
						postMessage(reply);
					}
				}
			
			}
			std::cout << "\n  server message processing thread is shutting down";
		};
		std::thread t(proc);
		msgProcThrd_ = std::move(t);
	}
}