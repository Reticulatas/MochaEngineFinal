#include "stdafx.h"
#include "CppUnitTest.h"
#include "meta.h"
#include "def.h"
#include "metafuncs.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineUnitTests
{		
	int TestInvoke1()	{
		return 5;
	}
	unsigned TestInvoke2(unsigned b) { return b; }
	int TestInvoke3(int b, char c) {
		return b;
	}

	struct A {
		int ab;
		char ac;

		def(A,
			add(int, ab)
			//add(char, ac)
		);
	};
	struct B {
		char bb;

		def(B,
			add(char, bb)
	    )
	};
	meta_define(A);
	meta_define(B);

	TEST_CLASS(MetaUnitTests)
	{
	public:

		TEST_METHOD(FuncInvoke)
		{
			Assert::IsTrue(meta::metafunction<decltype(&TestInvoke1),&TestInvoke1>::invoke() == 5, L"No param invoke failed");
			Assert::IsTrue(meta::metafunction<decltype(&TestInvoke2), &TestInvoke2>::invoke(3) == 3, L"1 param invoke failed");
			Assert::IsTrue(meta::metafunction<decltype(&TestInvoke3), &TestInvoke3>::invoke(10,4) == 10, L"2 param invoke failed");
		}

		TEST_METHOD(TypeMembers)
		{
			const char* names[] = { "ab", "ac" };
			const size_t sizes[] = { sizeof(int), sizeof(char) };
			unsigned i = 0;
			for (const meta::metamember* m : A::getmeta().getmemberslist())
			{
				Assert::IsTrue(strcmp(m->var_name, names[i]) == 0, L"Mismatched name in member list.");
				Assert::IsTrue(m->type_info->type_size == sizes[i], L"Mismatched size in member list.");
				Assert::IsTrue(m->type_info->getispod(), L"POD type not recognized as such");
				++i;
			}

			Assert::IsNotNull(A::getmeta().getmember("ab"), L"Class A did not report proper members");
			Assert::IsNull(A::getmeta().getmember("bob"), L"Class A did not report proper members");
		}

		TEST_METHOD(TypeDatabaseValidity)
		{
            //complex
			Assert::IsTrue(meta::database::gettypefromT<A>().getmeta().typeinfo == A::getmeta().typeinfo, L"Database type does not match internal type");
			Assert::IsTrue(meta::database::gettypefromT<B>().getmeta().typeinfo == B::getmeta().typeinfo, L"Database type does not match internal type");
			Assert::IsFalse(meta::database::gettypefromT<B>().getmeta().typeinfo == A::getmeta().typeinfo, L"Database type does match incorrect internal type");
			Assert::IsFalse(meta::database::gettypefromT<A>().getmeta().typeinfo == B::getmeta().typeinfo, L"Database type does match incorrect internal type");

            //primitives
			Assert::IsTrue(meta::database::gettypefromT<int>().getmeta().getispod(), L"Database primitive type does not have correct classification");
			Assert::IsFalse(meta::database::gettypefromT<A>().getmeta().getispod(), L"Database complex type does not have correct classification");
		}
		TEST_METHOD(TypeInfoEquality)
		{
			Assert::IsTrue(A::getmeta().getisobjecttype<A>(), L"Same types did not resolve equal");
			Assert::IsFalse(A::getmeta().getisobjecttype<B>(), L"Different types did resolve equal");
			Assert::IsTrue(B::getmeta().getisobjecttype<B>(), L"(2) Same types did not resolve equal");
			Assert::IsFalse(B::getmeta().getisobjecttype<A>(), L"(2) Different types did resolve equal");
		}

	};
}