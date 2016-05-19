// os2.cpp: 
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#pragma comment(lib, "psapi.lib")
#include <psapi.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#define SIZE 1024

struct ProcInfo {
	DWORD processID;
	TCHAR processName[200];
	TCHAR processUser[200];
	TCHAR processStatus;
	unsigned int processTime;
	unsigned int systemTime;
	unsigned int processPercent;
	unsigned int processMemory;
};

int choosesort = 1;

bool compareprocinfo(ProcInfo &a, ProcInfo &b) {
	switch (choosesort) {
	case 1: return a.processName < b.processName;
	case 2: return a.processID < b.processID;
	case 3: return a.processUser < b.processUser;
	case 4: return a.processPercent < b.processPercent;
	case 5: return a.processMemory < b.processMemory;
	default: return a.processName < b.processName;
	}
}

vector<ProcInfo> procInfo;
unsigned int processorsNumber;

void PreviewProcess(DWORD pID, unsigned int i)
{
	ProcInfo temp;
	// Get handle to process
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE, pID);
	// Get initial process time
	FILETIME CreationTime = { 0, 0 }, LocalCreationTime = { 0, 0 }, ExitTime = { 0, 0 }, KernelTime = { 0, 0 }, UserTime = { 0, 0 };
	GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);
	temp.processTime = KernelTime.dwLowDateTime + UserTime.dwLowDateTime;
	// Get initial system time
	SYSTEMTIME sys_time;
	GetSystemTime(&sys_time);
	temp.systemTime = sys_time.wMilliseconds + 1000 * sys_time.wSecond;
	procInfo.push_back(temp);
	// Release handle of process
	CloseHandle(hProcess);
}

void GetProcessInfo(DWORD pID, unsigned int i)
{
	// Get handle to process
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pID);
	// Get process name
	TCHAR baseName[200] = TEXT("<unknown>");
	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
			GetModuleBaseName(hProcess, hMod, baseName, sizeof(baseName) / sizeof(TCHAR));
	}
	//*pName = baseName;
	_tcscpy_s(procInfo[i].processName, baseName);

	// Get process user
	HANDLE tokenInfo = 0;
	TOKEN_USER *tokenUser;
	DWORD userLength, domainLength;
	TCHAR user[200] = TEXT("<unknown>");
	TCHAR domain[200], buffer[200];
	int psidNameUse;
	if (OpenProcessToken(hProcess, TOKEN_QUERY, &tokenInfo))
	{
		tokenUser = (TOKEN_USER*)buffer;
		if (GetTokenInformation(tokenInfo, (TOKEN_INFORMATION_CLASS)1, tokenUser, 200, &userLength))
		{
			userLength = 200;
			domainLength = 200;
			LookupAccountSid(0, tokenUser->User.Sid, user, &userLength,
				domain, &domainLength, (PSID_NAME_USE)&psidNameUse);
		}
	}
	if (tokenInfo)
		CloseHandle(tokenInfo);
	_tcscpy_s(procInfo[i].processUser, user);
	// Get process state
	DWORD dwCode;
	procInfo[i].processStatus = TEXT('?');
	if (GetExitCodeProcess(hProcess, &dwCode))
	{
		if (dwCode == STATUS_PENDING)
			procInfo[i].processStatus = TEXT('Y');
		else
			procInfo[i].processStatus = TEXT('N');
	}
	// Get process memory usage
	PROCESS_MEMORY_COUNTERS pmc;
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		procInfo[i].processMemory = pmc.WorkingSetSize / 1024;
	// Get process time and system time...
	FILETIME CreationTime = { 0, 0 }, LocalCreationTime = { 0, 0 }, ExitTime = { 0, 0 }, KernelTime = { 0, 0 }, UserTime = { 0, 0 };
	GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);
	SYSTEMTIME sys_time;
	GetSystemTime(&sys_time);
	// ... and calculate CPU load of process
	int delay = KernelTime.dwLowDateTime + UserTime.dwLowDateTime - procInfo[i].processTime;
	if (delay > 0)
		procInfo[i].processPercent = delay / 100 / 1000;
	else
		procInfo[i].processPercent = 0;
	// Release handle of process
	CloseHandle(hProcess);
}

