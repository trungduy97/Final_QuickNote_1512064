// 1512064_QuickNote.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "1512064_QuickNote.h"
#include <commctrl.h>
#include <Windowsx.h>
#include "TAG.h"
#include "NOTE.h"
#include "List.h"

#define MAX_LOADSTRING 100
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


HWND hTagList, hEdtNote, hNoteList, hButtonSave, hComboboxTag;
LIST *List;
wstring TagStr;
bool uTagStr = false;
NOTIFYICONDATA nData;
HMENU Menu;


int OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int Save(LIST *,HWND );
int getIndex(LIST *, HWND );
int getIdNote(LIST *, HWND , HWND );
int getListIndex(LIST *, HWND );

int updateListBoxNotes(LIST *, HWND);
int updateListBoxTag(LIST *, HWND, HWND);
int updateEditTextNote(HWND, HWND, NOTE);
float TiLe(TAG tag, vector<TAG> tagList); //tính tỉ lệ %
int drawChartColum(HWND hWnd, HFONT hFont, HDC hdc, vector<TAG> tagList, int x, int y, int Width, int Height, int dis); ////Vẽ biểu đồ thống kê cột
bool soSanh(TAG tag1, TAG tag2);


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Statistic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_QUICKNOTE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL haccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QUICKNOTE));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, haccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(192, 192, 192));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_QUICKNOTE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   400, 200, 550, 500, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return FALSE;
   }
   HICON hIcon;
   hIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_ICON1));

   nData.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes 
   nData.hWnd = (HWND)hWnd;              //handle of the window which will process this app. messages 
   nData.uID = IDI_ICON1;           //ID of the icon that will appear in the system tray 
   nData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; //ORing of all the flags 
   nData.hIcon = hIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
   nData.uCallbackMessage = TRAY_ICON_NOTIFICATION;
   LoadString(hInstance, IDS_APP_TITLE, nData.szTip, MAX_LOADSTRING);
   Shell_NotifyIcon(NIM_ADD, &nData);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (uTagStr)
	{
		SendMessage(hComboboxTag, WM_SETTEXT, 0, (LPARAM)TagStr.c_str());
		uTagStr = false;
	}
	switch (message)
	{
	case TRAY_ICON_NOTIFICATION:
		switch (LOWORD(lParam))
		{
		case WM_LBUTTONDBLCLK:
			ShowWindow(hWnd, SW_NORMAL);
		case WM_RBUTTONDOWN:
			POINT  Point;
			UINT uFlag = MF_BYPOSITION | MF_STRING;
			GetCursorPos(&Point);
			Menu = CreatePopupMenu();
			InsertMenu(Menu, 0xFFFFFFFF, uFlag, IDC_TRAY_ICON_SHOW, _T("VIEW NOTE")); 
			InsertMenu(Menu, 0xFFFFFFFF, uFlag, IDC_BTN_STATISTIC, _T("VIEW STATISTIC")); 	
			InsertMenu(Menu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"EXIT");

			SetForegroundWindow(hWnd);
			TrackPopupMenu(Menu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, Point.x, Point.y, 0, hWnd, NULL);
			return TRUE;
		
			return TRUE;
		}
		break;

	
	
		
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_CBBOX_TAG:
			{
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					TCHAR buff[1024];
					SendMessage(hComboboxTag, WM_GETTEXT, sizeof(buff), (LPARAM)buff);
					TagStr = buff;

					SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)buff);

					TagStr += L", ";
					TagStr += buff;
					TagStr = List->ListTag(List->addTag(TagStr));

					SendMessage(hComboboxTag, WM_SETTEXT, 0, (LPARAM)TagStr.c_str());
					uTagStr = true;
				}
			}
			break;
		
		case IDC_BTN_SAVE:
		{
			Save(List, hWnd);
			break;
		}
		case IDC_EDIT_NOTE:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				InvalidateRect(hEdtNote, NULL, TRUE);
			}
			break;
		}
		case IDC_LISTBOX_TAG:
		{
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:
			{
				updateListBoxNotes(List, hNoteList);
				break;
			}
			}
			break;
		}
		case IDC_LISTBOX_NOTE_LIST:
		{
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:
			{
				int currNoteID = getIdNote(List, hTagList, hNoteList);
				if (currNoteID == LB_ERR)
				{
					SendMessage(hEdtNote, WM_SETTEXT, 0, (LPARAM)L"");
					SendMessage(hComboboxTag, WM_SETTEXT, 0, (LPARAM)L"");
					break;
				}
				else
				{
					updateEditTextNote(hEdtNote, hComboboxTag, List->NoteList[currNoteID]);
				}
				break;
			}
			}
			break;
		}
		case IDC_TRAY_ICON_SHOW:
		{
			ShowWindow(hWnd, SW_NORMAL);
			break;
		}
		case IDC_BTN_STATISTIC:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_STATISTIC), hWnd, Statistic);
			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CREATE:
	{
		OnCreate(hWnd, message, wParam, lParam);
		break;
	}
	
	case WM_SYSCOMMAND:
		if (SC_CLOSE == wParam)
		{
			ShowWindow(hWnd, SW_HIDE);
			return TRUE;
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nData);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT r;
	if (GetWindowRect(hWnd, &r))
	{
		List = new LIST();
		int width = r.right - r.left;
		int height = r.bottom - r.top;

		HWND hWndStaticTagList = CreateWindow(L"Static", L"Danh sách Tag:", WS_CHILD | WS_VISIBLE, 10, 230, 120, 20, hWnd, NULL, hInst, NULL);
		HWND hWndStaticCurrNote = CreateWindow(L"Static", L"Nội dung ghi chú:", WS_CHILD | WS_VISIBLE, 10, 10, 130, 20, hWnd, NULL, hInst, NULL);
		HWND hWndStaticAddTag = CreateWindow(L"Static", L"Tag:", WS_CHILD | WS_VISIBLE, 330, 10, 30, 20, hWnd, NULL, hInst, NULL);
		HWND hWndStaticNoteList = CreateWindow(L"Static", L"Danh sách Notes:", WS_CHILD | WS_VISIBLE, 220, 230, 150, 20, hWnd, NULL, hInst, NULL);

		hTagList = CreateWindowEx(WS_EX_CLIENTEDGE, L"ListBox", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_SORT, 10, 250, 200, 190, hWnd, (HMENU)IDC_LISTBOX_TAG, hInst, NULL);
		hEdtNote = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE, 10, 30, 300, 200, hWnd, (HMENU)IDC_EDIT_NOTE, hInst, NULL);
		hComboboxTag = CreateWindowEx(WS_EX_STATICEDGE, L"COMBOBOX", L"MyCombo1", CBS_DROPDOWN | WS_HSCROLL | WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, 330, 30, 200, 25, hWnd, (HMENU)IDC_CBBOX_TAG, hInst, NULL);
		hNoteList = CreateWindowEx(WS_EX_CLIENTEDGE, L"ListBox", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY, 220, 250, 300, 190, hWnd, (HMENU)IDC_LISTBOX_NOTE_LIST, hInst, NULL);
																																																				 
		hButtonSave = CreateWindowEx(0, L"Button", L"SAVE", WS_CHILD | WS_VISIBLE, 370, 90, 120, 100, hWnd, (HMENU)IDC_BTN_SAVE, hInst, NULL);

		SendMessage(hComboboxTag, EM_SETCUEBANNER, FALSE, (LPARAM)L"Set tags");

		HFONT Font = CreateFont(18, 0, 0, 0, 510, FALSE, FALSE, FALSE, ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));

		SendMessage(hTagList, WM_SETFONT, (WPARAM)Font, TRUE);
		SendMessage(hNoteList, WM_SETFONT, (WPARAM)Font, TRUE);
		SendMessage(hEdtNote, WM_SETFONT, (WPARAM)Font, TRUE);
		SendMessage(hComboboxTag, WM_SETFONT, (WPARAM)Font, TRUE);

		Font = CreateFont(17, 0, 0, 0, 610, FALSE, FALSE, FALSE, ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));

		SendMessage(hWndStaticTagList, WM_SETFONT, (WPARAM)Font, TRUE);
		SendMessage(hWndStaticNoteList, WM_SETFONT, (WPARAM)Font, TRUE);
		SendMessage(hWndStaticCurrNote, WM_SETFONT, (WPARAM)Font, TRUE);

		List->loadFile();
		updateListBoxTag(List, hTagList, hComboboxTag);
		return 0;
	}
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

    }
    return (INT_PTR)FALSE;
}



