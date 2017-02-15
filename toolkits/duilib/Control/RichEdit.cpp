#include "StdAfx.h"

// These constants are for backward compatibility. They are the 
// sizes used for initialization and reset in RichEdit 1.0

namespace ui {

#define ID_RICH_UNDO			101
#define ID_RICH_CUT				102
#define ID_RICH_COPY			103
#define ID_RICH_PASTE			104
#define ID_RICH_CLEAR			105
#define ID_RICH_SELECTALL		106
#define ID_RICH_REDO			107

const LONG cInitTextMax = (32 * 1024) - 1;

EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

#include <textserv.h>

class TxtWinHost : public ITextHost
{
public:
    TxtWinHost();
    BOOL Init(RichEdit *re , const CREATESTRUCT *pcs);
    virtual ~TxtWinHost();

    ITextServices* GetTextServices(void) { return pserv; }
    void SetClientRect(RECT *prc);
    RECT* GetClientRect() { return &rcClient; }
    BOOL GetWordWrap(void) { return fWordWrap; }
    void SetWordWrap(BOOL fWordWrap);
    BOOL GetReadOnly();
    void SetReadOnly(BOOL fReadOnly);
    void SetFont(HFONT hFont);
    void SetColor(DWORD dwColor);
    SIZEL* GetExtent();
    void SetExtent(SIZEL *psizelExtent);
    void LimitText(LONG nChars);
    BOOL IsCaptured();
	BOOL IsShowCaret();
	void NeedFreshCaret();
	INT GetCaretWidth();
	INT GetCaretHeight();

    BOOL GetAllowBeep();
    void SetAllowBeep(BOOL fAllowBeep);
    WORD GetDefaultAlign();
    void SetDefaultAlign(WORD wNewAlign);
    BOOL GetRichTextFlag();
    void SetRichTextFlag(BOOL fNew);
    LONG GetDefaultLeftIndent();
    void SetDefaultLeftIndent(LONG lNewIndent);
    BOOL SetSaveSelection(BOOL fSaveSelection);
    HRESULT OnTxInPlaceDeactivate();
    HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
    BOOL GetActiveState(void) { return fInplaceActive; }
    BOOL DoSetCursor(RECT *prc, POINT *pt);
    void SetTransparent(BOOL fTransparent);
    void GetControlRect(LPRECT prc);
    LONG SetAccelPos(LONG laccelpos);
    WCHAR SetPasswordChar(WCHAR chPasswordChar);
    void SetDisabled(BOOL fOn);
    LONG SetSelBarWidth(LONG lSelBarWidth);
    BOOL GetTimerState();

    void SetCharFormat(CHARFORMAT2W &c);
    void SetParaFormat(PARAFORMAT2 &p);

    // -----------------------------
    //	IUnknown interface
    // -----------------------------
    virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG _stdcall AddRef(void);
    virtual ULONG _stdcall Release(void);

    // -----------------------------
    //	ITextHost interface
    // -----------------------------
    virtual HDC TxGetDC();
    virtual INT TxReleaseDC(HDC hdc);
    virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
    virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
    virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
    virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
    virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
    virtual void TxViewChange(BOOL fUpdate);
    virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    virtual BOOL TxShowCaret(BOOL fShow);
    virtual BOOL TxSetCaretPos(INT x, INT y);
    virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
    virtual void TxKillTimer(UINT idTimer);
    virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
    virtual void TxSetCapture(BOOL fCapture);
    virtual void TxSetFocus();
    virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
    virtual BOOL TxScreenToClient (LPPOINT lppt);
    virtual BOOL TxClientToScreen (LPPOINT lppt);
    virtual HRESULT TxActivate( LONG * plOldState );
    virtual HRESULT TxDeactivate( LONG lNewState );
    virtual HRESULT TxGetClientRect(LPRECT prc);
    virtual HRESULT TxGetViewInset(LPRECT prc);
    virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
    virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
    virtual COLORREF TxGetSysColor(int nIndex);
    virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
    virtual HRESULT TxGetMaxLength(DWORD *plength);
    virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
    virtual HRESULT TxGetPasswordChar(TCHAR *pch);
    virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
    virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
    virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
    virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
    virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
    virtual HRESULT TxNotify(DWORD iNotify, void *pv);
    virtual HIMC TxImmGetContext(void);
    virtual void TxImmReleaseContext(HIMC himc);
    virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

private:
    RichEdit *m_re;
    ULONG	cRefs;					// Reference Count
    ITextServices	*pserv;		    // pointer to Text Services object
    // Properties
    DWORD		dwStyle;				// style bits
    unsigned	fEnableAutoWordSel	:1;	// enable Word style auto word selection?
    unsigned	fWordWrap			:1;	// Whether control should word wrap
    unsigned	fAllowBeep			:1;	// Whether beep is allowed
    unsigned	fRich				:1;	// Whether control is rich text
    unsigned	fSaveSelection		:1;	// Whether to save the selection when inactive
    unsigned	fInplaceActive		:1; // Whether control is inplace active
    unsigned	fTransparent		:1; // Whether control is transparent
    unsigned	fTimer				:1;	// A timer is set
	unsigned    fCaptured           :1;
	unsigned    fShowCaret          :1;
	unsigned    fNeedFreshCaret     :1; // 修正改变大小后点击其他位置原来光标不能消除的问题

	INT         iCaretWidth;
	INT         iCaretHeight;
	INT         iCaretLastWidth;
	INT         iCaretLastHeight;

    LONG		lSelBarWidth;			// Width of the selection bar
    LONG  		cchTextMost;			// maximum text size
    DWORD		dwEventMask;			// DoEvent mask to pass on to parent window
    LONG		icf;
    LONG		ipf;
    RECT		rcClient;				// Client Rect for this control
    SIZEL		sizelExtent;			// Extent array
    CHARFORMAT2W cf;					// Default character format
    PARAFORMAT2	pf;					    // Default paragraph format
    LONG		laccelpos;				// Accelerator position
    WCHAR		chPasswordChar;		    // Password character
};

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
    return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
    return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

HRESULT InitDefaultCharFormat(RichEdit* re, CHARFORMAT2W* pcf, HFONT hfont) 
{
    memset(pcf, 0, sizeof(CHARFORMAT2W));
    LOGFONT lf;
    if( !hfont )
        hfont = GlobalManager::GetFont(re->GetFont());
    ::GetObject(hfont, sizeof(LOGFONT), &lf);

    DWORD dwColor = re->GetTextColor();
	if(re->GetManager()->IsLayered()) {
		RenderEngine::CheckAlphaColor(dwColor);
	}
    pcf->cbSize = sizeof(CHARFORMAT2W);
    pcf->crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    LONG yPixPerInch = GetDeviceCaps(re->GetManager()->GetPaintDC(), LOGPIXELSY);
    pcf->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    pcf->yOffset = 0;
    pcf->dwEffects = 0;
    pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
    if(lf.lfWeight >= FW_BOLD)
        pcf->dwEffects |= CFE_BOLD;
    if(lf.lfItalic)
        pcf->dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline)
        pcf->dwEffects |= CFE_UNDERLINE;
    pcf->bCharSet = lf.lfCharSet;
    pcf->bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
    _tcscpy(pcf->szFaceName, lf.lfFaceName);
#else
    //need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE) ;
#endif

    return S_OK;
}

HRESULT InitDefaultParaFormat(RichEdit* re, PARAFORMAT2* ppf) 
{	
    memset(ppf, 0, sizeof(PARAFORMAT2));
    ppf->cbSize = sizeof(PARAFORMAT2);
    ppf->dwMask = PFM_ALL;
    ppf->wAlignment = PFA_LEFT;
    ppf->cTabCount = 1;
    ppf->rgxTabs[0] = lDefaultTab;

    return S_OK;
}

HRESULT CreateHost(RichEdit *re, const CREATESTRUCT *pcs, TxtWinHost **pptec)
{
    HRESULT hr = E_FAIL;

    TxtWinHost *phost = new TxtWinHost();
    if(phost) {
        if (phost->Init(re, pcs)) {
            *pptec = phost;
            hr = S_OK;
        }
    }

    if (FAILED(hr)) {
        delete phost;
    }

    return TRUE;
}

TxtWinHost::TxtWinHost() : m_re(NULL)
{
    ::ZeroMemory(&cRefs, sizeof(TxtWinHost) - offsetof(TxtWinHost, cRefs));
    cchTextMost = cInitTextMax;
    laccelpos = -1;
}

TxtWinHost::~TxtWinHost()
{
    pserv->OnTxInPlaceDeactivate();
    pserv->Release();
}

////////////////////// Create/Init/Destruct Commands ///////////////////////

BOOL TxtWinHost::Init(RichEdit *re, const CREATESTRUCT *pcs)
{
    IUnknown *pUnk;
    HRESULT hr;

    m_re = re;
    // Initialize Reference count
    cRefs = 1;

    // Create and cache CHARFORMAT for this control
    if(FAILED(InitDefaultCharFormat(re, &cf, NULL)))
        goto err;

    // Create and cache PARAFORMAT for this control
    if(FAILED(InitDefaultParaFormat(re, &pf)))
        goto err;

    // edit controls created without a window are multiline by default
    // so that paragraph formats can be
    dwStyle = ES_MULTILINE;

    // edit controls are rich by default
    fRich = re->IsRich();

    cchTextMost = re->GetLimitText();

    if (pcs ) {
        dwStyle = pcs->style;
        if ( !(dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)) ) {
            fWordWrap = TRUE;
        }
    }

    if( !(dwStyle & ES_LEFT) ) {
        if(dwStyle & ES_CENTER)
            pf.wAlignment = PFA_CENTER;
        else if(dwStyle & ES_RIGHT)
            pf.wAlignment = PFA_RIGHT;
    }

    fInplaceActive = TRUE;

	PCreateTextServices TextServicesProc;
