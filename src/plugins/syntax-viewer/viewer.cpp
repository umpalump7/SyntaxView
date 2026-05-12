//****************************************************************************
// 
// Syntax Highlighting Plugin for Open Salamander
//
// Author: Michal Mores
//
// Email: xmoresm00@stud.fit.vut.cz
//
//****************************************************************************

// SPDX-FileCopyrightText: 2023 Open Salamander Authors
// SPDX-License-Identifier: GPL-2.0-or-later


#include "precomp.h"

#include "helper.h"

#define IDX_TB_TERMINATOR -2
#define IDX_TB_SEPARATOR -1
#define IDX_TB_CUT 0
#define IDX_TB_COPY 1
#define IDX_TB_PASTE 2
#define IDX_TB_FILTER 3
#define IDX_TB_COUNT 4

// Custom scroll
#define CUSTOM_FASTSCROLLUP 5630
#define CUSTOM_FASTSCROLLRIGHT 5630
#define CUSTOM_FASTSCROLLDOWN 5631
#define CUSTOM_FASTSCROLLLEFT 5631
#define CUSTOM_SCROLLREFRESH 5632

WORD CurrentEncoding = CM_VIEWER_ANSI;
UINT CurrentEncodingCodePage = CP_ACP;

#define FILTER_COUNT (CM_FILTER_LAST - CM_FILTER_FIRST + 1)

MENU_TEMPLATE_ITEM MenuTemplate[] =
    {
        {MNTT_PB, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},
        // Files
        {MNTT_PB, IDS_MENU_FILES, MNTS_B | MNTS_I | MNTS_A, CML_VIEWER_FILES, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_FILES_OPEN, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_OPEN, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_FILES_OPENEDITOR, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_OPENEDITOR, -1, 0, NULL},

        // INSERTED POPUP OTHER FILES
        {MNTT_SP, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},

        {MNTT_PB, IDS_MENU_FILES_OTHERFILES, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},

        {MNTT_IT, IDS_MENU_FILES_PREV, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_PREVFILE, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_FILES_NEXT, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_NEXTFILE, -1, 0, NULL},

        //{MNTT_SP, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},

        //{MNTT_IT, IDS_MENU_FILES_PREVSEL, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_PREVSELFILE, -1, 0, NULL},
        //{MNTT_IT, IDS_MENU_FILES_NEXTSEL, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_NEXTSELFILE, -1, 0, NULL},

        //{MNTT_SP, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},

        //{MNTT_IT, IDS_MENU_FILES_FIRST, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_FIRSTFILE, -1, 0, NULL},
        //{MNTT_IT, IDS_MENU_FILES_LAST, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_LASTFILE, -1, 0, NULL},

        {MNTT_PE},
        // INSERTED POPUP END

        {MNTT_SP, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_FILES_GOTOLINE, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_GOTOLINE, -1, 0, NULL},
        {MNTT_SP, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_FILES_EXIT, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_EXIT, -1, 0, NULL},
        {MNTT_PE},

        // Edit
        {MNTT_PB, IDS_MENU_EDIT, MNTS_B | MNTS_I | MNTS_A, CML_VIEWER_EDIT, -1, 0, NULL},
        //{MNTT_IT, IDS_MENU_EDIT_CUT, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_CUT, IDX_TB_CUT, 0, (DWORD*)vweCut},
        {MNTT_IT, IDS_MENU_EDIT_COPY, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_COPY, IDX_TB_COPY, 0, NULL},
        {MNTT_SP, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},
        //{MNTT_IT, IDS_MENU_EDIT_PASTE, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_PASTE, IDX_TB_PASTE, 0, (DWORD*)vwePaste},
        {MNTT_IT, IDS_MENU_EDIT_SELECTALL, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_SELECTALL, -1, 0, NULL},
        {MNTT_PE},

        // Search
        {MNTT_PB, IDS_MENU_SEARCH, MNTS_B | MNTS_I | MNTS_A, CML_VIEWER_SEARCH, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_SEARCH_FIND, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_FIND, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_SEARCH_FINDNEXT, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_FINDNEXT, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_SEARCH_FINDPREV, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_FINDPREV, -1, 0, NULL},
        {MNTT_PE},

        // View
        {MNTT_PB, IDS_MENU_VIEW, MNTS_B | MNTS_I | MNTS_A, CML_VIEWER_VIEW, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_VIEW_FULLSCREEN, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_FULLSCREEN, -1, 0, NULL},
        {MNTT_SP, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_VIEW_LINENUMBERING, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_LINENUMBERING, -1, 0, NULL},
        //{MNTT_SP, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},
        //{MNTT_PB, IDS_MENU_FILTER, MNTS_B | MNTS_I | MNTS_A, CML_VIEWER_FILTER, IDX_TB_FILTER, 0, NULL},
        //{MNTT_PE},
        {MNTT_PE},

        // Encoding
        {MNTT_PB, IDS_MENU_ENCODING, MNTS_B | MNTS_I | MNTS_A, CML_VIEWER_ENCODING, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_ENCODING_ANSI, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_ANSI, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_ENCODING_OEM, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_OEM, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_ENCODING_CP852, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_CP852, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_ENCODING_CP1250, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_CP1250, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_ENCODING_CP1252, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_CP1252, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_ENCODING_LATIN1, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_LATIN1, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_ENCODING_LATIN2, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_LATIN2, -1, 0, NULL},
        {MNTT_SP, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_ENCODING_UTF8, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_UTF8, -1, 0, NULL},
        {MNTT_PE},


        // Options
        {MNTT_PB, IDS_MENU_OPTIONS, MNTS_B | MNTS_I | MNTS_A, CML_VIEWER_OPTIONS, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_OPTIONS_CFG, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_CFG, -1, 0, NULL},
        {MNTT_PE},

        // Help
        {MNTT_PB, IDS_MENU_HELP, MNTS_B | MNTS_I | MNTS_A, CML_VIEWER_HELP, -1, 0, NULL},
        {MNTT_IT, IDS_MENU_HELP_ABOUT, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_ABOUT, -1, 0, NULL},
        {MNTT_PE},

        {MNTT_PE}, // terminator
};

MENU_TEMPLATE_ITEM PopupMenuTemplate[] =
    {
        {MNTT_PB, -1, MNTS_B | MNTS_I | MNTS_A, 0, -1, 0, NULL},
        //{MNTT_IT, IDS_MENU_EDIT_CUT, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_CUT, IDX_TB_CUT, 0, (DWORD*)vweCut},
        {MNTT_IT, IDS_MENU_EDIT_COPY, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_COPY, IDX_TB_COPY, 0, NULL},
        //{MNTT_IT, IDS_MENU_EDIT_PASTE, MNTS_B | MNTS_I | MNTS_A, CM_VIEWER_PASTE, IDX_TB_PASTE, 0, (DWORD*)vwePaste},
        {MNTT_PE}, // terminator
};

struct CButtonData
{
    int ImageIndex;                   // zero base index
    WORD ToolTipResID;                // resID se stringem pro tooltip
    WORD ID;                          // univerzalni Command
    CViewerWindowEnablerEnum Enabler; // ridici promenna pro enablovani tlacitka
};

//CButtonData ToolBarButtons[] =
//    {
//        // ImageIndex     ToolTipResID     ID                Enabler
//        //{IDX_TB_CUT, IDS_TBTT_CUT, CM_VIEWER_CUT, vweCut},
//        {IDX_TB_COPY, IDS_TBTT_COPY, CM_VIEWER_COPY, vweAlwaysEnabled},
//        {IDX_TB_SEPARATOR},
//        //{IDX_TB_PASTE, IDS_TBTT_PASTE, CM_VIEWER_PASTE, vwePaste},
//        {IDX_TB_SEPARATOR},
//        {IDX_TB_FILTER, IDS_TBTT_FILTER, CM_VIEWER_FILTER, vweAlwaysEnabled},
//        {IDX_TB_TERMINATOR}};

CWindowQueue ViewerWindowQueue("SyntaxView Viewers"); // seznam vsech oken viewru
CThreadQueue ThreadQueue("SyntaxView Viewers");       // seznam vsech threadu oken

HACCEL ViewerAccels = NULL; // akceleratory pro viewer

void WINAPI HTMLHelpCallback(HWND hWindow, UINT helpID)
{
    SalamanderGeneral->OpenHtmlHelp(hWindow, HHCDisplayContext, helpID, FALSE);
}

BOOL InitViewer()
{
    if (!InitializeWinLib(PluginNameEN, DLLInstance))
        return FALSE;
    SetWinLibStrings(LoadStr(IDS_INVALID_NUM), LoadStr(IDS_PLUGINNAME));
    SetupWinLibHelp(HTMLHelpCallback);
    ViewerAccels = LoadAccelerators(DLLInstance, MAKEINTRESOURCE(IDA_ACCELERATORS));
    return TRUE;
}

