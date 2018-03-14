///***************************************************************************
// File:        GENERIC.C
//
// Purpose:     Template for creating XLLs for Microsoft Excel.
//
//              This file contains sample code you can use as 
//              a template for writing your own Microsoft Excel XLLs. 
//              An XLL is a DLL that stands alone, that is, you
//              can open it by choosing the Open command from the
//              File menu. This code demonstrates many of the features 
//              of the Microsoft Excel C API.
//              
//              When you open GENERIC.XLL, it
//              creates a new Generic menu with four
//              commands:
//              
//                  Dialog...          displays a Microsoft Excel dialog box
//                  Dance              moves the selection around
//                                     until you press ESC
//                  Native Dialog...   displays a Windows dialog box
//                  Exit               Closes GENERIC.XLL and
//                                     removes the menu
//              
//              GENERIC.XLL also provides three functions,
//              Func1, FuncSum and FuncFib, which can be used whenever
//              GENERIC.XLL is open. 
//              
//              GENERIC.XLL can also be added with the
//              Add-in Manager.
//              
//              This file uses the framework library
//              (FRMWRK32.LIB).
// 
// Platform:    Microsoft Windows
//
// Functions:		
//              DllMain
//              xlAutoOpen
//              xlAutoClose
//              lpstricmp
//              xlAutoRegister12
//              xlAutoAdd
//              xlAutoRemove
//              xlAddInManagerInfo12
//              DIALOGMsgProc
//              ExcelCursorProc
//              HookExcelWindow
//              UnhookExcelWindow
//              fShowDialog
//              GetHwnd
//              Func1
//              FuncSum
//              fDance
//              fDialog
//              fExit
//              FuncFib
//
///***************************************************************************

//
// Includes
//
#include <windows.h>
#include <xlcall.h>
#include <framewrk.h>
#include <generic.h>

//
// Later, the instance handle is required to create dialog boxes.
// g_hInst holds the instance handle passed in by DllMain so that it is
// available for later use. hWndMain is used in several routines to
// store Microsoft Excel's hWnd. This is used to attach dialog boxes as
// children of Microsoft Excel's main window. A buffer is used to store
// the free space that DIALOGMsgProc will put into the dialog box.
//
//
// Global Variables
//
HWND g_hWndMain = NULL;
HANDLE g_hInst = NULL;
XCHAR g_szBuffer[20] = L"";

//
// Syntax of the Register Command:
//      REGISTER(module_text, procedure, type_text, function_text, 
//               argument_text, macro_type, category, shortcut_text,
//               help_topic, function_help, argument_help1, argument_help2,...)
//
//
// g_rgWorksheetFuncs will use only the first 11 arguments of 
// the Register function.
//
// This is a table of all the worksheet functions exported by this module.
// These functions are all registered (in xlAutoOpen) when you
// open the XLL. Before every string, leave a space for the
// byte count. The format of this table is the same as 
// arguments two through eleven of the REGISTER function.
// g_rgWorksheetFuncsRows define the number of rows in the table. The
// g_rgWorksheetFuncsCols represents the number of columns in the table.
//
#define g_rgWorksheetFuncsRows 3
#define g_rgWorksheetFuncsCols 10

static LPCWSTR g_rgWorksheetFuncs
[g_rgWorksheetFuncsRows][g_rgWorksheetFuncsCols] =
{
	{ L"Func1",                                     // Procedure
		L"UU",                                  // type_text
		L"Func1",                               // function_text
		L"Arg",                                 // argument_text
		L"1",                                   // macro_type
		L"Generic Add-In",                      // category
		L"",                                    // shortcut_text
		L"",                                    // help_topic
		L"Always returns the string 'Func1'",   // function_help
		L"Argument ignored"                     // argument_help1
	},
	{ L"FuncSum",
		L"UUUUUUUUUUUUUUUUUUUUUUUUUUUUUU", // up to 255 args in Excel 2007 and later,
										   // upto 29 args in Excel 2003 and earlier versions
		L"FuncSum",
		L"number1,number2,...",
		L"1",
		L"Generic Add-In",
		L"",                                    
		L"",                                  
		L"Adds the arguments",   
		L"Number1,number2,... are 1 to 29 arguments for which you want to sum."                   
	},
	{ L"FuncFib",
		L"UU",	
		L"FuncFib",
		L"Compute to...",
		L"1",
		L"Generic Add-In",
		L"",
		L"",
		L"Number to compute to"
		L"Computes the nth fibonacci number",
	},
};

//
// g_rgCommandFuncs
//
// This is a table of all the command functions exported by this module.
// These functions are all registered (in xlAutoOpen) when you
// open the XLL. Before every string, leave a space for the
// byte count. The format of this table is the same as 
// arguments two through eight of the REGISTER function.
// g_rgFuncsRows define the number of rows in the table. The
// g_rgCommandFuncsCols represents the number of columns in the table.
//
#define g_rgCommandFuncsRows 4
#define g_rgCommandFuncsCols 7

static LPCWSTR g_rgCommandFuncs[g_rgCommandFuncsRows][g_rgCommandFuncsCols] =
{
	{ L"fDialog",                   // Procedure
		L"A",                   // type_text
		L"fDialog",             // function_text
		L"",                    // argument_text
		L"2",                   // macro_type
		L"Generic Add-In",      // category
		L"l"                    // shortcut_text
	},
	{ L"fDance",
		L"A",
		L"fDance",
		L"",
		L"2",
		L"Generic Add-In",
		L"m"
	},
	{ L"fShowDialog",
		L"A",
		L"fShowDialog",
		L"",
		L"2",
		L"Generic Add-In",
		L"n"},
	{ L"fExit",
		L"A",
		L"fExit",
		L"",
		L"2",
		L"Generic Add-In",
		L"o"
	},
};

//
// g_rgMenu
//
// This is a table describing the Generic drop-down menu. It is in
// the same format as the Microsoft Excel macro language menu tables.
// The first column contains the name of the menu or command, the
// second column contains the function to be executed, the third
// column contains the (Macintosh only) shortcut key, the fourth
// column contains the help text for the status bar, and
// the fifth column contains the help text index. Leave a space
// before every string so the byte count can be inserted. g_rgMenuRows
// defines the number of menu items. 5 represents the number of
// columns in the table.
//

#define g_rgMenuRows 5
#define g_rgMenuCols 5

