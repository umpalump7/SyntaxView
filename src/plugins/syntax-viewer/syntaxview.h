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
#include "textmodel.h"

#include "searcher.h"

// globalni data
extern const char* PluginNameEN; // neprekladane jmeno pluginu, pouziti pred loadem jazykoveho modulu + pro debug veci
extern HINSTANCE DLLInstance;    // handle k SPL-ku - jazykove nezavisle resourcy
extern HINSTANCE HLanguage;      // handle k SLG-cku - jazykove zavisle resourcy

// obecne rozhrani Salamandera - platne od startu az do ukonceni pluginu
extern CSalamanderGeneralAbstract* SalamanderGeneral;

// rozhrani poskytujici upravene Windows controly pouzivane v Salamanderovi
extern CSalamanderGUIAbstract* SalamanderGUI;

BOOL InitViewer();
void ReleaseViewer();

// globalni data
extern BOOL CfgSavePosition;               // ukladat pozici okna/umistit dle hlavniho okna
extern WINDOWPLACEMENT CfgWindowPlacement; // neplatne, pokud CfgSavePosition != TRUE
extern std::string CfgThemeFileName;
extern std::string CfgSelectedFont;
extern int CfgDefaultFontSize;
extern int CfgTabSize;
extern BOOL CfgDefaultLineNumbering;
extern int CfgScrollSpeed;

extern BOOL ShowGrammarParsingErrors;

extern DWORD LastCfgPage; // start page (sheet) in configuration dialog

// [0, 0] - pro otevrena okna viewru: konfigurace pluginu se zmenila
#define WM_USER_VIEWERCFGCHNG WM_APP + 3346
// [0, 0] - pro otevrena okna viewru: je treba podriznou historie
#define WM_USER_CLEARHISTORY WM_APP + 3347
// [0, 0] - pro otevrena okna vieweru: Salamander pregeneroval fonty, mame zavolat SetFont() listam
#define WM_USER_SETTINGCHANGE WM_APP + 3248

char* LoadStr(int resID);

// prikazy pluginoveho menu
#define MENUCMD_VIEWBMPFROMCLIP 1

//
// ****************************************************************************
// CPluginInterface
//

class CPluginInterfaceForViewer : public CPluginInterfaceForViewerAbstract
{
public:
    virtual BOOL WINAPI ViewFile(const char* name, int left, int top, int width, int height,
                                 UINT showCmd, BOOL alwaysOnTop, BOOL returnLock, HANDLE* lock,
                                 BOOL* lockOwner, CSalamanderPluginViewerData* viewerData,
                                 int enumFilesSourceUID, int enumFilesCurrentIndex);
    virtual BOOL WINAPI CanViewFile(const char* name) { return TRUE; }
};

class CPluginInterfaceForMenuExt : public CPluginInterfaceForMenuExtAbstract
{
public:
    virtual DWORD WINAPI GetMenuItemState(int id, DWORD eventMask) { return 0; }
    virtual BOOL WINAPI ExecuteMenuItem(CSalamanderForOperationsAbstract* salamander, HWND parent,
                                        int id, DWORD eventMask);
    virtual BOOL WINAPI HelpForMenuItem(HWND parent, int id);
    virtual void WINAPI BuildMenu(HWND parent, CSalamanderBuildMenuAbstract* salamander) {}
};

class CPluginInterfaceForThumbLoader : public CPluginInterfaceForThumbLoaderAbstract
{
public:
    virtual BOOL WINAPI LoadThumbnail(const char* filename, int thumbWidth, int thumbHeight,
                                      CSalamanderThumbnailMakerAbstract* thumbMaker,
                                      BOOL fastThumbnail);
};

class CPluginInterface : public CPluginInterfaceAbstract
{
public:
    virtual void WINAPI About(HWND parent);

    virtual BOOL WINAPI Release(HWND parent, BOOL force);

    virtual void WINAPI LoadConfiguration(HWND parent, HKEY regKey, CSalamanderRegistryAbstract* registry);
    virtual void WINAPI SaveConfiguration(HWND parent, HKEY regKey, CSalamanderRegistryAbstract* registry);
    virtual void WINAPI Configuration(HWND parent);

    virtual void WINAPI Connect(HWND parent, CSalamanderConnectAbstract* salamander);

    virtual void WINAPI ReleasePluginDataInterface(CPluginDataInterfaceAbstract* pluginData) {}

    virtual CPluginInterfaceForArchiverAbstract* WINAPI GetInterfaceForArchiver() { return NULL; }
    virtual CPluginInterfaceForViewerAbstract* WINAPI GetInterfaceForViewer();
    virtual CPluginInterfaceForMenuExtAbstract* WINAPI GetInterfaceForMenuExt();
    virtual CPluginInterfaceForFSAbstract* WINAPI GetInterfaceForFS() { return NULL; }
    virtual CPluginInterfaceForThumbLoaderAbstract* WINAPI GetInterfaceForThumbLoader();

