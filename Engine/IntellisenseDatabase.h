#pragma once
#include <list>
#include <unordered_map>
#include <string>

namespace Intellisense
{
	struct Member
	{
		enum MEMBERTYPE
		{
			PROPERTY, METHOD
		};
		MEMBERTYPE type;
		std::string name;
	};

	struct Object
	{
		std::list<Member> members;
		typedef std::list<Member>::const_iterator memberIter;
	};

	void AddObject(const std::string name);
	void AddMember(const std::string objectName, const std::string fullname, const Member::MEMBERTYPE type);

	//Retrieve object based on typename, returns 0 if none found
	const Object* GetObjectFromName(std::string name);
};