static LPCWSTR g_rgMenu[g_rgMenuRows][g_rgMenuCols] =
{
	{L"&Generic",          L"",            L"",
		L"The Generic XLL Add-In",         	L""},
	{L"&Dialog...",        L"fDialog",     L"",
		L"Run a sample generic dialog",    	L""},
	{L"D&ance",            L"fDance",      L"",
		L"Make the selection dance around",	L""},
	{L"&Native Dialog...", L"fShowDialog", L"",
		L"Run a sample native dialog",     	L""},
	{L"E&xit",             L"fExit",       L"",
		L"Close the Generic XLL",          	L""},
};

//
// g_rgTool
//
// This is a table describing the toolbar. It is in the same format
// as the Microsoft Excel macro language toolbar tables. The first column
// contains the ID of the tool, the second column contains the function
// to be executed, the third column contains a logical value specifying
// the default image of the tool, the fourth column contains a logical
// value specifying whether the tool can be used, the fifth column contains
// a face for the tool, the sixth column contains the help_text that
// is displayed in the status bar, the seventh column contains the Balloon
// text (Macintosh Only), and the eighth column contains the help topics
// as quoted text. Leave a space before every string so the byte count
// can be inserted. g_rgToolRows defines the number of tools on the toolbar.
// 8 represents the number of columns in the table.
//

#define g_rgToolRows 3
#define g_rgToolCols 8

static LPCWSTR g_rgTool[g_rgToolRows][g_rgToolCols] =
{
	{L"211", L"fDance", L"FALSE", L"TRUE", L"", L"Dance the Selection", L"", L""},
	{L"0",   L"",       L"",      L"",     L"", L"",                    L"", L""},
	{L"212", L"fExit",  L"FALSE", L"TRUE", L"", L"Exit this example",   L"", L""},
};


//
// g_rgDialog
//
// This is a table describing the sample dialog box used in the fDialog()
// function. Admittedly, it would be more efficient to use ints for
// the first 5 columns, but that makes the code much more complicated.
// Storing the text in string tables is another method that could be used.
// Each string is byte counted, but you can also use normal strings and 
// copy them over into allocated memory with byte countes appended to the front.
// Alternatively, you can call TempStr12 on a normal string and use the
// string the call allocates to pass into the Excel functions.
// g_rgDialogRows determines the number of rows in the dialog box.
// 7 represents the number of columns in the table.
//

#define g_rgDialogRows 16
#define g_rgDialogCols 7

static LPCWSTR g_rgDialog[g_rgDialogRows][g_rgDialogCols] =
{
	{L"\000",   L"\000",    L"\000",    L"\003494", L"\003210", L"\025Generic Sample Dialog", L"\000"},
	{L"\0011",  L"\003330", L"\003174", L"\00288",  L"\000",    L"\002OK",                    L"\000"},
	{L"\0012",  L"\003225", L"\003174", L"\00288",  L"\000",    L"\006Cancel",                L"\000"},
	{L"\0015",  L"\00219",  L"\00211",  L"\000",    L"\000",    L"\006&Name:",                L"\000"},
	{L"\0016",  L"\00219",  L"\00229",  L"\003251", L"\000",    L"\000",                      L"\000"},
	{L"\00214", L"\003305", L"\00215",  L"\003154", L"\00273",  L"\010&College",              L"\000"},
	{L"\00211", L"\000",    L"\000",    L"\000",    L"\000",    L"\000",                      L"\0011"},
	{L"\00212", L"\000",    L"\000",    L"\000",    L"\000",    L"\010&Harvard",              L"\0011"},
	{L"\00212", L"\000",    L"\000",    L"\000",    L"\000",    L"\006&Other",                L"\000"},
	{L"\0015",  L"\00219",  L"\00250",  L"\000",    L"\000",    L"\013&Reference:",           L"\000"},
	{L"\00210", L"\00219",  L"\00267",  L"\003253", L"\000",    L"\000",                      L"\000"},
	{L"\00214", L"\003209", L"\00293",  L"\003250", L"\00263",  L"\017&Qualifications",       L"\000"},
	{L"\00213", L"\000",    L"\000",    L"\000",    L"\000",    L"\010&BA / BS",              L"\0011"},
	{L"\00213", L"\000",    L"\000",    L"\000",    L"\000",    L"\010&MA / MS",              L"\0011"},
	{L"\00213", L"\000",    L"\000",    L"\000",    L"\000",    L"\021&PhD / Other Grad",     L"\0010"},
	{L"\00215", L"\00219",  L"\00299",  L"\003160", L"\00296",  L"\015GENERIC_List1",         L"\0011"},
};

///***************************************************************************
// DllMain()
//
// Purpose:
//
//      Windows calls DllMain, for both initialization and termination.
//		It also makes calls on both a per-process and per-thread basis,
//		so several initialization calls can be made if a process is multithreaded.
//
//      This function is called when the DLL is first loaded, with a dwReason
//      of DLL_PROCESS_ATTACH.
//
// Parameters:
//
//      HANDLE hDLL         Module handle.
//      DWORD dwReason,     Reason for call
//      LPVOID lpReserved   Reserved
//
// Returns: 
//      The function returns TRUE (1) to indicate success. If, during
//      per-process initialization, the function returns zero, 
//      the system cancels the process.
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************

BOOL APIENTRY DllMain( HANDLE hDLL, 
					   DWORD dwReason, 
					   LPVOID lpReserved )
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:

		// The instance handle passed into DllMain is saved
		// in the global variable g_hInst for later use.

		g_hInst = hDLL;
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}
	return TRUE;
}


