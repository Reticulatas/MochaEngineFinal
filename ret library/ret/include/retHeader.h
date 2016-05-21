//do not include!
//standard includes and abstractions

#include <vector>
#include <list>
#include <exception>
#include <set>
#include <string>
#include <queue>
#include <typeinfo>

#ifdef dllexport
#ifdef RETLIBRARY_EXPORT
	#define RETAPI __declspec(dllexport)
#else
	#define RETAPI __declspec(dllimport)
#endif
#else
#define RETAPI
#endif

#ifdef RETBOOSTSERIALIZATION
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <fstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#endif