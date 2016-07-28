
// [0] = ListNo , [1] = Type , [2] = Position , [3] = Money
class CWash
{

public:
	DWORD lastSend;
	int lose_count;
	int account_type;
	int account_info[4];
	LPSTR account_name;
	char account_pwd[2][22];
	char strPath[260];
	char EXEtitle[34];
	char lose_account[1000][22];
	
	HWND GetHWND(LPCSTR strClass, LPCSTR strWindow )
	{
		DWORD dwPID = NULL;
		do{

			m_hWnd = FindWindowEx(NULL, m_hWnd, strClass, strWindow);
			GetWindowThreadProcessId(m_hWnd, &dwPID);
			Sleep(10);
		}while(dwPID != GetCurrentProcessId());
		return m_hWnd;
	}

	void SendData(int n)
	{
		HWND HW = NULL;
		COPYDATASTRUCT data;
		
		if((GetTickCount()-lastSend)<4000)
			return;
		lastSend = GetTickCount();
		account_info[2] = n;
		account_info[3] = GetMoneyInfo(0);
		HW = FindWindow(NULL,EXEtitle);
		
		if(HW == NULL)
			return;
		data.cbData = 20;		
		data.dwData = 0;				
		data.lpData = account_info;
		::SendMessage(HW,WM_COPYDATA,NULL,(LPARAM)(&data));
	}

	void Click(WORD wX,WORD wY,UINT uTimes = 1, UINT uDelay = 450)
	{
		VMProtectBeginMutation("Click");
		LPARAM LParam = MAKELPARAM(wX, wY);
		for(UINT i=0;i<uTimes;i++){
			PostMessage(m_hWnd, WM_LBUTTONDOWN, 0, LParam);
			PostMessage(m_hWnd, WM_LBUTTONUP, 0, LParam);
			Sleep(uDelay);
		}
		VMProtectEnd();
	}

	void Push(WORD wKey, UINT uTimes = 1, UINT uDelay = 100)
	{
		VMProtectBeginMutation("Push");
		for(UINT i=0;i<uTimes;i++){
			PostMessage(m_hWnd, WM_KEYDOWN, wKey, 0);
			PostMessage(m_hWnd, WM_KEYUP, wKey, 0);
			Sleep(uDelay);
		}
		VMProtectEnd();
	}

	void Input(char * lpStr)
	{	
		VMProtectBeginMutation("Input");
		for(int i=0; i<(int)strlen(lpStr); i++)
			SendMessage(m_hWnd,WM_CHAR,lpStr[i]&0x00FF,0);
		VMProtectEnd();
	}

	void Login()
	{	
		Push(VK_RETURN,1,100);
		Click(600, 475);	// 2P´ˆØ√
		Click(420, 475);	// 1P±KΩX
		Input(account_pwd[0]);
		Sleep(300);
		Click(520, 475);	// 2P ±KΩX
		Input(account_pwd[1]);
		Click(400, 570);	// µn§J
	}

	void SelectChannel(int Type, int Page, int Channel)
	{
		WORD X, Y;
		X = 380 + 70 * Type;
		Y = 120;
		Click(X,Y);
		if(Page > 5) {
			Click(680,520);
			Page %= 5;
			if(Page == 0) Page = 5;
		}
		X = 500 + 30 * Page;
		Y = 520;
		Click(X, Y);
		X = Channel % 2 ? 500:670;
		Y = 170 + 60 * int((Channel+1)/2);
		Click(X, Y);
	}

	void CreateRoom()
	{
		Push(VK_RETURN, 3);
		Click(250, 70);
		Push(VK_RETURN);
	}

