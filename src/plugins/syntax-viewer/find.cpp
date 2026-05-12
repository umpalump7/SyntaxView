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

#include "find.h"


// retezce pro regular expressions
const char* REGEXP_ANYCHAR = ".";
const char* REGEXP_SETOFCHAR = "[]";
const char* REGEXP_NOTSETOFCHAR = "[^]";
const char* REGEXP_RANGEOFCHAR = "[-]";
const char* REGEXP_BEGINOFLINE = "^";
const char* REGEXP_ENDOFLINE = "$";
const char* REGEXP_OR = "|";
const char* REGEXP_0ORMORE = "*";
const char* REGEXP_1ORMORE = "+";
const char* REGEXP_0OR1 = "?";

const char* REGEXP_PARENTHESIS_L_CHAR = "\\(";
const char* REGEXP_PARENTHESIS_R_CHAR = "\\)";
const char* REGEXP_DOT_CHAR = "\\.";
const char* REGEXP_PLUS_CHAR = "\\+";
const char* REGEXP_ASTERISK_CHAR = "\\*";

const char* REGEXP_ALPHANUMERIC_CHAR = "[a-zA-Z0-9]";
const char* REGEXP_ALPHABETIC_CHAR = "[a-zA-Z]";
const char* REGEXP_DECIMAL_DIGIT = "[0-9]";

const char* REGEXP_DECIMAL_NUMBER = "([0-9]+)";
const char* REGEXP_HEXADECIMAL_NUMBER = "([0-9a-fA-F]+)";
const char* REGEXP_REAL_NUMBER = "(([0-9]+\\.[0-9]*)|([0-9]*\\.[0-9]+)|([0-9]+))";

const char* REGEXP_QUOTED_STRING = "((\"[^\"]*\")|('[^']*'))";
const char* REGEXP_ALPHABETIC_STRING = "([a-zA-Z]+)";
const char* REGEXP_IDENTIFIER = "([a-zA-Z_$][a-zA-Z0-9_$]*)";

const char* EXECUTE_SEPARATOR = "Separator";
const char* EXECUTE_BROWSE = "Browse";       // prochazeni adresaru pomoci Open dialogu
const char* EXECUTE_BROWSEDIR = "BrowseDir"; // prochazeni adresaru pomoci GetTargetDirectory
const char* EXECUTE_TERMINATOR = "Terminator";
const char* EXECUTE_SUBMENUSTART = "SubMenuStart";
const char* EXECUTE_SUBMENUEND = "SubMenuEnd";

CExecuteItem RegularExpressionItems[] =
{
    {REGEXP_ANYCHAR, IDS_REGEXP_ANYCHAR, 0},
    {REGEXP_SETOFCHAR, IDS_REGEXP_SETOFCHAR, EIF_CURSOR_1},
    {REGEXP_NOTSETOFCHAR, IDS_REGEXP_NOTSETOFCHAR, EIF_CURSOR_1},
    {REGEXP_RANGEOFCHAR, IDS_REGEXP_RANGEOFCHAR, EIF_CURSOR_2},
    {REGEXP_BEGINOFLINE, IDS_REGEXP_BEGINOFLINE, 0},
    {REGEXP_ENDOFLINE, IDS_REGEXP_ENDOFLINE, 0},
    {REGEXP_OR, IDS_REGEXP_OR, 0},
    {REGEXP_0ORMORE, IDS_REGEXP_0ORMORE, 0},
    {REGEXP_1ORMORE, IDS_REGEXP_1ORMORE, 0},
    {REGEXP_0OR1, IDS_REGEXP_0OR1, 0},
    {EXECUTE_SEPARATOR, 0, 0},
    {REGEXP_PARENTHESIS_L_CHAR, IDS_REGEXP_PARENTHESIS_L_CHAR, 0},
    {REGEXP_PARENTHESIS_R_CHAR, IDS_REGEXP_PARENTHESIS_R_CHAR, 0},
    {REGEXP_DOT_CHAR, IDS_REGEXP_DOT_CHAR, 0},
    {REGEXP_PLUS_CHAR, IDS_REGEXP_PLUS_CHAR, 0},
    {REGEXP_ASTERISK_CHAR, IDS_REGEXP_ASTERISK_CHAR, 0},
    {EXECUTE_SEPARATOR, 0, 0},
    {REGEXP_ALPHANUMERIC_CHAR, IDS_REGEXP_ALPHANUMERIC_CHAR, 0},
    {REGEXP_ALPHABETIC_CHAR, IDS_REGEXP_ALPHABETIC_CHAR, 0},
    {REGEXP_DECIMAL_DIGIT, IDS_REGEXP_DECIMAL_DIGIT, 0},
    {EXECUTE_SEPARATOR, 0, 0},
    {REGEXP_DECIMAL_NUMBER, IDS_REGEXP_DECIMAL_NUMBER, 0},
    {REGEXP_HEXADECIMAL_NUMBER, IDS_REGEXP_HEXADECIMAL_NUMBER, 0},
    {REGEXP_REAL_NUMBER, IDS_REGEXP_REAL_NUMBER, 0},
    {EXECUTE_SEPARATOR, 0, 0},
    {REGEXP_ALPHABETIC_STRING, IDS_REGEXP_ALPHABETIC_STRING, 0},
    {REGEXP_IDENTIFIER, IDS_REGEXP_IDENTIFIER, 0},
    {REGEXP_QUOTED_STRING, IDS_REGEXP_QUOTED_STRING, 0},
    {EXECUTE_SEPARATOR, 0, 0},
    {EXECUTE_TERMINATOR, 0, 0},
};

