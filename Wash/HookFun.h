#pragma once

int CurrentPosition = -1;

enum{
	POS_NONE,		// 
	POS_LOGIN,		// µn¤Jµe­±
	POS_CHANNEL,	// ÀW¹D¿ï¾Ü
	POS_LOBBY,		// ¹CÀ¸¤jÆU
	POS_ROOM,		// ¹CÀ¸©Ð¶¡
	POS_GAME,		// ¹CÀ¸¦a¹Ï
	POS_SHOP,		// ÁÊª«°Ó³õ
	POS_SHOP2,		// ©ç½æ¤¤¤ß
};

typedef struct{
	DWORD CAClass1;		// 00AB653C
	DWORD CAClass2;		// 00AB6F44
	DWORD CANull1;		// ªÅ
	DWORD CAUnknown;	// 0x40=¥¼ª¾ 
	BYTE* Data;			// Data«ü°w
	DWORD CANull2;		// NULL
	DWORD CANull3;		// NULL
	DWORD DataType;		// ¼Æ¾Ú¥]ÀY
	DWORD DataLength;	// ¼Æ¾ÚÁ`ªø«×
	DWORD DataPos;		// Data+N
	DWORD CAEncrypt2;	// 0x02=¥[±K
	DWORD CAEncrypt3;	// 0x03=¥[±K
	DWORD CANull4;		// NULL
}RecvData;

bool __stdcall RecvCheck(RecvData * Data);

void DebugLog( const char* format, ... )
{
	va_list	valist;
	FILE *fp;
	char buf1[32],buf2[32];
	fopen_s( &fp, "D:\\Log.txt", "a" ) ;
	_strdate_s( buf1, 32 );
	_strtime_s( buf2, 32 );
	fprintf( fp, "[%s %s - ] ", buf1, buf2 );
	va_start( valist, format );
	vfprintf( fp, format, valist );
	va_end( valist );
	fclose(fp);
	return;
}

int dwTempCount = 0;
DWORD dwESP,dwTemp;
DWORD dwPositionBase;
DWORD RtlAllocateHeapAdr = NULL;

