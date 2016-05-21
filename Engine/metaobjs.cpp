
#include "stdafx.h"
#include "metaobjs.h"

unsigned meta::metamember::get_actual_size_from_base(const void* base_ptr)
{
	if (type_info->getisobjecttype<std::string>())
	{
		return reinterpret_cast<const std::string*>(get_raw_ptr_to_member(base_ptr))->size();
	}
	return type_info->type_size;
}

const void* meta::metamember::get_ptr_to_member_content(const void* base_ptr_to_struct) const
{
	//special cases go here
	if (type_info->getisobjecttype<std::string>())
		return reinterpret_cast<const std::string*>(reinterpret_cast<const char*>(base_ptr_to_struct) + offset)->c_str();
	return reinterpret_cast<const char*>(base_ptr_to_struct) + offset;
}

const void* meta::metamember::get_raw_ptr_to_member(const void* base_ptr_to_struct) const
{
	return reinterpret_cast<const char*>(base_ptr_to_struct) + offset;
}

bool meta::metaidentifiable::has_tag(std::string& tag) const
{
	if (std::find(tags.begin(), tags.end(), tag) == tags.end())
		return false;
	return true;
}

meta::metamember::metamember(size_t off_, metaobj* _t, const char* name) : type_info(_t), offset(off_)
{
	size_t namesize = strlen(name) + 1;
	var_name = (char*)malloc(namesize);
	strcpy_s(var_name, namesize, name);
}

meta::metamember::metamember(const metamember& o)
{
	type_info = o.type_info;
	offset = o.offset;
	size_t namesize = strlen(o.var_name) + 1;
	var_name = (char*)malloc(namesize);
	strcpy_s(var_name, namesize, o.var_name);

	tags = o.tags;
}

void meta::metamember::set_content(void* base_ptr, void* data, size_t data_size) const
{
	void* underlying_data = const_cast<void*>(get_raw_ptr_to_member(base_ptr));
	if (type_info->getisobjecttype<std::string>())
		reinterpret_cast<std::string*>(underlying_data)->assign(std::string(reinterpret_cast<char*>(data), data_size));
	else
		memcpy(underlying_data, data, data_size);
}


bool meta::metaclass::getispod() const
{
	return false;
}

meta::metaclass::MemberIterator meta::metaclass::getmembersend() const
{
	return _members.end();
}

meta::metaclass::MemberIterator meta::metaclass::getmembersbegin() const
{
	return _members.begin();
}

const meta::metaclass::MemberList& meta::metaclass::getmemberslist() const
{
	return _members;
}

const meta::metamember* meta::metaclass::getmember(const std::string& c) const
{
	_memberslookupiter iter = _memberslookup.find(c);
	if (iter != _memberslookup.end())
		return iter->second;
	//throw std::exception(std::string(std::string(type_name) + " does not have a member " + c).c_str());
	return 0;
}

const meta::metamember* meta::metaclass::getmember(const char* c) const
{
	return getmember(std::string(c));
}

meta::metaclass& meta::metaclass::_tag_lastmember(const char* tag)
{
	std::string t(tag);
	if (!_lastaddedmember && !_lastaddedfunction)
		assert("Tagging comes after, not before, a member");
	if (_lastaddedmember)
	{
		_lastaddedmember->tags.emplace_back(t);
		if (std::find(meta::database::gettaglookuptable()[t].begin(), meta::database::gettaglookuptable()[t].end(), _lastaddedmember) != meta::database::gettaglookuptable()[t].end())
			assert("This tag already exists for this member");
		meta::database::gettaglookuptable()[t].push_back(_lastaddedmember);
	}
	else if (_lastaddedfunction)
	{
		_lastaddedfunction->tags.emplace_back(t);
		if (std::find(meta::database::gettagfunclookuptable()[t].begin(), meta::database::gettagfunclookuptable()[t].end(), _lastaddedfunction) != meta::database::gettagfunclookuptable()[t].end())
			assert("This tag already exists for this member");
		meta::database::gettagfunclookuptable()[t].push_back(_lastaddedfunction);
	}
	return *this;
}

meta::metaclass& meta::metaclass::_add_function(metafunction_method* v)
{
	_functions[v->name] = v;
	_lastaddedmember = 0;
	_lastaddedfunction = v;
	return *this;
}

meta::metaclass::metaclass() : _lastaddedfunction(0), _lastaddedmember(0), _base_info(0)
{
}

meta::metafunction_method* meta::metaclass::getfunctionbyname(std::string& fncname) const
{
	meta::metafunction_method* fnc = 0;
	try {
		fnc = _functions.at(fncname);
	}
	catch (std::out_of_range&)	{
		return 0;
	}
	return fnc;
}

meta::metastatic* meta::metaclass::getstaticbyname(std::string& varname) const
{
	meta::metastatic* m = 0;
	try {
		m = _staticslookup.at(varname);
	}
	catch (std::out_of_range& )	{
		return 0;
	}
	return m;
}

std::string meta::metaclass::gettoplevelname() const
{
	return type_name;
}

meta::metaobj::~metaobj()
{
	free(type_name);
}

meta::metaobj::metaobj() : type_id(meta::database::assignedtypeids++)
{

}

meta::metastatic::metastatic(void* ptr, metaobj* o, const char* name) : var_name(name), raw_ptr(ptr), type_info(o)
{
}

meta::metastatic::metastatic(const metastatic& o) : raw_ptr(o.raw_ptr), var_name(o.var_name), type_info(o.type_info)
{
}

unsigned meta::metaidentifiable::get_meta_id() const
{
	return meta_id;
}

meta::metaidentifiable::metaidentifiable()
{
	meta_id = _meta_id_store++;
}

unsigned meta::metaidentifiable::_meta_id_store = 0;

