/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include <algorithm>

//functor for std::for_each and similiar generic container modification
//calls delete on every POINTER in the container
//usage: 	std::for_each(removeComponentButtons.begin(), removeComponentButtons.end(), delete_ptr_element);
struct delete_ptr_element
{
	template <typename T>
	void operator()(T element) const
	{
		delete(element);
	}
};

//make a list of the missing elements from a second list compared to the first
//checklist - the bigger list to check against
//compList - the smaller list that is missing elements from the checkList
template <typename T>
void ListFindDifferences(std::list<T>& checkList, std::list<T>& compList, std::list<T>& destChangeList)
{
	std::list<T>::iterator iter = checkList.begin();
	for (; iter != checkList.end(); ++iter)
	{
		bool didFind = false;
		std::list<T>::iterator citer = compList.begin();
		for (; citer != compList.end(); ++citer)
		{
			if (*citer == *iter)
			{
				didFind = true;
				break;
			}
		}
		if (!didFind)
			destChangeList.push_back(*iter);
	}
}

void ListLogList(std::list<std::string>& l);