void ReleaseViewer()
{
    ReleaseWinLib(DLLInstance);
    DestroyAcceleratorTable(ViewerAccels);
    ViewerAccels = NULL;
}

class CViewerThread : public CThread
{
protected:
    char Name[MAX_PATH];
    int Left, Top, Width, Height;
    UINT ShowCmd;
    BOOL AlwaysOnTop;
    BOOL ReturnLock;

    HANDLE Continue; // po naplneni nasledujicich navratovych hodnot se tento event prepne do "signaled"
    HANDLE* Lock;
    BOOL* LockOwner;
    BOOL* Success;

    int EnumFilesSourceUID;    // UID zdroje pro enumeraci souboru ve vieweru
    int EnumFilesCurrentIndex; // index prvniho souboru ve vieweru ve zdroji

public:
    CViewerThread(const char* name, int left, int top, int width, int height,
                  UINT showCmd, BOOL alwaysOnTop, BOOL returnLock,
                  HANDLE* lock, BOOL* lockOwner, HANDLE contEvent,
                  BOOL* success, int enumFilesSourceUID,
                  int enumFilesCurrentIndex) : CThread("DMV Viewer")
    {
        lstrcpyn(Name, name, MAX_PATH);
        Left = left;
        Top = top;
        Width = width;
        Height = height;
        ShowCmd = showCmd;
        AlwaysOnTop = alwaysOnTop;
        ReturnLock = returnLock;

        Continue = contEvent;
        Lock = lock;
        LockOwner = lockOwner;
        Success = success;

        EnumFilesSourceUID = enumFilesSourceUID;
        EnumFilesCurrentIndex = enumFilesCurrentIndex;
    }

    virtual unsigned Body();
};



