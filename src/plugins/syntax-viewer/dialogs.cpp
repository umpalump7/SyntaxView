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
#include "find.h"

//****************************************************************************
//
// CCommonDialog
//

CCommonDialog::CCommonDialog(HINSTANCE hInstance, int resID, HWND hParent, CObjectOrigin origin)
    : CDialog(hInstance, resID, hParent, origin)
{
}

CCommonDialog::CCommonDialog(HINSTANCE hInstance, int resID, int helpID, HWND hParent, CObjectOrigin origin)
    : CDialog(hInstance, resID, helpID, hParent, origin)
{
}

INT_PTR
CCommonDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        // horizontalni i vertikalni vycentrovani dialogu k parentu
        if (Parent != NULL)
            SalamanderGeneral->MultiMonCenterWindow(HWindow, Parent, TRUE);
        break; // chci focus od DefDlgProc
    }
    }
    return CDialog::DialogProc(uMsg, wParam, lParam);
}

void CCommonDialog::NotifDlgJustCreated()
{
    SalamanderGUI->ArrangeHorizontalLines(HWindow);
}

//
// ****************************************************************************
// CCommonPropSheetPage
//

void CCommonPropSheetPage::NotifDlgJustCreated()
{
    SalamanderGUI->ArrangeHorizontalLines(HWindow);
}

//
// ****************************************************************************
// CConfigPageViewer
//

CConfigPageViewer::CConfigPageViewer()
    : CCommonPropSheetPage(NULL, HLanguage, IDD_CFGPAGEVIEWER, IDD_CFGPAGEVIEWER, NULL, NULL)
{
}

void CConfigPageViewer::Transfer(CTransferInfo& ti)
{
    ti.RadioButton(IDC_CFG_SAVEPOSONCLOSE, 1, CfgSavePosition);
    ti.RadioButton(IDC_CFG_SETBYMAINWINDOW, 0, CfgSavePosition);
	ti.CheckBox(IDC_CFG_GRAMMARERRORS, ShowGrammarParsingErrors);
	ti.CheckBox(IDC_CFG_DISPLAYLINENUMBERS, CfgDefaultLineNumbering);
    if (ti.Type == ttDataToWindow)
    {
        scrollSpeed = CfgScrollSpeed;
		SendDlgItemMessage(HWindow, IDC_CFG_SCROLLSPEED, WM_SETTEXT, 0, (LPARAM)std::to_string(CfgScrollSpeed).c_str());
	}
    else
    {
		CfgScrollSpeed = scrollSpeed;
    }
}


INT_PTR
CConfigPageViewer::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CALL_STACK_MESSAGE4("CConfigPageFontTheme::DialogProc(0x%X, 0x%IX, 0x%IX)", uMsg, wParam, lParam);

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        // Font
        HWND hTSpeedSpin = GetDlgItem(HWindow, IDC_CFG_SCROLLSPEEDSPIN);
        HWND hTSpeed = GetDlgItem(HWindow, IDC_CFG_SCROLLSPEED);
        SendMessage(hTSpeedSpin, UDM_SETBUDDY, (WPARAM)hTSpeed, 0);

        SendMessage(hTSpeedSpin, UDM_SETRANGE, 0, MAKELPARAM(10, 1));

        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) // control ID
        {
        case IDC_CFG_SCROLLSPEED:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                char buffer[10];
                GetDlgItemText(HWindow, IDC_CFG_SCROLLSPEED, buffer, sizeof(buffer));
                int value = atoi(buffer);
                if (value >= 1 && value <= 10) // todo
                {
                    scrollSpeed = value;
                }
                else if (value < 1) // todo
                {
                    if (buffer[0] != '\0')
                    {
                        scrollSpeed = 1;
                        SetDlgItemText(HWindow, IDC_CFG_SCROLLSPEED, "1");
                    }
                }
                else
                {
                    scrollSpeed = 10;
                    SetDlgItemText(HWindow, IDC_CFG_SCROLLSPEED, "10");
                }
            }
            break;
        }
        }
    }
    }
    return CCommonPropSheetPage::DialogProc(uMsg, wParam, lParam);
}

