#pragma once
/////////////////////////////////////////////////////////////////////////
// Message.h - defines message structure used in communication channel //
// ver 1.0															   //
// Author : Dushyant Sheoran, CSE687   							       //
//			dsheoran@syr.edu +1(704)-957-2519					       //
// Source : Jim Fawcett, CSE687-OnLine Object Oriented Design,		   //
//						 Fall 2017									   //
/////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines an EndPoint struct and a Message class.
*  - Endpoints define a message source or destination with an address and port number.
*  - Messages have an HTTP style structure with a set of attribute lines containing
*    name:value pairs.
*  - Message have a number of getter, setter methods for common attributes, and allow
*    definition of other "custom" attributes.
*  - Added if we want to send vector<strings> in the message cattributes
* Build Process:
* --------------
* devenv Messages.sln /rebuild debug

*
*  Required Files:
*  ---------------
*  Messages.h, Messages.cpp, Utilities.h, Utilities.cpp
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 03 Oct 2017
*  - first release
*
*/
#include "../Utilities/Utilities.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace MsgPassingCommunication
{
	///////////////////////////////////////////////////////////////////
	// EndPoint struct

	struct EndPoint
	{
		using Address = std::string;
		using Port = size_t;
		Address address;
		Port port;
		EndPoint(Address anAddress = "", Port aPort = 0);
		std::string toString();
		static EndPoint fromString(const std::string& str);
	};

	inline EndPoint::EndPoint(Address anAddress, Port aPort) : address(anAddress), port(aPort) {}

	inline std::string EndPoint::toString()
	{
		return address + ":" + Utilities::Converter<size_t>::toString(port);
	}

	inline EndPoint EndPoint::fromString(const std::string& str)
	{
		EndPoint ep;
		size_t pos = str.find_first_of(':');
		if (pos == str.length())
			return ep;
		ep.address = str.substr(0, pos);
		std::string portStr = str.substr(pos + 1);
		ep.port = Utilities::Converter<size_t>::toValue(portStr);
		return ep;
	}
	///////////////////////////////////////////////////////////////////
	// Message class
	// - follows the style, but not the implementation details of
	//   HTTP messages

	class Message
	{
	public:
		using Key = std::string;
		using Value = std::string;
		using Attribute = std::string;
		using customAttribute = std::vector<Key>;
		using customAttributes = std::unordered_map<Key, customAttribute>;
		using Attributes = std::unordered_map<Key, Value>;
		using Keys = std::vector<Key>;

		Message();
		Message(EndPoint to, EndPoint from);

		Attributes& attributes();
		customAttributes& cattributes();
		void attribute(const Key& key, const Value& value);
		void cattribute(const Key& key, const customAttribute& value);
		Keys keys();
		static Key attribName(const Attribute& attr);
		static Value attribValue(const Attribute& attr);
		bool containsKey(const Key& key);
		Attributes getattributes() { return attributes_; }
		Value value(const Key& key);
		bool remove(const Key& key);


		EndPoint to();
		void to(EndPoint ep);
		EndPoint from();
		void from(EndPoint ep);
		std::string name();
		void name(const std::string& nm);
		std::string messageText();
		void messageText(const std::string& msg);
		std::string command();
		void command(const std::string& cmd);
		std::string file();
		void file(const std::string& fl);
		size_t contentLength();
		void contentLength(size_t ln);
		void clear();
		std::string toString();
		static Message fromString(const std::string& src);
		std::ostream& show(std::ostream& out = std::cout);

	private:
		Attributes attributes_;
		customAttributes _cattributes;
		// name            : msgName
		// command         : msg Command
		// to              : dst EndPoint
		// from            : src EndPoint
		// file1            : file name
		// content-length  : body length in bytes
		// custom attributes
	};
}