///***************************************************************************
// xlAutoOpen()
//
// Purpose: 
//      Microsoft Excel call this function when the DLL is loaded.
//
//      Microsoft Excel uses xlAutoOpen to load XLL files.
//      When you open an XLL file, the only action
//      Microsoft Excel takes is to call the xlAutoOpen function.
//
//      More specifically, xlAutoOpen is called:
//
//       - when you open this XLL file from the File menu,
//       - when this XLL is in the XLSTART directory, and is
//         automatically opened when Microsoft Excel starts,
//       - when Microsoft Excel opens this XLL for any other reason, or
//       - when a macro calls REGISTER(), with only one argument, which is the
//         name of this XLL.
//
//      xlAutoOpen is also called by the Add-in Manager when you add this XLL 
//      as an add-in. The Add-in Manager first calls xlAutoAdd, then calls
//      REGISTER("EXAMPLE.XLL"), which in turn calls xlAutoOpen.
//
//      xlAutoOpen should:
//
//       - register all the functions you want to make available while this
//         XLL is open,
//
//       - add any menus or menu items that this XLL supports,
//
//       - perform any other initialization you need, and
//
//       - return 1 if successful, or return 0 if your XLL cannot be opened.
//
// Parameters:
//
// Returns: 
//
//      int         1 on success, 0 on failure
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) int WINAPI xlAutoOpen(void)
{

	static XLOPER12 xDLL,	   // name of this DLL //
	xMenu,	 // xltypeMulti containing the menu //
	xTool,	 // xltypeMulti containing the toolbar //
	xTest;	 // used for menu test //
	LPXLOPER12 pxMenu;	   // Points to first menu item //
	LPXLOPER12 px;		   // Points to the current item //
	LPXLOPER12 pxTool;	   // Points to first toolbar item //
	int i, j;			   // Loop indices //
	HANDLE   hMenu;		   // global memory holding menu //
	HANDLE  hTool;		   // global memory holding toolbar //

	//
	// In the following block of code the name of the XLL is obtained by
	// calling xlGetName. This name is used as the first argument to the
	// REGISTER function to specify the name of the XLL. Next, the XLL loops
	// through the g_rgWorksheetFuncs[] table, and the g_rgCommandFuncs[]
	// tableregistering each function in the table using xlfRegister. 
	// Functions must be registered before you can add a menu item.
	//
	
	Excel12f(xlGetName, &xDLL, 0);

	for (i=0; i<g_rgWorksheetFuncsRows; i++)
	{
		Excel12f(xlfRegister, 0, 1+ g_rgWorksheetFuncsCols,
			  (LPXLOPER12) &xDLL,
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][0]),
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][1]),
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][2]),
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][3]),
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][4]),
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][5]),
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][6]),
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][7]),
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][8]),
			  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][9]));
	}

	for (i=0; i<g_rgCommandFuncsRows; i++)
	{
		Excel12f(xlfRegister, 0, 1+ g_rgCommandFuncsCols,
			  (LPXLOPER12) &xDLL,
			  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][0]),
			  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][1]),
			  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][2]),
			  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][3]),
			  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][4]),
			  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][5]),
			  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][6]));
	}

	//
	// In the following block of code, the Generic drop-down menu is created.
	// Before creation, a check is made to determine if Generic already
	// exists. If not, it is added. If the menu needs to be added, memory is
	// allocated to hold the array of menu items. The g_rgMenu[] table is then
	// transferred into the newly created array. The array is passed as an
	// argument to xlfAddMenu to actually add the drop-down menu before the
	// help menu. As a last step the memory allocated for the array is
	// released.
	//
	// This block uses TempStr12() and TempNum12(). Both create a temporary
	// XLOPER12. The XLOPER12 created by TempStr12() contains the string passed to
	// it. The XLOPER12 created by TempNum12() contains the number passed to it.
	// The Excel12f() function frees the allocated temporary memory. Both
	// functions are part of the framework library.
	//

	Excel12f(xlfGetBar, &xTest, 3, TempInt12(10), TempStr12(L"Generic"), TempInt12(0));

	if (xTest.xltype == xltypeErr)
	{
		hMenu = GlobalAlloc(GMEM_MOVEABLE,sizeof(XLOPER12) * g_rgMenuCols * g_rgMenuRows);
		px = pxMenu = (LPXLOPER12) GlobalLock(hMenu);

		for (i=0; i < g_rgMenuRows; i++)
		{
			for (j=0; j < g_rgMenuCols; j++)
			{
				px->xltype = xltypeStr;
				px->val.str = TempStr12(g_rgMenu[i][j])->val.str;
				px++;
			}
		}

		xMenu.xltype = xltypeMulti;
		xMenu.val.array.lparray = pxMenu;
		xMenu.val.array.rows = g_rgMenuRows;
		xMenu.val.array.columns = g_rgMenuCols;

		Excel12f(xlfAddMenu,0,3,TempNum12(10),(LPXLOPER12)&xMenu,TempStr12(L"Help"));

		GlobalUnlock(hMenu);
		GlobalFree(hMenu);
	}

	//
	// In this block of code, the Test toolbar is created. Before
	// creation, a check is made to ensure that Test doesn't already
	// exist. If it does not, it is created. Memory is allocated to hold
	// the array containing the toolbar. The information from the g_rgTool[]
	// table is then transferred into this array. The toolbar is added with
	// xlfAddToolbar and subsequently displayed with xlcShowToolbar. Finally,
	// the memory allocated for the toolbar and the XLL filename is released.
	//
	// This block uses TempInt12(), TempBool12(), and TempMissing12(). All three
	// create a temporary XLOPER12. The XLOPER12 created by TempInt() contains
	// the integer passed to it. TempBool12() creates an XLOPER12 containing the
	// boolean value passed to it. TempMissing12() creates an XLOPER12 that
	// simulates a missing argument. The Excel12f() function frees the temporary
	// memory associated with these functions. All three are part of the
	// framework library.
	//

	Excel12f(xlfGetToolbar, &xTest, 2, TempInt12(1), TempStr12(L"Test"));

	if (xTest.xltype == xltypeErr)
	{
		hTool = GlobalAlloc(GMEM_MOVEABLE, sizeof(XLOPER12) * g_rgToolCols * g_rgToolRows);
		px = pxTool = (LPXLOPER12) GlobalLock(hTool);

		for (i = 0; i < g_rgToolRows; i++)
		{
			for (j = 0; j < g_rgToolCols; j++)
			{
				px->xltype = xltypeStr;
				px->val.str = const_cast< XCHAR* >(g_rgTool[i][j]);
				px++;
			}
		}

		xTool.xltype = xltypeMulti;
		xTool.val.array.lparray = pxTool;
		xTool.val.array.rows = g_rgToolRows;
		xTool.val.array.columns = g_rgToolCols;

		Excel12f(xlfAddToolbar,0,2,TempStr12(L"Test"),(LPXLOPER)&xTool);

		Excel12f(xlcShowToolbar, 0, 6, TempStr12(L"Test"), TempBool12(1),
			  TempInt12(5), TempMissing12(), TempMissing12(), TempInt12(999));

		GlobalUnlock(hTool);
		GlobalFree(hTool);
	}

	// Free the XLL filename //
	Excel12f(xlFree, 0, 2, (LPXLOPER12) &xTest, (LPXLOPER12) &xDLL);

	return 1;
}


