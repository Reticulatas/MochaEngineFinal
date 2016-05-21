#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Include/ret.h"
#include "PropertyTable.h"
#include "DynamicPropertyTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ret_UnitTests
{		
	TEST_CLASS(UnitTest_Meta)
	{
		class A : public ret::meta::iMetaIdentifiable
		{
		};
		class B : public A
		{
		};
	public:
		TEST_METHOD(Meta_iMetaId)
		{
			A* a = new A();
			B* b = new B();
			Assert::IsTrue(a->GetIsObjectType<A>());
			Assert::IsTrue(b->GetIsObjectType<B>());
			Assert::IsFalse(a->GetIsObjectType<B>());
			Assert::IsFalse(b->GetIsObjectType<A>());
		}
	};

	TEST_CLASS(UnitTest_SlotTable)
	{
	public:

		struct Foo : public ret::ISlottable
		{
			int bar;
			Foo( ) : bar(10000) { }
			Foo( int c ) { bar = c; }
			virtual ~Foo() { }
		};
		
		TEST_METHOD(SlotTable_AddDeleteEquality)
		{
			// TODO: Your test code here
			ret::SlotTable<Foo> s;

			for (int i = 0; i < 100; ++i)
			{
				Foo* f = s.New(i);
				Assert::AreEqual(i, f->bar);
			}
				//Assert::AreEqual((int)s.Delete(f), (int)ret::SlotTableError::ST_NOERROR);
			s.Reset();

			Assert::AreEqual(0, (int)s.GetNumberOfObjects());
		}

		TEST_METHOD(SlotTable_NewManaged)
		{
			ret::SlotTable<Foo> s;
			Foo * f = new Foo(10);

			s.NewManaged(f);
			s.Delete(f);

			Assert::AreEqual(0, (int)s.GetNumberOfObjects());
			s.Clear();
			Assert::AreEqual(0, (int)s.GetNumberOfObjects());
			s.Reset();
			Assert::AreEqual(0, (int)s.GetNumberOfObjects());
		}

		TEST_METHOD(SlotTable_Iteration)
		{
			// TODO: Your test code here
			ret::SlotTable<Foo> s;

			for (int i = 0; i < 10; ++i)
			{
				Foo* f = s.New(i);
				f->bar = i;
			}

			unsigned i = 0;
			for (auto& o : *s)
			{
				++i;
			}

			Assert::AreEqual(s.GetNumberOfObjects(), i);
		}

		TEST_METHOD(managedPtrEquality)
		{
			ret::managedPtr<Foo> v;
			v->bar = 5;

			Assert::AreEqual(v->bar, 5);
		}
		TEST_METHOD(managedPtrTransferOfOwnership)
		{
			ret::managedPtr<Foo> v;
			v->bar = 5;
			ret::managedPtr<Foo> r(v);
			Assert::IsNull(v());
			Assert::IsNotNull(r());

			Assert::AreEqual(r->bar, 5);
		}
		TEST_METHOD(ownerPtrValidity)
		{
			Foo* f = new Foo();
			f->bar = 3;
			ret::ownerPtr<Foo> v(f);
			ret::ownerPtr<Foo> v2;
		}
	};
}