WCHAR* ViewerHistory[VIEWER_HISTORY_SIZE];

WCHAR LowerCase[256];//!!TOdo

void HistoryComboBox(HWND hWindow, CTransferInfo& ti, int ctrlID, WCHAR* Text,
    int textLen, BOOL hexMode, int historySize, WCHAR* history[],
    BOOL changeOnlyHistory)
{
    CALL_STACK_MESSAGE6("HistoryComboBox(, , %d, , %d, %d, %d, , %d)",
        ctrlID, textLen, hexMode, historySize, changeOnlyHistory);
    HWND hwnd;
    if (changeOnlyHistory || ti.GetControl(hwnd, ctrlID))
    {
        if (!changeOnlyHistory && ti.Type == ttDataToWindow)
        {
            SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
            SendMessage(hwnd, CB_LIMITTEXT, textLen - 1, 0);
            SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)Text);
        }
        else
        {
            if (!changeOnlyHistory)
            {
                SendMessageW(hwnd, WM_GETTEXT, textLen, (LPARAM)Text);

                // TODO
                //COMBOBOXINFO cbi = { sizeof(COMBOBOXINFO) };
                //if (GetComboBoxInfo(hwnd, &cbi)) {
                //    HWND hwndEdit = cbi.hwndItem;   // edit control
                //    HWND hwndList = cbi.hwndList;   // dropdown list
                //}

                //SendMessage(cbi.hwndItem, EM_SETSEL, 0, -1); // Select all
                //SendMessage(cbi.hwndItem, WM_COPY, 0, 0);    // Copy to clipboard

                //if (OpenClipboard(hwnd)) {
                //    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
                //    if (hData != NULL) {
                //        LPCWSTR pszText = (LPCWSTR)GlobalLock(hData);
                //        if (pszText != NULL) {
                //            wcsncpy(Text, pszText, textLen);
                //            Text[textLen-1] = 0;
                //            GlobalUnlock(hData);
                //        }
                //    }
                //    CloseClipboard();
                //}



                SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
                SendMessage(hwnd, CB_LIMITTEXT, textLen - 1, 0);
                SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)Text);
            }

            // osetreni hex-modu
            if (hexMode)
            {
                WCHAR* s = Text;
                BOOL openedQuotes = FALSE;
                WCHAR* lastQuotes = NULL;
                while (*s != 0 && (openedQuotes || *s == L' ' || *s >= L'0' && *s <= L'9' ||
                    LowerCase[*s] >= L'a' && LowerCase[*s] <= L'f' ||
                    *s == L'"'))
                {
                    if (*s == L'"')
                    {
                        openedQuotes = !openedQuotes;
                        lastQuotes = s;
                    }
                    s++;
                }
                if (openedQuotes)
                    s = lastQuotes;
                if (*s != 0) // obsahuje ne-hexa znak
                {
                    if (!changeOnlyHistory)
                    {
                        SalamanderGeneral->SalMessageBox(hWindow, LoadStr(IDS_STRINGISNOTHEX), LoadStr(IDS_ERRORTITLE),
                            MB_OK | MB_ICONEXCLAMATION);
                        SetFocus(hwnd);
                        SendMessageW(hwnd, CB_SETEDITSEL, 0, MAKELPARAM(s - Text, 1 + (s - Text)));
                    }
                    ti.ErrorOn(ctrlID);
                }
            }
            // vse o.k. zalozime do historie
            if (ti.IsGood())
            {
                if (Text[0] != 0)
                {
                    BOOL insert = TRUE;
                    int i;
                    for (i = 0; i < historySize; i++)
                    {
                        if (history[i] != NULL)
                        {
                            if (wcscmp(history[i], Text) == 0) // je-li uz v historii
                            {                                  // pujde na 0. pozici
                                if (i > 0)
                                {
                                    WCHAR* swap = history[i];
                                    memmove(history + 1, history, i * sizeof(WCHAR*));
                                    history[0] = swap;
                                }
                                insert = FALSE;
                                break;
                            }
                        }
                        else
                            break;
                    }

                    if (insert)
                    {
                        WCHAR* newText = (WCHAR*)malloc(wcslen(Text)*2 + 2);
                        if (newText != NULL)
                        {
                            wcscpy(newText, Text);
                            if (history[historySize - 1] != NULL)
                                free(history[historySize - 1]);
                            memmove(history + 1, history,
                                (historySize - 1) * sizeof(WCHAR*));
                            history[0] = newText;
                        }
                        //else
                            //TRACE_E(LOW_MEMORY);
                    }
                }
            }
        }

        if (!changeOnlyHistory)
        {
            int i;
            for (i = 0; i < historySize; i++) // naplneni listu combo-boxu
                if (history[i] != NULL)
                    SendMessageW(hwnd, CB_ADDSTRING, 0, (LPARAM)history[i]);
                else
                    break;
        }
    }
}