//Statistic diaglog
INT_PTR CALLBACK Statistic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);
		HFONT hFont_Title = CreateFont(30, 0, 0, 0, 1540, FALSE, FALSE, FALSE, ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));

		HWND hWndStatic = CreateWindow(L"Static", L"THỐNG KÊ CÁC TAG", WS_CHILD | WS_VISIBLE| SS_CENTER, 0, 20, 800, 30, hDlg, NULL, hInst, NULL);

		SendMessage(hWndStatic, WM_SETFONT, (WPARAM)hFont_Title, TRUE);

		HFONT hFont = CreateFont(16, 0, 0, 0, 550, FALSE, FALSE, FALSE, ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));

		vector<TAG> tagList = List->TagList;
		sort(tagList.begin(), tagList.end(), soSanh);
		
		drawChartColum(hDlg, hFont, hdc, tagList, 50, 100, 350, 25, 25);
		break;
	}
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

int updateListBoxTag(LIST *L, HWND hWndLB, HWND hWndComboBoxTag)
{
	SendMessage(hWndLB, LB_RESETCONTENT, NULL, NULL);
	wstringstream s;
	for (int i = 0; i < L->TagList.size(); i++)
	{
		wstringstream().swap(s);
		s << L->TagList[i].TagName << "(" << L->TagList[i].Id.size() << ")";

		SendMessage(hWndLB, LB_ADDSTRING, 0, (LPARAM)s.str().c_str());
	}
	TCHAR buff[1024];
	SendMessage(hWndComboBoxTag, WM_GETTEXT, sizeof(buff), (LPARAM)buff);
	TagStr = buff;
	SendMessage(hWndComboBoxTag, CB_RESETCONTENT, 0, 0);

	vector<TAG> tagList = L->TagList;
	sort(tagList.begin(), tagList.end(), soSanh);

	if (tagList.size() > 1)
	{
		for (int i = 1; i < tagList.size(); i++)
		{
			SendMessage(hWndComboBoxTag, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)tagList[i].TagName.c_str());
		}
	}
	TagStr += L", ";
	TagStr += buff;
	TagStr = L->ListTag(L->addTag(TagStr));

	SendMessage(hWndComboBoxTag, WM_SETTEXT, 0, (LPARAM)TagStr.c_str());

	return 0;
}
int Save(LIST *L, HWND hWnd)
{
	WCHAR note[1000], tag[200];
	GetWindowText(hEdtNote, note, sizeof(note));
	GetWindowText(hComboboxTag, tag, sizeof(tag));

	int csNote = getIdNote(L, hTagList, hNoteList);
	if (csNote != LB_ERR)
	{
		L->saveListNote(L->addTag(tag), note, csNote);
	}
	else
	{
		L->saveListNote(L->addTag(tag), note,-1);
	}

	int Loi = L->saveFile();
	if (Loi != 0)
	{
		wstringstream s;
		s << L"Không thể mở file\n Lỗi = " << Loi;
		MessageBox(hWnd, s.str().c_str(), L"Thông báo", MB_OK);
	}
	else
	{
		MessageBox(hWnd, L"Lưu thành công", L"Thông báo", MB_OK);
		int currSelectedTag = getListIndex(L, hTagList);
		int noteIndexInTag = (int)SendMessage(hNoteList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		updateListBoxTag(L, hTagList, hComboboxTag);
		if (currSelectedTag != LB_ERR)
		{
			SendMessage(hTagList, LB_SETCURSEL, (WPARAM)currSelectedTag, (LPARAM)0);
			updateListBoxNotes(L, hNoteList);
			SendMessage(hNoteList, LB_SETCURSEL, (WPARAM)noteIndexInTag, (LPARAM)0);

			int currSelectedNoteID = getIdNote(L, hTagList, hNoteList);
			if (currSelectedNoteID != LB_ERR)
			{
				updateEditTextNote(hEdtNote, hComboboxTag, L->NoteList[currSelectedNoteID]);
			}
		}
		else
		{
			SendMessage(hEdtNote, WM_SETTEXT, 0, (LPARAM)L"");
			SendMessage(hComboboxTag, WM_SETTEXT, 0, (LPARAM)L"");

		}
	}
	return Loi;
}



int updateEditTextNote(HWND hWndEdtNote, HWND hWndCBBTag, NOTE note)
{
	SendMessage(hWndEdtNote, WM_SETTEXT, 0, (LPARAM)note.Content.c_str());

	wstring tag;
	for (int i = 0; i < note.Tag.size(); i++)
	{
		tag += note.Tag[i] + L", ";
	}

	tag = List->ListTag(List->addTag(tag));
	SendMessage(hWndCBBTag, WM_SETTEXT, 0, (LPARAM)tag.c_str());

	return 0;
}
int updateListBoxNotes(LIST *L, HWND hWndListBox)
{
	int csTag = getIndex(L, hTagList);
	wstring strTag = L->TagList[csTag].TagName;
	SendMessage(hWndListBox, LB_RESETCONTENT, NULL, NULL);

	int Pos = -1;
	for (int i = 0; i < L->TagList.size(); i++)
	{
		if (L->TagList[i].TagName == strTag)
		{
			Pos = i;
			break;
		}
	}

	if (Pos != -1)
	{
		wstring ws;
		int noteId;
		for (int i = 0; i < L->TagList[Pos].Id.size(); i++)
		{
			noteId = L->TagList[Pos].Id[i];
			ws = L->NoteList[noteId].Content.substr(0, 40);
			ws += L"...";
			
			SendMessage(hWndListBox, LB_ADDSTRING, 0, (LPARAM)ws.c_str());
		}
	}
	//
	SendMessage(hEdtNote, WM_SETTEXT, 0, (LPARAM)L"");
	SendMessage(hComboboxTag, WM_SETTEXT, 0, (LPARAM)L"");
	return 0;
}


float TiLe(TAG tag, vector<TAG> tagList)   //tính tỉ lệ %
{
	if (tag.Id.size() == 0)
	{
		return 0;
	}


	long double sum = 0;
	for (int i = 1; i < tagList.size(); i++)
	{
		sum += tagList[i].Id.size();
	}

	float result = tag.Id.size() *1.0 / sum;
	return  result;
}

int getIndex(LIST *L, HWND hWndTagList)
{
	int itemIndex = (int)SendMessage(hWndTagList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	if (itemIndex == LB_ERR)// No selection
	{
		return LB_ERR;
	}
	WCHAR TagStr[TAG_MAX_LENGTH];
	
	int i = (int)SendMessage(hWndTagList, LB_GETTEXT, itemIndex, (LPARAM)TagStr);

	wstring ws = TagStr;
	
	while (ws[ws.size()-1] != '(')
	{
		ws.pop_back();
	}
	ws.pop_back();

	int tagPos = -1; 
	for (int i = 0; i < L->TagList.size(); i++)
	{
		if (L->TagList[i].TagName == ws)
		{
			tagPos = i;
			break;
		}
	}

	return tagPos;
}
int getIdNote(LIST *L, HWND hWndTagList, HWND hWndNoteList)
{
	int tagIndex = getIndex(L, hWndTagList);
	int noteIndex = (int)SendMessage(hWndNoteList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	if (noteIndex == LB_ERR)
	{
		return LB_ERR;
	}

	return L->TagList[tagIndex].Id[noteIndex];
}
int getListIndex(LIST *L, HWND hWndTagList)
{
	int itemIndex = (int)SendMessage(hWndTagList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	if (itemIndex == LB_ERR)// No selection
	{
		return LB_ERR;
	}
	return itemIndex;
}



///DRAW CHART

int drawChartColum(HWND hWnd, HFONT hFont, HDC hdc, vector<TAG> tagList, int X, int Y, int Width, int Height, int distance)
{
	int txtWidth = 100;
	int yText = Y;
	int yBar = Y;
	int xBar = X + txtWidth + 10;
	int rbWidth = 0;

	HBRUSH hbrush = CreateSolidBrush(RGB(43, 191, 223));

	wstringstream ws;
	ws.imbue(locale(""));

	int n = tagList.size();
	if (n > 6)
	{
		n = 6;
	}
	for(int i = 1; i <n; i++)
	{
		float per = TiLe(tagList[i], tagList);
		rbWidth = Width*per + 3;

		RECT r;
		r.left = xBar;
		r.top = yBar;
		r.right = xBar + rbWidth;
		r.bottom = yBar + Height;
		FillRect(hdc, &r, hbrush);

		ws.str(L"");
		ws << tagList[i].TagName << ":\0";
		HWND hWndName = CreateWindowEx(0, L"Static", ws.str().c_str(), WS_VISIBLE | WS_CHILD, X, yBar, txtWidth, Height, hWnd, NULL, NULL, NULL);


		ws.str(L"");
		ws << " " << tagList[i].Id.size() << "(" << fixed << setprecision(0) << per * 100 << "%)\0";
		HWND hWndLast = CreateWindowEx(0, L"Static", ws.str().c_str(), WS_VISIBLE | WS_CHILD, xBar + rbWidth, yBar, txtWidth, Height, hWnd, NULL, NULL, NULL);
	
		//setfont
		SendMessage(hWndName, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(hWndLast, WM_SETFONT, (WPARAM)hFont, TRUE);

		if (i == 1)
		{
			hbrush = CreateSolidBrush(RGB(17, 29, 249));
		}
		if (i == 2)
		{
			hbrush = CreateSolidBrush(RGB(227, 238, 28));
		}
		if (i == 3)
		{
			hbrush = CreateSolidBrush(RGB(249, 75, 17));
		}
		if (i == 4)
		{
			hbrush = CreateSolidBrush(RGB(21, 244, 44));
		}

		yText += Height + distance;
		yBar += Height + distance;
	}

	return 0;
}



bool soSanh(TAG tag1, TAG tag2) { 
	return tag1.Id.size() > tag2.Id.size(); 
}