#ifdef _UNICODE		
	HMODULE hmod = LoadLibrary(_T("Msftedit.dll"));
#else
	HMODULE hmod = LoadLibrary(_T("Riched20.dll"));
#endif
	if (hmod) {
		TextServicesProc = (PCreateTextServices)GetProcAddress(hmod,"CreateTextServices");
	}
	if (TextServicesProc) {
		HRESULT hr = TextServicesProc(NULL, this, &pUnk);
	}

    hr = pUnk->QueryInterface(IID_ITextServices,(void **)&pserv);

    // Whether the previous call succeeded or failed we are done
    // with the private interface.
    pUnk->Release();

    if(FAILED(hr)) {
        goto err;
    }

    // Set window text
    if(pcs && pcs->lpszName) {
#ifdef _UNICODE		
        if(FAILED(pserv->TxSetText((TCHAR *)pcs->lpszName)))
            goto err;
#else
        size_t iLen = _tcslen(pcs->lpszName);
        LPWSTR lpText = new WCHAR[iLen + 1];
        ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, pcs->lpszName, -1, (LPWSTR)lpText, iLen) ;
        if(FAILED(pserv->TxSetText((LPWSTR)lpText))) {
            delete[] lpText;
            goto err;
        }
        delete[] lpText;
#endif
    }

    return TRUE;

err:
    return FALSE;
}

/////////////////////////////////  IUnknown ////////////////////////////////


HRESULT TxtWinHost::QueryInterface(REFIID riid, void **ppvObject)
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextHost)) 
    {
        AddRef();
        *ppvObject = (ITextHost *) this;
        hr = S_OK;
    }

    return hr;
}

ULONG TxtWinHost::AddRef(void)
{
    return ++cRefs;
}

ULONG TxtWinHost::Release(void)
{
    ULONG c_Refs = --cRefs;

    if (c_Refs == 0)
    {
        delete this;
    }

    return c_Refs;
}

/////////////////////////////////  Far East Support  //////////////////////////////////////

HIMC TxtWinHost::TxImmGetContext(void)
{
    return NULL;
}

void TxtWinHost::TxImmReleaseContext(HIMC himc)
{
}

//////////////////////////// ITextHost Interface  ////////////////////////////

HDC TxtWinHost::TxGetDC()
{
    return m_re->GetManager()->GetPaintDC();
}

int TxtWinHost::TxReleaseDC(HDC hdc)
{
    return 1;
}

BOOL TxtWinHost::TxShowScrollBar(INT fnBar, BOOL fShow)
{
    ScrollBar* pVerticalScrollBar = m_re->GetVerticalScrollBar();
    ScrollBar* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if( fnBar == SB_VERT && pVerticalScrollBar ) {
        pVerticalScrollBar->SetVisible(fShow == TRUE);
    }
    else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
        pHorizontalScrollBar->SetVisible(fShow == TRUE);
    }
    else if( fnBar == SB_BOTH ) {
        if( pVerticalScrollBar ) pVerticalScrollBar->SetVisible(fShow == TRUE);
        if( pHorizontalScrollBar ) pHorizontalScrollBar->SetVisible(fShow == TRUE);
    }
    return TRUE;
}

BOOL TxtWinHost::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
    if( fuSBFlags == SB_VERT ) {
        m_re->EnableScrollBar(true, m_re->GetHorizontalScrollBar() != NULL);
        m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }
    else if( fuSBFlags == SB_HORZ ) {
        m_re->EnableScrollBar(m_re->GetVerticalScrollBar() != NULL, true);
        m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }
    else if( fuSBFlags == SB_BOTH ) {
        m_re->EnableScrollBar(true, true);
        m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
        m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }
    return TRUE;
}

BOOL TxtWinHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
    ScrollBar* pVerticalScrollBar = m_re->GetVerticalScrollBar();
    ScrollBar* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if( fnBar == SB_VERT && pVerticalScrollBar ) {
        if( nMaxPos - nMinPos - rcClient.bottom + rcClient.top <= 0 ) {
            pVerticalScrollBar->SetVisible(false);
        }
        else {
            pVerticalScrollBar->SetVisible(true);
            pVerticalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.bottom + rcClient.top);
        }
    }
    else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
        if( nMaxPos - nMinPos - rcClient.right + rcClient.left <= 0 ) {
            pHorizontalScrollBar->SetVisible(false);
        }
        else {
            pHorizontalScrollBar->SetVisible(true);
            pHorizontalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.right + rcClient.left);
        }   
    }
    return TRUE;
}

BOOL TxtWinHost::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
{
    ScrollBar* pVerticalScrollBar = m_re->GetVerticalScrollBar();
    ScrollBar* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if( fnBar == SB_VERT && pVerticalScrollBar ) {
        pVerticalScrollBar->SetScrollPos(nPos);
    }
    else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
        pHorizontalScrollBar->SetScrollPos(nPos);
    }
    return TRUE;
}

void TxtWinHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
    if( prc == NULL ) {
        m_re->GetManager()->Invalidate(rcClient);
        return;
    }
    RECT rc = *prc;
    m_re->GetManager()->Invalidate(rc);
}

void TxtWinHost::TxViewChange(BOOL fUpdate) 
{
    if( m_re->OnTxViewChanged(fUpdate) ) m_re->Invalidate();
}

BOOL TxtWinHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
	iCaretWidth = xWidth;
	iCaretHeight = yHeight;
	if (m_re->GetManager()->IsLayered()) {
		return m_re->GetManager()->CreateCaret(hbmp, xWidth, yHeight);
	}
	else {
		return ::CreateCaret(m_re->GetManager()->GetPaintWindow(), hbmp, xWidth, yHeight);
	}
}

BOOL TxtWinHost::TxShowCaret(BOOL fShow)
{
	fShowCaret = fShow;
	if (m_re->GetManager()->IsLayered()) {
		if(m_re->GetManager()->GetCurrentCaretObject() == m_re) {
			if((m_re->IsReadOnly() || !m_re->Activate())) {
				m_re->GetManager()->ShowCaret(false);
				return TRUE;
			}
		}

		return m_re->GetManager()->ShowCaret(fShow == TRUE);
	}
	else {
		if(fShow)
			return ::ShowCaret(m_re->GetManager()->GetPaintWindow());
		else
			return ::HideCaret(m_re->GetManager()->GetPaintWindow());
	}
}

BOOL TxtWinHost::TxSetCaretPos(INT x, INT y)
{
	if (m_re->GetManager()->IsLayered()) {
		m_re->GetManager()->SetCaretPos(m_re, x, y);
		return true;
	}
	else {
		return ::SetCaretPos(x, y);
	}
}

BOOL TxtWinHost::TxSetTimer(UINT idTimer, UINT uTimeout)
{
	fTimer = TRUE;
	return m_re->GetManager()->SetTimer(m_re, idTimer, uTimeout) == TRUE;
}

void TxtWinHost::TxKillTimer(UINT idTimer)
{
	m_re->GetManager()->KillTimer(m_re, idTimer);
	fTimer = FALSE;
}

void TxtWinHost::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
{
    return;
}

void TxtWinHost::TxSetCapture(BOOL fCapture)
{
    if (fCapture) m_re->GetManager()->SetCapture();
    else m_re->GetManager()->ReleaseCapture();
    fCaptured = fCapture;
}

void TxtWinHost::TxSetFocus()
{
    m_re->SetFocus();
}

void TxtWinHost::TxSetCursor(HCURSOR hcur,	BOOL fText)
{
    ::SetCursor(hcur);
}

BOOL TxtWinHost::TxScreenToClient(LPPOINT lppt)
{
    return ::ScreenToClient(m_re->GetManager()->GetPaintWindow(), lppt);	
}

BOOL TxtWinHost::TxClientToScreen(LPPOINT lppt)
{
    return ::ClientToScreen(m_re->GetManager()->GetPaintWindow(), lppt);
}

HRESULT TxtWinHost::TxActivate(LONG *plOldState)
{
    return S_OK;
}

HRESULT TxtWinHost::TxDeactivate(LONG lNewState)
{
    return S_OK;
}

HRESULT TxtWinHost::TxGetClientRect(LPRECT prc)
{
    *prc = rcClient;
    GetControlRect(prc);
    return NOERROR;
}

HRESULT TxtWinHost::TxGetViewInset(LPRECT prc) 
{
    prc->left = prc->right = prc->top = prc->bottom = 0;
    return NOERROR;	
}

HRESULT TxtWinHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
    *ppCF = &cf;
    return NOERROR;
}

HRESULT TxtWinHost::TxGetParaFormat(const PARAFORMAT **ppPF)
{
    *ppPF = &pf;
    return NOERROR;
}

COLORREF TxtWinHost::TxGetSysColor(int nIndex) 
{
	DWORD dwColor = ::GetSysColor(nIndex);
	RenderEngine::CheckAlphaColor(dwColor);
    return dwColor;
}

HRESULT TxtWinHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    *pstyle = !fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
    return NOERROR;
}

HRESULT TxtWinHost::TxGetMaxLength(DWORD *pLength)
{
    *pLength = cchTextMost;
    return NOERROR;
}