void ReleaseFindHistory() {
    for (int i = 0; i < VIEWER_HISTORY_SIZE; i++)
    {
        if (ViewerHistory[i] != NULL)
        {
            free(ViewerHistory[i]);
            ViewerHistory[i] = NULL;
        }
    }
}

void DoHexValidation(HWND edit, const int textLen)
{
    CALL_STACK_MESSAGE2("DoHexValidation(, %d)", textLen);
    int start, end;
    SendMessage(edit, CB_GETEDITSEL, (WPARAM)&start, (LPARAM)&end);
    char* text = new char[textLen];
    if (text == NULL)
    {
        //TRACE_E(LOW_MEMORY);
        return;
    }
    SendMessage(edit, WM_GETTEXT, textLen, (LPARAM)text);
    char* s = text;
    while (*s != 0 && *s == ' ')
        s++;
    if (s != text)
    {
        start -= (int)(s - text);
        end -= (int)(s - text);
        if (start < 0)
            start = 0;
        if (end < 0)
            end = 0;
        memmove(text, s, strlen(s) + 1);
    }
    s = text;
    BOOL openedQuotes = FALSE;
    char* st = s, * strEnd = text + strlen(text);
    while (*s != 0)
    {
        if (*s == '"')
        {
            if (!openedQuotes && s > text && *(s - 1) != ' ' && strEnd - text < textLen - 1)
            {
                if (start > s - text)
                    start++;
                if (end > s - text)
                    end++;
                memmove(s + 1, s, (strEnd - s) + 1);
                *s++ = ' ';
                strEnd++;
            }
            else
            {
                if (openedQuotes && s + 1 < strEnd && *(s + 1) != ' ' &&
                    strEnd - text < textLen - 1)
                {
                    if (start >= (s - text) + 1)
                        start++;
                    if (end >= (s - text) + 1)
                        end++;
                    memmove(s + 2, s + 1, strEnd - s);
                    *(s + 1) = ' ';
                    strEnd++;
                }
                if (openedQuotes && s + 1 < strEnd)
                    s++;
                st = s + 1;
            }
            openedQuotes = !openedQuotes;
        }
        else
        {
            if (!openedQuotes)
            {
                if (*s == ' ')
                {
                    if (st == s) // '  ' -> ' '
                    {
                        s--;
                        if (start >= st - text)
                            start--;
                        if (end >= st - text)
                            end--;
                        memmove(s, st, (strEnd - st) + 1);
                        strEnd--;
                    }
                    else
                        st = s + 1;
                }
                else
                {
                    if ((s - st) == 2) // 'ABC' -> 'AB C'
                    {
                        if (strEnd - text < textLen - 1)
                        {
                            if (start >= s - text)
                                start++;
                            if (end >= s - text)
                                end++;
                            memmove(s + 1, s, (strEnd - s) + 1);
                            *s = ' ';
                            st = s + 1;
                            strEnd++;
                        }
                    }
                }
            }
        }
        s++;
    }
    SendMessage(edit, WM_SETTEXT, 0, (LPARAM)text);
    SendMessage(edit, CB_SETEDITSEL, 0, MAKELPARAM(start, end));
    delete[](text);
}


