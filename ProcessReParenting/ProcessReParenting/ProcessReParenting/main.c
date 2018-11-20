#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <Windows.h>

uint32_t get_explorer_pid()
{

	uint32_t pid = 0;
	HWND hShell = GetShellWindow();
	GetWindowThreadProcessId(hShell, &pid);
	return pid;
}

HANDLE open_explorer_process()
{
	uint32_t explorerPid = get_explorer_pid();
	return OpenProcess(PROCESS_ALL_ACCESS, false, explorerPid);
}

void main(int argc, char** argv)
{
	STARTUPINFOEX startupInfoEx = { 0 };
	PROCESS_INFORMATION piProcess = { 0 };
	bool fSuccess = false;
	LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList = NULL;
	SIZE_T size = 0;
	HANDLE hExplorer = NULL;

	// initialize attribute list
	fSuccess =
		InitializeProcThreadAttributeList(NULL, 1, 0, &size) ||
		GetLastError() == ERROR_INSUFFICIENT_BUFFER;

	if (fSuccess)
	{
		lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, size);
		fSuccess = lpAttributeList != NULL;
	}

	if (fSuccess)
	{
		fSuccess = InitializeProcThreadAttributeList(lpAttributeList, 1, 0, &size);
	}

	// set attribute list
	if (fSuccess)
	{
		hExplorer = open_explorer_process();
		fSuccess = UpdateProcThreadAttribute(
			lpAttributeList,
			0,
			PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,
			&hExplorer,
			sizeof(HANDLE),
			NULL,
			NULL
		);
	}

	// create process with the new attribute list
	if (fSuccess)
	{
		startupInfoEx.StartupInfo.cb = sizeof(STARTUPINFOEX);
		startupInfoEx.lpAttributeList = lpAttributeList;

		fSuccess = CreateProcess(
			argv[1],
			NULL,
			NULL,
			NULL,
			true,
			EXTENDED_STARTUPINFO_PRESENT | CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&startupInfoEx.StartupInfo,
			&piProcess
		);
	}

	// cleanup
	if (lpAttributeList != NULL)
	{
		DeleteProcThreadAttributeList(lpAttributeList);
		HeapFree(GetProcessHeap(), 0, lpAttributeList);
	}
}