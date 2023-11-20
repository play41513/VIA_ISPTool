=====================================================
=
=         VLI Hub PD ISP Tool Console Mode, Version 14.0xx.00         ==
=====================================================

Useage:
	Use: "Administrator: Command Prompt"
	Working Folder:	"HubPDConsol_Release_yyyymmdd"
	Display Info:	"HUBIspTool_Console.exe /I"	(Only Display)
	Install Driver:	"HUBIspTool_Console.exe /A"
	Update Firmware:	"HUBIspTool_Console.exe /U"
	Update Firmware:	"HUBIspTool_Console.exe /UD"
	Uninstall Driver:	"HUBIspTool_Console.exe /R"
	Get PD Version:	"HUBIspTool_Console.exe /F0"
	Get Hub1 Version:	"HUBIspTool_Console.exe /F1"
	Get Hub2 Version:	"HUBIspTool_Console.exe /F2"
	Get PD Bin File:	"HUBIspTool_Console.exe /B0"
	Get Hub1 Bin File:	"HUBIspTool_Console.exe /B1"
	Get Hub2 Bin File:	"HUBIspTool_Console.exe /B2"
	Set GPIOB High:	"HUBIspTool_Console.exe /GPIOBH"
	Set GPIOB Low:	"HUBIspTool_Console.exe /GPIOBL"
	Disable Codec	"HUBIspTool_Console.exe /T2OFF"
	Write SN:		"HUBIspTool_Console.exe /WRSN 123456ABCDEF"
	Check SN:	"HUBIspTool_Console.exe /CKSN 123456ABCDEF"

	Write BB iProduct:	"HUBIspTool_Console.exe /WRIPR 40AV"
	Check BB iProduct:	"HUBIspTool_Console.exe /CKIPR 40AV"
	Write BB iSN:	"HUBIspTool_Console.exe /WRISN 456789"
	Check BB iSN:	"HUBIspTool_Console.exe /CKISN 456789"
	Write BB iVersion:	"HUBIspTool_Console.exe /WRIVR 22181838"
	Check BB iVersion:	"HUBIspTool_Console.exe /CKIVR 22181838"
	Write BB BCD:	"HUBIspTool_Console.exe /WRBCD 6756"
	Check BB BCD:	"HUBIspTool_Console.exe /CKBCD 6756"
	Clear BB Info:	"HUBIspTool_Console.exe /CLBBI"

HubBinFile_Folder:
	HubBinFile Must Put Under Folder: \Bin\

PDBinFile_Folder:
	PDBinFile Must Put Under Folder: \VL10x_Bin\

Error_Code:
1	// Success (01)
0	// General_Fail (00)
-1	// FW_Mismatch (FF)
-2	// FW_Update_Fail (FE)
-3	// No_Hub (FD)
-4	// Hub_Mismatch (FC)
-5	// Ini_File_Error (FB)
-6	// No_Filter_Driver (FA)
-7	// Install_Driver_Fail (F9)
-8	// Uninstall_Driver_Fail (F8)
-9	// Tool_Path_With_&_Error (F7)
-10	// FW_Digital_Sign_Error (F6)
-11	// FW_BackUp_Error (F5)
-12  	// Invalid_Parameter (F4)
-13 	// Get_Version_Fail (F3)
-14	// Control_GPIO_Fail (F2)
-15	// Create_Process_Fail (F1 Delete)
-16	// KeyIn_SerialNumber_Error (F0)
-17	// Write_SerialNumber_Error (EF)
-18	// Check_SerialNumber_Error (EE)
-19	// Get_SPI_SR_Fail (ED)
-20	// Create_Process_Fail (EC)
-21	// Access_Register_Fail (EB)
-22	// Write_Info_Fail (EA)
-23 	// Check_info_Mismatch (E9)

-30	// Unknown_Fail (E2)