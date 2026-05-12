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

#pragma once

#define VIEWER_HISTORY_SIZE 30

extern WCHAR* ViewerHistory[VIEWER_HISTORY_SIZE];

void HistoryComboBox(HWND hWindow, CTransferInfo& ti, int ctrlID, WCHAR* Text,
    int textLen, BOOL hexMode, int historySize, WCHAR* history[],
    BOOL changeOnlyHistory = FALSE);
void ReleaseFindHistory();
void DoHexValidation(HWND edit, const int textLen);

BOOL ChangeToArrowButton(HWND hParent, int ctrlID);

//******************************************************************************
//
// CComboboxEdit
//
// Protoze je combobox vyprasenej, nelze klasickou cestou (CB_GETEDITSEL) zjistit
// po opusteni focusu, jaka byla selection. To resi tento control.
//

class CComboboxEdit : public CWindow
{
protected:
    DWORD SelStart;
    DWORD SelEnd;

public:
    CComboboxEdit();

    void GetSel(DWORD* start, DWORD* end);

    void ReplaceText(const char* text);

protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

//******************************************************************************
//
// CExecuteItem
//

// implicitne se po vyberu polozky nahradi vybrany text v editlajne
// kurzor se postavi za tento text
// a do editlajny se presune focus
// nasledujici flagy modifikuji implicitni chovani:
#define EIF_REPLACE_ALL 0x01 // obsah cele editlajny bude nahrazen
#define EIF_CURSOR_1 0x02    // kurzor bude postaven jeden znak pred konec vlozeneho textu
#define EIF_CURSOR_2 0x04    // kurzor bude postaven dva znaky pred konec vlozeneho textu
#define EIF_VARIABLE 0x08    // vlozeny text bude ohranicen: $(text)
#define EIF_DONT_FOCUS 0x10  // po akci nebude focus postaven do editlajny

struct CExecuteItem
{
    const char* Keyword; // string vlozeny do editlajny
    int NameResID;       // string z resourcu, ktery bude vybalen v menu
    BYTE Flags;          // EIF_xxxx
};

//******************************************************************************
//
// Vlastni funkce
//

// vybali Popupmenu s prislusnym seznamem a po zvoleni polozy vlozi text do editlajny
// hParent:          dialog, ve kterem je editlajna/combobx a browse tlacitko
// buttonResID:      id browse tlacitka
// editlineResID:    id editlajny/comboboxu
// combobox:         pokud je TRUE, editlineResID identifikuje editlajnu; jinak combobox
//                   pokud je TRUE, k editlineResID by mel byt pripojeny control CComboboxEdit
// executeItems:     pole, ze ktereho je naplneno menu
// filterResID:      text do browse okna otevreneho ve specialnim pripade z menu
// replaceWholeText: pokud je TRUE, cely obsah editlineResID bude zmenen; jinak
//                   se nahradi pouze selection
const CExecuteItem* TrackExecuteMenu(HWND hParent, int buttonResID, int editlineResID,
    BOOL combobox, CExecuteItem* executeItems, int filterResID = 0);

extern CExecuteItem RegularExpressionItems[];