unsigned
CViewerThread::Body()
{
    CALL_STACK_MESSAGE1("CViewerThread::Body()");
    TRACE_I("Begin");

    CViewerWindow* window = new CViewerWindow(EnumFilesSourceUID, EnumFilesCurrentIndex);
    if (window != NULL)
    {
        if (ReturnLock)
        {
            *Lock = window->GetLock();
            *LockOwner = TRUE;
        }
        CALL_STACK_MESSAGE1("ViewerThreadBody::CreateWindowEx");
        if (!ReturnLock || *Lock != NULL)
        {
            if (CfgSavePosition && CfgWindowPlacement.length != 0)
            {
                WINDOWPLACEMENT place = CfgWindowPlacement;
                // GetWindowPlacement cti Taskbar, takze pokud je Taskbar nahore nebo vlevo,
                // jsou hodnoty posunute o jeho rozmery. Provedeme korekci.
                RECT monitorRect;
                RECT workRect;
                SalamanderGeneral->MultiMonGetClipRectByRect(&place.rcNormalPosition, &workRect, &monitorRect);
                OffsetRect(&place.rcNormalPosition, workRect.left - monitorRect.left,
                           workRect.top - monitorRect.top);
                SalamanderGeneral->MultiMonEnsureRectVisible(&place.rcNormalPosition, TRUE);
                Left = place.rcNormalPosition.left;
                Top = place.rcNormalPosition.top;
                Width = place.rcNormalPosition.right - place.rcNormalPosition.left;
                Height = place.rcNormalPosition.bottom - place.rcNormalPosition.top;
                ShowCmd = place.showCmd;
            }

			std::string windowTitle = std::string(Name) + " - SyntaxView";
            if (window->CreateEx(AlwaysOnTop ? WS_EX_TOPMOST : 0,
                                 CWINDOW_CLASSNAME2,
                                 windowTitle.c_str(),
                                 WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                                 Left,
                                 Top,
                                 Width,
                                 Height,
                                 NULL,
                                 NULL,
                                 DLLInstance,
                                 window) != NULL)
            {
                CALL_STACK_MESSAGE1("ViewerThreadBody::ShowWindow");
                ShowWindow(window->HWindow, ShowCmd);
                SetForegroundWindow(window->HWindow);
                UpdateWindow(window->HWindow);
                *Success = TRUE;
            }
            else
            {
                if (ReturnLock && *Lock != NULL)
                    HANDLES(CloseHandle(*Lock));
            }
        }
    }

    CALL_STACK_MESSAGE1("ViewerThreadBody::SetEvent");
    BOOL openFile = *Success;
    SetEvent(Continue); // pustime dale hl. thread, od tohoto bodu nejsou platne nasl. promenne:
    Continue = NULL;    // vymaz je zbytecny, jen pro prehlednost
    Lock = NULL;        // vymaz je zbytecny, jen pro prehlednost
    LockOwner = NULL;   // vymaz je zbytecny, jen pro prehlednost
    Success = NULL;     // vymaz je zbytecny, jen pro prehlednost


    // pokud probehlo vse bez potizi, otevreme v okne pozadovany soubor
    if (openFile)
    {
        CALL_STACK_MESSAGE1("ViewerThreadBody::OpenFile");
        window->OpenFile(Name, FALSE, CurrentEncodingCodePage);


        CALL_STACK_MESSAGE1("ViewerThreadBody::message-loop");
        // message loopa
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            if (!window->IsMenuBarMessage(&msg) &&
                !TranslateAccelerator(window->HWindow, ViewerAccels, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    if (window != NULL)
        delete window;

    TRACE_I("End");
    return 0;
}

BOOL WINAPI
CPluginInterfaceForViewer::ViewFile(const char* name, int left, int top, int width, int height,
                                    UINT showCmd, BOOL alwaysOnTop, BOOL returnLock, HANDLE* lock,
                                    BOOL* lockOwner, CSalamanderPluginViewerData* viewerData,
                                    int enumFilesSourceUID, int enumFilesCurrentIndex)
{
    HANDLE contEvent = HANDLES(CreateEvent(NULL, FALSE, FALSE, NULL));
    if (contEvent == NULL)
    {
        TRACE_E("Unable to create Continue event.");
        return FALSE;
    }

    // 'viewerData' se v DemoView nepouzivaji, jinak by bylo potreba predat hodnoty (ne odkazem)
    // do threadu vieweru...
    BOOL success = FALSE;
    CViewerThread* t = new CViewerThread(name, left, top, width, height,
                                         showCmd, alwaysOnTop, returnLock, lock,
                                         lockOwner, contEvent, &success,
                                         enumFilesSourceUID, enumFilesCurrentIndex);
    if (t != NULL)
    {
        if (t->Create(ThreadQueue) != NULL) // thread se spustil
        {
            t = NULL;                                 // zbytecne nulovani, jen pro poradek (ukazatel uz muze byt dealokovany)
            WaitForSingleObject(contEvent, INFINITE); // pockame, az thread zpracuje predana data a vrati vysledky
        }
        else
            delete t; // pri chybe je potreba dealokovat objekt threadu
    }
    HANDLES(CloseHandle(contEvent));
    return success;
}

//
// ****************************************************************************
// CRendererWindow
//

CRendererWindow::CRendererWindow()
    : CWindow(ooStatic)
{
    hFont = CreateFont(
        font_size, 0, 0, 0, // height = 16px, width = auto
        FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        FF_MODERN | FIXED_PITCH,
        font_name.c_str());
}

CRendererWindow::~CRendererWindow()
{
}

void CRendererWindow::OnContextMenu(const POINT* p)
{
    CGUIMenuPopupAbstract* popup = SalamanderGUI->CreateMenuPopup();
    if (popup != NULL)
    {
        popup->LoadFromTemplate(HLanguage, PopupMenuTemplate, Viewer->Enablers, Viewer->HGrayToolBarImageList,
                                Viewer->HHotToolBarImageList);
        popup->SetStyle(MENU_POPUP_UPDATESTATES);
        popup->Track(MENU_TRACK_RIGHTBUTTON, p->x, p->y, HWindow, NULL);
        SalamanderGUI->DestroyMenuPopup(popup);
    }
}


void CRendererWindow::change_font(int size, std::string font, bool initialization)
{
	hFont = CreateFont(
		-size, 0, 0, 0, // height = size px, width = auto
		FW_NORMAL, FALSE, FALSE, FALSE,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY,
		FF_MODERN | FIXED_PITCH,
        font.c_str());

    HDC hDC = GetDC(HWindow);
    HFONT oldFont = (HFONT)SelectObject(hDC, hFont);
    TEXTMETRIC tm;
    GetTextMetrics(hDC, &tm);
    fontWidth = tm.tmAveCharWidth;
    fontHeight = tm.tmHeight + tm.tmExternalLeading;

    UpdateScrollAttributes(initialization, initialization);

    SelectObject(hDC, oldFont);
    ReleaseDC(HWindow, hDC);
}

void CRendererWindow::print_lines(int first_line, int last_line, HDC hDC, TextModel* h)
{
    TEXTMETRIC tm;
    GetTextMetrics(hDC, &tm);
    int y_offset = 0;
    int x_chars = 0;
    int x_offset = 0;

	COLORREF default_fg = h->get_fg();
    for (int i = first_line; i <= last_line; i++)
    {
        // Print all lines
        std::vector<Token> tokens = h->get_line(i);
        if (tokens.size() == 0)
        {
            return;
        }

        if (display_line_numbers) {
            SetTextColor(hDC, default_fg);
            TextOut(hDC, 5 - (scrollPosX * tm.tmAveCharWidth), y_offset, std::to_string(i + 1).c_str(), std::to_string(i + 1).length());
        }
        
        for (int i = 0; i < tokens.size(); i++)
        {            
            int last_index = 0;
            SetTextColor(hDC, tokens[i].fg_color);
            for (int ii = 0; ii < tokens[i].token_len; ii++)
            {
                WCHAR c = *(tokens[i].start_ptr + ii);
                if (c == L'\t')
                {
                    TextOutW(hDC, /*10 + */(tm.tmAveCharWidth * (line_display_offset + 1) + (x_chars * tm.tmAveCharWidth)) - (scrollPosX * tm.tmAveCharWidth), /*10 + */y_offset, tokens[i].start_ptr + last_index, ii - last_index);
                    x_chars += ii - last_index;
                    x_chars += tab_size - (x_chars % tab_size);
                    last_index = ii + 1;
                }
                else if (ii == tokens[i].token_len - 1)
                {
                    int len = ii - last_index + 1;
                    int maxChars = (windowWidth / tm.tmAveCharWidth) + 1;
                    if (len > maxChars + scrollPosX) {
                        len = maxChars + scrollPosX;
                    }
                    TextOutW(hDC, /*10 + */(tm.tmAveCharWidth * (line_display_offset+1) + (x_chars * tm.tmAveCharWidth)) - (scrollPosX * tm.tmAveCharWidth),/* 10 */+ y_offset, tokens[i].start_ptr + last_index, len);
                    x_chars += tokens[i].token_len;
                }
            }
        }
        if (x_chars > Viewer->TextModelPtr->get_longest_line_length())
        {
            SCROLLINFO si = { sizeof(si), SIF_ALL };
            GetScrollInfo(HWindow, SB_HORZ, &si);
            si.nMax = x_chars + 1;
            si.nPage = windowWidth / (fontWidth != 0 ? fontWidth : 16);
            Viewer->TextModelPtr->set_longest_line_length(si.nMax);
            SetScrollInfo(HWindow, SB_HORZ, &si, TRUE);
            
        }
        // Move position to next line
        x_offset = 0;
        x_chars = 0;
        y_offset += tm.tmHeight;
    }
}



void CRendererWindow::invert_area(text_point start_point, text_point end_point, HDC hDC)
{
    if (start_point.y > end_point.y || (start_point.y == end_point.y) && start_point.x > end_point.x) {
        text_point tmp = start_point;
        start_point = end_point;
        end_point = tmp;
    }

    start_point.x += (1 + line_display_offset) - scrollPosX;
	end_point.x += (1 + line_display_offset) - scrollPosX;

	start_point.y -= scrollPosY;
	end_point.y -= scrollPosY;

    for (int line = start_point.y; line <= end_point.y; line++)
    {
        HRGN rgn;
        if (start_point.y == end_point.y)
        {
            rgn = CreateRectRgn(start_point.x * fontWidth, line * fontHeight, end_point.x * fontWidth, line * fontHeight + fontHeight);
        }
        else if (line == start_point.y) {
            rgn = CreateRectRgn(start_point.x * fontWidth, line * fontHeight, windowWidth, line * fontHeight + fontHeight);
        }
        else if (line == end_point.y)
        {
            rgn = CreateRectRgn((fontWidth * (line_display_offset + 1 - (scrollPosX < (line_display_offset + 1) ? scrollPosX : (line_display_offset + 1)))), line * fontHeight, end_point.x * fontWidth, line * fontHeight + fontHeight);
        }
        else
        {
            rgn = CreateRectRgn((fontWidth * (line_display_offset + 1 - (scrollPosX < (line_display_offset + 1) ? scrollPosX : (line_display_offset + 1)))), line * fontHeight, windowWidth, (end_point.y - 1) * fontHeight + fontHeight);
            line = end_point.y - 1;
        }
        InvertRgn(hDC, rgn);
        DeleteObject(rgn);
    }
}

LRESULT
CRendererWindow::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = HANDLES(BeginPaint(HWindow, &ps));
        HFONT oldFont = (HFONT)SelectObject(hDC, hFont);
        if (hDC != NULL)
        {
            if (Viewer->TextModelPtr != NULL)
            {
                RECT rc;
                GetClientRect(HWindow, &rc);                            // get size of client area
                COLORREF bgColor = Viewer->TextModelPtr->get_bg();     // background color
                HBRUSH hBrush = CreateSolidBrush(bgColor); 
                FillRect(hDC, &rc, hBrush);
                DeleteObject(hBrush); 
                SetBkColor(hDC, bgColor);

                print_lines(first_visible_line, last_visible_line, hDC, Viewer->TextModelPtr);

                if (selection_active)
                {
                    invert_area(s_start, s_end, hDC);
                }
            }
            SelectObject(hDC, oldFont);
        }
        HANDLES(EndPaint(HWindow, &ps));
        return 0;
    }
    case WM_SIZE: 
    {
        windowWidth = LOWORD(lParam);
        windowHeight = HIWORD(lParam);
        break;
    }
    case WM_VSCROLL:
    {
        SCROLLINFO si = {sizeof(si), SIF_ALL};
        GetScrollInfo(HWindow, SB_VERT, &si);

        int pos = si.nPos;

        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            si.nPos--;
            break;
        case SB_LINEDOWN:
            si.nPos++;
            break;
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
        case SB_TOP:
            si.nPos = 0;
            break;
        case SB_BOTTOM:
            si.nPos = si.nMax;
            break;
		case CUSTOM_FASTSCROLLUP:
			si.nPos -= CfgScrollSpeed;
			break;
		case CUSTOM_FASTSCROLLDOWN:
			si.nPos += CfgScrollSpeed;
			break;
        case CUSTOM_SCROLLREFRESH:
            si.nPos = lParam;
            break;
        }

        si.fMask = SIF_POS;
        SetScrollInfo(HWindow, SB_VERT, &si, TRUE);
        GetScrollInfo(HWindow, SB_VERT, &si);

        if (si.nPos != pos)
        {
            if (Viewer->TextModelPtr != NULL)
            {
                first_visible_line = si.nPos;
                last_visible_line = si.nPos + (windowHeight / fontHeight);
            }

            scrollPosY = si.nPos;
            if (Viewer->TextModelPtr->reparse_occured(first_visible_line, last_visible_line) && (LOWORD(wParam) == SB_LINEUP || LOWORD(wParam) == SB_THUMBPOSITION || LOWORD(wParam) == CUSTOM_SCROLLREFRESH))
            {
                InvalidateRect(HWindow, NULL, TRUE);
            }
            else
            {
                ScrollWindowEx(HWindow, 0, (pos - scrollPosY) * fontHeight, NULL, NULL, NULL, NULL, SW_INVALIDATE);
            }
        }
        return 0;
    }
    case WM_HSCROLL:
    {
        SCROLLINFO si = {sizeof(si), SIF_ALL};
        GetScrollInfo(HWindow, SB_HORZ, &si);

        int pos = si.nPos;

        switch (LOWORD(wParam))
        {
        case SB_LINELEFT:
            si.nPos--;
            break;
        case SB_LINERIGHT:
            si.nPos++;
            break;
        case SB_PAGELEFT:
            si.nPos -= si.nPage;
            break;
        case SB_PAGERIGHT:
            si.nPos += si.nPage;
            break;
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
        case CUSTOM_FASTSCROLLLEFT:
            si.nPos -= CfgScrollSpeed;
            break;
        case CUSTOM_FASTSCROLLRIGHT:
            si.nPos += CfgScrollSpeed;
            break;
        }

        si.fMask = SIF_POS;
        SetScrollInfo(HWindow, SB_HORZ, &si, TRUE);
        GetScrollInfo(HWindow, SB_HORZ, &si);

        if (si.nPos != pos)
        {
            
            scrollPosX = si.nPos;
            ScrollWindowEx(HWindow, (pos - scrollPosX)* fontWidth, 0, NULL, NULL, NULL, NULL, SW_INVALIDATE);
        }
        return 0;
    }

    case WM_MOUSEWHEEL:
    {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (GET_KEYSTATE_WPARAM(wParam) & MK_CONTROL)
        {
            if (delta > 0)
                font_size--;
            else
                font_size++;
            if (font_size >= 0)
            {
                font_size = -1;
            }
            else
            {
                change_font(font_size, font_name, false);
                InvalidateRect(HWindow, NULL, TRUE);
            }
        }
        else
        {
            if (GetKeyState(VK_MENU) & 0x8000)
            {
                if (delta > 0)
                    SendMessage(HWindow, WM_HSCROLL, CUSTOM_FASTSCROLLLEFT, 0);
                else
					SendMessage(HWindow, WM_HSCROLL, CUSTOM_FASTSCROLLRIGHT, 0);
            }
            else
            {
                if (delta > 0)
                    SendMessage(HWindow, WM_VSCROLL, CUSTOM_FASTSCROLLUP, 0);
                else
					SendMessage(HWindow, WM_VSCROLL, CUSTOM_FASTSCROLLDOWN, 0);
            }
        }
        return 0;
    }
    case WM_COMMAND:
    {
        SendMessage(Viewer->HWindow, WM_COMMAND, wParam, lParam); // forward commands from context menu
        break;
    }
    case WM_LBUTTONDOWN:
    {
        SetCapture(HWindow);
        CancelSelection();
        __int64 x = (short)LOWORD(lParam);
        __int64 y = (short)HIWORD(lParam);
        text_point cursor_pos;
        cursor_pos.x = (x / fontWidth) + scrollPosX - (1 + line_display_offset);
        cursor_pos.y = (y / fontHeight) + scrollPosY;
        ApplyCursorSelectionBounds(&cursor_pos);
        
        StartSelection(cursor_pos);
        lmb_down = true;
        break;
    }
    case WM_LBUTTONUP:
    {
        lmb_down = false;
        KillTimer(HWindow, 1);
        ReleaseCapture();
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (lmb_down) {
            __int64 x = (short)LOWORD(lParam);
            __int64 y = (short)HIWORD(lParam);
            text_point cursor_pos;
            cursor_pos.x = (x / fontWidth) + scrollPosX - (1 + line_display_offset);
            cursor_pos.y = (y / fontHeight) + scrollPosY;
            ApplyCursorSelectionBounds(&cursor_pos);

            ExtendSelection(cursor_pos);

            RECT rc;
            GetClientRect(HWindow, &rc);

            if (y < 0 || y > rc.bottom)
            {
                SetTimer(HWindow, 1, 30, NULL);
            }
            else
            {
                KillTimer(HWindow, 1);
            }
        }
        break;

    }
    case WM_TIMER:
    {
        if (wParam == 1)
        {
            POINT p;
            GetCursorPos(&p);
            ScreenToClient(HWindow, &p);
            __int64 x = (short)p.x;
            __int64 y = (short)p.y;
            text_point cursor_pos;
            cursor_pos.x = (x / fontWidth) + scrollPosX - (1 + line_display_offset);
            cursor_pos.y = (y / fontHeight) + scrollPosY;
			ApplyCursorSelectionBounds(&cursor_pos);

            ExtendSelection(cursor_pos);
			if (y < windowHeight/2)
			{
				SendMessage(HWindow, WM_VSCROLL, SB_LINEUP, 0);
			}
			else
			{
				SendMessage(HWindow, WM_VSCROLL, SB_LINEDOWN, 0);
			}
        }
        break;
    }
    case WM_RBUTTONDOWN:
    {
        POINT p;
        GetCursorPos(&p);
        OnContextMenu(&p);
        break;
    }
    case WM_SETCURSOR:
    {
        if (LOWORD(lParam) != HTVSCROLL && LOWORD(lParam) != HTHSCROLL) {
            SetCursor(LoadCursor(NULL, IDC_IBEAM));
            return TRUE;
        }
    }
    }
    return CWindow::WindowProc(uMsg, wParam, lParam);
}

