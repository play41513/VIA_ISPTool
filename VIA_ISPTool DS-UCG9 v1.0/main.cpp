//---------------------------------------------------------------------------
#include <windows.h>    // 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <cfgmgr32.h>	// 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <SetupAPI.h>	// 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <vcl.h>
#pragma hdrstop

#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
	/*HRGN hRgnR = CreateEllipticRgn(0,0,plbtnStartBackG->
	Width,plbtnStartBackG->Height);
	SetWindowRgn(plbtnStartBackG->Handle,hRgnR,TRUE);

	hRgnR = CreateEllipticRgn(0,0,plbtnStart->
	Width,plbtnStart->Height);
	SetWindowRgn(plbtnStart->Handle,hRgnR,TRUE); */
	handleThread = NULL;
	dwTestTime = 0;
	bAutoUpdate = false;
}
//---------------------------------------------------------------------------
AnsiString TfrmMain::DosCommand(AnsiString sCmdline)
{
	PROCESS_INFORMATION proc = {0}; //關於進程資訊的一個結構
	long ret;
	bool sPipe;
	STARTUPINFOA start = {0};
	SECURITY_ATTRIBUTES sa = {0};
	HANDLE hReadPipe ;
	HANDLE hWritePipe;
	AnsiString sOutput;
	AnsiString sBuffer;
	unsigned long lngBytesRead;
	char cBuffer[256];
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor=0;
	sa.bInheritHandle = TRUE;
	sPipe=::CreatePipe(&hReadPipe, &hWritePipe,&sa, 0); //創建管道
	if (!sPipe)
	{
	sOutput="CreatePipe failed. Error: "+AnsiString(GetLastError());
	//memoMsg->Lines->Add("CreatePipe failed. Error: "+AnsiString(GetLastError()));
	return sOutput;
	}
	start.cb = sizeof(STARTUPINFOA);
	start.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	start.hStdOutput = hWritePipe;
	start.hStdError = hWritePipe;
	start.wShowWindow = SW_HIDE;
	sBuffer = sCmdline;
	ret =::CreateProcessA(0, sBuffer.c_str(), &sa, &sa, TRUE, NORMAL_PRIORITY_CLASS, 0, 0, &start, &proc);
	if (ret == 0)
	{
	sOutput="Bad command or filename";
	//memoMsg->Lines->Add("Bad command or filename");
	return sOutput;
	}
	::CloseHandle(hWritePipe);
	DWORD dw = WaitForSingleObject(proc.hProcess, 5000);
	if(dw == WAIT_TIMEOUT)
	{
		::CloseHandle(proc.hProcess);
		::CloseHandle(proc.hThread);
		::CloseHandle(hReadPipe);
		return "";
	}
	do
	{
	memset(cBuffer,'\0',sizeof(cBuffer));
	ret = ::ReadFile(hReadPipe, &cBuffer, 255, &lngBytesRead, 0);
	//替換字串
	for(unsigned long i=0; i< lngBytesRead; i++){
		if(cBuffer[i] == '\0'){
			cBuffer[i] = ' ';
		}else if(cBuffer[i] == '\n'){
			cBuffer[i] = ' ';
		}
	}
	sBuffer=StrPas(cBuffer);
	sOutput = sOutput+sBuffer;
	Application->ProcessMessages();

	} while (ret != 0 );
	::CloseHandle(proc.hProcess);
	::CloseHandle(proc.hThread);
	::CloseHandle(hReadPipe);
	return sOutput;
}
DWORD WINAPI ThreadExecute(LPVOID Param)
{
	while(true)
	{
		switch(frmMain->dwStep)
		{
			case STEP_CHECK_DEV_ONLINE:
				frmMain->dwTestTime = 0;
				frmMain->dwStep
				 = frmMain->EnumUSB() ? STEP_CHECK_VERSION : STEP_CHECK_DEV_ONLINE;
				if(frmMain->dwStep == STEP_CHECK_VERSION)
				{
					frmMain->plResult->Caption = "Testing...";
					frmMain->dwTimeOut = GetTickCount()+20000;
					frmMain->plbtnStart->Enabled = false;
					frmMain->dwTestTime = GetTickCount();
				}
				break;
			case STEP_CHECK_VERSION:
				DeleteFile("Result.txt");
				frmMain->DosCommand("HUBIspTool_Console.exe /F0");
				frmMain->dwStep = STEP_READ_VERSION;
				break;
			case STEP_READ_VERSION:
				frmMain->dwMsg = frmMain->CheckVersionFile();
				if(frmMain->dwMsg != MSG_VERSION_FAIL_CONN)
				{
					if(frmMain->dwMsg == MSG_VERSION_FILE_NO_FIND)
						break;
					if(frmMain->ckbUpdateFW->Caption == "")
					{
						frmMain->dwStep
						 = frmMain->dwMsg == MSG_VERSION_PASS ? STEP_VERSION_UPDATE_SUCCESS : STEP_VERSION_ERROR;
						if(frmMain->dwStep == STEP_VERSION_ERROR)
						{
							if(GetTickCount() > frmMain->dwTimeOut)
								frmMain->astrErrorMsg    = "(!)WRONG FW-Version";
							else
								frmMain->dwStep 		 = STEP_CHECK_VERSION;

						}
					}
					else
					{
						frmMain->dwStep
						 = frmMain->dwMsg == MSG_VERSION_PASS ? STEP_VERSION_UPDATE_SUCCESS : STEP_UPDATE_VERSION;
						if(frmMain->bAutoUpdate) frmMain->dwStep = STEP_UPDATE_VERSION;
					}
				}
				else if(GetTickCount() > frmMain->dwTimeOut)
				{
					frmMain->dwStep = STEP_VERSION_ERROR;
					frmMain->astrErrorMsg    = "(!)Failed to Read Version";
				}
				else
					frmMain->dwStep = STEP_CHECK_VERSION;
				break;
			case STEP_UPDATE_VERSION:
				frmMain->plResult->Caption = "Updating...";
				frmMain->DosCommand("ISPTool.exe ISP RTD2142_ACTIONSTAR_DSUCF7_User_V0p0D.bin");
				frmMain->dwStep = STEP_CHECK_UPDATE_VERSION;
				frmMain->dwTimeOut = GetTickCount()+60000;
				break;
			case STEP_CHECK_UPDATE_VERSION:
				if(!frmMain->GetProcessIdFromName("ISPTool.exe"))
				{
					frmMain->dwTimeOut = GetTickCount()+10000;
					while(true)
					{
						frmMain->DosCommand("ISPTool.exe ReadVersion");
						frmMain->dwMsg = frmMain->CheckVersionFile();
						if(frmMain->dwMsg != MSG_VERSION_FAIL_CONN)
						{
							frmMain->dwStep
							 = frmMain->dwMsg == MSG_VERSION_PASS ? STEP_VERSION_UPDATE_SUCCESS : STEP_VERSION_ERROR;
							if(frmMain->dwStep == STEP_VERSION_ERROR)
								frmMain->astrErrorMsg    = "(!)WRONG FW-Version";
							break;
						}
						else if(GetTickCount() > frmMain->dwTimeOut)
						{
							frmMain->dwStep = STEP_VERSION_ERROR;
							frmMain->astrErrorMsg    = "(!)Failed to Read Version";
							break;
						}
						frmMain->Refresh();
						frmMain->Delay(100);
					}
				}
				else
				{
					if(frmMain->plResult->Caption.Pos("Updating..."))
						frmMain->plResult->Caption = "Updating   ";
					else if(frmMain->plResult->Caption.Pos("Updating.."))
						frmMain->plResult->Caption = "Updating...";
					else if(frmMain->plResult->Caption.Pos("Updating."))
						frmMain->plResult->Caption = "Updating.. ";
					else
						frmMain->plResult->Caption = "Updating.  ";

					if(GetTickCount() > frmMain->dwTimeOut)
					{
						frmMain->dwStep = STEP_VERSION_ERROR;
						frmMain->DosCommand("Taskkill /im ISPTool.exe /F");
						frmMain->astrErrorMsg    = "(!)Failed to Update Version";
					}
				}
				break;
			case STEP_VERSION_ERROR:
				frmMain->dwTestTime = 0;
				frmMain->plResult->Color = clRed;
				frmMain->plResult->Caption = 	frmMain->astrErrorMsg;
				frmMain->dwStep = STEP_CHECK_DEV_OFFLINE;
				break;
			case STEP_VERSION_UPDATE_SUCCESS:
				frmMain->dwTestTime = 0;
				frmMain->plResult->Color = clGreen;
				frmMain->plResult->Caption = "PASS";
				frmMain->dwStep = STEP_CHECK_DEV_OFFLINE;
				break;
			case STEP_CHECK_DEV_OFFLINE:
				frmMain->plbtnStart->Enabled = true;
				if(frmMain->dwStep != STEP_STOP)
				{
					frmMain->dwStep
					 = frmMain->EnumUSB() ? STEP_CHECK_DEV_OFFLINE : STEP_CHECK_DEV_ONLINE;
                }
				if(frmMain->dwStep == STEP_CHECK_DEV_ONLINE)
				{
					frmMain->plResult->Color = clBtnHighlight;
					frmMain->plResult->Caption = "Waiting...";
					frmMain->plPDVersion->Caption = "";
					frmMain->lbTime->Caption = "";
                }
				break;
		}
		frmMain->Refresh();
		frmMain->Delay(100);
	}
	return 1;
}
bool TfrmMain::EnumUSB()
{

	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i,j;
	AnsiString SS,USBPath;
	PSP_DEVICE_INTERFACE_DETAIL_DATA   pDetail   =NULL;
	GUID GUID_USB =StringToGUID(GUID_USB_HUB);
	//--------------------------------------------------------------------------
	//   獲取設備資訊
	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_USB,
	0,   //   Enumerator
	0,
	DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	if   (hDevInfo   ==   INVALID_HANDLE_VALUE){
		//DEBUG("ERROR - SetupDiGetClassDevs()"); //   查詢資訊失敗
	}
	else{
	//--------------------------------------------------------------------------
		SP_DEVICE_INTERFACE_DATA   ifdata;
		DeviceInfoData.cbSize   =   sizeof(SP_DEVINFO_DATA);
		for (i=0;SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData);i++)	//   枚舉每個USB設備
		{
			ifdata.cbSize   =   sizeof(ifdata);
			if (SetupDiEnumDeviceInterfaces(								//   枚舉符合該GUID的設備介面
			hDevInfo,           //   設備資訊集控制碼
			NULL,                         //   不需額外的設備描述
			(LPGUID)&GUID_USB,//GUID_CLASS_USB_DEVICE,                     //   GUID
			(ULONG)i,       //   設備資訊集�堛熙]備序號
			&ifdata))                 //   設備介面資訊
			{
				ULONG predictedLength   =   0;
				ULONG requiredLength   =   0;
				//   取得該設備介面的細節(設備路徑)
				SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   設備資訊集控制碼
					&ifdata,          //   設備介面資訊
					NULL,             //   設備介面細節(設備路徑)
					0,         	      //   輸出緩衝區大小
					&requiredLength,  //   不需計算輸出緩衝區大小(直接用設定值)
					NULL);            //   不需額外的設備描述
				//   取得該設備介面的細節(設備路徑)
				predictedLength=requiredLength;
				pDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT,   predictedLength);
				pDetail->cbSize   =   sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

				if(SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   設備資訊集控制碼
					&ifdata,             //   設備介面資訊
					pDetail,             //   設備介面細節(設備路徑)
					predictedLength,     //   輸出緩衝區大小
					&requiredLength,     //   不需計算輸出緩衝區大小(直接用設定值)
					NULL))               //   不需額外的設備描述
				{
					try
					{
						char   ch[512];
						for(j=0;j<predictedLength;j++)
						{
						ch[j]=*(pDetail->DevicePath+8+j);
						}
						SS=ch;
						SS = SS.SubString(1,SS.Pos("{")-2);
						if(SS.UpperCase().Pos("VID_2109&PID_0822")
						||SS.UpperCase().Pos("VID_2109&PID_2822"))
						{
							GlobalFree(pDetail);
							SetupDiDestroyDeviceInfoList(hDevInfo);
							return true;
						}
						//Memo1->Lines->Add(SS);
						//DEBUG(SS);
					}
					catch(...)
					{
						//DEBUG("列舉失敗");
                    }
				}
				GlobalFree(pDetail);
			}
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return false;
}
DWORD TfrmMain::CheckVersionFile()
{

	bool bError = false;
	DWORD dwMsg = 0;
	if(FileExists("Result.txt"))
	{
		/*
		Return_Value = 0x0A110403
		*/
		Delay(500);//產生後檔案讀太快會讀到空白
		AnsiString strTemp = NULL;
		strTemp = Findfilemsg("Result.txt", "Return_Value",0);
		plPDVersion->Caption = strTemp.SubString(16,strTemp.Length()-15);
		if(!strTemp.Pos("0A110403"))
		{
			plPDVersion->Font->Color = clRed;
			return MSG_VERSION_FAIL_DATA;
			bError = true;
		}
		else
			plPDVersion->Font->Color = clBlue;
	}
	else
	{
		return MSG_VERSION_FILE_NO_FIND;
	}
	if(bError)
	{
		if(dwMsg  != MSG_VERSION_FAIL_CONN)
			return MSG_VERSION_FAIL_DATA;
		return MSG_VERSION_FAIL_CONN;
    }
	return MSG_VERSION_PASS;
}
AnsiString TfrmMain::Findfilemsg(AnsiString filename, AnsiString findmsg,
	int rownum) { // 找檔案找到字串行回傳幾行後的字串
	ifstream lanfile(filename.c_str());
	std::string filemsg;
	if (lanfile.is_open()) {
		while (!lanfile.eof()) {
			getline(lanfile, filemsg);
			if (strstr(filemsg.c_str(), findmsg.c_str())) {
				for (int i = 0; i < rownum; i++)
					getline(lanfile, filemsg);
				lanfile.close();
				return(AnsiString)filemsg.c_str();
			}
		}
		lanfile.close();
		return NULL;
	}
	else
		return NULL;
}
void TfrmMain::Delay(ULONG iMilliSeconds) // 原版delay time 用在thread裡面
{
	ULONG iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}
