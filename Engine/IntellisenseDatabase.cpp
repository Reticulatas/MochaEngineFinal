#include "stdafx.h"
#include "IntellisenseDatabase.h"

std::unordered_map<std::string, Intellisense::Object> elements;
typedef std::unordered_map<std::string, Intellisense::Object>::const_iterator elementIter;

void Intellisense::AddObject( std::string name )
{
	elements[name];
}

void Intellisense::AddMember( std::string objectName, std::string fullname, Intellisense::Member::MEMBERTYPE type )
{
	Member m;
	m.name = fullname;
	m.type = type;
	elements[objectName].members.push_back(m);
}

const Intellisense::Object* Intellisense::GetObjectFromName( std::string name )
{
	elementIter iter = elements.find(name);
	if (iter != elements.end())
		return &(iter->second);
	return 0;
}