#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define EXE_FILENAME "C:\\Users\\danfu\\source\\repos\\HelloWorldMutex\\x64\\Debug\\HelloWorldMutex.exe"

int main() {

	HANDLE hMyMutex = CreateMutexA(
		NULL,
		FALSE,
		"MyMutex"
	);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi[2];

	INT size = strlen(EXE_FILENAME) + 3;
	PCHAR param = (PCHAR)malloc(size * sizeof(CHAR));

	for (int i = 0; i < 2; i++) {
		sprintf_s(param, size, "%s %d", EXE_FILENAME, i);

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi[i], sizeof(pi[i]));

		CreateProcessA(
			NULL,	
			param, 
			NULL,
			NULL,
			FALSE,
			0, 
			NULL, 
			NULL, 
			&si, 
			&pi[i]
		);
	}

	for (int i = 0; i < 2; i++) {
		WaitForSingleObject(pi[i].hProcess, INFINITE);
		CloseHandle(pi[i].hProcess);
		CloseHandle(pi[i].hThread);
	}
	ReleaseMutex(hMyMutex);

	free(param);

	return 0;
}