	bool CheckID(LPSTR EnemyID)
	{
		// ¿À≈ÁID
		for(int i = 0;i < lose_count ; i ++)
		{
			if(strcmp(EnemyID,lose_account[i]) == 0) 
				return true;
		}
		return false;
	}
	/*
	void CheckExit()
	{
		VMProtectBeginMutation("CheckExit");
		char * TempUserID;

		DisconnectGame();

		if(account_info[1]==2)
		{
			ExitGameRoom();
			return;
		}

		TempUserID = GetGameUserID(0);

		if(strcmp((char*)account_name,TempUserID)==0)
			TempUserID = GetGameUserID(1);

		if(CheckID(TempUserID))
			ExitGameRoom();
		VMProtectEnd();
	}
	*/
	// v104
	void SendData(WORD Type,BYTE * Data,int DataLen, BOOL bEnable = FALSE)
	{
		VMProtectBeginVirtualization("send");
		long CAPacket[11] = {0};
		long * CAPacketClass = &CAPacket[0];

		CAPacket[0] = 0x00BDB370;	// Class2	// v90 B7AF08 v91 B856D0 V92 B8D8A8
		CAPacket[1] = 0x00BF934C;	// Class2	// v90 B97644 v91 BA2E34 V92 BAB0A4
		CAPacket[2] = NULL;			// Unknown
		CAPacket[3] = bEnable ? 0x80 : 0x40;			// Unknown
		CAPacket[4] = (long)Data;	// Data
		CAPacket[5] = NULL;			// Unknown
		CAPacket[6] = NULL;			// Unknown
		CAPacket[7] = Type;			// DataType
		CAPacket[8] = DataLen;		// DataLen
		CAPacket[9] = 2;			// Unknown
		CAPacket[10] = 2;			// Unknown
		__asm{
			PUSHAD
			MOV ECX,DWORD PTR DS:[0x00DEE7D8]
			TEST ECX,ECX
			JE END
			PUSH CAPacketClass
			MOV DWORD PTR DS:[0x00E78864],00
			MOV EAX, 0x008DA3C4
			CALL EAX
END:
			POPAD
		}
		VMProtectEnd();
	}

	// v103
	int GetMoneyInfo(int UserNo)
	{
		VMProtectBeginMutation("GetMoneyInfo");
		int MoneyInfo = 0;
		DWORD TidTemp = NULL;
		__asm{
			PUSH EAX
			PUSH EBX
			MOV EAX,DWORD PTR FS:[0x18]
			MOV EBX,DWORD PTR DS:[EAX+0x24]
			MOV TidTemp,EBX
			MOV EBX,DWORD PTR DS:[0x00E33B9C]
			MOV DWORD PTR DS:[EAX+0x24],EBX
			POP EBX
			POP EAX
			PUSHAD
			MOV ECX,DWORD PTR DS:[0x00DEE720]
			TEST ECX,ECX
			JE END
			PUSH UserNo
			MOV EAX,0x008F7F29	// v100 0x008C9787 v102 0x008CA595
			CALL EAX
			MOV MoneyInfo,EAX
END:
			POPAD
			PUSH EAX
			PUSH EBX
			MOV EAX,DWORD PTR FS:[0x18]
			MOV EBX,TidTemp
			MOV DWORD PTR DS:[EAX+0x24],EBX
			POP EBX
			POP EAX
		}
		VMProtectEnd();
		return MoneyInfo;
	}

	/*
	// v103
	void ExitGameRoom()
	{
		VMProtectBeginMutation("ExitGameRoom");
		__asm
		{
			PUSHAD
			MOV ECX,0x00DEE77C	// v100 00E4C0DC
			MOV ECX,DWORD PTR DS:[ECX]
			CMP ECX,0
			JE END
			PUSH 0
			PUSH 0
			MOV EAX,0x0065F33D	// v100 0064E7A7 v102 0x0064F3E0
			CALL EAX
END:
			POPAD
		}
		VMProtectEnd();
	}
	
	// v103
	void DisconnectGame()
	{
		VMProtectBeginVirtualization("DisconnectGame");
		// VM 7D6B1B
		__asm{
			PUSHAD
			MOV ECX,0x00DF1280	// v100 00E61F48
			MOV EAX,0x007D7C3A	// v100 007B7F4B
			MOV ECX,DWORD PTR DS:[ECX]
			CMP ECX,0
			JE END
			PUSH 0x20000
			PUSH 0
			CALL EAX
END:
			POPAD
		}
		VMProtectEnd();
	}

	// v103
	char * GetGameUserID(int UserNo)
	{
		VMProtectBeginVirtualization("GetGameUserID");
		long IDCLASS = NULL;
		long * pIDClass = &IDCLASS;
		char * RetnChar = NULL;
		__asm{
			PUSHAD
			XOR EAX,EAX
			MOV ECX,0x00DEE780	// v100 E4A990
			MOV ECX,DWORD PTR DS:[ECX]
			CMP ECX,EAX
			JE END
			PUSH UserNo
			PUSH pIDClass
			MOV EAX,0x0083574A	// v100 8120DB v102 0x00812C63
			CALL EAX
			MOV EAX,IDCLASS
			CMP EAX,0
			JE END
			ADD EAX,0x08
			MOV RetnChar,EAX
END:
			POPAD
		}
		VMProtectEnd();
		return RetnChar;
	}
	*/
	HWND m_hWnd;	// •ÿº–µ¯µ°HANDLE
};