void __declspec(naked) HOOK_RtlAllocateHeap()
{
	
	VMProtectBeginMutation("Postion_ESP");
	__asm
	{
		PUSH EBX
		PUSH ECX
		PUSH ESI
		MOV ESI,ESP
		ADD ESI,0x12C
		SUB ESI,0x0E4
		MOV EBX,DWORD PTR DS:[ESI]
		MOV ECX, 0x005FB743	// v102 5ECFA9
		CMP EBX,ECX 
		POP ESI
		POP ECX
		POP EBX
		JNZ END
		PUSH EBX
		MOV EBX,CACOUNTROL
		MOV DWORD PTR DS:[ESP+0x40],EBX
		POP EBX
		JMP END
CACOUNTROL:
		POP ECX
		MOV DWORD PTR SS:[EBP-0x78],EAX
		MOV BYTE PTR SS:[EBP-0x04],0x0C
		CMP EAX,EBX
		JE CACOUNTROL2
		MOV ECX,EAX
	}
	VMProtectEnd();
	__asm
	{
		MOV EAX,0x009B78B7
		CALL EAX
		JMP CACOUNTROL3
CACOUNTROL2:
		XOR EAX,EAX
	}

	__asm
	{
CACOUNTROL3:
		MOV BYTE PTR SS:[EBP-0x04],0x0B
		LEA ECX,DWORD PTR SS:[EBP-0x74]
	}
	
	VMProtectBeginMutation("Postion_FUN");
	__asm
	{
		MOV DWORD PTR DS:[ESI+0x120],EAX
		MOV EAX,0x009AC56B // V96 963B42 V97 97D632 V98 97D652 V99 97D682
		CALL EAX
		MOV BYTE PTR SS:[EBP-0x04],0x0E
		MOV ECX,DWORD PTR DS:[0x00E7B260]
		MOV EAX,0x00965700 // V96 923660 V97 93D010 V98 93D030 V99 93D060
		CALL EAX
		MOV ECX,DWORD PTR DS:[0x00E7B260]
		MOV EDI,EAX
		MOV EAX,0x009656F0 // V96 923650 V97 93D000 V98 93D020 V99 93D050
		CALL EAX
		MOV DWORD PTR SS:[EBP-0x28],EDI
		MOV EDI,0x0BCD13C
		PUSH EDI
		MOV DWORD PTR SS:[EBP-0x34],EBX
		MOV DWORD PTR SS:[EBP-0x30],EBX
		MOV DWORD PTR SS:[EBP-0x2C],EAX
		MOV EAX,0x0091A540
		CALL EAX
		POP ECX
		PUSH EAX
		PUSH EDI
		LEA ECX,DWORD PTR SS:[EBP-0x74]
		MOV EAX,0x0040151C
		CALL EAX
		MOV dwTemp,EAX
		PUSH EAX
		MOV EAX,DWORD PTR DS:[ESP+0x90]
		MOV dwESP,EAX
		POP EAX
		PUSHAD

	}
	VMProtectEnd();

	if(dwPositionBase==NULL)
	{
		CurrentPosition = POS_LOGIN;
		dwPositionBase = dwTemp - 0x7C; // 0x6C
	}

	if(dwPositionBase == dwTemp )	// LOGIN & CHANNELªºEAX¬Û¦P¡A¶·¯S§O³B²z***
		CurrentPosition = POS_LOGIN;
	else if (dwPositionBase == dwTemp+0x08)
		CurrentPosition = POS_CHANNEL;
	else if (dwPositionBase == dwTemp+0x5C)
		CurrentPosition = POS_LOBBY;
	else if (dwPositionBase == dwTemp+0xE8)
		CurrentPosition = POS_ROOM;
	else if (dwPositionBase == dwTemp+0xB0)
		CurrentPosition = POS_GAME;
	else if (dwPositionBase == dwTemp+0x7C)
		CurrentPosition = POS_SHOP;
	else if (dwPositionBase == dwTemp+0x54)
		CurrentPosition = POS_SHOP2;

	__asm
	{
		POPAD
		PUSH 0x005FB7AA	// V94 5B0AEF V95 5B0B11 V96 5B0B2B V97 5C4AB8 V98 5C4ACB V99 5C4AED
		RETN
END:
		JMP RtlAllocateHeapAdr
	}
	
}


void __declspec(naked) HOOK_ZwCreateMutant_X32 ()
{
	__asm
	{
		CALL NEAR DWORD PTR DS:[EDX]
		XOR EAX,EAX
		RETN 0x10
	}
}

void __declspec(naked) HOOK_ZwCreateMutant_X64()
{
	__asm
	{
		ADD ESP,0x4
		XOR EAX,EAX
		RETN 0x10
	}
}

// §Ö³t¶}©Ð&&¦X¬ù²¾°£ v104
DWORD SleepAdr = NULL;
void __declspec(naked) HOOK_Sleep()
{
	VMProtectBeginMutation("HOOK_Sleep");
	__asm
	{
		PUSH EAX
		XOR EAX,EAX
		MOV EAX,0x009AC30F
		CMP DWORD PTR DS:[ESP+0x04],EAX // V100 9AF4DD
		POP EAX
		JNZ END
		// §Ö³t¶}©Ð 104
		PUSH EBX
		MOV EBX,0x00611367
		CMP DWORD PTR DS:[ESP+0x24],EBX // V100 604076
		POP EBX
		JE HOOK
		// ¦X¬ù²¾°£ 104
		PUSH EBX
		MOV EBX,0x00554062
		CMP DWORD PTR DS:[ESP+0x24],EBX	// V100 5471B1
		POP EBX
		JE HOOK
		JMP END
HOOK:
		MOV DWORD PTR DS:[ESP+0x04],0x009AC333 // V100 9AF501
		MOV DWORD PTR DS:[ESI+0xA4],0x03
		RETN 0x04
END:
		JMP SleepAdr
	}
	VMProtectEnd();
}

