
#include <iostream>
#include <stdio>

class Object
{
	public:
		virtual ~Object() = 0;
};

template <typename T>
class Member : public Object
{
	T _value;
	template <typename T2>
	Relationship<T,T2> _relationship;

public:
	virtual ~Member(void) { }

	T& operator*(void) const
 	{ 
		//validate relationship on access
					
		return _value; 
	}
};

//base relation template
template <typename T1, T2>
class Relation
{
public:
	//return whether the relationship between the two is valid
	virtual bool Equal(Member<T1>& k_, Member<T2>& l_) const = 0;  
	//returns the value of T2 based on the value of T1
	virtual T2& Assignment(Member<T1>& k_) const = 0;  
	//returns the value of T1 based on the value of T2
	virtual T1& Assignment(Member<T2>& k_) const = 0;  

	virtual ~Relation(void) { }
};

//relation defines T2 to be twice T1
template <typename T1, T2>
class TwiceRelation : public Relation
{
public:
	virtual bool Equal(Member<T1>& k_, Member<T2>& l_) const 	{ return *k_ * 2 == *l_; }
	virtual T2& Assignment(Member<T1>& k_) const 							{ return *k_ * 2; } 
	virtual T1& Assignment(Member<T2>& k_) const							{ return *k_ * 0.5f; }
};

template <typename T1, T2>
class Relationship : public Object
{
	Member<T1>* _m1;
	Member<T2>* _m2;
	Relation<T1, T2> _relation;
	
public:	
	Relationship(Relation<T1, T2> relation_) : Object(), _relation(relation_) { } 

	virtual ~Relationship(void) { }
};

int main(void)
{

}
