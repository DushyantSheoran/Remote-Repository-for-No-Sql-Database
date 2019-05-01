#pragma once
/////////////////////////////////////////////////////////////////////////////////
// DateTime.h - represents clock time										  //
//  ver 1.0                                                                   //
//  Language:         Visual C#  2017                                         //
//  Platform:         Lenovo Z580 Windows 10                                  //
//  Application :     Key/Value DataBase , S18                                //
//  Author      :     Harika Bandaru, Syracuse University                     //
//                    hbandaru@syr.edu (936)-242-5972)						  //
// Source		:     Jim Fawcett, CSE687 - Object Oriented Design, Spring 18 //
////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* The DateTime class represents clock time, and supports:
* - creating default instances and instances from specific time points
* - return times as formatted strings
* - building time points and durations from years, months, days, hours, ...
* - performing addition and subtraction of times
* - comparing times
* - extracting counts of years, months, days, hours, minutes, and seconds
*
* Required Files:
* ---------------
*   DateTime.h, DateTime.cpp
*
* Maintenance History:
* --------------------
* ver 1.0 : 18 Feb 2017
*/
#include<chrono>
#include<ctime>
#include<string>
//------< clas interface for DateTime >-----------
class DateTime
{
public:
	using SysClock = std::chrono::system_clock;
	using TimePoint = std::chrono::system_clock::time_point;
	using Duration = std::chrono::system_clock::duration;

	DateTime();
	DateTime(std::string dtStr);
	DateTime(const TimePoint& tp);
	operator std::string();

	std::string now();
	TimePoint timepoint();
	size_t ticks();
	std::string time();
	static TimePoint makeTime(
		size_t yrs, size_t mon, size_t day,
		size_t hrs = 0, size_t min = 0, size_t sec = 0
	);
	static Duration makeDuration(
		size_t hrs, size_t min, size_t sec = 0, size_t millisec = 0
	);

	DateTime operator+=(const Duration& dt);
	DateTime operator+(const Duration& dt);
	DateTime operator-=(const Duration& dt);
	DateTime operator-(const Duration& dt);
	bool operator<(const DateTime& dt);
	bool operator>(const DateTime& dt);

	size_t year();
	size_t month();
	size_t day();
	size_t hour();
	size_t minute();
	size_t second();
private:
	char* ctime(const std::time_t* pTime);
	std::tm* localtime(const time_t* pTime);
	TimePoint tp_;

};