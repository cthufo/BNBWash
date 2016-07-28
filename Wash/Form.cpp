#include "Main.h"

BOOL CALLBACK DlgProc (HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
	VMProtectBeginMutation("DlgProc");
	COPYDATASTRUCT *pCopyData = NULL;
	switch(message){
		case WM_INITDIALOG:
			break;
		case WM_COMMAND:
			break;
		case WM_COPYDATA:
			ShowWindow(hDlg,0);
			pCopyData = (COPYDATASTRUCT*)lParam;  
			switch(pCopyData->dwData)
			{
			case 0x00:
				strcat_s(infoPath, 260, "\\data\\");
				strcat_s(infoPath, 260, (char*)pCopyData->lpData);
				break;
			case 0x01:
				memcpy(&Wash->account_info[0],pCopyData->lpData,pCopyData->cbData);
				break;
			case 0x02:
				bOnOff = !bOnOff;
				break;
			default:
				break;
			}
			break;
		default:
			break;
	}
	VMProtectEnd();
	return FALSE;
}

void FormThread(HINSTANCE hinst)
{
	VMProtectBeginMutation("FormThread");
	DialogBox(hinst,(LPCTSTR)FUCK_YOU_NG, NULL, DlgProc);
	VMProtectEnd();
}