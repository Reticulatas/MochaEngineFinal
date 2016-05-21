#include "stdafx.h"
#include "SpeechRecognition.h"
//#include <sphelper.h>
//#include <sapi.h>
#include <iostream>
#include <string>
#include "Log.h"
//#include "CommandManager.h"

#ifdef SPEECHRECOGNITIONENABLED

#define GRAMMARID (ULONGLONG)0
#define RULENAME L"ruleName1"

HANDLE RECThread;

DWORD WINAPI T_RecThread(LPVOID param)
{
	SpeechRecognition::getInstance()->InitContext();
	return 0;
}

SpeechRecognition* SpeechRecognition::getInstance( void )
{
	static SpeechRecognition* instance = 0;
	if (!instance)
		instance = new SpeechRecognition();
	return instance;
}

std::wstring SpeechRecognition::GetText()
{
	const ULONG maxEvents = 10;
	SPEVENT events[maxEvents];

	ULONG eventCount;
	HRESULT hr;
	hr = recoContext->GetEvents(maxEvents, events, &eventCount);
	if (eventCount == 0)
		return std::wstring();

	if(!(hr == S_OK || hr == S_FALSE)) {
		CheckReturn(hr);
	}

	ISpRecoResult* recoResult;
	recoResult = reinterpret_cast<ISpRecoResult*>(events[0].lParam);

	wchar_t* text;
	hr = recoResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, FALSE, &text, NULL);
	CheckReturn(hr);

	std::wstring outText(text);

	CoTaskMemFree(text);

	return outText;
}

void SpeechRecognition::Init()
{
	//add command manager words
	std::list<const MochaCommand*> commands;
	GetAllCommands(commands);
	for (const MochaCommand*& c : commands)
		wordList.push_back(std::wstring(c->name.toWideCharPointer()));

	RECThread = CreateThread(NULL,0, T_RecThread, 0,0,0);
}

void SpeechRecognition::InitContext()
{
	//init com
	if (FAILED(::CoInitialize(nullptr))) 	{
		Log("Failed to initialize com library", LogEntry::Error);
		return;
	}

	HRESULT hr;
	ISpRecognizer* recognizer;
	hr = CoCreateInstance(CLSID_SpSharedRecognizer, nullptr, CLSCTX_ALL, IID_ISpRecognizer, reinterpret_cast<void**>(&recognizer));
	CheckReturn(hr);

	hr = recognizer->CreateRecoContext(&recoContext);
	CheckReturn(hr);
	
	//pause context
	hr = recoContext->Pause(0);
	CheckReturn(hr);

	//make grammar library
	ISpRecoGrammar* recoGrammar = InitGrammar();

	handleEvent = recoContext->GetNotifyEventHandle();
	if (handleEvent == INVALID_HANDLE_VALUE)
		CheckReturn(E_FAIL);

	ULONGLONG interest = SPFEI(SPEI_RECOGNITION);
	hr = recoContext->SetInterest(interest, interest);
	CheckReturn(hr);

	//Activate grammar
	hr = recoGrammar->SetRuleState(RULENAME, 0, SPRS_ACTIVE);
	CheckReturn(hr);

	//enable context again
	hr = recoContext->Resume(0);
	CheckReturn(hr);
	
	std::cout << "Waiting for mocha...." << std::endl;

	Update();	

	std::cout << "Hello!" << std::endl;

}

void SpeechRecognition::CheckReturn( const HRESULT& result )
{
	if (result == S_OK)
		return;

	std::string message;
	switch(result) {
	case E_INVALIDARG:
		message = "One or more arguments are invalids.";
		break;
	case E_ACCESSDENIED:
		message = "Access Denied.";
		break;
	case E_NOINTERFACE:
		message = "Interface does not exist.";
		break;
	case E_NOTIMPL:
		message = "Not implemented method.";
		break;
	case E_OUTOFMEMORY:
		message = "Out of memory.";
		break;
	case E_POINTER:
		message = "Invalid pointer.";
		break;
	case E_UNEXPECTED:
		message = "Unexpected error.";
		break;
	case E_FAIL:
		message = "Failure";
		break;
	default:
		message = "Unknown : " + std::to_string(result);
		break;
	}

	throw std::exception(message.c_str());
}

ISpRecoGrammar* SpeechRecognition::InitGrammar()
{
	HRESULT hr;
	SPSTATEHANDLE sate;

	ISpRecoGrammar* recoGrammar;
	hr = recoContext->CreateGrammar(GRAMMARID, &recoGrammar);
	CheckReturn(hr);

	WORD langID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
	hr = recoGrammar->ResetGrammar(langID);
	CheckReturn(hr);

	//create rules
	hr = recoGrammar->GetRule(RULENAME, 0, SPRAF_TopLevel | SPRAF_Active, true, &sate);
	CheckReturn(hr);

	//add words
	const std::wstring commandWstr = std::wstring(L"Mocha");
	hr = recoGrammar->AddWordTransition(sate, NULL, commandWstr.c_str(), L" ", SPWT_LEXICAL, 1, nullptr);
	CheckReturn(hr);

	for (std::wstring& ws : wordList)
	{
		std::string sentence(ws.begin(), ws.end());
		std::vector<std::string> words = split(sentence, ' ');
		for (std::string& w : words)
		{
			std::wstring wsword(w.begin(), w.end());
			hr = recoGrammar->AddWordTransition(sate, NULL, wsword.c_str(), L" ", SPWT_LEXICAL, 1, nullptr);
		}
		CheckReturn(hr);
	}
	
	hr = recoGrammar->Commit(0);
	CheckReturn(hr);

	return recoGrammar;
}

void SpeechRecognition::Free()
{
	CloseHandle(RECThread);
	::CoUninitialize();
}

bool SpeechRecognition::Tick()
{
	while (1)
	{
		HANDLE handles[1];
		handles[0] = handleEvent;
		WaitForMultipleObjects(1, handles, FALSE, INFINITE);
		std::wstring ws = GetText();
		if (!ws.empty())
			std::cout << std::string(ws.begin(), ws.end()) << std::endl;
	}
	return true;
}

#endif