void CRendererWindow::StartSelection(text_point start)
{
    s_start = start;
    s_end = start;
}

void CRendererWindow::CancelSelection()
{
    if (selection_active)
    {
        rm_s_start = s_start;
        rm_s_end = s_end;
        selection_active = false;

        HDC hDC = GetDC(HWindow);
        invert_area(rm_s_start, rm_s_end, hDC);
        ReleaseDC(HWindow, hDC);
    }
}


void CRendererWindow::ExtendSelection(text_point new_end)
{
    if (new_end.x != s_end.x || new_end.y != s_end.y)
    {
		s_new_end = new_end;
        selection_active = true;

        HDC hDC = GetDC(HWindow);
        invert_area(s_end, s_new_end, hDC);
        ReleaseDC(HWindow, hDC);
		s_end = s_new_end;

        if (s_start.x == s_end.x && s_start.y == s_end.y)
        {
            selection_active = false;
        }
    }
}

void CRendererWindow::NewSelection(text_point start, text_point end)
{
    CancelSelection();
    s_start = start;
    s_end = s_start;
    s_new_end = end;
    selection_active = true;

    HDC hDC = GetDC(HWindow);
    invert_area(s_end, s_new_end, hDC);
    ReleaseDC(HWindow, hDC);

    s_end = s_new_end;
}

void CRendererWindow::ModifySelectionByKeyboard(WPARAM wParam)
{
    if (selection_active)
    {
        text_point new_end = s_end;
        switch (wParam)
        {
        case VK_LEFT:
			if (new_end.x == 0 && new_end.y > 0)
			{
				new_end.y--;
				new_end.x = Viewer->TextModelPtr->get_line_length(new_end.y);
			}
            else
            {
                new_end.x--;
            }
            break;
        case VK_RIGHT:
			if (new_end.x >= Viewer->TextModelPtr->get_line_length(new_end.y) && new_end.y < Viewer->TextModelPtr->get_line_count() - 1)
			{
				new_end.y++;
				new_end.x = 0;
			}
            else
            {
                new_end.x++;
            }
            break;
        case VK_UP:
            new_end.y--;
            break;
        case VK_DOWN:
            new_end.y++;
            break;
        default:
            return;
        }
        ApplyCursorSelectionBounds(&new_end);
        ExtendSelection(new_end);

        if (new_end.y > last_visible_line - 2 && wParam == VK_DOWN)
        {
			SendMessage(HWindow, WM_VSCROLL, SB_LINEDOWN, 0);
		}
        else if (new_end.y < first_visible_line + 2 && wParam == VK_UP)
        {
            SendMessage(HWindow, WM_VSCROLL, SB_LINEUP, 0);
        }
    }
}

bool CRendererWindow::GetSelectionContent(WCHAR** string, int* len)
{
	if (selection_active && Viewer->TextModelPtr != NULL && Viewer->TextModelPtr->process_selection(string, len, s_start, s_end))
	{
		return true;
	}
    else
    {
        return false;
    }
}

void CRendererWindow::ApplyCursorSelectionBounds(text_point* cursor_pos)
{
    if (cursor_pos->y < 0) {
        cursor_pos->y = 0;
    }

    if (cursor_pos->y > Viewer->TextModelPtr->get_line_count() - 1)
    {
        cursor_pos->y = Viewer->TextModelPtr->get_line_count();
        cursor_pos->x = 0;
    }
    else 
    {
        if (cursor_pos->x < 0) 
        {
            cursor_pos->x = 0;
        }
        else 
        {
			int line_length = Viewer->TextModelPtr->get_line_length(cursor_pos->y);
            if (cursor_pos->x > line_length)
            {
				cursor_pos->x = line_length;
            }
        }
    }
}

void CRendererWindow::UpdateScrollAttributes(bool initialization, bool update_window_size)
{
    if (update_window_size)
    {
        RECT renderer_r;
        GetClientRect(HWindow, &renderer_r);
        windowHeight = renderer_r.bottom - renderer_r.top;
        windowWidth = renderer_r.right - renderer_r.left;
    }
    
    SCROLLINFO si_vert = { sizeof(si_vert), SIF_ALL };
    GetScrollInfo(HWindow, SB_VERT, &si_vert);
    if (initialization)
    {
        si_vert.nPos = 0;
        si_vert.nMin = 0;
        si_vert.nMax = Viewer->TextModelPtr->get_line_count();
    }
    si_vert.nPage = windowHeight / (fontHeight != 0 ? fontHeight : 19);
    SetScrollInfo(HWindow, SB_VERT, &si_vert, TRUE);

    SCROLLINFO si_horz = { sizeof(si_horz), SIF_ALL };
    GetScrollInfo(HWindow, SB_HORZ, &si_horz);
    if (initialization)
    {
        si_horz.nPos = 0;
        si_horz.nMin = 0;
        si_horz.nMax = Viewer->TextModelPtr->get_longest_line_length() + 1 + line_display_offset;
    }
    si_horz.nPage = windowWidth / (fontWidth != 0 ? fontWidth : 16);
    SetScrollInfo(HWindow, SB_HORZ, &si_horz, TRUE);

    GetScrollInfo(HWindow, SB_VERT, &si_vert);
    GetScrollInfo(HWindow, SB_HORZ, &si_horz);
    scrollPosY = si_vert.nPos;
    scrollPosX = si_horz.nPos;

    first_visible_line = si_vert.nPos;
    last_visible_line = si_vert.nPos + (windowHeight / (fontHeight != 0 ? fontHeight : 19));
}

