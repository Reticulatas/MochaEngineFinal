/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include <map>

//--TYPEMVC-------

class TypeMVC
{
	int viewY;
public:
	TypeMVC(const std::string _type, void* const _inputData, void * _outputData, int _viewY);
	std::string Type;
	void* InputData;
	void* OutputData;

	class GUIView
	{
	public:
		GUIView(const TypeMVC*);
		~GUIView(void);
		void Construct(void);
		void GetOutput(void);

		class GUIViewModel
		{
		protected:
			std::string Type;
			unsigned dataSize;
			
		public:
			static std::map<std::string, GUIViewModel*> GUIViewModels;
			typedef std::pair<std::string, GUIViewModel*> GUIViewModelsElement;
			typedef std::map<std::string, GUIViewModel*>::iterator GUIViewModels_iter;

			// data -> string
			virtual std::string Encode(const void * data) = 0;
			// string -> data
			virtual void* Decode(std::string data) = 0;

			unsigned GetSize() { return dataSize; }
		};
	};

	friend class GUIView;
};

//----------------

/* GUIFactory_VariableModel checks if input is correct to make a variableModel from
 * This includes cross checking the serialization databases to see if the present
 * typename exists and is registered with an applicable model, among other sanity
 * checks */
class GUIController_VariableModel;
class GUIFactory_VariableModel
{
public:
	GUIFactory_VariableModel(void);
	/* If Create() fails, a null pointer is returned */
	GUIController_VariableModel* Create(std::string type, std::string varname, void * defaultData,
		int startY);
};

/* GUIController manages a single variable represented in GUI form							*/
/* The lifetime of this object is guaranteed to match that of the created GUI and variable	*/
/* Output produced is unique to variable types, thus manual addition may be required		*/
class GUIController_VariableModel
{
	GUIController_VariableModel(std::string type, std::string varname, void * defaultData, int startY);

	std::string mType;
	std::string mVarname;
	void* mDefaultData;

	TypeMVC::GUIView::GUIViewModel* mGVM;

	int startY;

	/* Initial creation of managed GUI components */
	void CreateGUI(void);
public:
	~GUIController_VariableModel(void);

	/* Current GUI data result */
	std::string GetDataPack(void);

	//y coordinate the controls ended at
	int endY;

	friend GUIFactory_VariableModel;
};

TypeMVC::GUIView::GUIViewModel* GetDataFromType(const std::string type);
class Component;
void CopyDataToComponentMemoryOffset(::Component* c, size_t offset, void* dataToCopy);
void* GetDataFromComponentMemoryOffset(::Component* c, size_t offset);
void CopyDataToVoidPtr(void * dest, void * src, size_t dataSize);