
#include "EmbeddedBrowser.h"
#include <exdisp.h>
#include <mshtml.h>


CWebBrowser::CWebBrowser(HWND hWndOwner, LPRECT prect)
{
	dwRef = 1;
	hWnd = hWndOwner;
	pStorage = new CStorage;

	RECT rc;
	if (prect)
		memcpy(&rc, prect, sizeof(RECT));
	else
		GetClientRect(hWnd, &rc);
	if (!hWnd) return;

	OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0, (IOleClientSite*)this, (IStorage*)pStorage, (void**)&pObj);

	pObj->SetHostNames(L"dic5", NULL);
	OleSetContainedObject((IUnknown*)pObj, TRUE);
	pObj->DoVerb(OLEIVERB_SHOW, NULL, (IOleClientSite*)this, -1, hWnd, &rc);

	Resize(&rc);

	IWebBrowser2 *pWB; VARIANT v;
	pObj->QueryInterface(IID_IWebBrowser2, (void**)&pWB);
	VariantInit(&v); v.vt = VT_BSTR; v.bstrVal = SysAllocString(L"about:blank");
	pWB->Navigate2(&v, NULL, NULL, NULL, NULL);
	VariantClear(&v);
	pWB->Release();
}

CWebBrowser::~CWebBrowser()
{
	pObj->Close(OLECLOSE_NOSAVE);
	pObj->Release();

	delete pStorage;
}


STDMETHODIMP CWebBrowser::QueryInterface(REFIID riid, LPVOID* ppv)
{
    if (riid == IID_IUnknown)					*ppv = (IUnknown*)(IOleClientSite*)this; // Hani: remove IUnknown from explicit base classes
    else if (riid == IID_IOleClientSite)		*ppv = (IOleClientSite*)this;
    else if (riid == IID_IOleWindow)			*ppv = (IOleWindow*)(IOleClientSite*)this; // Hani: remove IOleWindow from explicit base classes
    else if (riid == IID_IOleInPlaceSite)		*ppv = (IOleInPlaceSite*)this;
    else if (riid == IID_IOleInPlaceUIWindow)	*ppv = (IOleInPlaceUIWindow*)this;
    else if (riid == IID_IOleInPlaceFrame)		*ppv = (IOleInPlaceFrame*)this;
    else if (riid == IID_IDocHostUIHandler)		*ppv = (IDocHostUIHandler*)this;
    else { *ppv = NULL; return E_NOINTERFACE; }
    AddRef(); return S_OK;
}
STDMETHODIMP_(ULONG) CWebBrowser::AddRef() { return ++dwRef; }
STDMETHODIMP_(ULONG) CWebBrowser::Release() { if (--dwRef == 0) { delete this; return 0; } return dwRef; }


STDMETHODIMP CWebBrowser::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*ppFrame					= (IOleInPlaceFrame*)this;
	*ppDoc						= NULL;

	lpFrameInfo->fMDIApp		= FALSE;
	lpFrameInfo->hwndFrame		= hWnd;
	lpFrameInfo->haccel			= NULL;
	lpFrameInfo->cAccelEntries	= 0;

	// Give the browser the dimensions of where it can draw. We give it our entire window to fill.
	// We do this in InPlace_OnPosRectChange() which is called right when a window is first
	// created anyway, so no need to duplicate it here.
	
	//GetClientRect(lpFrameInfo->hwndFrame, lprcPosRect);
	//GetClientRect(lpFrameInfo->hwndFrame, lprcClipRect);

	return S_OK;
}

STDMETHODIMP CWebBrowser::OnPosRectChange(LPCRECT lprcPosRect)
{
	IOleInPlaceObject *pInPlaceObj;
	if (SUCCEEDED(pObj->QueryInterface(IID_IOleInPlaceObject, (void**)&pInPlaceObj)))
		pInPlaceObj->SetObjectRects(lprcPosRect, lprcPosRect);

	return S_OK;
}


STDMETHODIMP CWebBrowser::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER;
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
	return S_OK;
}

STDMETHODIMP CWebBrowser::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	if (lpMsg && lpMsg->message==WM_KEYDOWN && lpMsg->wParam==VK_TAB)
	{
		MessageBox(hWnd, L"Tab pressed.", 0, 0);
		return S_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CWebBrowser::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	MessageBox(hWnd, pchURLIn, L"Url", 0);
	if (lstrlen(pchURLIn)>=4 && CompareString(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), NORM_IGNORECASE, pchURLIn, 4, L"app:", 4)==CSTR_EQUAL)
	{
		if (*ppchURLOut = (OLECHAR*)CoTaskMemAlloc(24))
		{
			lstrcpy(*ppchURLOut, L"about:blank");
			PostMessage(hWnd, WM_APP, (WPARAM)_wtoi(pchURLIn+4), 0);
			return S_OK;
		}
	}

	*ppchURLOut = NULL;
    return S_FALSE;
}


BOOL CWebBrowser::DisplayString(BSTR bstr)
{
	static const SAFEARRAYBOUND SAB_ARRAYBOUNDS = {1, 0};

	IWebBrowser2 *pWB;
	IHTMLDocument2 *pHTML;
	IDispatch *pDispatch;
	VARIANT *pV;
	SAFEARRAY *pSA;


	pObj->QueryInterface(IID_IWebBrowser2, (void**)&pWB);
	pWB->get_Document(&pDispatch);
	pDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pHTML);


	pSA = SafeArrayCreate(VT_VARIANT, 1, (SAFEARRAYBOUND*)&SAB_ARRAYBOUNDS);
    SafeArrayAccessData(pSA, (void**)&pV);
	pV->vt = VT_BSTR;
	pV->bstrVal = SysAllocString(bstr);

	pHTML->write(pSA);
	pHTML->close();


	SafeArrayDestroy(pSA);
	pHTML->Release();
	pDispatch->Release();
	pWB->Release();

	return TRUE;
}

BOOL CWebBrowser::Resize(LPRECT prc)
{
	IWebBrowser2 *pWB;
	pObj->QueryInterface(IID_IWebBrowser2, (void**)&pWB);

	pWB->put_Left(prc->left);
	pWB->put_Top(prc->top);
	pWB->put_Width(prc->right-prc->left);
	pWB->put_Height(prc->bottom-prc->top);

	pWB->Release();
	return TRUE;
}

BOOL CWebBrowser::GetRect(LPRECT prc)
{
	long x, y, h, w;
	IWebBrowser2 *pWB;
	pObj->QueryInterface(IID_IWebBrowser2, (void**)&pWB);

	pWB->get_Left(&x);
	pWB->get_Top(&y);
	pWB->get_Width(&w);
	pWB->get_Height(&h);

	pWB->Release();

	prc->left = x;
	prc->top = y;
	prc->right = x+w;
	prc->bottom = y+h;

	return TRUE;
}

BOOL CWebBrowser::ScrollContent(long diff)
{
	IWebBrowser2 *pWB;
	IDispatch *pDispatch;
	IHTMLDocument2 *pHTML;
	IHTMLElement *e;
	IHTMLElement2 *e2;
	long top;


	pObj->QueryInterface(IID_IWebBrowser2, (void**)&pWB);
	pWB->get_Document(&pDispatch);
	pDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pHTML);
	pHTML->get_body(&e);
	e->QueryInterface(IID_IHTMLElement2, (void**)&e2);

	e2->get_scrollTop(&top);
	e2->put_scrollTop(top+diff);

	e2->Release();
	e->Release();
	pHTML->Release();
	pDispatch->Release();
	pWB->Release();
	return TRUE;
}