HRESULT TxtWinHost::TxGetScrollBars(DWORD *pdwScrollBar)
{
    *pdwScrollBar =  dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
        ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return NOERROR;
}

HRESULT TxtWinHost::TxGetPasswordChar(TCHAR *pch)
{
#ifdef _UNICODE
    *pch = chPasswordChar;
#else
    ::WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
    return NOERROR;
}

HRESULT TxtWinHost::TxGetAcceleratorPos(LONG *pcp)
{
    *pcp = laccelpos;
    return S_OK;
} 										   

HRESULT TxtWinHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
{
    return S_OK;
}

HRESULT TxtWinHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
{
    return S_OK;
}

HRESULT TxtWinHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
{
    DWORD dwProperties = 0;

    if (fRich)
    {
        dwProperties = TXTBIT_RICHTEXT;
    }

    if (dwStyle & ES_MULTILINE)
    {
        dwProperties |= TXTBIT_MULTILINE;
    }

    if (dwStyle & ES_READONLY)
    {
        dwProperties |= TXTBIT_READONLY;
    }

    if (dwStyle & ES_PASSWORD)
    {
        dwProperties |= TXTBIT_USEPASSWORD;
    }

    if (!(dwStyle & ES_NOHIDESEL))
    {
        dwProperties |= TXTBIT_HIDESELECTION;
    }

    if (fEnableAutoWordSel)
    {
        dwProperties |= TXTBIT_AUTOWORDSEL;
    }

    if (fWordWrap)
    {
        dwProperties |= TXTBIT_WORDWRAP;
    }

    if (fAllowBeep)
    {
        dwProperties |= TXTBIT_ALLOWBEEP;
    }

    if (fSaveSelection)
    {
        dwProperties |= TXTBIT_SAVESELECTION;
    }

    *pdwBits = dwProperties & dwMask; 
    return NOERROR;
}


HRESULT TxtWinHost::TxNotify(DWORD iNotify, void *pv)
{
    if( iNotify == EN_REQUESTRESIZE ) {
        RECT rc;
        REQRESIZE *preqsz = (REQRESIZE *)pv;
        GetControlRect(&rc);
        rc.bottom = rc.top + preqsz->rc.bottom;
        rc.right  = rc.left + preqsz->rc.right;
        SetClientRect(&rc);
    }
    m_re->OnTxNotify(iNotify, pv);
    return S_OK;
}

HRESULT TxtWinHost::TxGetExtent(LPSIZEL lpExtent)
{
    *lpExtent = sizelExtent;
    return S_OK;
}

HRESULT	TxtWinHost::TxGetSelectionBarWidth (LONG *plSelBarWidth)
{
    *plSelBarWidth = lSelBarWidth;
    return S_OK;
}

void TxtWinHost::SetWordWrap(BOOL fWordWrap)
{
    fWordWrap = fWordWrap;
    pserv->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
}

BOOL TxtWinHost::GetReadOnly()
{
    return (dwStyle & ES_READONLY) != 0;
}

void TxtWinHost::SetReadOnly(BOOL fReadOnly)
{
    if (fReadOnly) {
        dwStyle |= ES_READONLY;
    }
    else {
        dwStyle &= ~ES_READONLY;
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, fReadOnly ? TXTBIT_READONLY : 0);
}

void TxtWinHost::SetFont(HFONT hFont) 
{
    if( hFont == NULL ) return;
    LOGFONT lf;
    ::GetObject(hFont, sizeof(LOGFONT), &lf);
    LONG yPixPerInch = ::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSY);
    cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    if(lf.lfWeight >= FW_BOLD) cf.dwEffects |= CFE_BOLD;
    if(lf.lfItalic) cf.dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline) cf.dwEffects |= CFE_UNDERLINE;
    cf.bCharSet = lf.lfCharSet;
    cf.bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
    _tcscpy(cf.szFaceName, lf.lfFaceName);
#else
    //need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, cf.szFaceName, LF_FACESIZE) ;
#endif

    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