void CRendererWindow::ToggleLineNumbering()
{

    display_line_numbers = !display_line_numbers;
    if (display_line_numbers)
    {
        line_display_offset = std::to_string(Viewer->TextModelPtr->get_line_count() + 1).length() + 1;
    }
    else
    {
        line_display_offset = 0;
    }

    SCROLLINFO si_horz = { sizeof(si_horz), SIF_ALL };
    GetScrollInfo(HWindow, SB_HORZ, &si_horz);
    si_horz.nMax = Viewer->TextModelPtr->get_longest_line_length() + 1 + line_display_offset;
    SetScrollInfo(HWindow, SB_HORZ, &si_horz, TRUE);
    GetScrollInfo(HWindow, SB_HORZ, &si_horz);
    scrollPosX = si_horz.nPos;

    InvalidateRect(HWindow, NULL, TRUE);
}

//
// ****************************************************************************
// CViewerWindow
//

CViewerWindow::CViewerWindow(int enumFilesSourceUID, int enumFilesCurrentIndex) : CWindow(ooStatic)
{
    Lock = NULL;
    Name[0] = 0;
    Renderer.Viewer = this;
    HRebar = NULL;
    MainMenu = NULL;
    MenuBar = NULL;
    ToolBar = NULL;
    HGrayToolBarImageList = NULL;
    HHotToolBarImageList = NULL;
    TextModelPtr = NULL;

    EnumFilesSourceUID = enumFilesSourceUID;
    EnumFilesCurrentIndex = enumFilesCurrentIndex;

    ZeroMemory(Enablers, sizeof(Enablers));
}

CViewerWindow::~CViewerWindow()
{
    if (TextModelPtr != NULL)
    {
        delete TextModelPtr;
        TextModelPtr = NULL;
    }
	if (SearcherPtr != NULL)
	{
        delete SearcherPtr;
		SearcherPtr = NULL;
	}
}

BOOL CViewerWindow::IsMenuBarMessage(CONST MSG* lpMsg)
{
    if (MenuBar == NULL)
        return FALSE;
    return MenuBar->IsMenuBarMessage(lpMsg);
}

BOOL CViewerWindow::InitializeGraphics()
{
    HBITMAP hTmpMaskBitmap;
    HBITMAP hTmpGrayBitmap;
    HBITMAP hTmpColorBitmap;

    hTmpColorBitmap = HANDLES(LoadBitmap(DLLInstance, MAKEINTRESOURCE(SalamanderGeneral->CanUse256ColorsBitmap() ? IDB_TOOLBAR256 : IDB_TOOLBAR16)));
    BOOL ok = SalamanderGUI->CreateGrayscaleAndMaskBitmaps(hTmpColorBitmap, RGB(255, 0, 255),
                                                           hTmpGrayBitmap, hTmpMaskBitmap);
    if (ok) // ziskane handly bitmap vlozime do HANDLES (ukazka rucniho vkladani; jednodussi
    {       // v teto situaci (DeleteObject okamzite nasleduje) je pouzit u DeleteObject makro NOHANDLES)
        HANDLES_ADD(__htBitmap, __hoCreateDIBitmap, hTmpGrayBitmap);
        HANDLES_ADD(__htBitmap, __hoCreateDIBitmap, hTmpMaskBitmap);
    }
    HHotToolBarImageList = ImageList_Create(16, 16, ILC_MASK | ILC_COLORDDB, IDX_TB_COUNT, 1);
    HGrayToolBarImageList = ImageList_Create(16, 16, ILC_MASK | ILC_COLORDDB, IDX_TB_COUNT, 1);
    ImageList_Add(HHotToolBarImageList, hTmpColorBitmap, hTmpMaskBitmap);
    ImageList_Add(HGrayToolBarImageList, hTmpGrayBitmap, hTmpMaskBitmap);
    HANDLES(DeleteObject(hTmpMaskBitmap));
    HANDLES(DeleteObject(hTmpGrayBitmap));
    HANDLES(DeleteObject(hTmpColorBitmap));
    return TRUE;
}

BOOL CViewerWindow::ReleaseGraphics()
{
    if (HHotToolBarImageList != NULL)
        ImageList_Destroy(HHotToolBarImageList);
    if (HGrayToolBarImageList != NULL)
        ImageList_Destroy(HGrayToolBarImageList);
    return TRUE;
}

void CViewerWindow::UpdateEnablers()
{
    //  Enablers[vweCut] = Renderer.Database.IsOpened();
    //  Enablers[vwePaste] = lstrcmpi(Renderer.Database.GetParserName(), "csv") == 0;
    if (ToolBar != NULL)
        ToolBar->UpdateItemsState();
}

BOOL CViewerWindow::FillToolBar()
{
    //TLBI_ITEM_INFO2 tii;
    //tii.Mask = TLBI_MASK_IMAGEINDEX | TLBI_MASK_ID | TLBI_MASK_ENABLER | TLBI_MASK_STYLE;

    //ToolBar->SetImageList(HGrayToolBarImageList);
    //ToolBar->SetHotImageList(HHotToolBarImageList);

    //int i;
    //for (i = 0; ToolBarButtons[i].ImageIndex != IDX_TB_TERMINATOR; i++)
    //{
    //    if (ToolBarButtons[i].ImageIndex == IDX_TB_SEPARATOR)
    //    {
    //        tii.Style = TLBI_STYLE_SEPARATOR;
    //    }
    //    else
    //    {
    //        if (ToolBarButtons[i].ImageIndex == IDX_TB_FILTER)
    //            tii.Style = TLBI_STYLE_DROPDOWN | TLBI_STYLE_WHOLEDROPDOWN;
    //        else
    //            tii.Style = 0;
    //        tii.ImageIndex = ToolBarButtons[i].ImageIndex;
    //        tii.ID = ToolBarButtons[i].ID;
    //        if (ToolBarButtons[i].Enabler == vweAlwaysEnabled)
    //            tii.Enabler = NULL;
    //        else
    //            tii.Enabler = Enablers + ToolBarButtons[i].Enabler;
    //    }
    //    ToolBar->InsertItem2(0xFFFFFFFF, TRUE, &tii);
    //}

    //// obehne enablery
    //ToolBar->UpdateItemsState();
    return TRUE;
    
}

BOOL CViewerWindow::InsertMenuBand()
{
    REBARBANDINFO rbbi;
    ZeroMemory(&rbbi, sizeof(rbbi));

    rbbi.cbSize = sizeof(REBARBANDINFO);
    rbbi.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID;
    rbbi.cxMinChild = MenuBar->GetNeededWidth();
    rbbi.cyMinChild = MenuBar->GetNeededHeight();
    rbbi.fStyle = RBBS_NOGRIPPER;
    rbbi.hwndChild = MenuBar->GetHWND();
    rbbi.wID = BANDID_MENU;
    SendMessage(HRebar, RB_INSERTBAND, (WPARAM)0, (LPARAM)&rbbi);
    return TRUE;
}

BOOL CViewerWindow::InsertToolBarBand()
{
    REBARBANDINFO rbbi;
    ZeroMemory(&rbbi, sizeof(rbbi));

    rbbi.cbSize = sizeof(REBARBANDINFO);
    rbbi.fMask = RBBIM_SIZE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID;
    rbbi.cxMinChild = ToolBar->GetNeededWidth();
    rbbi.cyMinChild = ToolBar->GetNeededHeight();
    rbbi.cx = 10000;
    rbbi.fStyle = RBBS_NOGRIPPER;
    rbbi.hwndChild = ToolBar->GetHWND();
    rbbi.wID = BANDID_TOOLBAR;
    SendMessage(HRebar, RB_INSERTBAND, (WPARAM)2, (LPARAM)&rbbi);
    return TRUE;
}

void CViewerWindow::LayoutWindows()
{
    RECT r;
    GetClientRect(HWindow, &r);
    SendMessage(HWindow, WM_SIZE, SIZE_RESTORED,
                MAKELONG(r.right - r.left, r.bottom - r.top));
}

void FillMenuFilter(CGUIMenuPopupAbstract* popup, int cmdFirst, int filterCount)
{
    popup->RemoveAllItems();

    MENU_ITEM_INFO mi;
    memset(&mi, 0, sizeof(mi));
    mi.Mask = MENU_MASK_TYPE | MENU_MASK_ID | MENU_MASK_STRING | MENU_MASK_STATE;
    mi.Type = MENU_TYPE_STRING;
    mi.State = 0;

    char buff[MAX_PATH + 3];
    mi.String = buff;
    int index = 0;
    while (index < filterCount)
    {
        wsprintf(buff, "&%d %s %d", index < 9 ? index + 1 : 0, LoadStr(IDS_FILTER), index + 1);
        mi.ID = cmdFirst + index;
        popup->InsertItem(-1, TRUE, &mi);
        index++;
    }
}


