#pragma once

class CScript;

//typedef for c++ -> c# marshalling
extern "C"
{
	typedef void(__stdcall *MochaDotNetCallback)(void);
	typedef void(__stdcall *MochaDotNetCScriptEventCallback)(const CScript* from, char* name, const void* data, char* data_type);
	typedef void(__stdcall *MochaDotNetCScriptCallback)(const CScript* cs);
	typedef void(__stdcall *MochaDotNetCScriptCopyCallback)(const CScript* from, const CScript* to);
}