void TxtWinHost::SetColor(DWORD dwColor)
{
	RenderEngine::CheckAlphaColor(dwColor);
    cf.crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

SIZEL* TxtWinHost::GetExtent() 
{
    return &sizelExtent;
}

void TxtWinHost::SetExtent(SIZEL *psizelExtent) 
{ 
    sizelExtent = *psizelExtent; 
    pserv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
}

void TxtWinHost::LimitText(LONG nChars)
{
    cchTextMost = nChars;
    if( cchTextMost <= 0 ) cchTextMost = cInitTextMax;
    pserv->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

BOOL TxtWinHost::IsCaptured()
{
	return fCaptured;
}

BOOL TxtWinHost::IsShowCaret()
{
	return fShowCaret;
}

void TxtWinHost::NeedFreshCaret()
{
	fNeedFreshCaret = TRUE;
}

INT TxtWinHost::GetCaretWidth()
{
	return iCaretWidth;
}

INT TxtWinHost::GetCaretHeight()
{
	return iCaretHeight;
}

BOOL TxtWinHost::GetAllowBeep()
{
	return fAllowBeep;
}

void TxtWinHost::SetAllowBeep(BOOL fAllowBeep)
{
	fAllowBeep = fAllowBeep;

	pserv->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

WORD TxtWinHost::GetDefaultAlign()
{
    return pf.wAlignment;
}

void TxtWinHost::SetDefaultAlign(WORD wNewAlign)
{
    pf.wAlignment = wNewAlign;

    pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL TxtWinHost::GetRichTextFlag()
{
    return fRich;
}

void TxtWinHost::SetRichTextFlag(BOOL fNew)
{
    fRich = fNew;

    pserv->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, fNew ? TXTBIT_RICHTEXT : 0);
}

LONG TxtWinHost::GetDefaultLeftIndent()
{
    return pf.dxOffset;
}

void TxtWinHost::SetDefaultLeftIndent(LONG lNewIndent)
{
    pf.dxOffset = lNewIndent;

    pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void TxtWinHost::SetClientRect(RECT *prc) 
{
    rcClient = *prc;

    LONG xPerInch = ::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSX); 
    LONG yPerInch =	::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSY); 
    sizelExtent.cx = DXtoHimetricX(rcClient.right - rcClient.left, xPerInch);
    sizelExtent.cy = DYtoHimetricY(rcClient.bottom - rcClient.top, yPerInch);

    pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

BOOL TxtWinHost::SetSaveSelection(BOOL f_SaveSelection)
{
    BOOL fResult = f_SaveSelection;
    fSaveSelection = f_SaveSelection;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, fSaveSelection ? TXTBIT_SAVESELECTION : 0);

    return fResult;		
}

HRESULT	TxtWinHost::OnTxInPlaceDeactivate()
{
    HRESULT hr = pserv->OnTxInPlaceDeactivate();

    if (SUCCEEDED(hr)) {
        fInplaceActive = FALSE;
    }

    return hr;
}

HRESULT	TxtWinHost::OnTxInPlaceActivate(LPCRECT prcClient)
{
    fInplaceActive = TRUE;

    HRESULT hr = pserv->OnTxInPlaceActivate(prcClient);

    if (FAILED(hr))
    {
        fInplaceActive = FALSE;
    }

    return hr;
}

BOOL TxtWinHost::DoSetCursor(RECT *prc, POINT *pt)
{
    RECT rc = prc ? *prc : rcClient;

    // Is this in our rectangle?
    if (PtInRect(&rc, *pt)) {
        RECT *prcClient = (!fInplaceActive || prc) ? &rc : NULL;
        pserv->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL,  m_re->GetManager()->GetPaintDC(), NULL, prcClient, pt->x, pt->y);

        return TRUE;
    }

    return FALSE;
}

void TxtWinHost::GetControlRect(LPRECT prc)
{
    prc->top = rcClient.top;
    prc->bottom = rcClient.bottom;
    prc->left = rcClient.left;
    prc->right = rcClient.right;
}

void TxtWinHost::SetTransparent(BOOL f_Transparent)
{
    fTransparent = f_Transparent;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG TxtWinHost::SetAccelPos(LONG l_accelpos)
{
    LONG laccelposOld = l_accelpos;

    laccelpos = l_accelpos;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

    return laccelposOld;
}

WCHAR TxtWinHost::SetPasswordChar(WCHAR ch_PasswordChar)
{
    WCHAR chOldPasswordChar = chPasswordChar;

    chPasswordChar = ch_PasswordChar;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, (chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

    return chOldPasswordChar;
}

void TxtWinHost::SetDisabled(BOOL fOn)
{
    cf.dwMask	 |= CFM_COLOR | CFM_DISABLED;
    cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

    if( !fOn ) {
        cf.dwEffects &= ~CFE_DISABLED;
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
        TXTBIT_CHARFORMATCHANGE);
}

LONG TxtWinHost::SetSelBarWidth(LONG l_SelBarWidth)
{
    LONG lOldSelBarWidth = lSelBarWidth;

    lSelBarWidth = l_SelBarWidth;

    if (lSelBarWidth) {
        dwStyle |= ES_SELECTIONBAR;
    }
    else {
        dwStyle &= (~ES_SELECTIONBAR);
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

    return lOldSelBarWidth;
}

BOOL TxtWinHost::GetTimerState()
{
    return fTimer;
}

void TxtWinHost::SetCharFormat(CHARFORMAT2W &c)
{
    cf = c;
}

void TxtWinHost::SetParaFormat(PARAFORMAT2 &p)
{
    pf = p;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
IMPLEMENT_DUICONTROL(RichEdit)

RichEdit::RichEdit() : win_host_(NULL), is_vscrollbar_fixing_(false), want_tab_(true), want_return_(true), 
    want_ctrl_return_(true), rich_(true), read_only_(false), word_wrap_(false), text_color_(0), font_(-1), 
    text_limit_(cInitTextMax), twh_style_(ES_MULTILINE), draw_caret_(true), inited_(false), lead_byte_(0),btn_state_(0),
	tip_color_(0xFFBAC0C5), tip_align_(DT_SINGLELINE | DT_LEFT)
{
#ifndef _UNICODE
	m_fAccumulateDBC =true;
#else
	accumulate_dbc_= false;
#endif
	::ZeroMemory(&text_padding_rect_, sizeof(text_padding_rect_));
}

RichEdit::~RichEdit()
{
    if( win_host_ ) {
        win_host_->Release();
        pm_->RemoveMessageFilter(this);
    }
}

std::wstring RichEdit::GetClass() const
{
    return DUI_CTR_RICHEDIT;
}

LPVOID RichEdit::GetInterface(std::wstring pstrName)
{
    if( _tcsicmp(pstrName.c_str(), DUI_CTR_RICHEDIT) == 0 ) return static_cast<RichEdit*>(this);
    return Box::GetInterface(pstrName);
}

UINT RichEdit::GetControlFlags() const
{
    if( !IsEnabled() ) return Control::GetControlFlags();

    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void RichEdit::AttachReturn(const EventCallback & cb)
{
	on_event_[kEventReturn] += cb;
}

void RichEdit::AttachTextChange(const EventCallback & cb)
{
	on_event_[kEventTextChange] += cb;
}

bool RichEdit::IsMultiLine()
{
	return (twh_style_ & ES_MULTILINE) == ES_MULTILINE;
}

void RichEdit::SetMultiLine(bool bMultiLine)
{
	if(!bMultiLine) twh_style_ &= ~ES_MULTILINE;
	else  twh_style_ |= ES_MULTILINE;
}

bool RichEdit::IsWantTab()
{
    return want_tab_;
}

void RichEdit::SetWantTab(bool bWantTab)
{
    want_tab_ = bWantTab;
}

bool RichEdit::IsWantReturn()
{
    return want_return_;
}

void RichEdit::SetWantReturn(bool bWantReturn)
{
    want_return_ = bWantReturn;
}

bool RichEdit::IsWantCtrlReturn()
{
    return want_ctrl_return_;
}

void RichEdit::SetWantCtrlReturn(bool bWantCtrlReturn)
{
    want_ctrl_return_ = bWantCtrlReturn;
}

bool RichEdit::IsRich()
{
    return rich_;
}

void RichEdit::SetRich(bool bRich)
{
    rich_ = bRich;
    if( win_host_ ) win_host_->SetRichTextFlag(bRich);
}

bool RichEdit::IsReadOnly()
{
    return read_only_;
}

void RichEdit::SetReadOnly(bool bReadOnly)
{
    read_only_ = bReadOnly;
    if( win_host_ ) win_host_->SetReadOnly(bReadOnly);
}

bool RichEdit::GetWordWrap()
{
    return word_wrap_;
}

void RichEdit::SetWordWrap(bool bWordWrap)
{
    word_wrap_ = bWordWrap;
    if( win_host_ ) win_host_->SetWordWrap(bWordWrap);
}

int RichEdit::GetFont()
{
    return font_;
}

void RichEdit::SetFont(int index)
{
    font_ = index;
    if( win_host_ ) {
        win_host_->SetFont(GlobalManager::GetFont(font_));
    }
}

void RichEdit::SetFont(std::wstring pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
    if( win_host_ ) {
        LOGFONT lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        _tcsncpy(lf.lfFaceName, pStrFontName.c_str(), LF_FACESIZE);
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -nSize;
        if( bBold ) lf.lfWeight += FW_BOLD;
        if( bUnderline ) lf.lfUnderline = TRUE;
        if( bItalic ) lf.lfItalic = TRUE;
        HFONT hFont = ::CreateFontIndirect(&lf);
        if( hFont == NULL ) return;
        win_host_->SetFont(hFont);
        ::DeleteObject(hFont);
    }
}

void RichEdit::SetEnabled(bool bEnabled)
{
	if (enabled_ == bEnabled) return;

	if( win_host_ ) {
		win_host_->SetColor(bEnabled ? text_color_ : GlobalManager::GetDefaultDisabledColor());
	}
	
	Box::SetEnabled(bEnabled);
}

LONG RichEdit::GetWinStyle()
{
    return twh_style_;
}

void RichEdit::SetWinStyle(LONG lStyle)
{
    twh_style_ = lStyle;
}

DWORD RichEdit::GetTextColor()
{
    return text_color_;
}

void RichEdit::SetTextColor(DWORD dwTextColor)
{
    text_color_ = dwTextColor;
    if( win_host_ ) {
        win_host_->SetColor(dwTextColor);
    }
}

int RichEdit::GetLimitText()
{
    return text_limit_;
}

void RichEdit::SetLimitText(int iChars)
{
    text_limit_ = iChars;
    if( win_host_ ) {
        win_host_->LimitText(text_limit_);
    }
}

long RichEdit::GetTextLength(DWORD dwFlags) const
{
    GETTEXTLENGTHEX textLenEx;
    textLenEx.flags = dwFlags;
#ifdef _UNICODE
    textLenEx.codepage = 1200;
#else
    textLenEx.codepage = CP_ACP;
#endif
    LRESULT lResult;
    TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
    return (long)lResult;
}

std::wstring RichEdit::GetText() const
{
    long lLen = GetTextLength(GTL_DEFAULT);
    LPTSTR lpText = NULL;
    GETTEXTEX gt;
    gt.flags = GT_DEFAULT;
#ifdef _UNICODE
    gt.cb = sizeof(TCHAR) * (lLen + 1) ;
    gt.codepage = 1200;
    lpText = new TCHAR[lLen + 1];
    ::ZeroMemory(lpText, (lLen + 1) * sizeof(TCHAR));
#else
    gt.cb = sizeof(TCHAR) * lLen * 2 + 1;
    gt.codepage = CP_ACP;
    lpText = new TCHAR[lLen * 2 + 1];
    ::ZeroMemory(lpText, (lLen * 2 + 1) * sizeof(TCHAR));
#endif
    gt.lpDefaultChar = NULL;
    gt.lpUsedDefChar = NULL;
    TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)lpText, 0);
    std::wstring sText(lpText);
    delete[] lpText;
    return sText;
}

void RichEdit::SetText(std::wstring pstrText)
{
    text_ = pstrText;
    if( !win_host_ ) return;
	win_host_->SetColor(text_color_);
    SetSel(0, -1);
    ReplaceSel(pstrText, FALSE);
}

bool RichEdit::GetModify() const
{ 
    if( !win_host_ ) return false;
    LRESULT lResult;
    TxSendMessage(EM_GETMODIFY, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

void RichEdit::SetModify(bool bModified) const
{ 
    TxSendMessage(EM_SETMODIFY, bModified, 0, 0);
}

void RichEdit::GetSel(CHARRANGE &cr) const
{ 
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
}

void RichEdit::GetSel(long& nStartChar, long& nEndChar) const
{
    CHARRANGE cr;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
    nStartChar = cr.cpMin;
    nEndChar = cr.cpMax;
}

int RichEdit::SetSel(CHARRANGE &cr)
{ 
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (int)lResult;
}

int RichEdit::SetSel(long nStartChar, long nEndChar)
{
    CHARRANGE cr;
    cr.cpMin = nStartChar;
    cr.cpMax = nEndChar;
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (int)lResult;
}

void RichEdit::ReplaceSel(std::wstring lpszNewText, bool bCanUndo)
{
#ifdef _UNICODE		
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText.c_str(), 0); 
#else
    int iLen = _tcslen(lpszNewText);
    LPWSTR lpText = new WCHAR[iLen + 1];
    ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
    ::MultiByteToWideChar(CP_ACP, 0, lpszNewText, -1, (LPWSTR)lpText, iLen) ;
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpText, 0); 
    delete[] lpText;
#endif
}

void RichEdit::ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo)
{
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
}

std::wstring RichEdit::GetSelText() const
{
    if( !win_host_ ) return std::wstring();
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
    LPWSTR lpText = NULL;
    lpText = new WCHAR[cr.cpMax - cr.cpMin + 1];
    ::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(WCHAR));
    TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpText, 0);
    std::wstring sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

int RichEdit::SetSelAll()
{
    return SetSel(0, -1);
}

int RichEdit::SetSelNone()
{
    return SetSel(-1, 0);
}

bool RichEdit::GetZoom(int& nNum, int& nDen) const
{
    LRESULT lResult;
    TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool RichEdit::SetZoom(int nNum, int nDen)
{
    if (nNum < 0 || nNum > 64) return false;
    if (nDen < 0 || nDen > 64) return false;
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, nNum, nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool RichEdit::SetZoomOff()
{
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

WORD RichEdit::GetSelectionType() const
{
    LRESULT lResult;
    TxSendMessage(EM_SELECTIONTYPE, 0, 0, &lResult);
    return (WORD)lResult;
}

bool RichEdit::GetAutoURLDetect() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETAUTOURLDETECT, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

bool RichEdit::SetAutoURLDetect(bool bAutoDetect)
{
    LRESULT lResult;
    TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0, &lResult);
    return (BOOL)lResult == FALSE;
}

DWORD RichEdit::GetEventMask() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);
    return (DWORD)lResult;
}

DWORD RichEdit::SetEventMask(DWORD dwEventMask)
{
    LRESULT lResult;
    TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lResult);
    return (DWORD)lResult;
}

std::wstring RichEdit::GetTextRange(long nStartChar, long nEndChar) const
{
    TEXTRANGEW tr = { 0 };
    tr.chrg.cpMin = nStartChar;
    tr.chrg.cpMax = nEndChar;
    LPWSTR lpText = NULL;
    lpText = new WCHAR[nEndChar - nStartChar + 1];
    ::ZeroMemory(lpText, (nEndChar - nStartChar + 1) * sizeof(WCHAR));
    tr.lpstrText = lpText;
    TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);
    std::wstring sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

void RichEdit::HideSelection(bool bHide, bool bChangeStyle)
{
    TxSendMessage(EM_HIDESELECTION, bHide, bChangeStyle, 0);
}

void RichEdit::ScrollCaret()
{
    TxSendMessage(EM_SCROLLCARET, 0, 0, 0);
}

int RichEdit::InsertText(long nInsertAfterChar, std::wstring lpstrText, bool bCanUndo)
{
    int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

int RichEdit::AppendText(std::wstring lpstrText, bool bCanUndo)
{
    int nRet = SetSel(-1, -1);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

DWORD RichEdit::GetDefaultCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool RichEdit::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
    if( !win_host_ ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    if( (BOOL)lResult == TRUE ) {
        CHARFORMAT2W cfw;
        cfw.cbSize = sizeof(CHARFORMAT2W);
        TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cfw, 0);
        win_host_->SetCharFormat(cfw);
        return true;
    }
    return false;
}

DWORD RichEdit::GetSelectionCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool RichEdit::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
	if(pm_->IsLayered()) {
		RenderEngine::CheckAlphaColor(cf.crTextColor);
		RenderEngine::CheckAlphaColor(cf.crBackColor);
	}
    if( !win_host_ ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);
    return (BOOL)lResult == TRUE;
}

bool RichEdit::SetWordCharFormat(CHARFORMAT2 &cf)
{
    if( !win_host_ ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION|SCF_WORD, (LPARAM)&cf, &lResult); 
    return (BOOL)lResult == TRUE;
}

DWORD RichEdit::GetParaFormat(PARAFORMAT2 &pf) const
{
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    return (DWORD)lResult;
}

bool RichEdit::SetParaFormat(PARAFORMAT2 &pf)
{
    if( !win_host_ ) return false;
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    if( (BOOL)lResult == TRUE ) {
        win_host_->SetParaFormat(pf);
        return true;
    }
    return false;
}

bool RichEdit::CanUndo()
{
	if( !win_host_ ) return false;
	LRESULT lResult;
	TxSendMessage(EM_CANUNDO, 0, 0, &lResult);
	return (BOOL)lResult == TRUE; 
}

bool RichEdit::CanRedo()
{
	if( !win_host_ ) return false;
	LRESULT lResult;
	TxSendMessage(EM_CANREDO, 0, 0, &lResult);
	return (BOOL)lResult == TRUE; 
}

bool RichEdit::CanPaste()
{
	if( !win_host_ ) return false;
	LRESULT lResult;
	TxSendMessage(EM_CANPASTE, 0, 0, &lResult);
	return (BOOL)lResult == TRUE; 
}

bool RichEdit::Redo()
{ 
    if( !win_host_ ) return false;
    LRESULT lResult;
    TxSendMessage(EM_REDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

bool RichEdit::Undo()
{ 
    if( !win_host_ ) return false;
    LRESULT lResult;
    TxSendMessage(EM_UNDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

void RichEdit::Clear()
{ 
	SetSel(0, -1);
    TxSendMessage(WM_CLEAR, 0, 0, 0); 
}

void RichEdit::Copy()
{ 
    TxSendMessage(WM_COPY, 0, 0, 0); 
}

void RichEdit::Cut()
{ 
    TxSendMessage(WM_CUT, 0, 0, 0); 
}

void RichEdit::Paste()
{ 
    TxSendMessage(WM_PASTE, 0, 0, 0); 
}

int RichEdit::GetLineCount() const
{ 
    if( !win_host_ ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);
    return (int)lResult; 
}

std::wstring RichEdit::GetLine(int nIndex, int nMaxLength) const
{
    LPWSTR lpText = NULL;
    lpText = new WCHAR[nMaxLength + 1];
    ::ZeroMemory(lpText, (nMaxLength + 1) * sizeof(WCHAR));
    *(LPWORD)lpText = (WORD)nMaxLength;
    TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpText, 0);
    std::wstring sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

int RichEdit::LineIndex(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINEINDEX, nLine, 0, &lResult);
    return (int)lResult;
}

int RichEdit::LineLength(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINELENGTH, nLine, 0, &lResult);
    return (int)lResult;
}

bool RichEdit::LineScroll(int nLines, int nChars)
{
    LRESULT lResult;
    TxSendMessage(EM_LINESCROLL, nChars, nLines, &lResult);
    return (BOOL)lResult == TRUE;
}

long RichEdit::LineFromChar(long nIndex) const
{ 
    if( !win_host_ ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);
    return (long)lResult;
}

CDuiPoint RichEdit::PosFromChar(UINT nChar) const
{ 
    POINTL pt; 
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar, 0); 
    return CDuiPoint(pt.x, pt.y); 
}

int RichEdit::CharFromPos(CDuiPoint pt) const
{ 
    POINTL ptl = {pt.x, pt.y}; 
    if( !win_host_ ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lResult);
    return (int)lResult; 
}

void RichEdit::EmptyUndoBuffer()
{ 
    TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, 0); 
}

UINT RichEdit::SetUndoLimit(UINT nLimit)
{ 
    if( !win_host_ ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_SETUNDOLIMIT, (WPARAM) nLimit, 0, &lResult);
    return (UINT)lResult; 
}

long RichEdit::StreamIn(int nFormat, EDITSTREAM &es)
{ 
    if( !win_host_ ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult;
}

long RichEdit::StreamOut(int nFormat, EDITSTREAM &es)
{ 
    if( !win_host_ ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult; 
}

void RichEdit::DoInit()
{
	if(inited_) return ;

    CREATESTRUCT cs;
    cs.style = twh_style_;
    cs.x = 0;
    cs.y = 0;
    cs.cy = 0;
    cs.cx = 0;
    cs.lpszName = text_.c_str();
    CreateHost(this, &cs, &win_host_);
    if( win_host_ ) {
        win_host_->SetTransparent(TRUE);
        LRESULT lResult;
        win_host_->GetTextServices()->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);
		win_host_->GetTextServices()->TxSendMessage(EM_SETEVENTMASK, 0, ENM_DROPFILES|ENM_LINK|ENM_CHANGE, &lResult);
        win_host_->OnTxInPlaceActivate(NULL);
        pm_->AddMessageFilter(this);
		if (!enabled_) {
			win_host_->SetColor(GlobalManager::GetDefaultDisabledColor());
		}
    }
	
	inited_= true;
}

HRESULT RichEdit::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
{
    if( win_host_ ) {
        if( msg == WM_KEYDOWN && TCHAR(wparam) == VK_RETURN ) {
            if( !want_return_ || (::GetKeyState(VK_CONTROL) < 0 && !want_ctrl_return_) ) {
				if (pm_ != NULL) {
					((Control*)this)->Notify(kEventReturn);
				}
                return S_OK;
            }
        }
        return win_host_->GetTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
    }
    return S_FALSE;
}

IDropTarget* RichEdit::GetTxDropTarget()
{
    IDropTarget *pdt = NULL;
    if( win_host_->GetTextServices()->TxGetDropTarget(&pdt) == NOERROR ) return pdt;
    return NULL;
}

bool RichEdit::OnTxViewChanged(BOOL bUpdate)
{
    return true;
}

bool RichEdit::SetDropAcceptFile(bool bAccept) 
{
	LRESULT lResult;
	TxSendMessage(EM_SETEVENTMASK, 0, ENM_DROPFILES|ENM_LINK, &lResult);
	return (BOOL)lResult == FALSE;
}

void RichEdit::OnTxNotify(DWORD iNotify, void *pv)
{
	switch(iNotify)
	{ 
	case EN_CHANGE:
		{
			((Control*)this)->Notify(kEventTextChange);
			break;
		}
	case EN_DROPFILES:   
	case EN_MSGFILTER:   
	case EN_OLEOPFAILED:   
	case EN_PROTECTED:   
	case EN_SAVECLIPBOARD:   
	case EN_SELCHANGE:   
	case EN_STOPNOUNDO:   
	case EN_LINK:
	case EN_OBJECTPOSITIONS:   
	case EN_DRAGDROPDONE:   
		{
			if(pv)                        // Fill out NMHDR portion of pv   
			{   
				LONG nId =  GetWindowLong(this->GetManager()->GetPaintWindow(), GWL_ID);   
				NMHDR  *phdr = (NMHDR *)pv;   
				phdr->hwndFrom = this->GetManager()->GetPaintWindow();   
				phdr->idFrom = nId;   
				phdr->code = iNotify;  

				if(SendMessage(this->GetManager()->GetPaintWindow(), WM_NOTIFY, (WPARAM) nId, (LPARAM) pv))   
				{   
					//hr = S_FALSE;   
				}   
			}    
		}
		break;
	}
}

// 多行非rich格式的richedit有一个滚动条bug，在最后一行是空行时，LineDown和SetScrollPos无法滚动到最后
// 引入iPos就是为了修正这个bug
void RichEdit::SetScrollPos(SIZE szPos, bool bMsg)
{
    int cx = 0;
    int cy = 0;
    if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
        int iLastScrollPos = vertical_scrollbar_->GetScrollPos();
        vertical_scrollbar_->SetScrollPos(szPos.cy);
        cy = vertical_scrollbar_->GetScrollPos() - iLastScrollPos;
    }
    if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
        int iLastScrollPos = horizontal_scrollbar_->GetScrollPos();
        horizontal_scrollbar_->SetScrollPos(szPos.cx);
        cx = horizontal_scrollbar_->GetScrollPos() - iLastScrollPos;
    }
    if( cy != 0 ) {
        int iPos = 0;
        if( win_host_ && !rich_ && vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) 
            iPos = vertical_scrollbar_->GetScrollPos();
        WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, vertical_scrollbar_->GetScrollPos());
        TxSendMessage(WM_VSCROLL, wParam, 0L, 0);
        if( win_host_ && !rich_ && vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
            if( cy > 0 && vertical_scrollbar_->GetScrollPos() <= iPos )
                vertical_scrollbar_->SetScrollPos(iPos);
        }
    }
    if( cx != 0 ) {
        WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, horizontal_scrollbar_->GetScrollPos());
        TxSendMessage(WM_HSCROLL, wParam, 0L, 0);
    }
}

