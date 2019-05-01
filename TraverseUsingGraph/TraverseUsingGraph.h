#pragma once
/////////////////////////////////////////////////////////////////////////////////
// TraveseUsingGraph.h - Implements browsing component using graph			   //
//  ver 1.0                                                                    //
//  Language:         Visual C#  2017                                          //
//  Platform:         Lenovo Z580 Windows 10                                   //
//  Application :     Software Repository TestBed , S18                        //
// Author : Dushyant Sheoran, CSE687   							               //
//			dsheoran@syr.edu +1(704)-957-2519					               //
/////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides two classes:
* TraverseUsingGraph
* That implements graph walks using the graph structure the Repository already have with NoSql Db.
* Can have any type of walk based on the callable object passed to the walker.
*
* iterable_queue<T>
* A STL queue which has iterator operations on it.
*
* Member functions:
* -------------------
* walker	        : This decides type of graph walk based on callable object passed to it
* getDependencies   : Retrives all the dependecies taking care of the circular dependent notion
* find				: Finds the element present in the Db which is typically a grsph structure
* DFS_q				: The walker implemented to wlak is Depth First search.
*

* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* DateTime.h, DateTime.cpp
* Utilities.h, Utilities.cpp
* Version.h; Persist.h
* FileSystem --> static library
* XmlDocumnet --> static library
* Datetime --> static library
*
*
* Build Process:
*------------------
* CL /EHsc TraverseUsingGraph.cpp ../FileSystem/FileSystemDemo/FileSystem.cpp ../DateTime/DateTime.cpp
* ../XmlDocument/XmlDocument/XmlDocument.cpp ../XmlDocument/XmlElement/XmlElement.cpp
* ../XmlDocument/XmlParser/XmlParser.cpp ../XmlDocument/XmlElementParts/XmlElementParts.cpp
* ../XmlDocument/XmlElementParts/Tokenizer.cpp
*
*
* Maintenance History:
* --------------------
* ver 1.0 : Feb 2018
* - first release
*/
#include <memory>
#include<iostream>
#include<queue>
#include "../DBCore/DBCore.h"
#include "../DBCore/Definitions.h"
using namespace NoSqlDb;
//-----------------< Class declaration >---------
namespace RepositoryService {
	template<typename T>
	using Sptr1 = DbElement<T>;
	template<typename V, typename P, typename T>

	class TraverseUsingGraph {
	public:
		TraverseUsingGraph(DbCore<T> &db) :db_(db) {}
		template<typename R, typename callObj>
		R walker(callObj t, Sptr1<T> pNode, P p, DbCore<T> &db, Key fileName, int brow = 0, bool walkAll = false);
		void unmarkAll();
		static void identity(std::ostream& out = std::cout);
		DbCore<T>& persistDb() { return db_; }
		template<typename R>
		R getDependencies(Key fileName, Sptr1<T> pnode, P proc);
		Sptr1<T> find(const Key &filename);

	private:
		DbCore<T> &db_;
	};
	//Iterable queue source stack_overflow 
	template<typename T, typename Container = std::deque<T> >
	class iterable_queue : public std::queue<T, Container>
	{
	public:
		using iterator = typename Container::iterator;
		using const_iterator = typename Container::const_iterator;
		iterator begin() { return this->c.begin(); }
		iterator end() { return this->c.end(); }
		const_iterator begin() const { return this->c.begin(); }
		const_iterator end() const { return this->c.end(); }
	};
	//---------------< DFS walk of graph using a queue >------------------
	template<typename P, typename T>
	iterable_queue<Key> DFS_q(Sptr1<T> pNode, P p, DbCore<T> &db, Key fileName, int chkin = 0)
	{
		std::queue<Sptr1<T>> nodeQueue;
		nodeQueue.push(pNode);
		iterable_queue<Key> traversed;
		iterable_queue<Key> copy;
		if (chkin == 1)
		{
			traversed.push(fileName);
		}
		while (nodeQueue.size() > 0)
		{
			Sptr1<T> pCurrentNode = nodeQueue.front();
			nodeQueue.pop();
			if (traversed.size() > 0) {
				for (auto var : traversed)
					copy.push(var);
				fileName = traversed.front();
				traversed.pop();
			}
			p(pCurrentNode, fileName);
			for (auto pChild : pCurrentNode.children())
			{
				if (std::find(copy.begin(), copy.end(), pChild) != copy.end())
					continue;
				traversed.push(pChild);
				nodeQueue.push(db[pChild]);
			}

		}
		return copy;

	}
	template<typename V, typename P, typename T>
	inline void TraverseUsingGraph<V, P, T>::identity(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}

	//-----------------< get the dependencies of the given node >--------------
	template<typename V, typename P, typename T>
	template<typename R>
	R TraverseUsingGraph<V, P, T>::getDependencies(Key fileName, Sptr1<T> pNode, P proc)
	{
		DbCore<T> &db = persistDb();
		return walker<R>(DFS_q<P,T>(pNode,proc,db_,fileName,0), pNode, proc,db_,fileName);
	}
	//-------------------------< To unmark all the nodes visited already This way cam be used.
// but in this project implemented by taking a queue that contains all the visited nodes >------
	template<typename V, typename P, typename T>
	void TraverseUsingGraph<V, P, T>::unmarkAll()
	{
		for (auto k : db_.keys())
		{
			db_[k].payLoad().isTraversed(false);
		}
	}
	//----------------< walker function that calls graph walk based on the callable passed callable object >---------
	template<typename V, typename P, typename T>
	template<typename R, typename callObj>
	R TraverseUsingGraph<V, P, T>::walker(callObj t, Sptr1<T> pNode, P p, DbCore<T> &db, Key filename, int brow, bool walkAll)
	{
		return t(pNode, p, db, filename, brow);

	}
	template<typename V, typename P, typename T>
	Sptr1<T> TraverseUsingGraph<V, P, T>::find(const Key& filename)
	{
		DbElement<T> dbele;
		if (db_.contains(filename))
			return db_[filename];
		else return dbele;
	}

	

}

