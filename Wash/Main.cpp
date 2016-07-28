#include "Main.h"
#include "HookFun.h"

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;
CWash * Wash = new CWash();

BOOL bOnOff = TRUE, bMoney = FALSE;
char vmpKey[1024] = {0};
char infoPath[260] = {0};

void JMPHook(DWORD dwSource, LPVOID lpTarget)
{
	DWORD OldProtect, OldProtect2;
	VirtualProtect((LPVOID)dwSource, 5, PAGE_READWRITE, &OldProtect);
	*(LPBYTE)dwSource = 0xE9;
	*(LPDWORD)(dwSource + 1) = (DWORD)lpTarget - dwSource - 5 ;
	VirtualProtect((LPVOID)dwSource, 5, OldProtect, &OldProtect2);
}


BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

	if(NULL != fnIsWow64Process)
		fnIsWow64Process(GetCurrentProcess(),&bIsWow64);

	return bIsWow64;
}


void InitHook()
{
	// Hook CreateMutant
	VMProtectBeginUltra("InitHook");
	DWORD ZwCreateMutant_Adr = NULL;
	ZwCreateMutant_Adr = (DWORD)GetProcAddress(GetModuleHandle("ntdll.dll"),"ZwCreateMutant");
	if(!IsWow64())
	{
		JMPHook(ZwCreateMutant_Adr+0x0A,HOOK_ZwCreateMutant_X32);
	}else{
		JMPHook(ZwCreateMutant_Adr+0x12,HOOK_ZwCreateMutant_X64);
	}

	// Wait For Game Loaded v103
	while(*(LPBYTE)0x00B946BB != 0xC3)
		Sleep(1);
	while(*(LPBYTE)0x00B9582F == NULL)
		Sleep(1);
	
	// Hook Game Data v103
	SleepAdr = *(DWORD*)0x00B952B4;
	CreateFileAdr = *(DWORD*)0x00B951E8;
	RtlAllocateHeapAdr = *(DWORD*)0x00B953F4;
	RegQueryValueExAdr = *(DWORD*)0x00B95028;
	//timeGetTimeAdr = *(DWORD*)0x00BE164C;

	*(DWORD*)(0x00B952B4) = (DWORD)HOOK_Sleep;
	*(DWORD*)(0x00B951E8) = (DWORD)HOOK_CreateFile;
	*(DWORD*)(0x00B953F4) = (DWORD)HOOK_RtlAllocateHeap;
	*(DWORD*)(0x00B95028) = (DWORD)HOOK_RegQueryValueEx;
	*(DWORD*)(0x00B9521C) = (DWORD)HOOK_ReadProcessMemory;
	//*(DWORD*)(0x00BE164C) = (DWORD)HOOK_timeGetTime;
	VMProtectEnd();
}
int CheckTime = 0;
void WashThread()
{
	VMProtectBeginMutation("WashThread");
	DWORD CHECKPID;
	HWND CHECKHW = NULL;
	bool IsInTime = false;
	char char_temp[100] = {0}, char_temp2[100] = {0};
	int int_temp = 0, AutoCC = 0, Section1 = 0, Section2 = 0, Section3 = 0;

	InitHook();

	// Path
	GetCurrentDirectory(MAX_PATH, infoPath);
	GetCurrentDirectory(MAX_PATH, Wash->strPath);
	strcat_s(Wash->strPath, 260, "\\BBTitle.dat");
	GetPrivateProfileString("Info", "title", NULL, Wash->EXEtitle, 33, Wash->strPath);

	// Wait For Game
	Wash->GetHWND(NULL, "ÃzÃz¤ý");
	Wash->lastSend = NULL;

	// Wait For Data
	while(infoPath[0] == NULL || Wash->account_info[0]== -1 )
		Sleep(100);

	// AccountInfo 
	Wash->account_info[1] = GetPrivateProfileInt("Info", "Type", 0, (LPCSTR)infoPath);
	GetPrivateProfileString("Info", "Account", NULL, account_name[0], 21, (LPCSTR)infoPath);
	GetPrivateProfileString("Info", "PassWord", NULL, Wash->account_pwd[0], 21, (LPCSTR)infoPath);
	GetPrivateProfileString("Info", "Account2", NULL, account_name[1], 21, (LPCSTR)infoPath);
	GetPrivateProfileString("Info", "PassWord2", NULL, Wash->account_pwd[1], 21, (LPCSTR)infoPath); 
	Wash->account_name = account_name[0];
	account_len[0] = strlen(account_name[0]) + 1;
	account_len[1] = strlen(account_name[1]) + 1;
	
	// AccountSetting
	int_temp = GetPrivateProfileInt("Setting", "GetMoney", 0, (LPCSTR)infoPath);
	bMoney = (BOOL)(int_temp);

	// Channel
	AutoCC = GetPrivateProfileInt("Info", "CC", 0, (LPCSTR)infoPath);
	Section1 = GetPrivateProfileInt("Info", "Section1", 0, (LPCSTR)infoPath);
	Section2 = GetPrivateProfileInt("Info", "Section2", 0, (LPCSTR)infoPath);
	Section3 = GetPrivateProfileInt("Info", "Section3", 0, (LPCSTR)infoPath);

	// LoseAccount
	Wash->lose_count = GetPrivateProfileInt("Lose", "Count", 0, (LPCSTR)infoPath);
	memset(Wash->lose_account, NULL, sizeof Wash->lose_account);
	for(int i=0;i<Wash->lose_count;i++){
		sprintf_s(char_temp2,100,"%d",i);
		GetPrivateProfileString("Lose", char_temp2, NULL, Wash->lose_account[i], 21, (LPCSTR)infoPath);
	}

	// Starting Game
	while(GetModuleHandle("BlackCall.aes")==NULL)
		Sleep(100);
	Wash->SendData(-1);

	// Wait For Login
	while(CurrentPosition!=1)
	Sleep(100);

	// AUTO DIE
	*(DWORD*)0x00E7AD00 = (DWORD)CA_VMHook;	// v102 0x00F12E48

	// RECV
	void * CopyClassAdr = VirtualAlloc(NULL,0x10000,MEM_COMMIT,PAGE_READWRITE);
	memcpy(CopyClassAdr,(LPBYTE)0x00BF5000,0x10000);
	*(DWORD*) ((DWORD)CopyClassAdr+0x474+0x0A8) = (DWORD)RecvHook;
	*(DWORD*)(*(DWORD*)(0x00DEE70C)) = ((DWORD)CopyClassAdr+0x474); // v104 DEE70C

	Sleep(2000);

	while(TRUE){
		if(bOnOff){
			char tmp[100] = {0};
			time_t t = time(NULL);
			if(AutoCC){
				strftime(tmp,20,"%X",localtime(&t));

				for(int i=0;i<(int)strlen(tmp);i++)
					if(tmp[i]==':')
						for(int j=i;j<(int)strlen(tmp);j++)
							tmp[j] = tmp[j+1];
			}
			Wash->SendData(CurrentPosition);
			switch(CurrentPosition){
				case POS_LOGIN:
					Sleep(1500);
					Wash->Login();
					Sleep(3200);
					break;
				case POS_CHANNEL:
					Wash->SelectChannel(Section1, Section2, Section3);
					Wash->SendData(POS_CHANNEL);
					Sleep(2600);
					Wash->SendData(POS_CHANNEL);
					Sleep(2600);
					break;
				case POS_LOBBY:
					break;
				case POS_ROOM:
					break;
				case POS_GAME:
					break;
				case POS_SHOP:
					break;
				case POS_SHOP2:
					break;
				default:
					break;
			}
		}else{
			Wash->SendData(0);
		}

		if( (GetTickCount()-CheckTime) >= 900000 ){
			CheckTime = GetTickCount();
			if(VMProtectSetSerialNumber(vmpKey)){
				TerminateProcess(GetCurrentProcess(),0);
				__asm{
					XOR EAX,EAX
					RETN
				}
			}
		}
		do
		{
			CHECKHW = FindWindowEx(NULL,CHECKHW,NULL,"CrashDlg");
			GetWindowThreadProcessId(CHECKHW,&CHECKPID);
			if(CHECKPID == GetCurrentProcessId())
				TerminateProcess(GetCurrentProcess(),0);
		}while(CHECKHW != NULL);
		CHECKHW = NULL;
		CHECKPID = NULL;

		do
		{
			CHECKHW = FindWindowEx(NULL,CHECKHW,NULL,"login fail");
			GetWindowThreadProcessId(CHECKHW,&CHECKPID);
			if(CHECKPID == GetCurrentProcessId())
				TerminateProcess(GetCurrentProcess(),0);
		}while(CHECKHW != NULL);
		CHECKHW = NULL;
		CHECKPID = NULL;

		do
		{
			CHECKHW = FindWindowEx(NULL,CHECKHW,NULL,"Microsoft Visual C++ Runtime Library");
			GetWindowThreadProcessId(CHECKHW,&CHECKPID);
			if(CHECKPID == GetCurrentProcessId())
				TerminateProcess(GetCurrentProcess(),0);
		}while(CHECKHW != NULL);
		CHECKHW = NULL;
		CHECKPID = NULL;

		do
		{
			CHECKHW = FindWindowEx(NULL,CHECKHW,NULL,"Error!");
			GetWindowThreadProcessId(CHECKHW,&CHECKPID);
			if(CHECKPID == GetCurrentProcessId())
				TerminateProcess(GetCurrentProcess(),0);
		}while(CHECKHW != NULL);
		CHECKHW = NULL;
		CHECKPID = NULL;

		if(*(DWORD*)0x00DEE70C==NULL)
			TerminateProcess(GetCurrentProcess(),0);

		Sleep(300);
	}

	VMProtectEnd();
}



BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	VMProtectBeginUltra("DllMain");

	char vmpHWID[256] = {0};
	HANDLE ThreadID = NULL;

	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			//::MessageBoxA(NULL,"","",NULL);
			/*
			//*(DWORD*)0x00F11D78 = (DWORD)CA_VMHook;
			timeGetTimeAdr = *(DWORD*)0x00BE064C;
			*(DWORD*)(0x00BE064C) = (DWORD)HOOK_timeGetTime;
			break;*/
			
			
			VMProtectGetCurrentHWID(vmpHWID,256);
			if(GetModuleHandle("BNB.EXE")!=NULL){
				::MessageBoxA(NULL, vmpHWID, "", MB_OK|MB_ICONINFORMATION);
				return false;
			}
			if(_access("key.dat", 0)!=0){
				return false;
			}else{
				FILE * fp = NULL;
				fopen_s(&fp, "key.dat", "r");
				fread_s(vmpKey,1024,1024,1,fp);
				if(VMProtectSetSerialNumber(vmpKey))
					return false;
			}
			
			CheckTime = GetTickCount();
			Wash->account_info[0] = -1;
			ThreadID = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)WashThread,NULL,NULL,NULL);
			if(ThreadID)
				CloseHandle(ThreadID);
			ThreadID = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)FormThread,hinstDLL,0,NULL);
			if(ThreadID)
				CloseHandle(ThreadID);
			break;
		case DLL_PROCESS_DETACH:
			break;
		default:
			break;
	}
	VMProtectEnd();
	return TRUE;
}