void RichEdit::LineUp()
{
    TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
}

void RichEdit::LineDown()
{
    int iPos = 0;
    if( win_host_ && !rich_ && vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) 
        iPos = vertical_scrollbar_->GetScrollPos();
    TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);
    if( win_host_ && !rich_ && vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
        if( vertical_scrollbar_->GetScrollPos() <= iPos )
            vertical_scrollbar_->SetScrollPos(vertical_scrollbar_->GetScrollRange());
    }
}

void RichEdit::PageUp()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEUP, 0L, 0);
}

void RichEdit::PageDown()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L, 0);
}

void RichEdit::HomeUp()
{
    TxSendMessage(WM_VSCROLL, SB_TOP, 0L, 0);
}

void RichEdit::EndDown()
{
    TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);
}

void RichEdit::LineLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);
}

void RichEdit::LineRight()
{
    TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);
}

void RichEdit::PageLeft()
{
    TxSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L, 0);
}

void RichEdit::PageRight()
{
    TxSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L, 0);
}

void RichEdit::HomeLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LEFT, 0L, 0);
}

void RichEdit::EndRight()
{
    TxSendMessage(WM_HSCROLL, SB_RIGHT, 0L, 0);
}

void RichEdit::DoEvent(EventArgs& event)
{
    if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
        if( parent_ != NULL ) parent_->DoEvent(event);
        else Control::DoEvent(event);
        return;
    }

    else if( event.type_ == kEventSetCursor && IsEnabled() ) {
        if( win_host_ && win_host_->DoSetCursor(NULL, &event.mouse_point_) ) {
            return;
        }
    }
	if( event.type_ == kEventSetFocus ) {
		if( win_host_ ) {
			win_host_->OnTxInPlaceActivate(NULL);
			win_host_->GetTextServices()->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
		}
		focused_ = true;
		Invalidate();
		return;
	}
	if( event.type_ == kEventKillFocus )  {
		if( win_host_ ) {
			win_host_->OnTxInPlaceActivate(NULL);
			win_host_->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
		}
		focused_ = false;
		Invalidate();
		return;
	}
	else if( event.type_ == kEventTimer ) {
		if( win_host_ ) {
			win_host_->GetTextServices()->TxSendMessage(WM_TIMER, event.w_param_, event.l_param_, 0);
		} 
		return;
	}
    else if( event.type_ == kMouseScrollWheel ) {
        if( (event.key_state_ & MK_CONTROL) != 0  ) {
            return;
        }
    }
    else if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick ) 
    {
        return;
    }
    else if( event.type_ == kEventMouseMove ) 
    {
        return;
    }
    else if( event.type_ == kEventMouseButtonUp ) 
    {
        return;
    }
	else if( event.type_ == kEventMouseEnter )
	{
		if( IsEnabled() ) {
			btn_state_ |= UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	else if( event.type_ == kEventMouseLeave )
	{
		if( IsEnabled() ) {
			btn_state_ &= ~UISTATE_HOT;
			Invalidate();
		}
		return;
	}
    if( event.type_ > kEventKeyBegin && event.type_ < kEventKeyEnd )
    {
        return;
    }
    Box::DoEvent(event);
}


std::wstring RichEdit::GetNormalImage()
{
	return normal_image_;
}

void RichEdit::SetNormalImage(std::wstring pStrImage)
{
	normal_image_ = pStrImage;
	Invalidate();
}

std::wstring RichEdit::GetHotImage()
{
	return hot_image_;
}

void RichEdit::SetHotImage(std::wstring pStrImage)
{
	hot_image_ = pStrImage;
	Invalidate();
}

std::wstring RichEdit::GetFocusedImage()
{
	return focused_image_;
}

void RichEdit::SetFocusedImage(std::wstring pStrImage)
{
	focused_image_ = pStrImage;
	Invalidate();
}

std::wstring RichEdit::GetDisabledImage()
{
	return disabled_image_;
}

void RichEdit::SetDisabledImage(std::wstring pStrImage)
{
	disabled_image_ = pStrImage;
	Invalidate();
}

RECT RichEdit::GetTextPadding() const
{
	return text_padding_rect_;
}

void RichEdit::SetTextPadding(RECT rc)
{
	text_padding_rect_ = rc;
	Invalidate();
}

void RichEdit::SetTipValue( std::wstring pStrTipValue )
{
	tip_	= pStrTipValue;
}

std::wstring RichEdit::GetTipValue()
{
	return tip_;
}

void RichEdit::SetTipValueColor( std::wstring pStrColor )
{
	
	tip_color_ = GlobalManager::GetColor(pStrColor);
}

DWORD RichEdit::GetTipValueColor()
{
	return tip_color_;
}

void RichEdit::SetTipValueAlign(UINT uAlign)
{
	tip_align_ = uAlign;
	if(GetText().empty()) Invalidate();
}

UINT RichEdit::GetTipValueAlign()
{
	return tip_align_;
}

void RichEdit::PaintStatusImage(HDC hDC)
{
	if( IsFocused() ) btn_state_ |= UISTATE_FOCUSED;
	else btn_state_ &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() ) btn_state_ |= UISTATE_DISABLED;
	else btn_state_ &= ~ UISTATE_DISABLED;

	if( (btn_state_ & UISTATE_DISABLED) != 0 ) {
		if( !disabled_image_.empty() ) {
			if( !DrawImage(hDC, disabled_image_) ) {}
			else return;
		}
	}
	else if( (btn_state_ & UISTATE_FOCUSED) != 0 ) {
		if( !focused_image_.empty() ) {
			if( !DrawImage(hDC, focused_image_) ) {}
			else return;
		}
	}
	else if( (btn_state_ & UISTATE_HOT ) != 0 ) {
		if( !hot_image_.empty() ) {
			if( !DrawImage(hDC, hot_image_) ) {}
			else return;
		}
	}

	if( !normal_image_.empty() ) {
		if( !DrawImage(hDC, normal_image_) ) {}
		else return;
	}
}