//
// ****************************************************************************
// CConfigPageFontTheme
//
CConfigPageFontTheme::CConfigPageFontTheme()
    : CCommonPropSheetPage(NULL, HLanguage, IDD_FONT_THEME, IDD_FONT_THEME, NULL, NULL)
{
}

void CConfigPageFontTheme::Transfer(CTransferInfo& ti)
{
	if (ti.Type == ttDataToWindow)
	{
        // Font
        selectedFont = CfgSelectedFont;
        defaultFontSize = CfgDefaultFontSize;
        tabSize = CfgTabSize;
        SendDlgItemMessage(HWindow, IDC_STATIC_FONTNAME, WM_SETTEXT, -1, (LPARAM)CfgSelectedFont.c_str());
        int pointSize = -MulDiv(CfgDefaultFontSize, 72, GetDeviceCaps(GetDC(NULL), LOGPIXELSY));
        SendDlgItemMessage(HWindow, IDC_STATIC_FONTSIZE, WM_SETTEXT, 0, (LPARAM)std::to_string(pointSize).c_str());

        SendDlgItemMessage(HWindow, IDC_TABSIZE, WM_SETTEXT, 0, (LPARAM)std::to_string(CfgTabSize).c_str());

        // Theme
        auto it = std::find(themePaths.begin(), themePaths.end(), CfgThemeFileName);
		size_t index = 0;
        if (it != themePaths.end()) {
            index = std::distance(themePaths.begin(), it);
        }
		SendDlgItemMessage(HWindow, IDC_THEMELIST, LB_SETCURSEL, index, 0);
	}
    else
    {
        // Font
        CfgSelectedFont = selectedFont;
        CfgDefaultFontSize = defaultFontSize;
        CfgTabSize = tabSize;

        // Theme
        if (selectedIndex != -1)
        {
            CfgThemeFileName = themePaths[selectedIndex];
        }
    }
}