bool TfrmMain::GetProcessIdFromName(AnsiString processName)
{
	PROCESSENTRY32 pe;
	DWORD id = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if( !Process32First(hSnapshot,&pe) )
		return 0;
	char pname[300];
	do
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if( Process32Next(hSnapshot,&pe)==FALSE )
			break;
		//把WCHAR*型別轉換為const char*型別
		sprintf(pname,"%ws",pe.szExeFile);
		//比較兩個字串，如果找到了要找的程序
		if(strcmp(pe.szExeFile,processName.c_str()) == 0)
		{
			CloseHandle(hSnapshot);
			return true;
			break;
		}

	} while(1);

	CloseHandle(hSnapshot);

	return false;
}
void __fastcall TfrmMain::plbtnStartClick(TObject *Sender)
{
	if(plbtnStart->Caption.Pos("START"))
	{
		//plbtnStart->Color = clGray;
		plbtnStart->Caption = "STOP";
		frmMain->plResult->Color = clBtnHighlight;
		frmMain->plResult->Caption = "Waiting...";
		plPDVersion->Caption = "";
		lbTime->Caption = "";
		ckbUpdateFW->Enabled = false;
		ImgLogo->Enabled = false;
		frmMain->dwStep = STEP_CHECK_DEV_ONLINE;
		if(handleThread == NULL)
			handleThread = CreateThread(0, 1024, ThreadExecute, NULL, 0,NULL);
	}
	else
	{
		//plbtnStart->Color = clActiveCaption;
		plbtnStart->Caption = "START";
		ckbUpdateFW->Enabled = true;
		frmMain->dwStep = STEP_STOP;
		ImgLogo->Enabled = true;
		Delay(1000);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ckbUpdateFWClick(TObject *Sender)
{
	if(ckbUpdateFW->Caption.Pos("√"))
		ckbUpdateFW->Caption = "";
	else
		ckbUpdateFW->Caption = "√";
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(handleThread != NULL)
		CloseHandle(handleThread);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::TimerTestTimeTimer(TObject *Sender)
{
	if(dwTestTime > 0)
	{
		AnsiString astrSS;
		astrSS.printf("%.2f",float((GetTickCount()-dwTestTime))/1000);
		lbTime->Caption = astrSS+"(sec)";
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::popUpdateClick(TObject *Sender)
{
	if(popUpdate->Caption.Pos("開啟"))
	{
		popUpdate->Caption = "關閉強制更新";
		bAutoUpdate = true;
	}
	else
	{
		popUpdate->Caption = "開啟強制更新";
		bAutoUpdate = false;
    }
}
//---------------------------------------------------------------------------

