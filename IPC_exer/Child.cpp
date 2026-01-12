#include <stdio.h>
#include <windows.h>


#define TOTAL_RANGE 1000000
#define MINI_RANGE 10000
#define MAX_PRIMES 80000
#define MUTEX_NAME "Index_Mutex"
#define MAPPING_NAME "SharedMemory"


typedef struct {
	INT NextPrimeIndex;
	INT All_Primes[MAX_PRIMES];
	INT NextRangeIndex;
}SHARED_DATA;


static int IsPrime(int num) {
	if (num < 2) return 0;
	if (num == 2) return 1;
	if (num % 2 == 0) return 0;
	for (int i = 3; i * i <= num; i++) {
		if (num % i == 0) {
			return 0;
		}
	}
	return 1;
}


int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Incorrect number of arguments\n");
	}


	HANDLE hMutex = OpenMutexA(
		SYNCHRONIZE,
		FALSE,
		MUTEX_NAME
	);
	if (!hMutex) {
		printf("Child: OpenMutex failed. Error: %lu\n", GetLastError());
		return 1;
	}

	HANDLE hMapFile = OpenFileMappingA(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		MAPPING_NAME
	);
	if (!hMapFile) {
		printf("Child: OpenFileMapping failed. Error: %lu\n", GetLastError());
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
		printf("Child: MapViewOfFile failed. Error: %lu\n", GetLastError());
		CloseHandle(hMapFile);
		CloseHandle(hMutex);
		return 1;
	}

	DWORD WaitResult;
	INT PrimeStorage[MINI_RANGE];
	int Primes_count = 0;

	INT range_start = 0;
	INT range_end = 0;


	while (1) {

		WaitResult = WaitForSingleObject(hMutex, 5000);
		if (WaitResult != WAIT_OBJECT_0 && WaitResult != WAIT_ABANDONED) {
			break;
		}

		range_start = Data->NextRangeIndex;
		Data->NextRangeIndex += MINI_RANGE;

		ReleaseMutex(hMutex);


		if (range_start >= TOTAL_RANGE) {
			break;
		}

		range_end = range_start + MINI_RANGE;

		if (range_end > TOTAL_RANGE) {
			range_end = TOTAL_RANGE;
		}

		Primes_count = 0;

		for (int i = range_start; i < range_end; i++) {
			if (IsPrime(i)) {
				PrimeStorage[Primes_count] = i;
				Primes_count++;
			}
		}


		WaitResult = WaitForSingleObject(hMutex, 5000);
		if (WaitResult != WAIT_OBJECT_0 && WaitResult != WAIT_ABANDONED) {
			break;
		}

		INT CurrentIndex = Data->NextPrimeIndex;
		Data->NextPrimeIndex += Primes_count;

		ReleaseMutex(hMutex);

		for (int j = 0; j < Primes_count; j++) {
			Data->All_Primes[CurrentIndex + j] = PrimeStorage[j];
		}
	}

	CloseHandle(hMutex);
	UnmapViewOfFile(Data);
	CloseHandle(hMapFile);

	return 0;
}