///***************************************************************************
// xlAutoClose()
//
// Purpose: Microsoft Excel call this function when the DLL is unloaded.
//
//      xlAutoClose is called by Microsoft Excel:
//
//       - when you quit Microsoft Excel, or 
//       - when a macro sheet calls UNREGISTER(), giving a string argument
//         which is the name of this XLL.
//
//      xlAutoClose is called by the Add-in Manager when you remove this XLL from
//      the list of loaded add-ins. The Add-in Manager first calls xlAutoRemove,
//      then calls UNREGISTER("GENERIC.XLL"), which in turn calls xlAutoClose.
// 
//      xlAutoClose is called by GENERIC.XLL by the function fExit. This function
//      is called when you exit Generic.
// 
//      xlAutoClose should:
// 
//       - Remove any menus or menu items that were added in xlAutoOpen,
// 
//       - do any necessary global cleanup, and
// 
//       - delete any names that were added (names of exported functions, and 
//         so on). Remember that registering functions may cause names to 
//         be created.
// 
//      xlAutoClose does NOT have to unregister the functions that were registered
//      in xlAutoOpen. This is done automatically by Microsoft Excel after
//      xlAutoClose returns.
// 
//      xlAutoClose should return 1.
//
// Parameters:
//
// Returns: 
//
//      int         1
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) int WINAPI xlAutoClose(void)
{
	int i;
	XLOPER12 xRes;

	//
	// This block first deletes all names added by xlAutoOpen or
	// xlAutoRegister12. Next, it checks if the drop-down menu Generic still
	// exists. If it does, it is deleted using xlfDeleteMenu. It then checks
	// if the Test toolbar still exists. If it is, xlfDeleteToolbar is
	// used to delete it.
	//

	//
	// Due to a bug in Excel the following code to delete the defined names
	// does not work.  There is no way to delete these
	// names once they are Registered
	// The code is left in, in hopes that it will be
	// fixed in a future version.
	//

	for (i = 0; i < g_rgWorksheetFuncsRows; i++)
		Excel12f(xlfSetName, 0, 1, TempStr12(g_rgWorksheetFuncs[i][2]));

	for (i = 0; i < g_rgCommandFuncsRows; i++)
		Excel12f(xlfSetName, 0, 1, TempStr12(g_rgCommandFuncs[i][2]));
	//
	// Everything else works as documented
	//
	Excel12f(xlfGetBar, &xRes, 3, TempInt12(10), TempStr12(L"Generic"), TempInt12(0));

	if (xRes.xltype != xltypeErr)
	{
		Excel12f(xlfDeleteMenu, 0, 2, TempNum12(10), TempStr12(L"Generic"));

		// Free the XLOPER12 returned by xlfGetBar //
		Excel12f(xlFree, 0, 1, (LPXLOPER12) &xRes);
	}

	Excel12f(xlfGetToolbar, &xRes, 2, TempInt12(7), TempStr12(L"Test"));

	if (xRes.xltype != xltypeErr)
	{
		Excel12f(xlfDeleteToolbar, 0, 1, TempStr12(L"Test"));

		// Free the XLOPER12 returned by xlfGetToolbar //
		Excel12f(xlFree, 0, 1, (LPXLOPER12) &xRes);
	}

	return 1;
}


///***************************************************************************
// lpwstricmp()
//
// Purpose: 
//
//      Compares a pascal string and a null-terminated C-string to see
//      if they are equal.  Method is case insensitive
//
// Parameters:
//
//      LPWSTR s    First string (null-terminated)
//      LPWSTR t    Second string (byte counted)
//
// Returns: 
//
//      int         0 if they are equal
//                  Nonzero otherwise
//
// Comments:
//
//      Unlike the usual string functions, lpwstricmp
//      doesn't care about collating sequence.
//
// History:  Date       Author        Reason
///***************************************************************************

int lpwstricmp(LPWSTR s, LPWSTR t)
{
	int i;

	if (wcslen(s) != *t)
		return 1;

	for (i = 1; i <= s[0]; i++)
	{
		if (towlower(s[i-1]) != towlower(t[i]))
			return 1;
	}										  
	return 0;
}


///***************************************************************************
// xlAutoRegister12()
//
// Purpose:
//
//      This function is called by Microsoft Excel if a macro sheet tries to
//      register a function without specifying the type_text argument. If that
//      happens, Microsoft Excel calls xlAutoRegister12, passing the name of the
//      function that the user tried to register. xlAutoRegister12 should use the
//      normal REGISTER function to register the function, only this time it must
//      specify the type_text argument. If xlAutoRegister12 does not recognize the
//      function name, it should return a #VALUE! error. Otherwise, it should
//      return whatever REGISTER returned.
//
// Parameters:
//
//      LPXLOPER12 pxName   xltypeStr containing the
//                          name of the function
//                          to be registered. This is not
//                          case sensitive.
//
// Returns: 
//
//      LPXLOPER12          xltypeNum containing the result
//                          of registering the function,
//                          or xltypeErr containing #VALUE!
//                          if the function could not be
//                          registered.
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) LPXLOPER12 WINAPI xlAutoRegister12(LPXLOPER12 pxName)
{
	static XLOPER12 xDLL, xRegId;
	int i;

	//
	// This block initializes xRegId to a #VALUE! error first. This is done in
	// case a function is not found to register. Next, the code loops through 
	// the functions in g_rgFuncs[] and uses lpwstricmp to determine if the 
	// current row in g_rgFuncs[] represents the function that needs to be 
	// registered. When it finds the proper row, the function is registered 
	// and the register ID is returned to Microsoft Excel. If no matching 
	// function is found, an xRegId is returned containing a #VALUE! error.
	//

	xRegId.xltype = xltypeErr;
	xRegId.val.err = xlerrValue;


	for (i=0; i<g_rgWorksheetFuncsRows; i++)
	{
		if (!lpwstricmp(const_cast< LPWSTR >(g_rgWorksheetFuncs[i][0]), pxName->val.str))
		{
			Excel12f(xlfRegister, 0, 1+ g_rgWorksheetFuncsCols,
				  (LPXLOPER12) &xDLL,
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][0]),
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][1]),
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][2]),
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][3]),
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][4]),
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][5]),
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][6]),
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][7]),
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][8]),
				  (LPXLOPER12) TempStr12(g_rgWorksheetFuncs[i][9]));
			/// Free oper returned by xl //
			Excel12f(xlFree, 0, 1, (LPXLOPER12) &xDLL);

			return(LPXLOPER12) &xRegId;
		}
	}

	for (i=0; i<g_rgCommandFuncsRows; i++)
	{
		if (!lpwstricmp(const_cast< LPWSTR >(g_rgCommandFuncs[i][0]), pxName->val.str))
		{
			Excel12f(xlfRegister, 0, 1+ g_rgCommandFuncsCols,
				  (LPXLOPER12) &xDLL,
				  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][0]),
				  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][1]),
				  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][2]),
				  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][3]),
				  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][4]),
				  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][5]),
				  (LPXLOPER12) TempStr12(g_rgCommandFuncs[i][6]));
			/// Free oper returned by xl //
			Excel12f(xlFree, 0, 1, (LPXLOPER12) &xDLL);

			return(LPXLOPER12) &xRegId;
		}
	}     
	return(LPXLOPER12) &xRegId;
}