void memusing(SYSTEM_INFO &sysInfo) {
	// Show memory usage
	system("cls");
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(memInfo);
	GlobalMemoryStatusEx(&memInfo);
	printf("Memory Usage Information:\n%d %% of memory in use\n", memInfo.dwMemoryLoad);
	printf("%d total MB of physical memory\n", memInfo.ullTotalPhys / 1024 / 1024);
	printf("%d free MB of physical memory\n", memInfo.ullAvailPhys / 1024 / 1024);
	printf("%d total MB of paging file\n", memInfo.ullTotalPageFile / 1024 / 1024);
	printf("%d free MB of paging file\n", memInfo.ullAvailPageFile / 1024 / 1024);
	printf("Page size: %u kB\n", sysInfo.dwPageSize / 1024);
}

void Print(ProcInfo &obj)
{
	_tprintf(TEXT("%.8s \t|%5u |%  .4s   |\t %c    |\t%3u %%  |\t%7u kB|\n"),
		obj.processName, obj.processID, obj.processUser,
		obj.processStatus, obj.processPercent, obj.processMemory);
}

void processTable(int &count) {
	procInfo.clear();
	system("cls");
	//TCHAR *processName[200], *processUser[200];
	DWORD bytesReturned, returnedProcesses;
	DWORD processID[1024];
	EnumProcesses(processID, sizeof(processID), &bytesReturned);
	returnedProcesses = bytesReturned / sizeof(DWORD);
	procInfo.reserve(processorsNumber + 1);

	// Collect initial processes information
	for (DWORD i = 0; i < returnedProcesses; i++)
		if (processID[i] != 0)
		{

			PreviewProcess(processID[i], i);
			procInfo[count].processID = processID[i];
			++count;
		}
	// Sleep to measure CPU shares
	Sleep(100);
	// Collect precesses info list
	EnumProcesses(processID, sizeof(processID), &bytesReturned);
	returnedProcesses = bytesReturned / sizeof(DWORD);
	//			system("cls");
	printf("List of processes:\n");
	for (int i = 0; i < count; i++) {
		if (procInfo[i].processID != 0)
		{
			GetProcessInfo(procInfo[i].processID, i);

		}
	}
}

void PrintProctable(int &count) {
	_tprintf(TEXT(" -------------------------------------------------------------------------\n"));
	_tprintf(TEXT("|Process Name  \t|PID  |   user |     Work  |\tCPU    |                Mem |\n"));
	_tprintf(TEXT(" -------------------------------------------------------------------------\n"));
	for (int i = 0; i < count; i++)
	{
		if (procInfo[i].processID != 4)
			Print(procInfo[i]); //Printing
	}
	_tprintf(TEXT(" ------------------------------------------------------------------------\n"));
	printf("Processes: %d\n", count);
}


int _tmain(int argc, _TCHAR* argv[])
{
	char menuItem = 'S';
	int count = 0;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo); 
	processorsNumber = sysInfo.dwNumberOfProcessors;
	while (true)
	{
		count = 0;
		switch (menuItem)
		{
		case 'S':
		{
			cout << " Please, choose sort method \n\t1 - Name \n\t2 - IDs  \n\t3 - User \n\t4 - CPU \n\t5 - Memory \n";
			cin >> choosesort;
			processTable(count);
			sort(procInfo.begin(), procInfo.end(), compareprocinfo);
			PrintProctable(count);
		}
		break;
		case 'M':
		{
			memusing(sysInfo);
		}
		break;
		case 'P':
		{
			processTable(count);
			PrintProctable(count);
		}
		break;
		case 'E':
		{
			cout << "Exit" << endl;
			return 0;
		}
		break;
		default:
		{
			processTable(count);
			PrintProctable(count);
		}
		}
		cout << "\nChoose action \n\tS - Sort process table \n\tP - Update Table \n\tM - View memory info \n\tE - Exit \n";
		menuItem = _getch();
	}
	return 0;
}