SIZE RichEdit::EstimateSize(SIZE szAvailable)
{
    return Box::EstimateSize(szAvailable);
}

void RichEdit::SetPos(RECT rc, bool bNeedInvalidate)
{
    Control::SetPos(rc, bNeedInvalidate);
    rc = rect_;

    rc.left += rc_inset_.left;
    rc.top += rc_inset_.top;
    rc.right -= rc_inset_.right;
    rc.bottom -= rc_inset_.bottom;
    bool bVScrollBarVisiable = false;
    if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
        bVScrollBarVisiable = true;
        rc.right -= vertical_scrollbar_->GetFixedWidth();
    }
    if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
        rc.bottom -= horizontal_scrollbar_->GetFixedHeight();
    }

	if( win_host_ ) {
		RECT rcRich = rc;
		rcRich.left += text_padding_rect_.left;
		rcRich.right -= text_padding_rect_.right;
		rcRich.top += text_padding_rect_.top;
		rcRich.bottom -= text_padding_rect_.bottom;
		win_host_->SetClientRect(&rcRich);
		if( bVScrollBarVisiable && (!vertical_scrollbar_->IsVisible() || is_vscrollbar_fixing_) ) {
			LONG lWidth = rcRich.right - rcRich.left + vertical_scrollbar_->GetFixedWidth();
			LONG lHeight = 0;
			SIZEL szExtent = { -1, -1 };
			win_host_->GetTextServices()->TxGetNaturalSize(
				DVASPECT_CONTENT, 
				GetManager()->GetPaintDC(), 
				NULL,
				NULL,
				TXTNS_FITTOCONTENT,
				&szExtent,
				&lWidth,
				&lHeight);
			if( lHeight > rcRich.bottom - rcRich.top ) {
				vertical_scrollbar_->SetVisible(true);
				vertical_scrollbar_->SetScrollPos(0);
				is_vscrollbar_fixing_ = true;
			}
			else {
				if( is_vscrollbar_fixing_ ) {
					vertical_scrollbar_->SetVisible(false);
					is_vscrollbar_fixing_ = false;
				}
			}
		}
	}

    if( vertical_scrollbar_ != NULL && vertical_scrollbar_->IsVisible() ) {
        RECT rcScrollBarPos = { rc.right, rc.top, rc.right + vertical_scrollbar_->GetFixedWidth(), rc.bottom};
        vertical_scrollbar_->SetPos(rcScrollBarPos);
    }
    if( horizontal_scrollbar_ != NULL && horizontal_scrollbar_->IsVisible() ) {
        RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + horizontal_scrollbar_->GetFixedHeight()};
        horizontal_scrollbar_->SetPos(rcScrollBarPos);
    }

	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) 
		szAvailable.cx += horizontal_scrollbar_->GetScrollRange();

	int nAdjustables = 0;
	int cxFixed = 0;
	int nEstimateNum = 0;
	for( int it1 = 0; it1 < items_.GetSize(); it1++ ) {
		Control* pControl = static_cast<Control*>(items_[it1]);
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) continue;
		SIZE sz = pControl->EstimateSize(szAvailable);
		if( sz.cx == 0 ) {
			nAdjustables++;
		}
		else {
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
		}
		cxFixed += sz.cx +  pControl->GetPadding().left + pControl->GetPadding().right;
		nEstimateNum++;
	}
	cxFixed += (nEstimateNum - 1) * child_padding_;

	int cxExpand = 0;
    int cxNeeded = 0;
	if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosX = rc.left;
	if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
		iPosX -= horizontal_scrollbar_->GetScrollPos();
	}
	int iAdjustable = 0;
	int cxFixedRemaining = cxFixed;
	for( int it2 = 0; it2 < items_.GetSize(); it2++ ) {
		Control* pControl = static_cast<Control*>(items_[it2]);
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) {
			SetFloatPos(it2);
			continue;
		}
		RECT rcPadding = pControl->GetPadding();
		szRemaining.cx -= rcPadding.left;
		SIZE sz = pControl->EstimateSize(szRemaining);
		if( sz.cx == 0 ) {
			iAdjustable++;
			sz.cx = cxExpand;

			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
		}
		else {
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

		}

		sz.cy = pControl->GetFixedHeight();
		if( sz.cy == 0 ) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
		if( sz.cy < 0 ) sz.cy = 0;
		if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
		if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

		RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left , rc.top + rcPadding.top + sz.cy};
		pControl->SetPos(rcCtrl, true);
		iPosX += sz.cx + child_padding_ + rcPadding.left + rcPadding.right;
        cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
		szRemaining.cx -= sz.cx + child_padding_ + rcPadding.right;
	}
    cxNeeded += (nEstimateNum - 1) * child_padding_;
	if( horizontal_scrollbar_ != NULL ) {
		if( cxNeeded > rc.right - rc.left ) {
			if( horizontal_scrollbar_->IsVisible() ) {
				horizontal_scrollbar_->SetScrollRange(cxNeeded - (rc.right - rc.left));
			}
			else {
				horizontal_scrollbar_->SetVisible(true);
				horizontal_scrollbar_->SetScrollRange(cxNeeded - (rc.right - rc.left));
				horizontal_scrollbar_->SetScrollPos(0);
				rc.bottom -= horizontal_scrollbar_->GetFixedHeight();
			}
		}
	}
}

