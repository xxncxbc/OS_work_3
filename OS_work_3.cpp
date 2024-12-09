#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <vector>
using namespace std;

CRITICAL_SECTION csMainToWork;
CRITICAL_SECTION csCountElementToMain;
HANDLE eventWork;

vector<__int16> arr;
int n, k, num_neg = 0;

DWORD WINAPI Work(LPVOID param) {
    EnterCriticalSection(&csMainToWork); //запускаемся
    LeaveCriticalSection(&csMainToWork);
    cout << endl << "Work is working!" << endl;
    int j = 0;

    for (int i = 0; i < n; i++) {
        if (arr[i] < 0 && arr[i] > k) {
            swap(arr[j], arr[i]);
            j++;
        }
    }
    cout << endl << "Work is done!" << endl;
    SetEvent(eventWork); //сигнализируем main и CountElement о начале работы

    return 0;
}

DWORD WINAPI CountElement(LPVOID param) {
    EnterCriticalSection(&csCountElementToMain);
    WaitForSingleObject(eventWork, INFINITE); //начинаем работу

    cout << endl << "CountElement is working!" << endl;

    for (int i = 0; i < n; i++) {
        if (arr[i] < 0) num_neg++;
    }

    cout << endl << "CountElement is done!" << endl;

    LeaveCriticalSection(&csCountElementToMain); //сигналим в main
    return 0;
}


int main(){

    InitializeCriticalSection(&csMainToWork);
    InitializeCriticalSection(&csCountElementToMain);
    eventWork = CreateEvent(NULL, TRUE, FALSE, NULL);

    cout << "Enter array size: " << endl;
    cin >> n;
    cout << "Enter elements: " << endl;
    arr.resize(n);
    for (int i = 0; i < n; i++) cin >> arr[i];
    cout << "Size of array : " << n << endl << "Elements: ";
    for (int i = 0; i < n; i++) cout << arr[i] << ", ";

    EnterCriticalSection(&csMainToWork); //для передачи работы в ворк

    HANDLE WorkThread = CreateThread(NULL, 0, Work, NULL, 0, NULL);
    HANDLE CountElementThread = CreateThread(NULL, 0, CountElement, NULL, 0, NULL);

    cout << endl << "Enter k: " << endl;
    cin >> k;

    LeaveCriticalSection(&csMainToWork); //запускаем work
    WaitForSingleObject(eventWork, INFINITE);

    cout << "Updated array: " << endl;
    for (int i = 0; i < n; i++) cout << arr[i] << ", ";
    cout << endl;
    EnterCriticalSection(&csCountElementToMain); //ждем СountElement
    LeaveCriticalSection(&csCountElementToMain);

    cout << "Number of negative numbers: " << num_neg << endl;

    DeleteCriticalSection(&csMainToWork);
    DeleteCriticalSection(&csCountElementToMain);
    CloseHandle(WorkThread);
    CloseHandle(CountElementThread);
    CloseHandle(CountElementThread);
    CloseHandle(eventWork);
}