INT_PTR
CConfigPageFontTheme::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CALL_STACK_MESSAGE4("CConfigPageFontTheme::DialogProc(0x%X, 0x%IX, 0x%IX)", uMsg, wParam, lParam);

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        // Font
        HWND hTSizeSpin = GetDlgItem(HWindow, IDC_TABSIZESPIN);
        HWND hTSize = GetDlgItem(HWindow, IDC_TABSIZE);
        SendMessage(hTSizeSpin, UDM_SETBUDDY, (WPARAM)hTSize, 0);

        SendMessage(hTSizeSpin, UDM_SETRANGE, 0, MAKELPARAM(10, 1));

        // Theme
        themePaths = getFilesWithExtension(getCurrentDllPath() + "\\themes", ".json");
        themeNames = getThemeList(themePaths);
        // Add a string to the listbox
        for (int i = 0; i < themeNames.size(); i++)
        {
            SendDlgItemMessage(HWindow, IDC_THEMELIST, LB_ADDSTRING, 0, (LPARAM)themeNames[i].c_str());
        }
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) // control ID
        {
        case IDC_CHOOSEFONT:
        {
            CHOOSEFONT cf = { 0 };
            LOGFONT lf = { 0 };
            strncpy(lf.lfFaceName, CfgSelectedFont.c_str(), LF_FACESIZE - 1);
            lf.lfFaceName[LF_FACESIZE - 1] = '\0';
            lf.lfHeight = CfgDefaultFontSize;

            cf.lStructSize = sizeof(cf);
            cf.hwndOwner = HWindow;
            cf.lpLogFont = &lf;
            cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_FIXEDPITCHONLY | CF_NOSCRIPTSEL;

            if (ChooseFont(&cf))
            {
                selectedFont = lf.lfFaceName;
                defaultFontSize = lf.lfHeight;
                SendDlgItemMessage(HWindow, IDC_STATIC_FONTNAME, WM_SETTEXT, -1, (LPARAM)selectedFont.c_str());
                int pointSize = -MulDiv(defaultFontSize, 72, GetDeviceCaps(GetDC(NULL), LOGPIXELSY));
                SendDlgItemMessage(HWindow, IDC_STATIC_FONTSIZE, WM_SETTEXT, 0, (LPARAM)std::to_string(pointSize).c_str());
            }
            break;
        }
        case IDC_TABSIZE:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                char buffer[10];
                GetDlgItemText(HWindow, IDC_TABSIZE, buffer, sizeof(buffer));
                int value = atoi(buffer);
                if (value >= 1 && value <= 10) // todo
                {
                    tabSize = value;
                }
                else if (value < 1) // todo
                {
                    if (buffer[0] != '\0')
                    {
                        tabSize = 1;
                        SetDlgItemText(HWindow, IDC_TABSIZE, "1");
                    }
                }
                else
                {
                    tabSize = 10;
                    SetDlgItemText(HWindow, IDC_TABSIZE, "10");
                }
            }
            break;
        }
        case IDC_THEMELIST:
        {
            if (HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == BN_CLICKED)
            {
                HWND hList = (HWND)lParam;

                selectedIndex = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);

                if (selectedIndex != LB_ERR)
                {
                }
            }
            break;
        }
        //hmm idok je prezitok TODO
        case IDOK:
        {
			if (selectedIndex != -1)
            {
                MessageBox(HWindow, themePaths[selectedIndex].c_str(), "Selected Item", MB_OK);
            }
            else
            {
                MessageBox(HWindow, "No item currently selected", "Selected Item", MB_OK);
            }
            break;
        }
        }
        break;
    }
    }
    // default zpracovani messagi
    return CCommonPropSheetPage::DialogProc(uMsg, wParam, lParam);
}


//*****************************************************************************
// CViewerGoToOffsetDialog
//

CViewerGoToLineDialog::CViewerGoToLineDialog(HWND parent, int* offset)
    : CCommonDialog(HLanguage, IDD_GOTOLINE, IDD_GOTOLINE, parent) {
    Offset = offset;
}

void CViewerGoToLineDialog::Validate(CTransferInfo& ti)
{
    int dummy;
    ti.EditLine(IDE_GOTO_LINE, dummy, TRUE);
}

void CViewerGoToLineDialog::Transfer(CTransferInfo& ti)
{
    ti.EditLine(IDE_GOTO_LINE, *Offset, TRUE);
}

INT_PTR
CViewerGoToLineDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CALL_STACK_MESSAGE4("CViewerGoToOffsetDialog::DialogProc(0x%X, 0x%IX, 0x%IX)", uMsg, wParam, lParam);
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        break;
    }
    }
    return CCommonDialog::DialogProc(uMsg, wParam, lParam);
}


//
//*****************************************************************************
// CFindSetDialog
//

int CFindSetDialog::Forward = TRUE;
int CFindSetDialog::WholeWords = FALSE;
int CFindSetDialog::CaseSensitive = FALSE;
int CFindSetDialog::HexMode = FALSE;
int CFindSetDialog::Regular = FALSE;

