//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <pngimage.hpp>
#include <jpeg.hpp>
#include <Menus.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include<tlhelp32.h>
#include<comdef.h>

#define STEP_CHECK_DEV_ONLINE		1
#define STEP_CHECK_VERSION			2
#define STEP_READ_VERSION			3
#define STEP_UPDATE_VERSION			4
#define STEP_CHECK_UPDATE_VERSION 	5
#define STEP_READ_UPDATE_VERSION 	6
#define STEP_VERSION_ERROR			7
#define STEP_VERSION_UPDATE_SUCCESS	8
#define STEP_CHECK_DEV_OFFLINE 		9
#define STEP_STOP 					10

#define MSG_VERSION_FAIL_CONN 		1
#define MSG_VERSION_FAIL_DATA 		2
#define MSG_VERSION_PASS			3
#define MSG_VERSION_FILE_NO_FIND	4

const char GUID_USB_HUB[] = "{F18A0E88-C30C-11D0-8815-00A0C906BED8}";
const char GUID_USB_DEVICE[] = "{A5DCBF10-6530-11D2-901F-00C04FB951ED}";
const char GUID_USBSTOR[] = "{53F5630D-B6BF-11D0-94F2-00A0C91EFB8B}";
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TPanel *plMain;
	TPanel *plTitle;
	TPanel *Panel4;
	TPanel *Panel6;
	TPanel *plResult;
	TPanel *Panel11;
	TImage *ImgLogo;
	TPanel *plbtnStartBackG;
	TPanel *plbtnStart;
	TPanel *ckbUpdateFW;
	TPanel *Panel13;
	TTimer *TimerTestTime;
	TLabel *lbTime;
	TPopupMenu *popUpdateFW;
	TMenuItem *popUpdate;
	TPanel *Panel7;
	TPanel *plPDVersion;
	void __fastcall plbtnStartClick(TObject *Sender);
	void __fastcall ckbUpdateFWClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall TimerTestTimeTimer(TObject *Sender);
	void __fastcall popUpdateClick(TObject *Sender);
private:	// User declarations
    AnsiString Findfilemsg(AnsiString filename, AnsiString findmsg,
	int rownum);
public:		// User declarations
	//
	DWORD dwStep,dwTimeOut,dwMsg,dwTestTime;
	HANDLE handleThread;
	AnsiString astrErrorMsg;
	HWND hwTsk;
	AnsiString DosCommand(AnsiString sCmdline);
	//
	DWORD CheckVersionFile();
	void Delay(ULONG iMilliSeconds);
	bool GetProcessIdFromName(AnsiString processName);
	bool EnumUSB();
	bool bAutoUpdate;
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
extern DWORD WINAPI ThreadExecute(LPVOID Param);
//---------------------------------------------------------------------------
#endif
