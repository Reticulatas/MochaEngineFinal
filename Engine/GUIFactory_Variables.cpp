/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
 
#include "GUIFactory_Variables.h"
#include "GUIFactory_TypeRegs.h"
#include "IRegisterable.h"
#include <array>

//Get Model from typename
TypeMVC::GUIView::GUIViewModel* GetDataFromType(const std::string type) 
{
	MakeListOfRegs();
	if (TypeMVC::GUIView::GUIViewModel::GUIViewModels.count(type))
		return TypeMVC::GUIView::GUIViewModel::GUIViewModels.at(type);
	return 0;
}

void* GetDataFromComponentMemoryOffset( ::Component* c, size_t offset )
{
	return (void*)((reinterpret_cast<char*>(c) + offset));
}

void CopyDataToVoidPtr(void * dest, void * src, size_t dataSize)
{
	if (!dest)
		Log("Error: set data on null field", LogEntry::Error);
	memcpy(dest, src, dataSize);
}
void CopyDataToComponentMemoryOffset( ::Component* c, size_t offset, size_t memSize, void* dataToCopy )
{
	void* destPtr = GetDataFromComponentMemoryOffset(c, offset);
	CopyDataToVoidPtr(destPtr, dataToCopy, memSize);
}

TypeMVC::TypeMVC( const std::string _type, void* const _inputData, void * _outputData, int _viewY )
	: Type(_type), InputData(_inputData), OutputData(_outputData), viewY(_viewY)
{
}

std::map<std::string, TypeMVC::GUIView::GUIViewModel*> GVMspace::GUIViewModels;

//------------------------------------------------------------------
//------------------------------------------------------------------

GUIController_VariableModel::GUIController_VariableModel( std::string type, std::string varname, void * defaultData, int _startY)
	: mType(type), mVarname(varname), mDefaultData(defaultData), startY(_startY)
{
	mGVM = GetDataFromType(type);
	CreateGUI();
}

GUIController_VariableModel::~GUIController_VariableModel( void )
{
}

void GUIController_VariableModel::CreateGUI( void )
{
	TypeMVC::GUIView::GUIViewModel * t = mGVM;

}

// Factory
GUIFactory_VariableModel::GUIFactory_VariableModel( void ) { }
GUIController_VariableModel* GUIFactory_VariableModel::Create( std::string type, std::string varname, void * defaultData, int startY )
{
	if (type == "" || varname == "")
		Log("Bad Type or Var name given to GUIController");

	TypeMVC::GUIView::GUIViewModel* mGVM = GetDataFromType(type);
	if (mGVM == 0)	{
		Log("Type name lookup of type [" + type + "] failed");
		return nullptr; }

	GUIController_VariableModel * product = new GUIController_VariableModel(type, varname, defaultData, startY);
	return product;
}