///***************************************************************************
// xlAutoAdd()
//
// Purpose:
//
//      This function is called by the Add-in Manager only. When you add a
//      DLL to the list of active add-ins, the Add-in Manager calls xlAutoAdd()
//      and then opens the XLL, which in turn calls xlAutoOpen.
//
// Parameters:
//
// Returns: 
//
//      int         1
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) int WINAPI xlAutoAdd(void)
{
	XCHAR szBuf[255];

	wsprintfW((LPWSTR)szBuf, L"Thank you for adding GENERIC.XLL\n "
			 L"built on %hs at %hs", __DATE__, __TIME__);

	// Display a dialog box indicating that the XLL was successfully added //
	Excel12f(xlcAlert, 0, 2, TempStr12(szBuf), TempInt12(2));
	return 1;
}

///***************************************************************************
// xlAutoRemove()
//
// Purpose:
//
//      This function is called by the Add-in Manager only. When you remove
//      an XLL from the list of active add-ins, the Add-in Manager calls
//      xlAutoRemove() and then UNREGISTER("GENERIC.XLL").
//   
//      You can use this function to perform any special tasks that need to be
//      performed when you remove the XLL from the Add-in Manager's list
//      of active add-ins. For example, you may want to delete an
//      initialization file when the XLL is removed from the list.
//
// Parameters:
//
// Returns: 
//
//      int         1
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) int WINAPI xlAutoRemove(void)
{
	// Show a dialog box indicating that the XLL was successfully removed //
	Excel12f(xlcAlert, 0, 2, TempStr12(L"Thank you for removing GENERIC.XLL!"),
		  TempInt12(2));
	return 1;
}

///***************************************************************************
// xlAddInManagerInfo12()
//
// Purpose:
//
//      This function is called by the Add-in Manager to find the long name
//      of the add-in. If xAction = 1, this function should return a string
//      containing the long name of this XLL, which the Add-in Manager will use
//      to describe this XLL. If xAction = 2 or 3, this function should return
//      #VALUE!.
//
// Parameters:
//
//      LPXLOPER12 xAction  What information you want. One of:
//                            1 = the long name of the
//                                add-in
//                            2 = reserved
//                            3 = reserved
//
// Returns: 
//
//      LPXLOPER12          The long name or #VALUE!.
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) LPXLOPER12 WINAPI xlAddInManagerInfo12(LPXLOPER12 xAction)
{
	static XLOPER12 xInfo, xIntAction;

	//
	// This code coerces the passed-in value to an integer. This is how the
	// code determines what is being requested. If it receives a 1, 
	// it returns a string representing the long name. If it receives 
	// anything else, it returns a #VALUE! error.
	//

	Excel12f(xlCoerce, &xIntAction, 2, xAction, TempInt12(xltypeInt));

	if (xIntAction.val.w == 1)
	{
		xInfo.xltype = xltypeStr;
		xInfo.val.str = const_cast< LPWSTR >(L"\022Generic Standalone DLL");
	}
	else
	{
		xInfo.xltype = xltypeErr;
		xInfo.val.err = xlerrValue;
	}

	//Word of caution - returning static XLOPERs/XLOPER12s is not thread safe
	//for UDFs declared as thread safe, use alternate memory allocation mechanisms
	return(LPXLOPER12) &xInfo;
}

///***************************************************************************
// DIALOGMsgProc()
//
// Purpose:
//
//     This procedure is associated with the native Windows dialog box that
//     fShowDialog displays. It provides the service routines for the events
//     (messages) that occur when the user operates one of the dialog
//     box's buttons, entry fields, or controls.
//
// Parameters:
//
//      HWND hWndDlg        Contains the HWND of the dialog box
//      UINT message        The message to respond to
//      WPARAM wParam       Arguments passed by Windows
//      LPARAM lParam
//
// Returns: 
//
//      INT_PTR                TRUE if message processed, FALSE if not.
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************

INT_PTR CALLBACK DIALOGMsgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	//
	// This block is a very simple message loop for a dialog box. It checks for
	// only three messages. When it receives WM_INITDIALOG, it uses a buffer to
	// set a static text item to the amount of free space on the stack. When it
	// receives WM_CLOSE it posts a CANCEL message to the dialog box. When it
	// receives WM_COMMAND it checks if the OK button was pressed. If it was,
	// the dialog box is closed and control returned to fShowDialog.
	//

	switch (message)
	{
	
	case WM_INITDIALOG:
		SetDlgItemText(hWndDlg, FREE_SPACE, g_szBuffer);
		break;

	case WM_CLOSE:
		PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			EndDialog(hWndDlg, FALSE);
			break;
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

///***************************************************************************
// ExcelCursorProc()
//
// Purpose:
//
//      When a modal dialog box is displayed over Microsoft Excel's window, the
//      cursor is a busy cursor over Microsoft Excel's window. This WndProc traps
//      WM_SETCURSORs and changes the cursor back to a normal arrow.
//
// Parameters:
//
//      HWND hWndDlg        Contains the HWND Window
//      UINT message        The message to respond to
//      WPARAM wParam       Arguments passed by Windows
//      LPARAM lParam
//
// Returns: 
//
//      LRESULT             0 if message handled, otherwise the result of the
//                          default WndProc
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************

// Create a place to store Microsoft Excel's WndProc address //
static WNDPROC g_lpfnExcelWndProc = NULL;

LRESULT CALLBACK ExcelCursorProc(HWND hwnd, 
                                 UINT wMsg, 
                                 WPARAM wParam, 
                                 LPARAM lParam)
{
	//
	// This block checks to see if the message that was passed in is a
	// WM_SETCURSOR message. If so, the cursor is set to an arrow; if not,
	// the default WndProc is called.
	//

	if (wMsg == WM_SETCURSOR)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return 0L;
	}
	else
	{
		return CallWindowProc(g_lpfnExcelWndProc, hwnd, wMsg, wParam, lParam);
	}
}

///***************************************************************************
// HookExcelWindow()
//
// Purpose:
//
//     This is the function that installs ExcelCursorProc so that it is
//     called before Microsoft Excel's main WndProc.
//
// Parameters:
//
//      HANDLE hWndExcel    This is a handle to Microsoft Excel's hWnd
//
// Returns: 
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************