// v104
void __declspec(naked) HOOK_ReadProcessMemory()
{
	VMProtectBeginMutation("HOOK_ReadProcessMemory");
	__asm PUSHAD
		SetLastError(1);
	__asm
	{
		POPAD
		XOR EAX,EAX
		RETN 0x14
	}
	VMProtectEnd();
}



// v104
int * RQ_Size;
char * RQ_Buffer;
char * RQ_valueName;
int account_len[2] = {0};
char account_name[2][22] = {0};
DWORD RegQueryValueExAdr = NULL;
void __declspec(naked) HOOK_RegQueryValueEx()
{
	VMProtectBeginMutation("HOOK_RegQueryValueEx");
	__asm
	{
		PUSH EAX
		MOV EAX,DWORD PTR DS:[ESP+0x0C]
		MOV RQ_valueName,EAX
		MOV EAX,DWORD PTR DS:[ESP+0x18]
		MOV RQ_Buffer,EAX
		MOV EAX,DWORD PTR DS:[ESP+0x1C]
		MOV RQ_Size,EAX
		POP EAX
		PUSHAD
	}

	if(strstr(RQ_valueName,"LastSelectedId01")!=NULL)
	{
		for(int i=0;i<account_len[0];i++)
			*(RQ_Buffer+i) = account_name[0][i];
		*RQ_Size = account_len[0];
		__asm 
		{
			POPAD
			XOR EAX,EAX
			RETN 0x18
		}
	}else if(strstr(RQ_valueName,"LastSelectedId")!=NULL)
	{
		for(int i=0;i<account_len[1];i++)
			*(RQ_Buffer+i) = account_name[1][i];
		*RQ_Size = account_len[1];
		__asm 
		{
			POPAD
			XOR EAX,EAX
			RETN 0x18
		}
	}

	__asm{
		POPAD
		JMP RegQueryValueExAdr
	}
	VMProtectEnd();
}

// v104
DWORD CreateFileAdr;
void __declspec(naked) HOOK_CreateFile()
{
	VMProtectBeginMutation("HOOK_CreateFile");
	__asm
	{
		CMP DWORD PTR DS:[ESP],0x008DEF3C
		JE HOOK
		JMP CreateFileAdr
HOOK:
		XOR EAX,EAX
		RETN 0x1C
	}
	VMProtectEnd();
}

static int RoleCount;
DWORD * ESP_ADR = NULL;
DWORD RoleID[8], RoleTMPID;
void __declspec(naked) CA_VMHook()
{
	VMProtectBeginMutation("VMHook");
	__asm
	{
		// VM ICS v104
		PUSH EAX
		MOV EAX,0x00410214
		CMP EAX,DWORD PTR DS:[ESP+0x04]
		POP EAX
		JNZ ORI
		PUSH EBP
		MOV EBP,0x00DEE75C
		DEC DWORD PTR DS:[EBP]
		POP EBP

		// VM ICS Check(AutoDie) v104
		PUSH ECX
		MOV ECX, 0x0065A086
		CMP DWORD PTR DS:[ESP+0x80],ECX
		POP ECX
		JNE NEXTCHECK
		PUSH ECX
		MOV ECX, AutoDieInit
		MOV DWORD PTR DS:[ESP+0x80],ECX
		POP ECX
		JMP ORI

NEXTCHECK:
		// VM ICS Check(AutoBomb) v104
		PUSH ECX
		MOV ECX, 0x0066DC70
		CMP DWORD PTR DS:[EBP+0x04],ECX
		POP ECX
		JNZ ORI
		// (AutoBomb Hook) v104
		PUSH ECX
		MOV ECX, 0x0066DC75
		MOV DWORD PTR DS:[EBP+0x04],ECX
		POP ECX

ORI:
		// ORI FUN v104
		MOV ECX,DWORD PTR DS:[0x00DEED74]
		PUSH 0
		MOV EAX,0x00464398
		CALL EAX
		RETN

AutoDieInit:
		PUSHAD
		MOV RoleTMPID,ESI
		CMP RoleCount,0x01
		JGE DieOrLiveInit
	}

		RoleID[RoleCount++] = RoleTMPID;
		
	__asm
	{
		POPAD
		PUSH 0x0065A086	// not die v104
		RETN
	}

DieOrLiveInit:
	if(RoleTMPID!=RoleID[0]){
		__asm JMP AutoDie
	}else{
		__asm
		{
			POPAD
			PUSH 0x0065A086	// not die v104
			RETN
		}
	}

AutoDie:
	__asm{
		POPAD
		PUSH 0x1D
		LEA ECX,DWORD PTR SS:[EBP-0x48]
		MOV DWORD PTR SS:[EBP-0x10],EAX
		MOV EAX, 0x00848D65
		CALL EAX
		MOV DWORD PTR SS:[EBP-0x4],EDI
		PUSH -0x1
		PUSH 0x1
		LEA ECX,DWORD PTR SS:[EBP-0x48]
		MOV EAX, 0x00848C94
		CALL EAX
		XOR EAX,EAX
		CMP DWORD PTR SS:[EBP-0x10],EDI
		LEA ECX,DWORD PTR SS:[EBP-0x48]
		SETNE AL
		PUSH EAX
		PUSH 0x2
		MOV EAX, 0x00848CD7
		CALL EAX
		TEST EAX,EAX
		PUSH 0x0065A0B9
		RETN
	}
	VMProtectEnd();
}

