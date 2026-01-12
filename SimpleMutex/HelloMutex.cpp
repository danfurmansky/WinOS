#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("No argument!\n");
		return 1;
	}

	int num = atoi(argv[1]);

	HANDLE hMyMutex = CreateMutexA(
		NULL,
		FALSE,
		"MyMutex"
	);

	DWORD WaitResult = WaitForSingleObject(
		hMyMutex,
		INFINITE
	);

	if (WaitResult == WAIT_OBJECT_0) {
		printf("Hello World! Process got argument: %d\n", num);
		Sleep(5000);
		ReleaseMutex(hMyMutex);
	}
	else {
		printf("waited too much or something failed");
	}

	CloseHandle(hMyMutex);
	return 0;
}