extern void FAR PASCAL HookExcelWindow(HANDLE hWndExcel)
{
	//
	// This block obtains the address of Microsoft Excel's WndProc through the
	// use of GetWindowLongPtr(). It stores this value in a global that can be
	// used to call the default WndProc and also to restore it. Finally, it
	// replaces this address with the address of ExcelCursorProc using
	// SetWindowLongPtr().
	//

	g_lpfnExcelWndProc = (WNDPROC) GetWindowLongPtr(static_cast< HWND >(hWndExcel), GWLP_WNDPROC);
	SetWindowLongPtr(static_cast< HWND >(hWndExcel), GWLP_WNDPROC, (LONG_PTR)(FARPROC) ExcelCursorProc);
}

///***************************************************************************
// UnhookExcelWindow()
//
// Purpose:
//
//      This is the function that removes the ExcelCursorProc that was
//      called before Microsoft Excel's main WndProc.
//
// Parameters:
//
//      HANDLE hWndExcel    This is a handle to Microsoft Excel's hWnd
//
// Returns: 
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************

extern void FAR PASCAL UnhookExcelWindow(HANDLE hWndExcel)
{
	//
	// This function restores Microsoft Excel's default WndProc using
	// SetWindowLongPtr to restore the address that was saved into
	// g_lpfnExcelWndProc by HookExcelWindow(). It then sets g_lpfnExcelWndProc
	// to NULL.
	//

	SetWindowLongPtr(static_cast< HWND >(hWndExcel), GWLP_WNDPROC, (LONG_PTR) g_lpfnExcelWndProc);
	g_lpfnExcelWndProc = NULL;
}

///***************************************************************************
// fShowDialog()
//
// Purpose:
//      This function loads and shows the native Windows dialog box.
//
// Parameters:
//
// Returns: 
//
//      int 0           To indicate successful completion
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) int WINAPI fShowDialog(void)
{
	static  XLOPER12 xRes, xStr;
	int     i;
	INT_PTR nRc = 0;				 // return code //

	//
	// This block first gets the amount of space left on the stack, coerces it
	// to a string, and then copies it into a buffer. It obtains this
	// information so that it can be displayed in the dialog box. The coerced
	// string is then released with xlFree. The buffer is then converted into
	// a null-terminated string. The hWnd of Microsoft Excel is then obtained
	// using GetHwnd. Messages are enabled using xlEnableXLMsgs in
	// preparation for showing the dialog box. hWndMain is passed to
	// HookExcelWindow so that an arrow cursor is displayed over Microsoft
	// Excel's window. The dialog box is then displayed. The hWnd is then passed
	// to UnhookExcelWindow(). Messages are then disabled using xlDisableXLMsgs.

	Excel12f(xlStack, (LPXLOPER12) &xRes, 0);

	Excel12f(xlCoerce, (LPXLOPER12) &xStr, 2, (LPXLOPER12) &xRes, (LPXLOPER12) TempInt12(xltypeStr));

	lstrcpynW(g_szBuffer, xStr.val.str, xStr.val.str[0]);

	Excel12f(xlFree, 0, 1, (LPXLOPER12) &xStr);

	nRc = g_szBuffer[0];
	for (i = 0; i <= nRc; i++)
		g_szBuffer[i] = g_szBuffer[i+1];
	g_szBuffer[nRc] = '\0';

	GetHwnd(&g_hWndMain);

	Excel12f(xlEnableXLMsgs, 0, 0);
	HookExcelWindow(g_hWndMain);

	nRc = DialogBox(static_cast< HINSTANCE >(g_hInst), L"TESTSTACK", g_hWndMain, DIALOGMsgProc);

	UnhookExcelWindow(g_hWndMain);
	Excel12f(xlDisableXLMsgs, 0, 0);

	return(0);
}



///***************************************************************************
// GetHwnd()
//
// Purpose:
//
//      This function returns the hWnd of Excel's main window. 
//
// Parameters:
//
//      HWND * phwnd    Will contain Excel's hWnd
//
// Returns: 
//
//      BOOL            FALSE  Could not find Excel's hWnd
//                      TRUE   Found Excel's hWnd
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
BOOL GetHwnd(HWND * pHwnd)
{
	XLOPER12 x;

	if (Excel12f(xlGetHwnd, &x, 0) == xlretSuccess)
	{
		*pHwnd = reinterpret_cast< HWND >(x.val.w);
		return TRUE;
	}
	return FALSE;
}




///***************************************************************************
// Func1()
//
// Purpose:
//
//      This is a typical user-defined function provided by an XLL.
//
// Parameters:
//
//      LPXLOPER12 x    (Ignored)
//
// Returns: 
//
//      LPXLOPER12      Always the string "Func1"
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) LPXLOPER12 WINAPI Func1 (LPXLOPER12 x)
{
	static XLOPER12 xResult;

	//
	// This function demonstrates returning a string value. The return
	// type is set to a string and filled with the name of the function.
	//

	xResult.xltype = xltypeStr;
	xResult.val.str = const_cast< LPWSTR >(L"\005Func1");

	//Word of caution - returning static XLOPERs/XLOPER12s is not thread safe
	//for UDFs declared as thread safe, use alternate memory allocation mechanisms
    return(LPXLOPER12) &xResult;
}