// v104
DWORD CA_CALL1=0x0091A34E;
DWORD CA_CALL2=0x009A0190;
DWORD CA_CALL3=0x0046B49F;
DWORD CA_CALL4=0x009A01C0;
DWORD CA_CALL5=0x00844A66;
DWORD CA_CALL6=0x00844A31;

void __declspec(naked) RecvHook()
{
	VMProtectBeginMutation("RecvHook");
	__asm{
		PUSH 0x18
		MOV EAX, 0x00AD8171	// v104
		CALL CA_CALL1
		MOV ESI,ECX
		MOV EDI,DWORD PTR SS:[EBP+0x08]
		PUSH 0x64
		MOV ECX,EDI
		CALL CA_CALL2
		PUSH EAX
		PUSH 0x65
		LEA ECX,DWORD PTR SS:[EBP-0x24]
		CALL CA_CALL3
		AND DWORD PTR SS:[EBP-0x04],0
		MOV ECX,EDI
		CALL CA_CALL4
		MOV ECX,DWORD PTR DS:[0x00E33B48]
		PUSH EDI
		CALL CA_CALL5
		TEST EAX,EAX
		JNZ END
		MOV ECX,EDI
		CALL CA_CALL4
		MOV ECX,DWORD PTR DS:[0x00DF12A8]
		PUSH EDI
		CALL CA_CALL6
		TEST EAX,EAX
		JNZ END
		CMP DWORD PTR DS:[ESI+0xD4],EAX
		JE END
		MOV ECX,EDI
		CALL CA_CALL4
		MOV ECX,DWORD PTR DS:[ESI+0x0D4]
		MOV EAX,DWORD PTR DS:[ECX]
		// hook
		PUSHAD
		PUSH EDI
		CALL RecvCheck
		CMP AL,0x01
		POPAD
		JNE END
		// recv ³B²z
		PUSH EDI
		CALL NEAR DWORD PTR DS:[EAX+0xC0]
END:
		PUSH 0x00844C68	// v104
		RETN
	}
	VMProtectEnd();
}

/*
4B
0968F6B0  A5 22 A5 6A 97 58 69 A5 61 EE 86 23 D4 A4 EC 67  ?¥j—Xi¥a?#Ô¤ìg
0968F6C0  AC 96 CA 53 85 96 CA 53 95 96 CA 53 93 96 CA 53  ?ÊS?ÊS?ÊS?ÊS
0968F6D0  90 96 CA 53 85 96 CA 53 95 96 CA 53 95 96 CA 53  ?ÊS?ÊS?ÊS?ÊS
0968F6E0  95                                               ?
*/

BYTE OpenAIData[] = {0xA5,0x33,0x5F,0x5A};
BYTE ChAILvData[] = {0xA5,0x30,0x59,0x5A,0x5E,0x79,0x5B,0x5B};