BOOL ChangeToArrowButton(HWND hParent, int ctrlID)
{
    //CALL_STACK_MESSAGE_NONE
        // stary pristup nefungoval pri kontrastnich barvach, kde by se sipka mela vykreslit inverzne
        // prechazime na nase vlastni kresleni
        //CButton* button = new CButton(hParent, ctrlID, BTF_RIGHTARROW);
    
  // pod XP neni BS_ICON kresleny pres theme, ma stary look
  // vykreslime si tedy tlacitko po nasem
  //CButton *button = new CButton(hParent, ctrlID, 0);
  HWND hButton = GetDlgItem(hParent, ctrlID);
  if (hButton == NULL)
  {
    TRACE_E("Cannot find button ctrlID=" << ctrlID << " in the window hParent=0x" << hParent);
    return FALSE;
  }
  LONG_PTR l = GetWindowLongPtr(hButton, GWL_STYLE);
  l |= BS_ICON;
  SetWindowLongPtr(hButton, GWL_STYLE, l);
  SendMessage(hButton, BM_SETIMAGE, IMAGE_ICON, (WPARAM)HANDLES(LoadIcon(DLLInstance, MAKEINTRESOURCE(IDI_BROWSE))));

    return TRUE;
}


//******************************************************************************
//
// CComboboxEdit
//

CComboboxEdit::CComboboxEdit()
    : CWindow(ooAllocated)
{
    SelStart = 0;
    SelEnd = -1;
}

LRESULT
CComboboxEdit::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CALL_STACK_MESSAGE4("CComboboxEdit::WindowProc(0x%X, 0x%IX, 0x%IX)", uMsg, wParam, lParam);
    switch (uMsg)
    {
    case WM_KILLFOCUS:
    {
        SendMessage(HWindow, EM_GETSEL, (WPARAM)&SelStart, (LPARAM)&SelEnd);
        break;
    }

    case EM_REPLACESEL:
    {
        LRESULT res = CWindow::WindowProc(uMsg, wParam, lParam);
        SendMessage(HWindow, EM_GETSEL, (WPARAM)&SelStart, (LPARAM)&SelEnd);
        return res;
    }
    }
    return CWindow::WindowProc(uMsg, wParam, lParam);
}

void CComboboxEdit::GetSel(DWORD* start, DWORD* end)
{
    if (GetFocus() == HWindow)
        SendMessage(HWindow, EM_GETSEL, (WPARAM)start, (LPARAM)end);
    else
    {
        *start = SelStart;
        *end = SelEnd;
    }
}

void CComboboxEdit::ReplaceText(const char* text)
{
    // musime ozivit selection, protoze dementni combobox ji zapomel
    SendMessage(HWindow, EM_SETSEL, SelStart, SelEnd);
    SendMessage(HWindow, EM_REPLACESEL, TRUE, (LPARAM)text);
}