void RichEdit::Move(SIZE szOffset, bool bNeedInvalidate)
{
	Box::Move(szOffset, bNeedInvalidate);
	if( win_host_ != NULL ) {
		RECT rc = rect_;
		rc.left += rc_inset_.left;
		rc.top += rc_inset_.top;
		rc.right -= rc_inset_.right;
		rc.bottom -= rc_inset_.bottom;

		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) rc.right -= vertical_scrollbar_->GetFixedWidth();
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) rc.bottom -= horizontal_scrollbar_->GetFixedHeight();
		win_host_->SetClientRect(&rc);
	}
}

void RichEdit::DoPaint(HDC hDC, const RECT& rcPaint)
{
    RECT rcTemp = { 0 };
    if( !::IntersectRect(&rcTemp, &rcPaint, &rect_) ) return;

    RenderClip clip;
    RenderClip::GenerateClip(hDC, rcTemp, clip);
    Control::DoPaint(hDC, rcPaint);

    if( win_host_ ) {
        RECT rc;
        win_host_->GetControlRect(&rc);
        // Remember wparam is actually the hdc and lparam is the update
        // rect because this message has been preprocessed by the window.
        win_host_->GetTextServices()->TxDraw(
            DVASPECT_CONTENT,  		// Draw Aspect
            /*-1*/0,				// Lindex
            NULL,					// Info for drawing optimazation
            NULL,					// target device information
            hDC,			        // Draw device HDC
            NULL, 				   	// Target device HDC
            (RECTL*)&rc,			// Bounding client rectangle
            NULL, 		            // Clipping rectangle for metafiles
            (RECT*)&rcPaint,		// Update rectangle
            NULL, 	   				// Call back function
            NULL,					// Call back parameter
            0);				        // What view of the object
        if( is_vscrollbar_fixing_ ) {
            LONG lWidth = rc.right - rc.left + vertical_scrollbar_->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            win_host_->GetTextServices()->TxGetNaturalSize(
                DVASPECT_CONTENT, 
                GetManager()->GetPaintDC(), 
                NULL,
                NULL,
                TXTNS_FITTOCONTENT,
                &szExtent,
                &lWidth,
                &lHeight);
            if( lHeight <= rc.bottom - rc.top ) {
                NeedUpdate();
            }
        }
    }

    if( items_.GetSize() > 0 ) {
        RECT rc = rect_;
        rc.left += rc_inset_.left;
        rc.top += rc_inset_.top;
        rc.right -= rc_inset_.right;
        rc.bottom -= rc_inset_.bottom;
        if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) rc.right -= vertical_scrollbar_->GetFixedWidth();
        if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) rc.bottom -= horizontal_scrollbar_->GetFixedHeight();

        if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
            for( int it = 0; it < items_.GetSize(); it++ ) {
                Control* pControl = static_cast<Control*>(items_[it]);
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl ->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &rect_, &pControl->GetPos()) ) continue;
                    pControl->DoPaint(hDC, rcPaint);
                }
            }
        }
        else {
            RenderClip childClip;
            RenderClip::GenerateClip(hDC, rcTemp, childClip);
            for( int it = 0; it < items_.GetSize(); it++ ) {
                Control* pControl = static_cast<Control*>(items_[it]);
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl ->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &rect_, &pControl->GetPos()) ) continue;
                    RenderClip::UseOldClipBegin(hDC, childClip);
                    pControl->DoPaint(hDC, rcPaint);
                    RenderClip::UseOldClipEnd(hDC, childClip);
                }
                else {
                    if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
                    pControl->DoPaint(hDC, rcPaint);
                }
            }
        }
    }

    if( vertical_scrollbar_ != NULL && vertical_scrollbar_->IsVisible() ) {
        if( ::IntersectRect(&rcTemp, &rcPaint, &vertical_scrollbar_->GetPos()) ) {
            vertical_scrollbar_->DoPaint(hDC, rcPaint);
        }
    }

    if( horizontal_scrollbar_ != NULL && horizontal_scrollbar_->IsVisible() ) {
        if( ::IntersectRect(&rcTemp, &rcPaint, &horizontal_scrollbar_->GetPos()) ) {
            horizontal_scrollbar_->DoPaint(hDC, rcPaint);
        }
    }
	// 绘制提示文字
	std::wstring sDrawText = GetText();
	if(sDrawText.empty() && !focused_) {
		DWORD dwTextColor = GetTipValueColor();
		std::wstring sTipValue = GetTipValue();
		RECT rc = rect_;
		rc.left += text_padding_rect_.left;
		rc.right -= text_padding_rect_.right;
		rc.top += text_padding_rect_.top;
		rc.bottom -= text_padding_rect_.bottom;
		UINT uTextAlign = GetTipValueAlign();
		if(IsMultiLine()) uTextAlign |= DT_TOP;
		else uTextAlign |= DT_VCENTER;
		RenderEngine::DrawText(hDC, pm_, rc, sTipValue, dwTextColor, font_, uTextAlign);
	}
}