///***************************************************************************
// FuncSum()
//
// Purpose:
//
//      This is a typical user-defined function provided by an XLL. This
//      function takes 1-29 arguments and computes their sum. Each argument
//      can be a single number, a range, or an array.
//
// Parameters:
//
//      LPXLOPER12 ...  1 to 29 arguments
//                      (can be references or values)
//
// Returns: 
//
//      LPXLOPER12      The sum of the arguments
//                      or #VALUE! if there are
//                      non-numerics in the supplied
//                      argument list or in an cell in a
//                      range or element in an array
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) LPXLOPER12 WINAPI FuncSum(
                        LPXLOPER12 px1,LPXLOPER12 px2,LPXLOPER12 px3,LPXLOPER12 px4,
                        LPXLOPER12 px5,LPXLOPER12 px6,LPXLOPER12 px7,LPXLOPER12 px8,
                        LPXLOPER12 px9,LPXLOPER12 px10,LPXLOPER12 px11,LPXLOPER12 px12,
                        LPXLOPER12 px13,LPXLOPER12 px14,LPXLOPER12 px15,LPXLOPER12 px16,
                        LPXLOPER12 px17,LPXLOPER12 px18,LPXLOPER12 px19,LPXLOPER12 px20,
                        LPXLOPER12 px21,LPXLOPER12 px22,LPXLOPER12 px23,LPXLOPER12 px24,
                        LPXLOPER12 px25,LPXLOPER12 px26,LPXLOPER12 px27,LPXLOPER12 px28,
                        LPXLOPER12 px29)
{
	static XLOPER12 xResult;// Return value 
	double d=0;				// Accumulate result 
	int iArg;				// The argument being processed 
	LPXLOPER12 FAR *ppxArg;	// Pointer to the argument being processed 
	XLOPER12 xMulti;		// Argument coerced to xltypeMulti 
	__int64 i;					// Row and column counters for arrays 
	LPXLOPER12 px;			// Pointer into array 
	int error = -1;			// -1 if no error; error code otherwise 

	//
	// This block accumulates the arguments passed in. Because FuncSum is a
	// Pascal function, the arguments will be evaluated right to left. For
	// each argument, this code checks the type of the argument and then does
	// things necessary to accumulate that argument type. Unless the caller
	// actually specified all 29 arguments, there will be some missing
	// arguments. The first case handles this. The second case handles
	// arguments that are numbers. This case just adds the number to the
	// accumulator. The third case handles references or arrays. It coerces
	// references to an array. It then loops through the array, switching on
	// XLOPER12 types and adding each number to the accumulator. The fourth
	// case handles being passed an error. If this happens, the error is
	// stored in error. The fifth and default case handles being passed
	// something odd, in which case an error is set. Finally, a check is made
	// to see if error was ever set. If so, an error of the same type is
	// returned; if not, the accumulator value is returned.
	//

	for (iArg = 0; iArg < 29; iArg++)
	{
		ppxArg = &px1 + iArg;

		switch ((*ppxArg)->xltype)
		{
		
		case xltypeMissing:
			break;

		case xltypeNum:
			d += (*ppxArg)->val.num;
			break;

		case xltypeRef:
		case xltypeSRef:
		case xltypeMulti:
			if (xlretUncalced == Excel12f(xlCoerce, &xMulti, 2,
									   (LPXLOPER12) *ppxArg, TempInt12(xltypeMulti)))
			{
				//
				// That coerce might have failed due to an 
				// uncalced cell, in which case, we need to 
				// return immediately. Microsoft Excel will
				// call us again in a moment after that cell
				// has been calced.
				//
				return 0;
			}

			for (i = 0;
				i < (xMulti.val.array.rows * xMulti.val.array.columns);
				i++)
			{

				// obtain a pointer to the current item //
				px = xMulti.val.array.lparray + i;

				// switch on XLOPER12 type //
				switch (px->xltype)
				{

				// if a num accumulate it //
				case xltypeNum:
					d += px->val.num;
					break;

				// if an error store in error //
				case xltypeErr:
					error = px->val.err;
					break;

				// if missing do nothing //
				case xltypeNil:
					break;

				// if anything else set error //
				default:
					error = xlerrValue;
					break;
				}
			}

			// free the returned array //
			Excel12f(xlFree, 0, 1, (LPXLOPER12) &xMulti);
			break;

		case xltypeErr:
			error = (*ppxArg)->val.err;
			break;

		default:
			error = xlerrValue;
			break;
		}

	}

	if (error != -1)
	{
		xResult.xltype = xltypeErr;
		xResult.val.err = error;
	}
	else
	{
		xResult.xltype = xltypeNum;
		xResult.val.num = d;
	}
	
	//Word of caution - returning static XLOPERs/XLOPER12s is not thread safe
	//for UDFs declared as thread safe, use alternate memory allocation mechanisms
	return(LPXLOPER12) &xResult;
}


///***************************************************************************
// FuncFib()
//
// Purpose:
//
//      A sample function that computes the nth Fibonacci number.
//      Features a call to several wrapper functions.
//
// Parameters:
//
//      LPXLOPER12 n    int to compute to
//
// Returns: 
//
//      LPXLOPER12      nth Fibonacci number
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) LPXLOPER12 WINAPI FuncFib (LPXLOPER12 n)
{
	static XLOPER12 xResult;
	XLOPER12 xlt;
	int val, max, error = -1;
	int fib[2] = {1,1};
	switch (n->xltype)
	{
	case xltypeNum:
		max = (int)n->val.num;
		if (max < 0)
			error = xlerrValue;
		for (val = 3; val <= max; val++)
		{
			fib[val%2] += fib[(val+1)%2];
		}
		xResult.xltype = xltypeNum;
		xResult.val.num = fib[(val+1)%2];
		break;
	case xltypeSRef:
		error = Excel12f(xlCoerce, &xlt, 2, n, TempInt12(xltypeInt));
		if (!error)
		{
			error = -1;
			max = xlt.val.w;
			if (max < 0)
				error = xlerrValue;
			for (val = 3; val <= max; val++)
			{
				fib[val%2] += fib[(val+1)%2];
			}
			xResult.xltype = xltypeNum;
			xResult.val.num = fib[(val+1)%2];
		}
		Excel12f(xlFree, 0, 1, &xlt);
		break;
	default:
		error = xlerrValue;
		break;
	}

	if ( error != - 1 )
	{
		xResult.xltype = xltypeErr;
		xResult.val.err = error;
	}

	//Word of caution - returning static XLOPERs/XLOPER12s is not thread safe
	//for UDFs declared as thread safe, use alternate memory allocation mechanisms
    return(LPXLOPER12) &xResult;
}


///***************************************************************************
// fDance()
//
// Purpose:
//
//      This is an example of a lengthy operation. It calls the function xlAbort
//      occasionally. This yields the processor (allowing cooperative 
//      multitasking), and checks if the user has pressed ESC to abort this
//      operation. If so, it offers the user a chance to cancel the abort.
//
// Parameters:
//
// Returns: 
//
//      int         1
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) int WINAPI fDance(void)
{
	DWORD dtickStart;
	XLOPER12 xAbort, xConfirm;
	int boolSheet;
	int col=0;
	XCHAR rgch[32];

	//
	// Check what kind of sheet is active. If it is a worksheet or macro
	// sheet, this function will move the selection in a loop to show
	// activity. In any case, it will update the status bar with a countdown.
	//
	// Call xlSheetId; if that fails the current sheet is not a macro sheet or
	// worksheet. Next, get the time at which to start. Then start a while
	// loop that will run for one minute. During the while loop, check if the
	// user has pressed ESC. If true, confirm the abort. If the abort is
	// confirmed, clear the message bar and return; if the abort is not
	// confirmed, clear the abort state and continue. After checking for an
	// abort, move the active cell if on a worksheet or macro. Then
	// update the status bar with the time remaining.
	//
	// This block uses TempActiveCell12(), which creates a temporary XLOPER12.
	// The XLOPER12 contains a reference to a single cell on the active sheet.
	// This function is part of the framework library.
	//

	boolSheet = (Excel12f(xlSheetId, 0, 0) == xlretSuccess);

	dtickStart = GetTickCount();

	while (GetTickCount() < dtickStart + 60000L)
	{
		Excel12f(xlAbort, &xAbort, 0);
		if (xAbort.val.xbool)
		{
			Excel12f(xlcAlert, &xConfirm, 2,
				  TempStr12(L"Are you sure you want to cancel this operation?"),
				  TempNum12(1));
			if (xConfirm.val.xbool)
			{
				Excel12f(xlcMessage, 0, 1, TempBool12(0));
				return 1;
			}
			else
			{
				Excel12f(xlAbort, 0, 1, TempBool12(0));
			}
		}

		if (boolSheet)
		{
			Excel12f(xlcSelect, 0, 1, TempActiveCell12(0,(BYTE)col));
			col = (col + 1) & 3;
		}

		wsprintfW(rgch,L"0:%lu",
				 (60000 + dtickStart - GetTickCount()) / 1000L);

		Excel12f(xlcMessage, 0, 2, TempBool12(1), TempStr12(rgch));

	}

	Excel12f(xlcMessage, 0, 1, TempBool12(0));

	return 1;
}

