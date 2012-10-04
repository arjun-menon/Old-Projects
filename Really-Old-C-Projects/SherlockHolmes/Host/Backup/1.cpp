//
// Desgined for Microsoft® Windows® Operating System
//

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	HINSTANCE hDLL = NULL;
	hDLL = LoadLibrary(TEXT("watson.dll"));
	if(hDLL != NULL) {
		FARPROC pDLL= NULL;
		pDLL = GetProcAddress(hDLL, TEXT("ActivateSystem"));
		if(pDLL != NULL)
			pDLL();
	}
	return EXIT_SUCCESS;
}