void CFindSetDialog::Transfer(CTransferInfo& ti)
{
    ti.CheckBox(IDC_FINDHEX, HexMode);
    ti.CheckBox(IDC_VIEWREGEXP, Regular);
    HistoryComboBox(HWindow, ti, IDC_FINDTEXT, Text, FIND_TEXT_LEN, !Regular && HexMode, VIEWER_HISTORY_SIZE, ViewerHistory);
    if (ti.Type == ttDataToWindow)
    { // inicializace hledaneho textu podle oznaceni ve viewru (parent tohoto dialogu)
        CWindowsObject* win = WindowsManager.GetWindowPtr(Parent);
        if (win != NULL/* && win->Is(otViewerWindow) */) // pro jistotu test je-li to okno viewru
        {
            CViewerWindow* view = (CViewerWindow*)win;
            WCHAR buf[FIND_TEXT_LEN];
            WCHAR hexBuf[FIND_TEXT_LEN];
            int len = sizeof(buf);
            if (view->GetFindText(buf, len))
            {
                if (HexMode)
                {
                    if (len * 3 > FIND_TEXT_LEN)
                        len = (FIND_TEXT_LEN - 1) / 3;
                    int i;
                    for (i = 0; i < len; i++)
                    {
                        swprintf_s(hexBuf + i * 3, 4, i == len - 1 ? L"%02X" : L"%02X ", (unsigned)buf[i]);
                    }
                    wcscpy(buf, hexBuf);
                }
                SendMessageW(GetDlgItem(HWindow, IDC_FINDTEXT), WM_SETTEXT, 0, (LPARAM)buf);
            }
        }
    }
    ti.RadioButton(IDC_SBACKWARD, 0, Forward);
    ti.RadioButton(IDC_SFORWARD, 1, Forward);
    ti.CheckBox(IDC_WHOLEWORDS, WholeWords);
    ti.CheckBox(IDC_CASESENSITIVE, CaseSensitive);
}

INT_PTR
CFindSetDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CALL_STACK_MESSAGE4("CFindSetDialog::DialogProc(0x%X, 0x%IX, 0x%IX)", uMsg, wParam, lParam);
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        CancelHexMode = HexMode;
        CancelRegular = Regular;

        EnableWindow(GetDlgItem(HWindow, IDC_FINDHEX), !Regular);
        EnableWindow(GetDlgItem(HWindow, IDC_WHOLEWORDS), !Regular);
        EnableWindow(GetDlgItem(HWindow, IDC_CASESENSITIVE), !Regular);

        ChangeToArrowButton(HWindow, IDC_REGEXP_BROWSE);

        CComboboxEdit* edit = new CComboboxEdit();
        if (edit != NULL)
        {
            HWND hCombo = GetDlgItem(HWindow, IDC_FINDTEXT);
            edit->AttachToWindow(GetWindow(hCombo, GW_CHILD));
        }
        break;
    }

    case WM_USER_CLEARHISTORY:
    {
        // mame promazat historie
        //ClearComboboxListbox(GetDlgItem(HWindow, IDC_FINDTEXT));
        return 0;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
        {
            HexMode = CancelHexMode; // aby byl Cancel korektni
            Regular = CancelRegular;
            break;
        }

        case IDC_REGEXP_BROWSE:
        {
            const CExecuteItem* item = TrackExecuteMenu(HWindow, IDC_REGEXP_BROWSE, IDC_FINDTEXT,
                TRUE, RegularExpressionItems);
            if (item != NULL)
            {
                BOOL regular = (IsDlgButtonChecked(HWindow, IDC_VIEWREGEXP) == BST_CHECKED);
                //if (item->Keyword == EXECUTE_HELP)
                //{
                //    // otevreme help se strankou venovanou regular expressions
                //    OpenHtmlHelp(NULL, HWindow, HHCDisplayContext, IDH_REGEXP, FALSE);
                //}
                if (!regular)
                {
                    // user zvolil nejaky vyraz -> zaskrtneme checkbox pro hledani regularu
                    CheckDlgButton(HWindow, IDC_VIEWREGEXP, BST_CHECKED);
                    PostMessage(HWindow, WM_COMMAND, MAKELPARAM(IDC_VIEWREGEXP, BN_CLICKED), 0);
                }
            }
            return 0;
        }

        case IDC_VIEWREGEXP:
        {
            Regular = (IsDlgButtonChecked(HWindow, IDC_VIEWREGEXP) != BST_UNCHECKED);
            EnableWindow(GetDlgItem(HWindow, IDC_FINDHEX), !Regular);
            EnableWindow(GetDlgItem(HWindow, IDC_WHOLEWORDS), !Regular);
            EnableWindow(GetDlgItem(HWindow, IDC_CASESENSITIVE), !Regular);

            break;
        }

        case IDC_FINDHEX:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                HexMode = (IsDlgButtonChecked(HWindow, IDC_FINDHEX) != BST_UNCHECKED);
                if (HexMode)
                    CheckDlgButton(HWindow, IDC_CASESENSITIVE, BST_CHECKED);
                return TRUE;
            }
            break;
        }

        case IDC_FINDTEXT:
        {
            if (!Regular && HexMode && HIWORD(wParam) == CBN_EDITUPDATE)
            {
                DoHexValidation((HWND)lParam, FIND_TEXT_LEN);
                return TRUE;
            }
            break;
        }
        }
        break;
    }
    }
    return CCommonDialog::DialogProc(uMsg, wParam, lParam);
}