const CExecuteItem*
TrackExecuteMenu(HWND hParent, int buttonResID, int editlineResID,
    BOOL combobox, CExecuteItem* executeItems, int filterResID)
{
    CALL_STACK_MESSAGE4("TrackExecuteMenu(, %d, %d, %d)", buttonResID, editlineResID, filterResID);
    HWND hButton = GetDlgItem(hParent, buttonResID);
    if (hButton == NULL)
        TRACE_E("Child window was not found: buttonResID=" << buttonResID);
    HWND hEdit = NULL;
    CComboboxEdit* comboEdit = NULL;
    if (combobox)
    {
        hEdit = GetWindow(GetDlgItem(hParent, editlineResID), GW_CHILD);
        if (hEdit != NULL)
        {
            comboEdit = (CComboboxEdit*)WindowsManager.GetWindowPtr(hEdit);
            if (comboEdit == NULL)
            {
                TRACE_E("CComboboxEdit was not found: editlineResID=" << editlineResID);
                return NULL;
            }
        }
    }
    else
        hEdit = GetDlgItem(hParent, editlineResID);
    if (hEdit == NULL)
    {
        TRACE_E("Child window was not found: editlineResID=" << editlineResID);
        return NULL;
    }

    RECT r;
    GetWindowRect(hButton, &r);
    POINT p;
    p.x = r.right;
    p.y = r.top;

    HMENU hMenu = CreatePopupMenu();
    CExecuteItem* item = executeItems;
    int i = 0;
    while (item[i].Keyword != EXECUTE_TERMINATOR)
    {
        if (item[i].Keyword == EXECUTE_SUBMENUSTART)
        {
            int subMenuIndex = i++;
            HMENU hSubMenu = CreatePopupMenu();
            while (item[i].Keyword != EXECUTE_SUBMENUEND && item[i].Keyword != EXECUTE_TERMINATOR)
            {
                if (item[i].Keyword == EXECUTE_SEPARATOR)
                    InsertMenu(hSubMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_SEPARATOR, 1, NULL);
                else
                    InsertMenu(hSubMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, (UINT_PTR)i + 1,
                        LoadStr(item[i].NameResID));
                i++;
            }
            InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hSubMenu,
                LoadStr(item[subMenuIndex].NameResID));
        }
        else
        {
            if (item[i].Keyword == EXECUTE_SEPARATOR)
                InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_SEPARATOR, 1, NULL);
            else
                InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, (UINT_PTR)i + 1,
                    LoadStr(item[i].NameResID));
        }
        i++;
    }

    TPMPARAMS tpmPar;
    tpmPar.cbSize = sizeof(tpmPar);
    tpmPar.rcExclude = r;
    DWORD cmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
        p.x, p.y, hParent, &tpmPar);
    DestroyMenu(hMenu);
    item = NULL;
    if (cmd != 0)
    {
        item = &executeItems[cmd - 1];
        char buff[255];
        //if (item->Keyword == EXECUTE_BROWSE)
        //{
        //    BrowseCommand(hParent, editlineResID, filterResID);
        //    return item;
        //}
        //if (item->Keyword == EXECUTE_BROWSEDIR)
        //{
        //    BrowseDirCommand(hParent, editlineResID, filterResID); // JRFIXME
        //    return item;
        //}
        //if (item->Keyword == EXECUTE_HELP)
        //    return item;

        if (item->Flags & EIF_VARIABLE)
            sprintf(buff, "$(%s)", item->Keyword);
        else
            sprintf(buff, "%s", item->Keyword);

        if (item->Flags & EIF_REPLACE_ALL)
        {
            SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)buff);
            SendMessage(hEdit, EM_SETSEL, lstrlen(buff), lstrlen(buff));
        }
        else
        {
            if (comboEdit != NULL)
                comboEdit->ReplaceText(buff);
            else
                SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)buff);
        }
        if ((item->Flags & EIF_DONT_FOCUS) == 0)
        {
            // v pripade comba je treba napred nastavit focus
            DWORD start;
            DWORD end;
            if (comboEdit != NULL)
            {
                SendMessage(hEdit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
                SetFocus(hEdit);
            }
            if (item->Flags & EIF_CURSOR_1 || item->Flags & EIF_CURSOR_2)
            {
                int delta = 1;
                if (item->Flags & EIF_CURSOR_2)
                    delta = 2;
                if (delta > lstrlen(buff))
                {
                    TRACE_E("delta > strlen(buff)");
                    delta = (int)strlen(buff);
                }
                if (comboEdit == NULL)
                    SendMessage(hEdit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
                SendMessage(hEdit, EM_SETSEL, end - delta, end - delta);
            }
            else if (comboEdit != NULL)
                SendMessage(hEdit, EM_SETSEL, end, end);
            if (comboEdit == NULL)
                SetFocus(hEdit);
            // default by zustal u nas -- vratime ho dialogu
            SendMessage(hButton, BM_SETSTYLE, BS_PUSHBUTTON, TRUE);
            HWND hDialog = hParent;
            DWORD dlgStyle;
            do
            {
                dlgStyle = (DWORD)GetWindowLongPtr(hDialog, GWL_STYLE);
                if (dlgStyle & DS_CONTROL)
                {
                    HWND hPar = GetParent(hDialog);
                    if (hPar == NULL)
                        break;
                    hDialog = hPar;
                }
            } while (dlgStyle & DS_CONTROL);
            DWORD defID = (DWORD)SendMessage(hDialog, DM_GETDEFID, 0, 0);
            if (HIWORD(defID) == DC_HASDEFID)
                SendMessage(GetDlgItem(hDialog, LOWORD(defID)), BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE);
        }
    }
    return item;
}


