/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include <map>
 
#include "SerializationIncludes.h"
#include "SerializationManager.h"
#include "meta.h"
 
#include "MemDebug.h"

/* Maximum number of serialized registrations before warnings start */
#define MAXREGS 256
#define REGNAMELENGTH 50

//see Serializations_Macro as to why this here
//(editor compilation issues)
#define ___DEC_EXT_REG___(type) class asIScriptEngine; void RegisterASBinds_##type(asIScriptEngine* engine);

/* Macro definitions for serialize method generation */
/* Use inside header file to define serialized variables */
#define DEPRECATED 
#define ___P_B_NVP(type) BOOST_SERIALIZATION_NVP(type);
#define ___SERIALIZE_BEGIN___ friend class boost::serialization::access; template<class Archive>\
	void serialize(Archive& archive_, unsigned int ver) {
#define ___DEFSER(varname, versionnum) \
  if (ver >= versionnum) archive_ & ___P_B_NVP(varname);
//serialize only if the version number = versionnum
#define ___DEFSERONLY(varname, versionnum) try { if (ver == versionnum) archive_ & ___P_B_NVP(varname); }\
	catch (boost::archive::archive_exception& e) { Log(e.what()); }
//serialize only if the version number <= versionnum
#define ___DEFSERONLYORLESS(varname, versionnum) try { if (ver <= versionnum) archive_ & ___P_B_NVP(varname); }\
	catch (boost::archive::archive_exception& e) { Log(e.what()); }
#define ___DEFBASE(baseclass) archive_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP(baseclass);
#define ___SERIALIZE_END___ }
//can be used to split logic for save/load - MUST HAVE BOTH
#define ___SERIALIZE_SAVE_BEGIN___ friend class boost::serialization::access; template<class Archive>\
	void save(Archive& archive_, const unsigned int ver) const {
#define ___SERIALIZE_LOAD_BEGIN___ friend class boost::serialization::access; template<class Archive>\
	void load(Archive& archive_, const unsigned int ver) { 

#define ___SERIALIZE_SAVE_LOAD___ BOOST_SERIALIZATION_SPLIT_MEMBER();

#define ___SERIALIZE_CLASS(classname, currentversion) BOOST_CLASS_VERSION(classname, currentversion);
#define ___SERIALIZE_TEMP_CLASS(classname, currentversion) namespace boost {\
namespace serialization {\
	template< typename T >\
	struct version< classname<T> >\
	{\
		typedef mpl::int_<currentversion> type;\
		typedef mpl::integral_c_tag tag;\
		BOOST_STATIC_CONSTANT(unsigned int, value = version::type::value);	}; } }

//#define ___SERIALIZE_DERIVED_CLASS(classname, currentversion) ___SERIALIZE_CLASS(classname, currentversion); BOOST_CLASS_EXPORT_GUID(classname, #classname);

//put these back in the def release to remove the object when scripts stop accessing it
	//if (as_ref_count == 0)\
		//Destroy(this);\

/* Extension class for all base object that need serialization	*/
/* For actual serialization functions, see Serializations.cpp	*/
class IRegisterable : public IMeta
{ 
	___SERIALIZE_BEGIN___
		//___DEFBASE(IMeta)
		___DEFSER(as_ref_count, 1)
	___SERIALIZE_END___
public:
	virtual ~IRegisterable() { }
	unsigned as_ref_count;
	int AddRef(void);
	int Release(void);
	IRegisterable() : as_ref_count(1) { }

	metabasedef(IRegisterable)
		endmetabasedef;
};
___SERIALIZE_CLASS(IRegisterable, 1)

//Contains data representing compile-time definitions of registrations
//component registrations
struct Reg
{
static int numOfRegs;
static char reg_names[MAXREGS][REGNAMELENGTH];
typedef IRegisterable*(*GenFncPtr)();
static GenFncPtr reg_gens[MAXREGS];
};

/* Namespace contains relevant functions to interface with serialized data */
namespace Serializer
{
	#define TYPENAMESIZE 50

	struct DefaultData
	{
		std::string type;
		std::string varname;
		void* defaultData;
		size_t memOffset;
		size_t memSize;

		typedef std::vector<DefaultData>::iterator VarIter;
	};
	/* Translation table from string -> DefaultTriple array (see Serializations.cpp) */
	struct KeyToDefs {
		char name[TYPENAMESIZE];
		Serializer::DefaultData* defaults;
		unsigned len; };

	/* Makes generic instance of an object without type information */
	IRegisterable* MakeInstanceFromString(std::string name);

	std::vector<std::string> GenerateRegisteredNameList(void);

	/* Returns a new object of type T based on the name of the class */
	/* Usage: Foo* newFoo = MakeInstOfTFromString<Foo>("Foo")		 */
	template <typename T>
	T* MakeInstanceOfTFromString(std::string name);

	/* Private use only, register type information */
	void _Assign(std::string name, Reg::GenFncPtr gen);
}

//Make Component from typename in string form
template <typename T>
T* Serializer::MakeInstanceOfTFromString(std::string name)
{
	IRegisterable* iserptr = MakeInstanceFromString(name);
	T* tptr = static_cast<T*>(iserptr);
	return tptr;
}
/* Macro defines an in place-struct to auto registers type info at pre-runtime */
#define ___COMP_REGISTER(T)											\
	IRegisterable* MakeInstance_##T() { return new T(); }		\
	struct Reg_##T {												\
		Reg_##T() { Serializer::_Assign(#T, &MakeInstance_##T); }	\
	};																\
	Reg_##T T##_registrationPacket

/* Macro signature that are used by the registration generator to know where the publics are */
#define ___BEGIN_REGISTERED___ ;
#define ___END_REGISTERED___ ;
#define ___BEGIN_ONLY_SERIALIZED___ ;
#define ___END_ONLY_SERIALIZED___ ;
/*#define ___END_ONLY_SERIALIZED_PRIVATE___(classtype) \
	template <class T>\
	friend void boost::serialization::serialize(T &ar, classtype& t, const BOOST_PFTO unsigned int);
	*/
/* Helper Macro for initializing a vector inline */
#define ___INIT_VECTOR(type, name, ...) {\
	static const type _initArray[] = __VA_ARGS__; \
	name = std::vector<type>(_initArray, _initArray + sizeof(_initArray) / sizeof(*_initArray)); }

//don't remove this comment :D
/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change