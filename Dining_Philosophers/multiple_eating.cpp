#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

CRITICAL_SECTION ghCriticalSection[5];

VOID Eat(INT index) {

	INT left_stick = index;
	INT right_stick = (index + 1) % 5;

	if (index == 4) {						//preventing Deadlock
		INT temp = left_stick;
		left_stick = right_stick;
		right_stick = temp;
	}

	EnterCriticalSection(&ghCriticalSection[left_stick]);
	EnterCriticalSection(&ghCriticalSection[right_stick]);

	LeaveCriticalSection(&ghCriticalSection[left_stick]);
	LeaveCriticalSection(&ghCriticalSection[right_stick]);
}


DWORD WINAPI ThreadEating(LPVOID index) {
	PINT Thread_index = (PINT)index;
	for (int i = 0; i < 1000000; i++) {
		Eat(*Thread_index);
	}
	return 0;
}


int main() {
	INT Threads_indexes[5] = { 0, 1, 2, 3, 4};
	HANDLE hThreads[5];

	for (int i = 0; i < 5; i++) {
		InitializeCriticalSection(&ghCriticalSection[i]);
	}

	clock_t start = clock();

	for (int i = 0; i < 5; i++) {

		LPVOID current_index = &Threads_indexes[i];

		hThreads[i] = CreateThread(
			NULL,
			0,
			ThreadEating,
			current_index,
			0,
			NULL
		);
	}

	WaitForMultipleObjects(5, hThreads, TRUE, INFINITE);

	clock_t end = clock();
	double time_taken = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

	printf("Elapsed time: %.1f ms.\n", time_taken);
	for (int j = 0; j < 5; j++) {
		CloseHandle(hThreads[j]);
	}

	return 0;
}