BYTE ChMAPData[] = {0xA5,0x3C,0xA7,0x1B,0x96,0xCA,0x52,0xCC};

BYTE CRTRoomData[] = {0xA5,0x22,0xA5,0x6A,0x97,0x58,0x69,0xA5,0x61,0xEE,0x86,0x23,0xD4,0xA4,0xEC,0x67,0xAC,0x96,0xCA,0x53,0x85,0x96,0xCA,0x53,0x95,0x96,0xCA,0x53,0x93,0x96,0xCA,0x53,0x90,0x96,0xCA,0x53,0x85,0x96,0xCA,0x53,0x95,0x96,0xCA,0x53,0x95,0x96,0xCA,0x53,0x95};

BYTE StartData[] = {0xA5,0x34,0x96,0xCA,0x53,0xCF,0x96,0xCA,0x06,0x8D,0x97,0x6F,0x52,0x95,0x4C,0xB2,0x53,0x95,0x8E,0xA9,0x07,0xF6,0x1E,0xDB,0xD3,0xC9,0x95,0xCB,0x90,0x6A,0x59,0x5A,0x61,0x16,0x9A,0xD7,0x10,0x1F,0x27,0x43,0x70,0xA5,0x82,0xD9,0x62,0x95,0xDA,0x4B,0xA7,0x56,0x93,0x9D,0x64,0xD7,0xF5,0x0D,0x34,0x5D,0x9A,0x1E,0x70,0xBB,0xDE,0xE6,0x8A,0x0D,0xE7,0x9A,0xFF,0x77,0x5A,0xD3,0xFB,0xD3,0xEE,0x56,0xE6,0xC5,0xA4,0x8F,0x00,0x79,0xDA,0xB7,0xC3,0x16,0x7F,0xC9,0x3D,0xDD,0xDD,0x3E,0x53,0x93,0x6C,0x43,0x96,0xCA,0x52,0xCC,0xA7,0x1B,0x96,0xCA,0x52,0xCC,0x5A,0x58,0x03,0xBE,0x59,0xCE};

BYTE EXITData[] = {0xA5,0x27};

/*
07348780  A5 3C A7 1B 96 CA 52 CC                          ??–ÊR?...
*/
bool __stdcall RecvCheck(RecvData * Data)
{
	VMProtectBeginMutation("RecvCheck");
	bool bRetn = true;
	if(!bOnOff)
		return bRetn;

	switch(Data->DataType)
	{
	case 0x11:	// ¹CÀ¸¤jÆU
		Wash->SendData(0x4B,(BYTE*)CRTRoomData,sizeof CRTRoomData);
		Wash->SendData(POS_LOBBY);
		break;
	case 0x4F:	// ©Ð¶¡³Ð¥ß
		RoleCount = 0;
		OpenAIData[2] = 0x5D;
		Wash->SendData(0x5A,(BYTE*)OpenAIData,4);
		OpenAIData[2] = 0x5C;
		Wash->SendData(0x5A,(BYTE*)OpenAIData,4);
		ChAILvData[4] = 0x5E;
		Wash->SendData(0x59,(BYTE*)ChAILvData,8);
		ChAILvData[4] = 0x5F;
		Wash->SendData(0x59,(BYTE*)ChAILvData,8);
		ChAILvData[4] = 0x5D;
		Wash->SendData(0x59,(BYTE*)ChAILvData,8);
		ChAILvData[4] = 0x5C;
		Wash->SendData(0x59,(BYTE*)ChAILvData,8);
		Wash->SendData(0x55,(BYTE*)ChMAPData,sizeof ChMAPData);
		Wash->SendData(0x5D,(BYTE*)StartData,sizeof StartData, TRUE);
		Wash->SendData(POS_ROOM);
		break;
	case 0x9E:	// ¹CÀ¸µ²§ô
		Wash->SendData(0x4E,(BYTE*)EXITData,sizeof EXITData);
		Wash->SendData(POS_GAME);
		bRetn = false;
		break;
	case 0x6F:  // ¹CÀ¸¶}©l
		break;
	}
	VMProtectEnd();
	return bRetn;
}
