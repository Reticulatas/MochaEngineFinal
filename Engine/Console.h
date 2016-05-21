/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
 

typedef enum
{
    INPUT_CONS,
    OUTPUT_CONS,
    BOTH
}CONSOLETYPE_e;

class ConsoleAdapter  
{
public:
	ConsoleAdapter( bool bAutoDestroy = true );
    ~ConsoleAdapter();
    bool CreateConsole( CONSOLETYPE_e eConsoleType );
    bool SpawnDumpConsole( LPCTSTR lpctszDumConsoleApp,CONSOLETYPE_e eConsoleType );
    void DestroyConsole();
private:
    bool ReplaceHandles();
    bool AttachConsole( DWORD dwProcessId );

    FILE m_fOldStdIn;
    FILE* m_fpCRTIn;
    int m_nCRTIn;

    FILE m_fOldStdOut;
    FILE* m_fpCRTOut;
    int m_nCRTOut;

    CONSOLETYPE_e m_eConsoleType;
    bool m_bDestroyed;
    bool m_bAutoDestroy;
    HANDLE m_hDumpConsole;
};