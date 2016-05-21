#define CATCH_CONFIG_MAIN
#include "stdafx.h"
#include "catch.hpp"
#include "metadatabase.h"
#include "meta.h"
#include "def.h"
#include "metafuncs.h"
#include <string>
#include <type_traits>

int TestInvoke1()	{
	return 5;
}
unsigned TestInvoke2(unsigned b) { return b; }
int TestInvoke3(int b, char c) {
	return b;
}
void TestInvoke4() {}

struct A : public IMeta {
	int ab;
	char ac;

	enum AEnum
	{
		FIRST,
		SECOND,
		THIRD
	};

	static int sb;

	int Foo() { return 6; }
	std::string Bob(std::string b) { return b; }

	metadef(A)
		m_add(int, ab)
		m_tag("TestTag1")
		m_add(char, ac)
		m_addfunc(Bob)
		m_addstatic(int, sb)
		/*m_addenum(AEnum)
		m_enum_val(FIRST)
		m_enum_val(SECOND)
		m_enum_val(THIRD)*/
		endmetadef;
};
int A::sb = 0;

struct B : public IMeta {
	char bb;

	void Foo() { }
	int Bob(int a) { return a; }
	void* PtrFoo(A* a) { return reinterpret_cast<void*>(a); }
	char BB() { return bb; }

	metadef(B)
		m_add(char, bb)
		m_addfunc(BB)
		m_tag("BBFuncTag")
		endmetadef;
};
meta_define(A);
meta_define(B);

TEST_CASE("Meta Enum", "[meta]")
{
	//static validity
	//REQUIRE(A::AEnum::FIRST == 0);

}

TEST_CASE("Baked Meta Functions", "[meta]")
{
	B b;
	meta::metafunction_method* fstore_storable = MAKEMETAMETHODVARIABLE(B, Bob);
	fstore_storable->prepack_params(1);
	REQUIRE(fstore_storable->invoke_with_pack_params(&b).GetAs<int>() == 1);
}

TEST_CASE("Static Variable Meta", "[meta]")
{
	A a;
	A::sb = 5;

	meta::metastatic* s = A::getmeta().getstaticbyname(std::string("sb"));
	REQUIRE(s != 0);
	REQUIRE(s->raw_ptr == &A::sb);
	REQUIRE(*((int*)s->raw_ptr) == 5);
	REQUIRE(std::string(s->var_name) == "sb");
	REQUIRE(s->type_info->getisobjecttype<int>());
}

TEST_CASE("Function Meta Post Invoke", "[meta]")
{
	B* bob = new B();
	bob->bb = 5;

	char ret = bob->mymeta()._functions.at("BB")->invoke(bob).GetAs<char>();
	REQUIRE(ret == 5);

	REQUIRE(bob->mymeta()._functions.at("BB")->has_tag(std::string("BBFuncTag")));
}

TEST_CASE("Function Invoking", "[meta]")
{
	REQUIRE((meta::metafunction<decltype(&TestInvoke1), &TestInvoke1>::invoke() == 5));
	REQUIRE((meta::metafunction<decltype(&TestInvoke2), &TestInvoke2>::invoke(10) == 10));
	REQUIRE((meta::metafunction<decltype(&TestInvoke3), &TestInvoke3>::invoke(15, 'c') == 15));
	meta::metafunction<decltype(&TestInvoke4), &TestInvoke4>::invoke();

	A a;
	B b;
	REQUIRE((meta::metafunction_method_impl<A, decltype(&A::Foo), &A::Foo>::invoke(&a) == 6));
	meta::metafunction_method_impl<B, decltype(&B::Foo), &B::Foo>::invoke(&b);
	REQUIRE((meta::metafunction_method_impl<B, decltype(&B::Bob), &B::Bob>::invoke(&b, 3) == 3));
	REQUIRE((meta::metafunction_method_impl<B, decltype(&B::PtrFoo), &B::PtrFoo>::invoke(&b, &a) == reinterpret_cast<void*>(&a)));

	//direct invoke
	REQUIRE(MAKEMETAMETHOD(B, Bob)::invoke(&b, 11) == 11);

	//storing global
	meta::metafunction_method* fstore_storable = MAKEMETAMETHODVARIABLE(B, Bob);
	REQUIRE(fstore_storable->invoke(&b, 1).GetAs<int>() == 1);

	meta::metafunction_method* fstore2 = MAKEMETAMETHODVARIABLE(B, Bob);
	REQUIRE(fstore2->invoke(&b, 6).GetAs<int>() == 6);

	meta::metafunction_method* fstore3 = MAKEMETAMETHODVARIABLE(B, Foo);
	fstore3->invoke(&b);
	meta::metafunction_method* fstore4 = MAKEMETAMETHODVARIABLE(B, PtrFoo);
	REQUIRE(fstore4->invoke(&b, &a).GetAs<void*>() == reinterpret_cast<void*>(&a));

	//retrieving class data
	b.bb = 10;
	meta::metafunction_method* fstore5 = MAKEMETAMETHODVARIABLE(B, BB);
	REQUIRE(fstore5->invoke(&b).GetAs<char>() == 10);
}

TEST_CASE("Type Members Validity", "[meta]")
{
	const char* names[] = { "ab", "ac" };
	const size_t sizes[] = { sizeof(int), sizeof(char) };
	unsigned i = 0;
	for (const meta::metamember* m : A::getmeta().getmemberslist())
	{
		REQUIRE((strcmp(m->var_name, names[i]) == 0));
		REQUIRE((m->type_info->type_size == sizes[i]));
		REQUIRE((m->type_info->getispod()));
		++i;
	}

	REQUIRE((A::getmeta().getmember("ab")));
	REQUIRE((A::getmeta().getmember("bob") == 0));
}

TEST_CASE("Meta: Correct Equality of Types", "[meta]")
{
	REQUIRE(A::getmeta().getisobjecttype<A>());
	REQUIRE(B::getmeta().getisobjecttype<B>());
	REQUIRE(!B::getmeta().getisobjecttype<A>());
	REQUIRE(!A::getmeta().getisobjecttype<B>());
}

TEST_CASE("Meta: Tagging Validity", "[meta]")
{
	bool t1 = (A::getmeta().getmember("ab")->tags.front() == "TestTag1");
	REQUIRE(t1 == true);

	REQUIRE(meta::database::gettaglookuptable()["TestTag1"].size() == 1);
}