void RichEdit::SetAttribute(std::wstring strName, std::wstring strValue)
{
	auto pstrName = strName.c_str();
	auto pstrValue = strValue.c_str();


    if( _tcsicmp(pstrName, _T("vscrollbar")) == 0 ) {
        if( _tcsicmp(pstrValue, _T("true")) == 0 ) twh_style_ |= ES_DISABLENOSCROLL | WS_VSCROLL;
    }
    else if( _tcsicmp(pstrName, _T("autovscroll")) == 0 ) {
        if( _tcsicmp(pstrValue, _T("true")) == 0 ) twh_style_ |= ES_AUTOVSCROLL;
    }
    else if( _tcsicmp(pstrName, _T("hscrollbar")) == 0 ) {
        if( _tcsicmp(pstrValue, _T("true")) == 0 ) twh_style_ |= ES_DISABLENOSCROLL | WS_HSCROLL;
    }
    else if( _tcsicmp(pstrName, _T("autohscroll")) == 0 ) {
        if( _tcsicmp(pstrValue, _T("true")) == 0 ) twh_style_ |= ES_AUTOHSCROLL;
    }
    else if( _tcsicmp(pstrName, _T("wanttab")) == 0 ) {
        SetWantTab(_tcsicmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcsicmp(pstrName, _T("wantreturn")) == 0 ) {
        SetWantReturn(_tcsicmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcsicmp(pstrName, _T("wantctrlreturn")) == 0 ) {
        SetWantCtrlReturn(_tcsicmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcsicmp(pstrName, _T("rich")) == 0 ) {
        SetRich(_tcsicmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcsicmp(pstrName, _T("multiline")) == 0 ) {
        if( _tcsicmp(pstrValue, _T("false")) == 0 ) twh_style_ &= ~ES_MULTILINE;
    }
    else if( _tcsicmp(pstrName, _T("readonly")) == 0 ) {
        if( _tcsicmp(pstrValue, _T("true")) == 0 ) { twh_style_ |= ES_READONLY; read_only_ = true; }
    }
    else if( _tcsicmp(pstrName, _T("password")) == 0 ) {
        if( _tcsicmp(pstrValue, _T("true")) == 0 ) {
			twh_style_ |= ES_PASSWORD;
		}
    }
    else if( _tcsicmp(pstrName, _T("align")) == 0 ) {
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            twh_style_ &= ~(ES_CENTER | ES_RIGHT);
            twh_style_ |= ES_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            twh_style_ &= ~(ES_LEFT | ES_RIGHT);
            twh_style_ |= ES_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
            twh_style_ &= ~(ES_LEFT | ES_CENTER);
            twh_style_ |= ES_RIGHT;
        }
    }
    else if( _tcsicmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
    else if( _tcsicmp(pstrName, _T("textcolor")) == 0 ) {
        SetTextColor(GlobalManager::GetColor(strValue));
    }
	else if( _tcsicmp(pstrName, _T("maxchar")) == 0 ) SetLimitText(_ttoi(pstrValue));
	else if( _tcsicmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
	else if( _tcsicmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
	else if( _tcsicmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
	else if( _tcsicmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
	else if( _tcsicmp(pstrName, _T("textpadding")) == 0 ) {
		RECT rcTextPadding = { 0 };
		LPTSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetTextPadding(rcTextPadding);
	}
	else if( _tcsicmp(pstrName, _T("tipvalue")) == 0 ) SetTipValue(pstrValue);
	else if( _tcsicmp(pstrName, _T("tipvaluecolor")) == 0 ) SetTipValueColor(pstrValue);
	else if( _tcsicmp(pstrName, _T("tipvaluealign")) == 0 ) {
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            tip_align_ = DT_SINGLELINE | DT_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            tip_align_ = DT_SINGLELINE | DT_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
           tip_align_ = DT_SINGLELINE | DT_RIGHT;
        }
    }
    else Box::SetAttribute(strName, strValue);
}

LRESULT RichEdit::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if( !IsVisible() || !IsEnabled() ) return 0;
    if( !IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
    if( uMsg == WM_MOUSEWHEEL && (LOWORD(wParam) & MK_CONTROL) == 0 ) return 0;

	if (uMsg == WM_IME_COMPOSITION) {
		// 解决微软输入法位置异常的问题
		HIMC hIMC = ImmGetContext(GetManager()->GetPaintWindow());
		if (hIMC)  {
			POINT point;
			GetCaretPos(&point);

			COMPOSITIONFORM Composition;
			Composition.dwStyle = CFS_POINT;
			Composition.ptCurrentPos.x = point.x;
			Composition.ptCurrentPos.y = point.y;
			ImmSetCompositionWindow(hIMC, &Composition);

			ImmReleaseContext(GetManager()->GetPaintWindow(),hIMC);
		}

		return 0;
	}

    bool bWasHandled = true;
    if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
        if( !win_host_->IsCaptured() ) {
            switch (uMsg) {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                {
                    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    Control* pHover = GetManager()->FindControl(pt);
                    if(pHover != this) {
                        bWasHandled = false;
                        return 0;
                    }
                }
                break;
            }
        }
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = win_host_->IsCaptured() ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if( dwHitResult == HITRESULT_OUTSIDE ) {
            RECT rc;
            win_host_->GetControlRect(&rc);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if( uMsg == WM_MOUSEWHEEL ) ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
            if( ::PtInRect(&rc, pt) && !GetManager()->IsCaptured() ) dwHitResult = HITRESULT_HIT;
        }
        if( dwHitResult != HITRESULT_HIT ) return 0;
        if( uMsg == WM_SETCURSOR ) bWasHandled = false;
        else if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDOWN ) {
            SetFocus();
        }
    }
#ifdef _UNICODE
    else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
#else
    else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR ) {
#endif
        if( !IsFocused() ) return 0;
    }
    else if( uMsg == WM_CONTEXTMENU ) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		POINT ptClient = pt;
        ::ScreenToClient(GetManager()->GetPaintWindow(), &ptClient);
        Control* pHover = GetManager()->FindControl(ptClient);
        if(pHover != this) {
            bWasHandled = false;
            return 0;
        }

		//创建一个弹出式菜单
		HMENU hPopMenu = CreatePopupMenu();
		AppendMenu(hPopMenu, 0, ID_RICH_UNDO, L"撤销(&U)");
		AppendMenu(hPopMenu, 0, ID_RICH_REDO, L"重做(&R)");
		AppendMenu(hPopMenu, MF_SEPARATOR, 0, L"");
		AppendMenu(hPopMenu, 0, ID_RICH_CUT, L"剪切(&X)");
		AppendMenu(hPopMenu, 0, ID_RICH_COPY, L"复制(&C)");
		AppendMenu(hPopMenu, 0, ID_RICH_PASTE, L"粘帖(&V)");
		AppendMenu(hPopMenu, 0, ID_RICH_CLEAR, L"清空(&L)");
		AppendMenu(hPopMenu, MF_SEPARATOR, 0, L"");
		AppendMenu(hPopMenu, 0, ID_RICH_SELECTALL, L"全选(&A)");

		//初始化菜单项
		UINT uUndo = (CanUndo() ? 0 : MF_GRAYED);
		EnableMenuItem(hPopMenu, ID_RICH_UNDO, MF_BYCOMMAND | uUndo);
		UINT uRedo = (CanRedo() ? 0 : MF_GRAYED);
		EnableMenuItem(hPopMenu, ID_RICH_REDO, MF_BYCOMMAND | uRedo);
		UINT uSel = ((GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
		UINT uReadonly = IsReadOnly() ? MF_GRAYED : 0;
		EnableMenuItem(hPopMenu, ID_RICH_CUT, MF_BYCOMMAND | uSel | uReadonly);
		EnableMenuItem(hPopMenu, ID_RICH_COPY, MF_BYCOMMAND | uSel);
		EnableMenuItem(hPopMenu, ID_RICH_CLEAR, MF_BYCOMMAND | uSel | uReadonly);
		EnableMenuItem(hPopMenu, ID_RICH_PASTE, MF_BYCOMMAND | uReadonly);

		TrackPopupMenu(hPopMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, GetManager()->GetPaintWindow(), NULL);
		DestroyMenu(hPopMenu);
    }
	else if( uMsg == WM_COMMAND ) {
		bHandled = FALSE;
		if( !IsFocused() ) return 0;
		UINT uCmd = (UINT)wParam;
		switch(uCmd) {
		case ID_RICH_UNDO:
			{
				Undo();
				break;
			}
		case ID_RICH_REDO:
			{
				Redo();
				break;
			}
		case ID_RICH_CUT:
			{
				Cut();
				break;
			}
		case ID_RICH_COPY:
			{
				Copy();
				break;
			}
		case ID_RICH_PASTE:
			{
				Paste();
				break;
			}
		case ID_RICH_CLEAR:
			{
				Clear();
				break;
			}
		case ID_RICH_SELECTALL:
			{
				SetSelAll();
				break;
			}
		default:break;
		}
	}
    else
    {
        switch( uMsg ) {
        case WM_HELP:
            bWasHandled = false;
            break;
        default:
            return 0;
        }
    }

	if(WM_CHAR == uMsg)
	{
#ifndef _UNICODE
		// check if we are waiting for 2 consecutive WM_CHAR messages
		if ( IsAccumulateDBCMode() )
		{
			if ( (GetKeyState(VK_KANA) & 0x1) )
			{
				// turn off accumulate mode
				SetAccumulateDBCMode ( false );
				m_chLeadByte = 0;
			}
			else
			{
				if ( !m_chLeadByte )
				{
					// This is the first WM_CHAR message, 
					// accumulate it if this is a LeadByte.  Otherwise, fall thru to
					// regular WM_CHAR processing.
					if ( IsDBCSLeadByte ( (WORD)wParam ) )
					{
						// save the Lead Byte and don't process this message
						m_chLeadByte = (WORD)wParam << 8 ;

						//TCHAR a = (WORD)wParam << 8 ;
						return 0;
					}
				}
				else
				{
					// This is the second WM_CHAR message,
					// combine the current byte with previous byte.
					// This DBC will be handled as WM_IME_CHAR.
					wParam |= m_chLeadByte;
					uMsg = WM_IME_CHAR;

					// setup to accumulate more WM_CHAR
					m_chLeadByte = 0; 
				}
			}
		}
#endif
	}

    LRESULT lResult = 0;
    HRESULT Hr = TxSendMessage(uMsg, wParam, lParam, &lResult);
    if( Hr == S_OK ) bHandled = bWasHandled;
    else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR )
        bHandled = bWasHandled;
    else if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) {
        if( win_host_->IsCaptured() ) bHandled = bWasHandled;
    }
    return lResult;
}

void RichEdit::SetAccumulateDBCMode( bool bDBCMode )
{
	accumulate_dbc_ = bDBCMode;
}

bool RichEdit::IsAccumulateDBCMode()
{
	return accumulate_dbc_;
}

} // namespace DuiLib
