
#pragma once
/////////////////////////////////////////////////////////////////////////////////
// IPayLoad.h  -	Provides an interface that PayLoad derives from			   //
//  ver 1.0                                                                    //
//  Language:         Visual C#  2017                                          //
//  Platform:         Lenovo Z580 Windows 10                                   //
//  Application :     Software Repository TestBed , S18                        //
// Author : Dushyant Sheoran, CSE687   							               //
//			dsheoran@syr.edu +1(704)-957-2519					               //
// Source		:     Jim Fawcett, CSE687 - Object Oriented Design, Spring 18  //
////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides class:
* - aplication specific payload part specifies FilePath ; Categories ;
*	check-in status for the application
* Member functions:
* -------------------
* isClosed	    : This represents whether the check-in is closed or not.
* toXmlElement  : Forms an Xml representation for the given data
*
*Build Process:
-------------------
* CL /EHsc PayLoad.cpp
* ../XmlDocument/XmlDocument/XmlDocument.cpp ../XmlDocument/XmlElement/XmlElement.cpp
* ../XmlDocument/XmlParser/XmlParser.cpp ../XmlDocument/XmlElementParts/XmlElementParts.cpp
* ../XmlDocument/XmlElementParts/Tokenizer.cpp ../StringUtilities/StringUtilities.cpp
*
* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* Utilities.h, Utilities.cpp
* XmlDocumnet --> static library
*
*
* Maintenance History:
* --------------------
* ver 1.0 :  Feb 2018
* - first release
*/


namespace NoSqlDb
{
#include "../XmlDocument/XmlElement/XmlElement.h"
	using Xml = std::string;
	using Sptr = std::shared_ptr<XmlProcessing::AbstractXmlElement>;

	template<typename P>
	struct IPayLoad
	{
		virtual Sptr toXmlElement() = 0;
		static P fromXmlElement(Sptr elem);
		virtual ~IPayLoad() {};
	};
}