    virtual void WINAPI Event(int event, DWORD param);
    virtual void WINAPI ClearHistory(HWND parent);
    virtual void WINAPI AcceptChangeOnPathNotification(const char* path, BOOL includingSubdirs) {}

    virtual void WINAPI PasswordManagerEvent(HWND parent, int event) {}
};

//
// ****************************************************************************
// CViewerWindow
//

#define BANDID_MENU 1
#define BANDID_TOOLBAR 2

enum CViewerWindowEnablerEnum
{
    vweAlwaysEnabled, // zero index is reserved
    vweCopy,
    vweCount
};

class CViewerWindow;

class CRendererWindow : public CWindow
{
public:
    CViewerWindow* Viewer;

    int font_size = CfgDefaultFontSize;
    std::string font_name = CfgSelectedFont;
    int tab_size = CfgTabSize;

    int first_visible_line = 0;
    int last_visible_line = 80;

    int windowHeight = 0;
    int windowWidth = 0;
    int fontWidth = 0;
    int fontHeight = 0;
    int line_display_offset = 0;
    bool display_line_numbers = false;

    int scrollPosX = 0;
    int scrollPosY = 0;

public:
    CRendererWindow();
    ~CRendererWindow();

    void OnContextMenu(const POINT* p);

    void change_font(int size, std::string font, bool initialization);

    void StartSelection(text_point start);
    void CancelSelection();
    void ExtendSelection(text_point new_end);
    void NewSelection(text_point start, text_point end);
    void ModifySelectionByKeyboard(WPARAM wParam);
    bool GetSelectionContent(WCHAR** string, int* len);
    void UpdateScrollAttributes(bool initialization, bool update_window_size);
    void ToggleLineNumbering();

protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HFONT hFont;


    bool lmb_down = false;
    bool selection_active = false;
    bool rerender_select = false;

    text_point s_start;
    text_point s_end;

    text_point s_new_end;

    text_point rm_s_start;
    text_point rm_s_end;

private:
    void print_lines(int first_line, int last_line, HDC hDC, TextModel* h);
    void invert_area(text_point start_point, text_point end_point, HDC hDC);
    void ApplyCursorSelectionBounds(text_point* cursor_pos);
};


#define BANDID_MENU 1
#define BANDID_TOOLBAR 2
#define BANDID_COMBO 3

class CViewerWindow : public CWindow
{
public:
    HANDLE Lock;                      // 'lock' objekt nebo NULL (do signaled stavu az zavreme soubor)
    char Name[MAX_PATH];              // jmeno souboru nebo ""
    CRendererWindow Renderer;         // vnitrni okno vieweru
    HIMAGELIST HGrayToolBarImageList; // toolbar a menu v sedivem provedeni (pocitano z barevneho)
    HIMAGELIST HHotToolBarImageList;  // toolbar a menu v barevnem provedeni

    DWORD Enablers[vweCount];

    HWND HRebar; // drzi MenuBar a ToolBar
    HANDLE HCombo;
    CGUIMenuPopupAbstract* MainMenu;
    CGUIMenuBarAbstract* MenuBar;
    CGUIToolBarAbstract* ToolBar;

    int EnumFilesSourceUID;    // UID zdroje pro enumeraci souboru ve vieweru
    int EnumFilesCurrentIndex; // index aktualniho souboru ve vieweru ve zdroji

    TextModel* TextModelPtr = NULL;
	Searcher* SearcherPtr = NULL;
    //std::string Content; // text suboru
    //std::vector<int> LineOffsets;

public:
    CViewerWindow(int enumFilesSourceUID, int enumFilesCurrentIndex);
    ~CViewerWindow();

    HANDLE GetLock();

    // je-li 'setLock' TRUE, dojde k nastaveni 'Lock' do signaled stavu (je
    // potreba po zavreni souboru)
    void OpenFile(const char* name, BOOL setLock = TRUE, UINT default_encoding = CP_ACP);

    BOOL IsMenuBarMessage(CONST MSG* lpMsg);

    BOOL GetFindText(WCHAR* buf, int& len);

protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    BOOL FillToolBar();

    BOOL InitializeGraphics();
    BOOL ReleaseGraphics();

    BOOL InsertMenuBand();
    BOOL InsertToolBarBand();

    void LayoutWindows();

    void UpdateEnablers();

    void ToggleFullscreen();
    void GotoLineDlg();
    void SearchDlg();
    void CheckEncodingMenuItem(WORD item);
    void ProcessSearchResult(search_match result);
    void NextPrevFile(bool prev);
};

extern CWindowQueue ViewerWindowQueue; // seznam vsech oken viewru
extern CThreadQueue ThreadQueue;       // seznam vsech threadu oken

// rozhrani pluginu poskytnute Salamanderovi
extern CPluginInterface PluginInterface;

// otevre konfiguracni dialog; pokud jiz existuje, zobrazi hlasku a vrati se
void OnConfiguration(HWND hParent);

// otevre About okno
void OnAbout(HWND hParent);