//
// ****************************************************************************
// CConfigDialog
//

// pomocny objekt pro centrovani konfiguracniho dialogu k parentovi
class CCenteredPropertyWindow : public CWindow
{
protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_WINDOWPOSCHANGING:
        {
            WINDOWPOS* pos = (WINDOWPOS*)lParam;
            if (pos->flags & SWP_SHOWWINDOW)
            {
                HWND hParent = GetParent(HWindow);
                if (hParent != NULL)
                    SalamanderGeneral->MultiMonCenterWindow(HWindow, hParent, TRUE);
            }
            break;
        }

        case WM_APP + 1000: // mame se odpojit od dialogu (uz je vycentrovano)
        {
            DetachWindow();
            delete this; // trochu prasarna, ale uz se 'this' nikdo ani nedotkne, takze pohoda
            return 0;
        }
        }
        return CWindow::WindowProc(uMsg, wParam, lParam);
    }
};

#ifndef LPDLGTEMPLATEEX
#include <pshpack1.h>
typedef struct DLGTEMPLATEEX
{
    WORD dlgVer;
    WORD signature;
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;
} DLGTEMPLATEEX, *LPDLGTEMPLATEEX;
#include <poppack.h>
#endif // LPDLGTEMPLATEEX

// pomocny call-back pro centrovani konfiguracniho dialogu k parentovi a vyhozeni '?' buttonku z captionu
int CALLBACK CenterCallback(HWND HWindow, UINT uMsg, LPARAM lParam)
{
    if (uMsg == PSCB_INITIALIZED) // pripojime se na dialog
    {
        CCenteredPropertyWindow* wnd = new CCenteredPropertyWindow;
        if (wnd != NULL)
        {
            wnd->AttachToWindow(HWindow);
            if (wnd->HWindow == NULL)
                delete wnd; // okno neni pripojeny, zrusime ho uz tady
            else
            {
                PostMessage(wnd->HWindow, WM_APP + 1000, 0, 0); // pro odpojeni CCenteredPropertyWindow od dialogu
            }
        }
    }
    if (uMsg == PSCB_PRECREATE) // odstraneni '?' buttonku z headeru property sheetu
    {
        // Remove the DS_CONTEXTHELP style from the dialog box template
        if (((LPDLGTEMPLATEEX)lParam)->signature == 0xFFFF)
            ((LPDLGTEMPLATEEX)lParam)->style &= ~DS_CONTEXTHELP;
        else
            ((LPDLGTEMPLATE)lParam)->style &= ~DS_CONTEXTHELP;
    }
    return 0;
}

CConfigDialog::CConfigDialog(HWND parent)
    : CPropertyDialog(parent, HLanguage, LoadStr(IDS_CFG_TITLE),
                      LastCfgPage, PSH_USECALLBACK | PSH_NOAPPLYNOW | PSH_HASHELP,
                      NULL, &LastCfgPage, CenterCallback)
{
    Add(&PageFontTheme);
    Add(&PageViewer);
}
