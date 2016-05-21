#pragma once
#include "ISubsystem.h"
#include <string>
#include <list>

#ifdef SPEECHRECOGNITIONENABLED

struct ISpRecoGrammar;
struct ISpRecoContext;
class SpeechRecognition : public ISubsystem
{
	//words to be registered and have-been-registered
	std::list<std::wstring> wordList;
	HANDLE handleEvent;

	void InitContext();
	ISpRecoGrammar* InitGrammar();
	bool Tick();
	ISpRecoContext* recoContext;

	void CheckReturn(const HRESULT& result);
	std::wstring GetText();
public:
	void Init();
	void Free();

	static SpeechRecognition* getInstance(void);

	friend DWORD WINAPI T_RecThread(LPVOID param);
};


#endif