LRESULT
CViewerWindow::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        InitializeGraphics();
        DragAcceptFiles(HWindow, TRUE); // drag&drop open file
        MainMenu = SalamanderGUI->CreateMenuPopup();
        if (MainMenu == NULL)
            return -1;
        ToolBar = SalamanderGUI->CreateToolBar(HWindow);
        if (ToolBar == NULL)
            return -1;
        MainMenu->LoadFromTemplate(HLanguage, MenuTemplate, Enablers, HGrayToolBarImageList, HHotToolBarImageList);
        MenuBar = SalamanderGUI->CreateMenuBar(MainMenu, HWindow);
        if (MenuBar == NULL)
            return -1;

        RECT r;
        GetClientRect(HWindow, &r);
        HRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, "",
                                WS_VISIBLE | /*WS_BORDER |  */ WS_CHILD |
                                    WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
                                    RBS_VARHEIGHT | CCS_NODIVIDER |
                                    RBS_BANDBORDERS | CCS_NOPARENTALIGN |
                                    RBS_AUTOSIZE,
                                0, 0, r.right, r.bottom, // dummy
                                HWindow, (HMENU)0, DLLInstance, NULL);

        // nechceme vizualni styly pro rebar
        SalamanderGUI->DisableWindowVisualStyles(HRebar);

        Renderer.CreateEx(WS_EX_STATICEDGE /*WS_EX_CLIENTEDGE*/,
                          CWINDOW_CLASSNAME2,
                          "",
                          WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPSIBLINGS,
                          0,
                          0,
                          0,
                          0,
                          HWindow,
                          NULL,
                          DLLInstance,
                          &Renderer);

        MenuBar->CreateWnd(HRebar);
        InsertMenuBand();
        ToolBar->CreateWnd(HRebar);
        FillToolBar();

        HWND hCombo = CreateWindowEx(
            0,
            WC_COMBOBOX,
            "",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
            0, 0, 150, 200,
            HRebar,
            (HMENU)IDC_SELECTGRAMMAR,
            DLLInstance,
            NULL
        );

        if (hCombo != NULL)
        {
            //HFONT hFont = (HFONT)SendMessage(MenuBar->GetHWND(), WM_GETFONT, 0, 0);
            //SendMessage(hCombo, WM_SETFONT, (WPARAM)hFont, TRUE);
            std::vector<std::string> grammars = removeFileExtension(getFilesWithExtension(getCurrentDllPath() + "\\grammars", "json"));

			for (int i = 0; i < grammars.size(); i++)
			{
				SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)grammars[i].c_str());
			}

            REBARBANDINFO rbbiCombo;
            ZeroMemory(&rbbiCombo, sizeof(rbbiCombo));
            rbbiCombo.cbSize = sizeof(REBARBANDINFO);
            //rbbiCombo.fMask = RBBIM_SIZE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID;
            rbbiCombo.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID;
            rbbiCombo.hwndChild = hCombo;
            rbbiCombo.cxMinChild = 150;
            rbbiCombo.cyMinChild = 24;
            //rbbiCombo.cx = 10000;
            rbbiCombo.fStyle = RBBS_NOGRIPPER;
            rbbiCombo.wID = BANDID_COMBO;
            SendMessage(HRebar, RB_INSERTBAND, (WPARAM)1, (LPARAM)&rbbiCombo);
            HCombo = hCombo;
        }

        InsertToolBarBand();
        ViewerWindowQueue.Add(new CWindowQueueItem(HWindow));
        break;
    }

    case WM_ERASEBKGND:
    {
        return TRUE;
    }

    case WM_DESTROY:
    {
        DragAcceptFiles(HWindow, FALSE); // drag&drop open file
        if (CfgSavePosition)
        {
            CfgWindowPlacement.length = sizeof(WINDOWPLACEMENT);
            GetWindowPlacement(HWindow, &CfgWindowPlacement);
        }
        if (MenuBar != NULL)
        {
            SalamanderGUI->DestroyMenuBar(MenuBar);
            MenuBar = NULL;
        }
        if (MainMenu != NULL)
        {
            SalamanderGUI->DestroyMenuPopup(MainMenu);
            MainMenu = NULL;
        }
        if (ToolBar != NULL)
        {
            SalamanderGUI->DestroyToolBar(ToolBar);
            ToolBar = NULL;
        }

        if (Renderer.HWindow != NULL)
            DestroyWindow(Renderer.HWindow);

        ViewerWindowQueue.Remove(HWindow);

        if (Lock != NULL)
        {
            SetEvent(Lock);
            Lock = NULL;
        }
        ReleaseGraphics();
        PostQuitMessage(0);
        break;
    }

    case WM_DROPFILES: // drag&drop open file
    {
        UINT drag;
        char path[MAX_PATH];

        drag = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0); // kolik souboru nam hodili
        if (drag > 0)
        {
            DragQueryFile((HDROP)wParam, 0, path, MAX_PATH);
            OpenFile(path, FALSE, CurrentEncodingCodePage);
        }
        DragFinish((HDROP)wParam);
        break;
    }

    case WM_NOTIFY:
    {
        LPNMHDR lphdr = (LPNMHDR)lParam;
        if (lphdr->code == RBN_AUTOSIZE)
        {
            LayoutWindows();
            return 0;
        }
        break;
    }

    case WM_SIZE:
    {
        if (Renderer.HWindow != NULL && HRebar != NULL && MenuBar != NULL)
        {
            RECT r;
            GetClientRect(HWindow, &r);

            RECT rebarRect;
            GetWindowRect(HRebar, &rebarRect);
            int rebarHeight = rebarRect.bottom - rebarRect.top;

            HDWP hdwp = HANDLES(BeginDeferWindowPos(2));
            if (hdwp != NULL)
            {
                // + 4: pri zvetsovani sirky okna mi nechodilo prekreslovani poslednich 4 bodu
                // v rebaru; ani po nekolika hodinach jsem nenasel pricinu; v Salamu to slape;
                // zatim to resim takto; treba si casem vzpomenu, kde je problem
                hdwp = HANDLES(DeferWindowPos(hdwp, HRebar, NULL,
                                              0, 0, r.right + 4, rebarHeight,
                                              SWP_NOACTIVATE | SWP_NOZORDER));

                hdwp = HANDLES(DeferWindowPos(hdwp, Renderer.HWindow, NULL,
                                              0, rebarHeight, r.right, r.bottom - rebarHeight,
                                              SWP_NOACTIVATE | SWP_NOZORDER));
                HANDLES(EndDeferWindowPos(hdwp));
            }

            Renderer.UpdateScrollAttributes(false, true);
            InvalidateRect(Renderer.HWindow, NULL, TRUE);
        }
        break;
    }

    case WM_SYSCOLORCHANGE:
    {
        // tady by se mely premapovat barvy
        TRACE_I("CViewerWindow::WindowProc - WM_SYSCOLORCHANGE");
        break;
    }

    case WM_USER_VIEWERCFGCHNG:
    {
        // tady by se mely projevit zmeny v konfiguraci pluginu
        TRACE_I("CViewerWindow::WindowProc - config has changed");
		if (Renderer.Viewer->TextModelPtr->update_theme() || Renderer.font_size != CfgDefaultFontSize || Renderer.font_name != CfgSelectedFont || Renderer.tab_size != CfgTabSize || Renderer.display_line_numbers != CfgDefaultLineNumbering)
		{
			if (Renderer.display_line_numbers != CfgDefaultLineNumbering)
			{
				Renderer.ToggleLineNumbering();
			}
			Renderer.font_size = CfgDefaultFontSize;
			Renderer.font_name = CfgSelectedFont;
            Renderer.tab_size = CfgTabSize;
			Renderer.change_font(Renderer.font_size, Renderer.font_name, false);
            InvalidateRect(Renderer.HWindow, NULL, TRUE);
		}
        InvalidateRect(HWindow, NULL, TRUE);
        return 0;
    }

    case WM_ACTIVATE:
    {
        if (!LOWORD(wParam))
        {
            // hlavni okno pri prepnuti z viewru nebude delat refresh
            SalamanderGeneral->SkipOneActivateRefresh();
        }
        break;
    }

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        if (wParam == VK_ESCAPE)
            PostMessage(HWindow, WM_CLOSE, 0, 0);

        BOOL ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        BOOL shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        BOOL altPressed = (GetKeyState(VK_MENU) & 0x8000) != 0;

        if ((wParam == VK_F10 && shiftPressed || wParam == VK_APPS))
        {
            POINT p;
            p.x = 0;
            p.y = 0;
            ClientToScreen(Renderer.HWindow, &p);
            Renderer.OnContextMenu(&p);
            return 0;
        }
        if (wParam == VK_SPACE || wParam == VK_BACK)
        {
            NextPrevFile(wParam == VK_BACK);
        }

        if (!ctrlPressed && !altPressed) 
        {
            switch (wParam)
            {
            case VK_PRIOR:
			case VK_NEXT:
            {
                SendMessage(Renderer.HWindow, WM_VSCROLL, MAKEWPARAM(wParam == VK_PRIOR ? SB_PAGEUP : SB_PAGEDOWN, 0), 0);
                break;
            }
            case VK_UP:
			case VK_DOWN:
            {
                if (shiftPressed)
                {
                    Renderer.ModifySelectionByKeyboard(wParam);
                }
                else
                {
                    SendMessage(Renderer.HWindow, WM_VSCROLL, MAKEWPARAM(wParam == VK_UP ? SB_LINEUP : SB_LINEDOWN, 0), 0);
                }
                break;
            }
			case VK_LEFT:
            case VK_RIGHT:
            {
                if (shiftPressed)
                {
					Renderer.ModifySelectionByKeyboard(wParam);
                }
                else
                {
                    SendMessage(Renderer.HWindow, WM_HSCROLL, MAKEWPARAM(wParam == VK_LEFT ? SB_LINELEFT : SB_LINERIGHT, 0), 0);
                }
                break;
            }
            case VK_HOME:
            case VK_END:
            {
                SendMessage(Renderer.HWindow, WM_VSCROLL, MAKEWPARAM(wParam == VK_HOME ? SB_TOP : SB_BOTTOM, 0), 0);
                break;
            }
            
            case VK_F3:
            {
                search_match result = SearcherPtr->continue_search();
                ProcessSearchResult(result);
                break;
            }
            case VK_F11:
            {
                ToggleFullscreen();
                break;
            }
            default:
            {
                break;
            }
            }
        }
        break;
    }

    case WM_USER_INITMENUPOPUP:
    {
        CGUIMenuPopupAbstract* popup = (CGUIMenuPopupAbstract*)wParam;
        WORD popupID = HIWORD(lParam);
        switch (popupID)
        {
        case CML_VIEWER_FILTER:
        {
            FillMenuFilter(popup, CM_FILTER_FIRST, FILTER_COUNT);
            break;
        }
        }
        return 0;
    }

    case WM_USER_TBDROPDOWN:
    {
        if (ToolBar != NULL && ToolBar->GetHWND() == (HWND)wParam)
        {
            int index = (int)lParam;
            TLBI_ITEM_INFO2 tii;
            tii.Mask = TLBI_MASK_ID;
            if (!ToolBar->GetItemInfo2(index, TRUE, &tii))
                return 0;

            RECT r;
            ToolBar->GetItemRect(index, r);

            switch (tii.ID)
            {
            case CM_VIEWER_FILTER:
            {
                CGUIMenuPopupAbstract* popup = SalamanderGUI->CreateMenuPopup();
                if (popup != NULL)
                {
                    FillMenuFilter(popup, CM_FILTER_FIRST, FILTER_COUNT);

                    // using windows popup menu
                    HMENU hMenu = CreatePopupMenu();
                    popup->FillMenuHandle(hMenu);
                    TPMPARAMS tpm;
                    tpm.cbSize = sizeof(tpm);
                    tpm.rcExclude = r;
                    DWORD flags = TPM_LEFTALIGN | TPM_VERTICAL;
                    TrackPopupMenuEx(hMenu, flags, r.left, r.bottom, HWindow, &tpm);
                    DestroyMenu(hMenu);

                    // using Salamander popup menu
                    //               popup->Track(0, r.left, r.bottom, HWindow, &r);

                    SalamanderGUI->DestroyMenuPopup(popup);
                }
                break;
            }
            }
        }
        return 0;
    }

    case WM_USER_SETTINGCHANGE:
    {
        if (MenuBar != NULL)
            MenuBar->SetFont();
        if (ToolBar != NULL)
            ToolBar->SetFont();
        return 0;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) >= CM_FILTER_FIRST && LOWORD(wParam) <= CM_FILTER_LAST)
        {
            char buff[100];
            wsprintf(buff, "%s %d", LoadStr(IDS_FILTER), LOWORD(wParam) - CM_FILTER_FIRST + 1);
            SalamanderGeneral->SalMessageBox(HWindow, buff, LoadStr(IDS_PLUGINNAME), MB_ICONINFORMATION | MB_OK);
            return 0;
        }

        switch (LOWORD(wParam))
        {
        case CM_VIEWER_OPEN:
        {
            char file[MAX_PATH];
            file[0] = 0;
            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = HWindow;
            char filterBuf[100];
            lstrcpyn(filterBuf, LoadStr(IDS_DMV_FILES_FILTER), 100);
            char* s = filterBuf;
            ofn.lpstrFilter = s;
            while (*s != 0) // vytvoreni double-null terminated listu
            {
                if (*s == '|')
                    *s = 0;
                s++;
            }
            ofn.lpstrFile = file;
            ofn.nMaxFile = MAX_PATH;
            ofn.nFilterIndex = 1;
            char curDir[MAX_PATH];
            lstrcpyn(curDir, Name, MAX_PATH);
            SalamanderGeneral->CutDirectory(curDir);
            ofn.lpstrInitialDir = curDir[0] != 0 ? curDir : NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

            if (SalamanderGeneral->SafeGetOpenFileName(&ofn))
            {
                std::string fileNameStr(file);
                fileNameStr += "- SyntaxView";
                SetWindowText(HWindow, fileNameStr.c_str());
                OpenFile(file, FALSE, CurrentEncodingCodePage);
            }
            break;
        }

        case CM_VIEWER_COPY:
        {
            WCHAR* string;
            int len;
            if (Renderer.GetSelectionContent(&string, &len))
            {
                WCHAR* clipboardString = (WCHAR*)malloc((len + 1) * sizeof(WCHAR));
                for (int i = 0; i < len; i++)
				{
					if (string[i] == L'\n' && i+1 < len &&string[i+1] == L'\r')
					{
                        clipboardString[i] = L'\r';
                        clipboardString[i+1] = L'\n';
                        i++;
					}
					else
					{
                        clipboardString[i] = string[i];
					}
				}

				SalamanderGeneral->CopyTextToClipboardW(clipboardString, len, FALSE, HWindow);
                free(clipboardString);
                
            }
            break;
        }
        case CM_VIEWER_FULLSCREEN:
        {
            ToggleFullscreen();
			break;
        }
        case CM_VIEWER_EXIT:
        {
            PostMessage(HWindow, WM_CLOSE, 0, 0);
            break;
        }

        case CM_VIEWER_CFG:
        {
            OnConfiguration(HWindow);
            break;
        }

        case CM_VIEWER_ABOUT:
        {
            OnAbout(HWindow);
            break;
        }
        case CM_VIEWER_GOTOLINE:
        {
            GotoLineDlg();
            break;
        }
		case CM_VIEWER_LINENUMBERING:
		{
            Renderer.ToggleLineNumbering();
            break;
        }
        case CM_VIEWER_SELECTALL:
        {
			Renderer.NewSelection({ 0, 0 }, { 0, (long) TextModelPtr->get_line_count() });    //todo conversion
            break;
        }
        case CM_VIEWER_FIND:
        {
            SearchDlg();
            break;
        }
        case CM_VIEWER_FINDNEXT:
        {
            search_match result = SearcherPtr->find_next();     // todo tu aj tu potom historicke searche, vzdy aj pri f3 aj toto sa nieco snazi
			ProcessSearchResult(result);
            break;
        }
        case CM_VIEWER_FINDPREV:
        {
            search_match result = SearcherPtr->find_prev();
			ProcessSearchResult(result);
            break;
        }
        case CM_VIEWER_ANSI:
        {
            OpenFile(Name, FALSE, CP_ACP);
			CheckEncodingMenuItem(CM_VIEWER_ANSI);
            break;
        }
        case CM_VIEWER_UTF8:
        {
            OpenFile(Name, FALSE, CP_UTF8);
			CheckEncodingMenuItem(CM_VIEWER_UTF8);
            break;
        }
        case CM_VIEWER_OEM:
        {
            OpenFile(Name, FALSE, CP_OEMCP);
			CheckEncodingMenuItem(CM_VIEWER_OEM);
            break;
        }
        case CM_VIEWER_CP852:
        {
            OpenFile(Name, FALSE, 852);
			CheckEncodingMenuItem(CM_VIEWER_CP852);
            break;
        }
        case CM_VIEWER_CP1250:
        {
            OpenFile(Name, FALSE, 1250);
			CheckEncodingMenuItem(CM_VIEWER_CP1250);
            break;
        }
        case CM_VIEWER_CP1252:
        {
            OpenFile(Name, FALSE, 1252);
			CheckEncodingMenuItem(CM_VIEWER_CP1252);
            break;
        }
        case CM_VIEWER_LATIN1:
        {
            OpenFile(Name, FALSE, 28591);
			CheckEncodingMenuItem(CM_VIEWER_LATIN1);
            break;
        }
        case CM_VIEWER_LATIN2:
        {
            OpenFile(Name, FALSE, 28592);
			CheckEncodingMenuItem(CM_VIEWER_LATIN2);
            break;
        }
		case CM_VIEWER_NEXTFILE:
		{
			NextPrevFile(false);
			break;
		}
		case CM_VIEWER_PREVFILE:
		{
			NextPrevFile(true);
			break;
		}
        case CM_VIEWER_OPENEDITOR:
        {
            std::wstring wName;
            wName.resize(MultiByteToWideChar(CP_UTF8, 0, Name, -1, nullptr, 0) - 1);

            MultiByteToWideChar(CP_UTF8, 0, Name, -1, (LPWSTR)wName.data(), (int)wName.size() + 1);

            OPENASINFO info = {};
            info.pcszFile = wName.c_str();
            info.oaifInFlags = OAIF_ALLOW_REGISTRATION | OAIF_EXEC;

            SHOpenWithDialog(NULL, &info);
        }
        case IDC_SELECTGRAMMAR:
        {
            switch (HIWORD(wParam))
            {
            case CBN_SELCHANGE:
            {
                int sel = (int)SendMessage((HWND)HCombo, CB_GETCURSEL, 0, 0);
                if (sel != CB_ERR)
                {
                    char buf[256];
                    SendMessage((HWND)HCombo, CB_GETLBTEXT, sel, (LPARAM)buf);
                    std::string grammarFile(buf);
					grammarFile += ".json";
                    if (TextModelPtr->update_grammar(grammarFile))
                    {
                        InvalidateRect(Renderer.HWindow, NULL, TRUE);
                    }
                }
                break;
            }
            case CBN_CLOSEUP:
            {
                SetFocus(HWindow);
                break;
            }
            }
            break;
        }
        }
        return 0;
    }

    case WM_USER_TBGETTOOLTIP:
    {
        //HWND hToolBar = (HWND)wParam;
        //if (ToolBar != NULL && hToolBar == ToolBar->GetHWND())
        //{
        //    TOOLBAR_TOOLTIP* tt = (TOOLBAR_TOOLTIP*)lParam;
        //    lstrcpy(tt->Buffer, LoadStr(ToolBarButtons[tt->Index].ToolTipResID));
        //    SalamanderGUI->PrepareToolTipText(tt->Buffer, FALSE);
        //}
        //return 0;
    }
    }
    return CWindow::WindowProc(uMsg, wParam, lParam);
}

