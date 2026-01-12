#include <stdio.h>
#include <windows.h>

 
#define SEARCH_RANGE 1000000
#define MINI_RANGE 10000
#define MAX_PRIMES 80000
#define NUM_CHILDREN 20
#define CHILD_FILENAME "C:\\Users\\danfu\\source\\repos\\Child\\x64\\Debug\\Child.exe" 
#define MUTEX_NAME "Index_Mutex"
#define MAPPING_NAME "SharedMemory"


typedef struct {
	INT NextPrimeIndex;
	INT All_Primes[MAX_PRIMES];
	INT NextRangeIndex;
}SHARED_DATA;


int main() {

	STARTUPINFOA si;
	PROCESS_INFORMATION pi[NUM_CHILDREN];

	char param[64];

	HANDLE hMutex = CreateMutexA(
		NULL,
		FALSE,
		MUTEX_NAME
	);
	if (!hMutex) {
		printf("Parent: CreateMutex failed. Error: %lu\n", GetLastError());
		return 1;
	}

	HANDLE hMapFile = CreateFileMappingA(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(SHARED_DATA),
		MAPPING_NAME
	);
	if (!hMapFile) {
		printf("Parent: CreateFileMapping failed. Error: %lu\n", GetLastError());
		CloseHandle(hMutex);
		return 1;
	}

	SHARED_DATA* Data = (SHARED_DATA*)MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(SHARED_DATA)
	);
	if (!Data) {
		printf("Parent: MapViewOfFile failed. Error: %lu\n", GetLastError());
		CloseHandle(hMapFile);
		CloseHandle(hMutex);
		return 1;
	}

	ZeroMemory(Data, sizeof(SHARED_DATA));
	Data->NextPrimeIndex = 0;
	Data->NextRangeIndex = 2;    // 2 because 1 and 0 are not primary

	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	for (int i = 0; i < NUM_CHILDREN; i++) {
		sprintf_s(param, sizeof(param), "%s %d", CHILD_FILENAME, i);

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi[i], sizeof(pi[i]));

		BOOL ok = CreateProcessA(
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

		if (!ok) {
            printf("Parent: CreateProcess for child %d failed. Error: %lu\n", i, GetLastError());
        }
        else {
            printf("Parent: Child %d created.\n", i);
        }
	}

	for (int j = 0; j < NUM_CHILDREN; j++) {
		if (pi[j].hProcess) {
			WaitForSingleObject(pi[j].hProcess, INFINITE);
			CloseHandle(pi[j].hProcess);
		}
		if (pi[j].hThread) {
			CloseHandle(pi[j].hThread);
		}
	}

	QueryPerformanceCounter(&end);

	printf("Parent: All children finished. Total primes found: %d\n", Data->NextPrimeIndex);

	double elapsedMs = 1000.0 * (end.QuadPart - start.QuadPart) / freq.QuadPart;

	printf("Elapsed time: %.6f miliseconds\n", elapsedMs);

	UnmapViewOfFile(Data);
	CloseHandle(hMapFile);
	CloseHandle(hMutex);

	return 0;
}

