/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include <algorithm> 
#include <functional> 
#include <cctype>
#include "MathUtilities.h"
#include <sstream>
#include <locale>
#include <chrono>

// trim from start
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

static inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


static inline std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

static inline std::string replacechars(const std::string &s, const char currentChar, const char newChar)
{
	std::string newS = std::string(s);
	for (unsigned int i = 0; i < s.length(); i++)
		if (s[i] == currentChar)
			newS[i] = newChar;
	return newS;
}

static inline std::string replacestrings(const std::string &s, const std::string &currentStr, const std::string &newStr)
{
	std::string newS = std::string(s);
	size_t pos = 0;
	while ((pos = newS.find(currentStr, pos)) != std::string::npos)
	{
		newS.replace(pos, currentStr.length(), newStr);
		pos += newStr.length();
	}
	return newS;
}

template <class T>
std::string ToString( T input )
{
	std::stringstream ss;
	ss << input;
	return ss.str();
}

static inline int Convert(std::string &s)
{
	return (atoi(s.c_str()));
}

//approximate string search algorithm - implemented from http://en.wikipedia.org/wiki/Levenshtein_distance
static inline int LevenshteinDistance(std::string& s, std::string& t)
{
	// degenerate cases
	if (s == t) return 0;
	if (s.length() == 0) return t.length();
	if (t.length() == 0) return s.length();

	// create two work vectors of integer distances
	int v0length = t.length()+1;
	int v1length = t.length()+1;
	int* v0 = new int[v0length];
	int* v1 = new int[v1length];

	// initialize v0 (the previous row of distances)
	// this row is A[0][i]: edit distance for an empty s
	// the distance is just the number of characters to delete from t
	for (int i = 0; i < v0length; i++)
		v0[i] = i;

	for (int i = 0; i < static_cast<int>(s.length()); i++)
	{
		// calculate v1 (current row distances) from the previous row v0

		// first element of v1 is A[i+1][0]
		//   edit distance is delete (i+1) chars from s to match empty t
		v1[0] = i + 1;

		// use formula to fill in the rest of the row
		for (int j = 0; j < static_cast<int>(t.length()); j++)
		{
			auto cost = (s[i] == t[j]) ? 0 : 1;
			v1[j + 1] = Minimum<int>(v1[j] + 1, Minimum<int>(v0[j + 1] + 1, v0[j] + cost));
		}

		// copy v1 (current row) to v0 (previous row) for next iteration
		for (int j = 0; j < v0length; j++)
			v0[j] = v1[j];
	}

	return v1[t.length()];
}

// Returns a nicely formatted current date and time (used in CrashHandler)
static std::string GetTimeStamp()
{
	// using std::chrono::system_clock;
	// std::chrono::duration<int, std::ratio<60 * 60 * 24> > one_day(1);
	std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
	// system_clock::time_point tomorrow = today + one_day;

	std::time_t currentTime = std::chrono::system_clock::to_time_t(today);
	std::cout << "today is: " << ctime(&currentTime);

	std::time_t currentTime2;
	time(&currentTime2);
	tm* timeinfo = localtime(&currentTime2); // asctime(timeinfo)

	std::string returnStr;
	const unsigned int buffersize = 128;
	char strBuffer[buffersize];
	// Convert to a nice format: eg. Wed_01-25-2015_14-50-24
	strftime(strBuffer, buffersize, "%a_%m-%d-%Y_%H-%M-%S", timeinfo);
	returnStr = strBuffer; // Copy over to std::string
	return returnStr;
}