///***************************************************************************
// fDialog()
//
// Purpose:
//
//      An example of how to create a Microsoft Excel
//      UDD (User Defined Dialog) from a DLL.
//
// Parameters:
//
// Returns: 
//
//      int         1
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) int WINAPI fDialog(void)
{
	int i, j;
	HANDLE hrgrgx;
	LPXLOPER12 prgrgx, px;
	XLOPER12 xDialog;
	XLOPER12 rgxList[5];
	XLOPER12 xList;
	XLOPER12 xDialogReturned;

	//
	// This block first allocates memory to hold the dialog box array. It then
	// fills this array with information from g_rgDialog[]. It replaces any
	// empty entries with NIL XLOPERs while filling the array. It then
	// creates the name "GENERIC_List1"to refer to an array containing the
	// list box values. The dialog box is then displayed. The dialog box is
	// redisplayed using the results from the last dialog box. Then the arrays
	// are freed and the name "GENERIC_List1"is deleted.
	//

	px = prgrgx = (LPXLOPER12) GlobalLock(hrgrgx = 
										GlobalAlloc(GMEM_MOVEABLE, sizeof(XLOPER12)* g_rgDialogCols * g_rgDialogRows));

	for (i = 0; i < g_rgDialogRows; i++)
	{
		for (j = 0; j < g_rgDialogCols; j++)
		{
			if (g_rgDialog[i][j][0] == 0)
				px->xltype = xltypeNil;
			else
			{
				px->xltype = xltypeStr;
				px->val.str = const_cast< XCHAR* >(g_rgDialog[i][j]);
			}
			px++;
		}
	}

	xDialog.xltype = xltypeMulti;
	xDialog.val.array.lparray = prgrgx;
	xDialog.val.array.rows = g_rgDialogRows;
	xDialog.val.array.columns = g_rgDialogCols;

	rgxList[0].val.str = (LPWSTR) L"\004Bake";
	rgxList[1].val.str = (LPWSTR) L"\005Broil";
	rgxList[2].val.str = (LPWSTR) L"\006Sizzle";
	rgxList[3].val.str = (LPWSTR) L"\003Fry";
	rgxList[4].val.str = (LPWSTR) L"\005Saute";

	for (i = 0; i < 5; i++)
	{
		rgxList[i].xltype = xltypeStr;
	}

	xList.xltype = xltypeMulti;
	xList.val.array.lparray = (LPXLOPER12) & rgxList;
	xList.val.array.rows = 5;
	xList.val.array.columns = 1;

	Excel12f(xlfSetName, 0, 2, TempStr12(L"GENERIC_List1"), (LPXLOPER12) &xList);

	//Create the dialog box

	Excel12f(xlfDialogBox, &xDialogReturned, 1, (LPXLOPER12) &xDialog);

	//handle clean up by passing in the return value of dialog creation call

	//if xlfDialogBox false in xDialogReturned then we need to call back to it
	//to perform some clean up which involves redrawing the main window and
	//setting up the dialog for repainting. xlfDialogBox will return false
	//above if the user hit cancel.


	Excel12f(xlfDialogBox, 0, 1, (LPXLOPER12) &xDialogReturned);

	Excel12f(xlFree, 0, 1, (LPXLOPER12) &xDialogReturned);

	GlobalUnlock(hrgrgx);
	GlobalFree(hrgrgx);

	Excel12f(xlfSetName, 0, 1, TempStr12(L"GENERIC_List1"));

	return 1;
}


///***************************************************************************
// fExit()
//
// Purpose:
//
//      This is a user-initiated routine to exit GENERIC.XLL You may be tempted to
//      simply call UNREGISTER("GENERIC.XLL") in this function. Don't do it! It
//      will have the effect of forcefully unregistering all of the functions in
//      this DLL, even if they are registered somewhere else! Instead, unregister
//      the functions one at a time.
//
// Parameters:
//
// Returns: 
//
//      int         1
//
// Comments:
//
// History:  Date       Author        Reason
///***************************************************************************
extern "C"
__declspec(dllexport) int WINAPI fExit(void)
{
	XLOPER12  xDLL,    // The name of this DLL //
	xFunc,             // The name of the function //
	xRegId;            // The registration ID //
	int i;

	//
	// This code gets the DLL name. It then uses this along with information
	// from g_rgFuncs[] to obtain a REGISTER.ID() for each function. The
	// register ID is then used to unregister each function. Then the code
	// frees the DLL name and calls xlAutoClose.
	//

	// Make xFunc a string //
	xFunc.xltype = xltypeStr;

	Excel12f(xlGetName, &xDLL, 0);

	for (i = 0; i < g_rgWorksheetFuncsRows; i++)
	{
		xFunc.val.str = (LPWSTR) (g_rgWorksheetFuncs[i][0]);
		Excel12f(xlfRegisterId,&xRegId,2,(LPXLOPER12)&xDLL,(LPXLOPER12)&xFunc);
		Excel12f(xlfUnregister, 0, 1, (LPXLOPER12) &xRegId);
	}

	for (i = 0; i < g_rgCommandFuncsRows; i++)
	{
		xFunc.val.str = (LPWSTR) (g_rgCommandFuncs[i][0]);
		Excel12f(xlfRegisterId,&xRegId,2,(LPXLOPER12)&xDLL,(LPXLOPER12)&xFunc);
		Excel12f(xlfUnregister, 0, 1, (LPXLOPER12) &xRegId);
	}

	Excel12f(xlFree, 0, 1,  (LPXLOPER12) &xDLL);

	return xlAutoClose();
}
