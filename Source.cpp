#include <stdio.h>
#include <windows.h>

#define DLL_To_Inject "ShowMsgDLL.dll"

int main() {

	CHAR fullpath[MAX_PATH];

	DWORD pathLen = GetFullPathNameA(
		DLL_To_Inject,
		MAX_PATH,
		fullpath,
		NULL
	);

	printf("%s\n", fullpath);

	PVOID addrLoadLibraryA = (PVOID)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");

	HANDLE hRemoteProcess = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		23972
	);
	if (NULL == hRemoteProcess) {
		printf("OpenProcess failed. Error: %lu\n", GetLastError());
		return 0;
	}

	SIZE_T BytesToAlloc = strlen(fullpath) + 1;

	PVOID memAddr = (PVOID)VirtualAllocEx(
		hRemoteProcess,
		NULL,
		BytesToAlloc,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE
	);
	if (NULL == memAddr) {
		printf("VirtualAllocEx failed. Error: %lu\n", GetLastError());
		return 0;
	}

	BOOL check = WriteProcessMemory(
		hRemoteProcess,
		memAddr,
		fullpath,
		BytesToAlloc,
		NULL
	);
	if (0 == check) {
		printf("WriteProcessMemory failed. Error: %lu\n", GetLastError());
		return 0;
	}

	HANDLE hRemoteThread = CreateRemoteThread(
		hRemoteProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)addrLoadLibraryA,
		memAddr,
		0,
		NULL
	);
	if (NULL == hRemoteThread) {
		printf("CreateRemoteThread failed. Error: %lu\n", GetLastError());
		return 0;
	}

	WaitForSingleObject(hRemoteThread, INFINITE);
	CloseHandle(hRemoteThread);
	CloseHandle(hRemoteProcess);
	return 0;
}