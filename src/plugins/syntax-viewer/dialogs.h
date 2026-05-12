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

#include <vector>
#define FIND_TEXT_LEN 201

//****************************************************************************
//
// CCommonDialog
//
// Dialog centrovany k parentu
//

class CCommonDialog : public CDialog
{
public:
    CCommonDialog(HINSTANCE hInstance, int resID, HWND hParent, CObjectOrigin origin = ooStandard);
    CCommonDialog(HINSTANCE hInstance, int resID, int helpID, HWND hParent, CObjectOrigin origin = ooStandard);

protected:
    INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void NotifDlgJustCreated();
};

//
// ****************************************************************************
// CCommonPropSheetPage
//

class CCommonPropSheetPage : public CPropSheetPage
{
public:
    CCommonPropSheetPage(TCHAR* title, HINSTANCE modul, int resID,
                         DWORD flags /* = PSP_USETITLE*/, HICON icon,
                         CObjectOrigin origin = ooStatic)
        : CPropSheetPage(title, modul, resID, flags, icon, origin) {}
    CCommonPropSheetPage(TCHAR* title, HINSTANCE modul, int resID, UINT helpID,
                         DWORD flags /* = PSP_USETITLE*/, HICON icon,
                         CObjectOrigin origin = ooStatic)
        : CPropSheetPage(title, modul, resID, helpID, flags, icon, origin) {}

protected:
    virtual void NotifDlgJustCreated();
};

//
// ****************************************************************************
// CConfigPageViewer
//

class CConfigPageViewer : public CCommonPropSheetPage
{
public:
    CConfigPageViewer();
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void Transfer(CTransferInfo& ti);
protected:
    int scrollSpeed = 3;
};


/*moja vec*/
class CConfigPageFontTheme : public CCommonPropSheetPage
{
public:
    CConfigPageFontTheme();
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void Transfer(CTransferInfo& ti);
    //virtual void Validate(CTransferInfo& ti);
protected:
    std::string selectedFont;
    int defaultFontSize;
    int tabSize;

    std::vector<std::string> themePaths;
    std::vector<std::string> themeNames;
    int selectedIndex = -1;
};

class CViewerGoToLineDialog : public CCommonDialog
{
public:
    CViewerGoToLineDialog(HWND parent, int* offset);

    virtual void Validate(CTransferInfo& ti);
    virtual void Transfer(CTransferInfo& ti);

protected:
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    int* Offset;
};

// ****************************************************************************
class CFindSetDialog : public CCommonDialog
{
public:
    static int Forward, // forward/backward (1/0)
        WholeWords,
        CaseSensitive,
        HexMode,
        Regular;

    WCHAR Text[FIND_TEXT_LEN];

    CFindSetDialog(HINSTANCE modul, HWND parent, int resID, UINT helpID)
        : CCommonDialog(modul, resID, helpID, parent, ooStatic)
    {
        //Forward = TRUE;
        //WholeWords = FALSE;
        //CaseSensitive = FALSE;
        //HexMode = FALSE;
        //Regular = FALSE;
        Text[0] = 0;
    }

    CFindSetDialog& operator=(CFindSetDialog& d)
    {
        Forward = d.Forward;
        WholeWords = d.WholeWords;
        CaseSensitive = d.CaseSensitive;
        HexMode = d.HexMode;
        Regular = d.Regular;
        memmove(Text, d.Text, FIND_TEXT_LEN);
        return *this;
    }

    virtual void Transfer(CTransferInfo& ti);

protected:
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    int CancelHexMode, // jen pro spravnou funkci tlacitka Cancel
        CancelRegular;
};

// ****************************************************************************


//
// ****************************************************************************
// CConfigDialog
//

class CConfigDialog : public CPropertyDialog
{
protected:
    CConfigPageViewer PageViewer;
	CConfigPageFontTheme PageFontTheme;

public:
    CConfigDialog(HWND parent);
};