void CViewerWindow::SearchDlg()
{
    CFindSetDialog fd(HLanguage, HWindow, IDD_FINDSET, 0);

    INT_PTR result = fd.Execute();
    if (result == -1) {
        DWORD err = GetLastError();
        // Optionally, log or display the error code
        char buf[128];
        sprintf(buf, "DialogBoxParam failed. GetLastError() = %lu", err);
        MessageBox(NULL, buf, "Dialog Error", MB_OK | MB_ICONERROR);
    }
    if (result == IDOK)
    {

        search_match result = SearcherPtr->search(fd.Text, fd.Forward, fd.Regular, fd.HexMode, fd.WholeWords, fd.CaseSensitive, Renderer.first_visible_line, Renderer.last_visible_line);

		ProcessSearchResult(result);
    }
}

void CViewerWindow::ProcessSearchResult(search_match result)
{
    if (result.start.x != -1)
    {
        if (TextModelPtr->data_to_screen_pos(result.start, result.end))
        {
            if (result.start.y < Renderer.first_visible_line || result.start.y > Renderer.last_visible_line - 1)
            {
				//SetScrollPos(Renderer.HWindow, SB_VERT, result.start.y, TRUE);
                SendMessage(Renderer.HWindow, WM_VSCROLL, MAKEWPARAM(CUSTOM_SCROLLREFRESH, 0), result.start.y);
            }
            Renderer.NewSelection(result.start, result.end);
        }
    }
    else
    {
        Renderer.CancelSelection();
        char buf[] = "No results";
        MessageBox(NULL, buf, "Search Error", MB_OK | MB_ICONERROR);
    }
}

