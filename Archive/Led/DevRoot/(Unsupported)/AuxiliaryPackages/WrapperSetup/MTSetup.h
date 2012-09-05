// MTSetup.h

// OS Flags
const	dwWin95	=	0x1;
const	dwWin98	=	0x2;
const	dwWin98SE =	0x4;
const	dwWinME =	0x8;
const	dwWinNT	=	0x10;
const	dwWin2K	=	0x20;

// Reboot flags
const	dwDCOM = 0x100;
const	dwMDAC = 0x200;


static const TCHAR g_szAppName[]			= TEXT("Wrapper");
 
//static DWORD	g_dwOSVerFlags			= 0;
//static DWORD	g_dwInitRebootReason	= 0;				// Global variable used to store the
															// reason the system initiated
															// a reboot.
static BOOL		g_bDCOMReboot				= FALSE;		
static BOOL		g_bMDACReboot				= FALSE;

BOOL Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI InstallThread(PVOID pvParam);
DWORD GetOperatingSystemVer();
BOOL InstallDCOM();
BOOL IsDCOMPresent();
BOOL IsWindowsInstallerPresent();
BOOL IsMDACPresent();
BOOL InstallMDAC();
LPTSTR SetStartupPath(LPTSTR lpPath);
BOOL RegWriteCmdLine(LPSTR lpCmdLine);
BOOL InstallMSI();
BOOL RegReadCmdLine(LPSTR lpCmdLine, DWORD* pcbKeySize);
BOOL IsAdmin();
BOOL SetRunOnceKey(DWORD dwReason);
BOOL CreateRunOnceSetupKey();
BOOL CleanReg();
