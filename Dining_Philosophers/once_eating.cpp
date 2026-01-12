#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

CRITICAL_SECTION ghCriticalSection[5] ;

DWORD WINAPI ThreadEating(LPVOID index) {
	INT Thread_index = *(PINT)index;

	INT left_stick = Thread_index;
	INT right_stick = (Thread_index + 1) % 5;

	if (Thread_index == 4) {						//preventing Deadlock
		INT temp = left_stick;
		left_stick = right_stick;
		right_stick = temp;
	}

	Sleep(100);

	EnterCriticalSection(&ghCriticalSection[left_stick]);
	EnterCriticalSection(&ghCriticalSection[right_stick]);

	printf("Philosofer number %d is eating using sticks %d and %d\n", Thread_index + 1, left_stick, right_stick);

	Sleep(300);

	LeaveCriticalSection(&ghCriticalSection[left_stick]);
	LeaveCriticalSection(&ghCriticalSection[right_stick]);

	return 0;
}


int main() {
	INT Threads_indexes[5] = { 0, 1, 2, 3, 4};
	HANDLE hThreads[5];

	for (int i = 0; i < 5; i++) {
		InitializeCriticalSection(&ghCriticalSection[i]);

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
	for (int j = 0; j < 5; j++) {
		CloseHandle(hThreads[j]);
	}
	return 0;
}