void CViewerWindow::GotoLineDlg()
{
    if (TextModelPtr != NULL)
    {
        int gotoLine = 0;
        if (CViewerGoToLineDialog(HWindow, &gotoLine).Execute() == IDOK)
        {

            int totalLines = TextModelPtr->get_line_count() - 1;
            gotoLine--;
            if (gotoLine < 0 || gotoLine > totalLines)
            {
                gotoLine = gotoLine > totalLines ? totalLines : 0;
            }
            SendMessage(Renderer.HWindow, WM_VSCROLL, MAKEWPARAM(CUSTOM_SCROLLREFRESH, 0), gotoLine);
        }
    }
}

void CViewerWindow::NextPrevFile(bool prev)
{
    BOOL ok = FALSE;
    BOOL srcBusy = FALSE;
    BOOL noMoreFiles = FALSE;
    char fileName[MAX_PATH];
    fileName[0] = 0;
    if (prev)
    {
        ok = SalamanderGeneral->GetPreviousFileNameForViewer(EnumFilesSourceUID,
            &EnumFilesCurrentIndex,
            Name, FALSE, TRUE,
            fileName, &noMoreFiles,
            &srcBusy);
    }
    else
    {
        ok = SalamanderGeneral->GetNextFileNameForViewer(EnumFilesSourceUID,
            &EnumFilesCurrentIndex,
            Name, FALSE, TRUE, fileName,
            &noMoreFiles, &srcBusy);
    }

    if (ok) {
		std::string fileNameStr(fileName);
		fileNameStr += "- SyntaxView";
        SetWindowText(HWindow, fileNameStr.c_str());
        OpenFile(fileName, FALSE, CurrentEncodingCodePage); // mame nove jmeno

    }
    else
    {
        if (noMoreFiles)
            TRACE_I("Next/previous file does not exist.");
        else
        {
            if (srcBusy)
                TRACE_I("Connected panel or Find window is busy, please try to repeat your request later.");
            else
            {
                if (EnumFilesSourceUID == -1)
                    TRACE_I("This service is not available from archive nor file system path.");
                else
                    TRACE_I("Connected panel or Find window does not contain original list of files.");
            }
        }
    }
}

BOOL CViewerWindow::GetFindText(WCHAR* buf, int& len)
{
    WCHAR* str;
    int len2;
    if (Renderer.GetSelectionContent(&str, &len2))
    {
        if (len2+1 > len)
        {
			len2 = len - 1;
        }
        lstrcpynW(buf, str, len2+1);
		len = len2;
        return TRUE;
            
    }
	return FALSE;
}

void CViewerWindow::CheckEncodingMenuItem(WORD item) {
    CGUIMenuPopupAbstract* sub = MainMenu->GetSubMenu(CML_VIEWER_ENCODING, FALSE);
    int itemCount = sub->GetItemCount();
    for (int i = 0; i < itemCount; i++) {
        sub->CheckItem(i, TRUE, FALSE);
    }

    if (TextModelPtr->encoding_forced())
    {
        sub->CheckItem(CM_VIEWER_UTF8, FALSE, TRUE);
    }
    else
    {
        sub->CheckItem(item, FALSE, TRUE);
        CurrentEncoding = item;
    }
}


void CViewerWindow::ToggleFullscreen()
{
    if (IsZoomed(HWindow))
        ShowWindow(HWindow, SW_RESTORE);
    else
        ShowWindow(HWindow, SW_MAXIMIZE);
}

HANDLE
CViewerWindow::GetLock()
{
    if (Lock == NULL)
        Lock = NOHANDLES(CreateEvent(NULL, FALSE, FALSE, NULL));
    return Lock;
}

void CViewerWindow::OpenFile(const char* name, BOOL setLock, UINT default_encoding)
{
    if (setLock && Lock != NULL)
    {
        SetEvent(Lock);
        Lock = NULL; // ted uz je to jen na disk-cache
    }

    // Save file path
    lstrcpyn(Name, name, MAX_PATH);
    Renderer.CancelSelection();

    if (TextModelPtr != NULL) {
		delete TextModelPtr;
    }
	if (SearcherPtr != NULL) {
		delete SearcherPtr;
	}

    TextModelPtr = new TextModel(Name, default_encoding);
	SearcherPtr = new Searcher(TextModelPtr);

    std::string chosen_grammar = TextModelPtr->get_active_grammar();
    std::vector<std::string> gm = removeFileExtension({ chosen_grammar });

    int index = (int)SendMessage(
        (HWND)HCombo,
        CB_FINDSTRINGEXACT,
        -1,
		(LPARAM)gm[0].c_str()
    );


    if (index != -1)
    {
        SendMessage((HWND)HCombo, CB_SETCURSEL, index, 0);
    }

    int font_size = CfgDefaultFontSize;
    std::string font_name = CfgSelectedFont;
	Renderer.change_font(font_size, font_name, true);
	Renderer.tab_size = CfgTabSize;

    if (CfgDefaultLineNumbering && !Renderer.display_line_numbers)
    {
		Renderer.ToggleLineNumbering();
    }
    if (!TextModelPtr->encoding_forced())
    {
        CurrentEncodingCodePage = default_encoding;
    }

    CheckEncodingMenuItem(CurrentEncoding);

    //TODO move 
	Enablers[vweCopy] = FALSE;
	UpdateEnablers();

    InvalidateRect(HWindow, NULL, TRUE);
    InvalidateRect(Renderer.HWindow, NULL, TRUE);
}
