
#pragma once

#include "Crib/WindowsBase.h"
#include <exdisp.h>
#include <mshtml.h>
#include <mshtmhst.h>


class CStorage : public IStorage
{
public:
	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppv) { return E_NOTIMPL; }
	STDMETHOD_(ULONG, AddRef)() { return 1; }
	STDMETHOD_(ULONG, Release)() { return 1; }

	// IStorage methods
	STDMETHOD(CreateStream)(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
	STDMETHOD(OpenStream)(const OLECHAR *pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
	STDMETHOD(CreateStorage)(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg) { return E_NOTIMPL; }
	STDMETHOD(OpenStorage)(const OLECHAR *pwcsName, IStorage *pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg) { return E_NOTIMPL; }
	STDMETHOD(CopyTo)(DWORD ciidExclude, const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest) { return E_NOTIMPL; }
	STDMETHOD(MoveElementTo)(const OLECHAR *pwcsName, IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags) { return E_NOTIMPL; }
	STDMETHOD(Commit)(DWORD grfCommitFlags) { return E_NOTIMPL; }
	STDMETHOD(Revert)() { return E_NOTIMPL; }
	STDMETHOD(EnumElements)(DWORD reserved1, void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum) { return E_NOTIMPL; }
	STDMETHOD(DestroyElement)(const OLECHAR *pwcsName) { return E_NOTIMPL; }
	STDMETHOD(RenameElement)(const OLECHAR *pwcsOldName, const OLECHAR *pwcsNewName) { return E_NOTIMPL; }
	STDMETHOD(SetElementTimes)(const OLECHAR *pwcsName, const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime) { return E_NOTIMPL; }
	STDMETHOD(SetClass)(REFCLSID clsid) { return S_OK; }
	STDMETHOD(SetStateBits)(DWORD grfStateBits, DWORD grfMask) { return E_NOTIMPL; }
	STDMETHOD(Stat)(STATSTG *pstatstg, DWORD grfStatFlag) { return E_NOTIMPL; }
};



class CWebBrowser : public /*IUnknown, */IOleClientSite, /*IOleWindow, */IOleInPlaceSite, IOleInPlaceFrame, IDocHostUIHandler
{
public:
	CWebBrowser(HWND, LPRECT);
	~CWebBrowser(void);

	// IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppv);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

	// IOleClient methods
	STDMETHOD(SaveObject)() { return E_NOTIMPL; }
	STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk) { return E_NOTIMPL; }
	STDMETHOD(GetContainer)(LPOLECONTAINER FAR* ppContainer) { ppContainer = NULL; return E_NOINTERFACE; }
	STDMETHOD(ShowObject)() { return S_OK; }
	STDMETHOD(OnShowWindow)(BOOL fShow) { return E_NOTIMPL; }
	STDMETHOD(RequestNewObjectLayout)() { return E_NOTIMPL; }

	// IOleWindow methods
	STDMETHOD(GetWindow)(HWND *phwnd) { *phwnd = hWnd; return S_OK; }
	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode) { return E_NOTIMPL; }

	// IOleInPlaceSite methods
	STDMETHOD(CanInPlaceActivate)() { return S_OK; }
	STDMETHOD(OnInPlaceActivate)() { return S_OK; }
	STDMETHOD(OnUIActivate)() { return S_OK; }
	STDMETHOD(GetWindowContext)(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	STDMETHOD(Scroll)(SIZE scrollExtent) { return E_NOTIMPL; }
	STDMETHOD(OnUIDeactivate)(BOOL fUndoable) { return S_OK; }
	STDMETHOD(OnInPlaceDeactivate)() { return S_OK; }
	STDMETHOD(DiscardUndoState)() { return E_NOTIMPL; }
	STDMETHOD(DeactivateAndUndo)() { return E_NOTIMPL; }
	STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

	// IOleInPlaceUIWindow methods
	STDMETHOD(GetBorder)(LPRECT lprectBorder) { return E_NOTIMPL; }
	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName) { return S_OK; }

	// IOleInPlaceFrame methods
	STDMETHOD(InsertMenus)(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) { return E_NOTIMPL; }
	STDMETHOD(SetMenu)(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject) { return S_OK; }
	STDMETHOD(RemoveMenus)(HMENU hmenuShared) { return E_NOTIMPL; }
	STDMETHOD(SetStatusText)(LPCOLESTR pszStatusText) { return S_OK; }
	STDMETHOD(EnableModeless)(BOOL fEnable) { return S_OK; }
	STDMETHOD(TranslateAccelerator)(LPMSG lpmsg, WORD wID) { return E_NOTIMPL; }

	// IDocHostUIHandler methods
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved) { return S_FALSE; }
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc) { return S_OK; }
	STDMETHOD(HideUI)() { return S_OK; }
	STDMETHOD(UpdateUI)() { return S_OK; }
	//STDMETHOD(EnableModeless)(BOOL fEnable) { return S_OK; }
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate) { return S_OK; }
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate) { return S_OK; }
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow) { return S_OK; }
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
	STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw) { return S_FALSE; }
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget) { return S_FALSE; }
	STDMETHOD(GetExternal)(IDispatch **ppDispatch) { ppDispatch = NULL; return S_FALSE; }
	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
	STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet) { *ppDORet = NULL; return S_FALSE; }

	// Utility functions
	BOOL DisplayString(BSTR);
	BOOL Resize(LPRECT);
	BOOL GetRect(LPRECT);
	BOOL ScrollContent(long);


protected:
	DWORD dwRef;

	IOleObject *pObj;
	CStorage *pStorage;
	HWND hWnd;
};