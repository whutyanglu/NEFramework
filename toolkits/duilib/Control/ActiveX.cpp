#include "StdAfx.h"

namespace ui {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	class ActiveXCtrl;


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	class CActiveXWnd : public Window
	{
	public:
		CActiveXWnd() : m_iLayeredTick(0), m_bDrawCaret(false) {}
		HWND Init(ActiveXCtrl* pOwner, HWND hWndParent);

		std::wstring GetWindowClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		void DoVerb(LONG iVerb);

		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnPrint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		ActiveXCtrl* owner_;
		int m_iLayeredTick;
		bool m_bDrawCaret;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	class CActiveXEnum : public IEnumUnknown
	{
	public:
		CActiveXEnum(IUnknown* pUnk) : m_pUnk(pUnk), m_dwRef(1), m_iPos(0)
		{
			m_pUnk->AddRef();
		}
		~CActiveXEnum()
		{
			m_pUnk->Release();
		}

		LONG m_iPos;
		ULONG m_dwRef;
		IUnknown* m_pUnk;

		STDMETHOD_(ULONG,AddRef)()
		{
			return ++m_dwRef;
		}
		STDMETHOD_(ULONG,Release)()
		{
			LONG lRef = --m_dwRef;
			if( lRef == 0 ) delete this;
			return lRef;
		}
		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
		{
			*ppvObject = NULL;
			if( riid == IID_IUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
			else if( riid == IID_IEnumUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
			if( *ppvObject != NULL ) AddRef();
			return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
		}
		STDMETHOD(Next)(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
		{
			if( pceltFetched != NULL ) *pceltFetched = 0;
			if( ++m_iPos > 1 ) return S_FALSE;
			*rgelt = m_pUnk;
			(*rgelt)->AddRef();
			if( pceltFetched != NULL ) *pceltFetched = 1;
			return S_OK;
		}
		STDMETHOD(Skip)(ULONG celt)
		{
			m_iPos += celt;
			return S_OK;
		}
		STDMETHOD(Reset)(void)
		{
			m_iPos = 0;
			return S_OK;
		}
		STDMETHOD(Clone)(IEnumUnknown **ppenum)
		{
			return E_NOTIMPL;
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	class CActiveXFrameWnd : public IOleInPlaceFrame
	{
	public:
		CActiveXFrameWnd(ActiveX* pOwner) : m_dwRef(1), owner_(pOwner), m_pActiveObject(NULL)
		{
		}
		~CActiveXFrameWnd()
		{
			if( m_pActiveObject != NULL ) m_pActiveObject->Release();
		}

		ULONG m_dwRef;
		ActiveX* owner_;
		IOleInPlaceActiveObject* m_pActiveObject;

		// IUnknown
		STDMETHOD_(ULONG,AddRef)()
		{
			return ++m_dwRef;
		}
		STDMETHOD_(ULONG,Release)()
		{
			ULONG lRef = --m_dwRef;
			if( lRef == 0 ) delete this;
			return lRef;
		}
		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
		{
			*ppvObject = NULL;
			if( riid == IID_IUnknown ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
			else if( riid == IID_IOleWindow ) *ppvObject = static_cast<IOleWindow*>(this);
			else if( riid == IID_IOleInPlaceFrame ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
			else if( riid == IID_IOleInPlaceUIWindow ) *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);
			if( *ppvObject != NULL ) AddRef();
			return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
		}  
		// IOleInPlaceFrameWindow
		STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
		{
			return S_OK;
		}
		STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
		{
			return S_OK;
		}
		STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
		{
			return S_OK;
		}
		STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
		{
			return S_OK;
		}
		STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
		{
			return S_OK;
		}
		STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
		{
			return S_FALSE;
		}
		// IOleWindow
		STDMETHOD(GetWindow)(HWND* phwnd)
		{
			if( owner_ == NULL ) return E_UNEXPECTED;
			*phwnd = owner_->GetManager()->GetPaintWindow();
			return S_OK;
		}
		STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
		{
			return S_OK;
		}
		// IOleInPlaceUIWindow
		STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
		{
			return S_OK;
		}
		STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
		{
			return INPLACE_E_NOTOOLSPACE;
		}
		STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
		{
			return S_OK;
		}
		STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
		{
			if( pActiveObject != NULL ) pActiveObject->AddRef();
			if( m_pActiveObject != NULL ) m_pActiveObject->Release();
			m_pActiveObject = pActiveObject;
			return S_OK;
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class ActiveXCtrl :
		public IOleClientSite,
		public IOleInPlaceSiteWindowless,
		public IOleControlSite,
		public IObjectWithSite,
		public IOleContainer
	{
		friend class ActiveX;
		friend class CActiveXWnd;
	public:
		ActiveXCtrl();
		~ActiveXCtrl();

		// IUnknown
		STDMETHOD_(ULONG,AddRef)();
		STDMETHOD_(ULONG,Release)();
		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

		// IObjectWithSite
		STDMETHOD(SetSite)(IUnknown *pUnkSite);
		STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);

		// IOleClientSite
		STDMETHOD(SaveObject)(void);       
		STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
		STDMETHOD(GetContainer)(IOleContainer** ppContainer);        
		STDMETHOD(ShowObject)(void);        
		STDMETHOD(OnShowWindow)(BOOL fShow);        
		STDMETHOD(RequestNewObjectLayout)(void);

		// IOleInPlaceSiteWindowless
		STDMETHOD(CanWindowlessActivate)(void);
		STDMETHOD(GetCapture)(void);
		STDMETHOD(SetCapture)(BOOL fCapture);
		STDMETHOD(GetFocus)(void);
		STDMETHOD(SetFocus)(BOOL fFocus);
		STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
		STDMETHOD(ReleaseDC)(HDC hDC);
		STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
		STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
		STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
		STDMETHOD(AdjustRect)(LPRECT prc);
		STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

		// IOleInPlaceSiteEx
		STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);        
		STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);       
		STDMETHOD(RequestUIActivate)(void);

		// IOleInPlaceSite
		STDMETHOD(CanInPlaceActivate)(void);       
		STDMETHOD(OnInPlaceActivate)(void);        
		STDMETHOD(OnUIActivate)(void);
		STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
		STDMETHOD(Scroll)(SIZE scrollExtant);
		STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
		STDMETHOD(OnInPlaceDeactivate)(void);
		STDMETHOD(DiscardUndoState)( void);
		STDMETHOD(DeactivateAndUndo)( void);
		STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

		// IOleWindow
		STDMETHOD(GetWindow)(HWND* phwnd);
		STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

		// IOleControlSite
		STDMETHOD(OnControlInfoChanged)(void);      
		STDMETHOD(LockInPlaceActive)(BOOL fLock);       
		STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);        
		STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);       
		STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
		STDMETHOD(OnFocus)(BOOL fGotFocus);
		STDMETHOD(ShowPropertyFrame)(void);

		// IOleContainer
		STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
		STDMETHOD(LockContainer)(BOOL fLock);

		// IParseDisplayName
		STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);

	protected:
		HRESULT CreateActiveXWnd();

	protected:
		LONG m_dwRef;
		ActiveX* owner_;
		CActiveXWnd* wnd_;
		IUnknown* m_pUnkSite;
		IViewObject* m_pViewObject;
		IOleInPlaceObjectWindowless* m_pInPlaceObject;
		bool m_bLocked;
		bool m_bFocused;
		bool m_bCaptured;
		bool m_bUIActivated;
		bool m_bInPlaceActive;
		bool m_bWindowless;
	};

	ActiveXCtrl::ActiveXCtrl() : 
	m_dwRef(1), 
		owner_(NULL), 
		wnd_(NULL),
		m_pUnkSite(NULL), 
		m_pViewObject(NULL),
		m_pInPlaceObject(NULL),
		m_bLocked(false), 
		m_bFocused(false),
		m_bCaptured(false),
		m_bWindowless(true),
		m_bUIActivated(false),
		m_bInPlaceActive(false)
	{
	}

	ActiveXCtrl::~ActiveXCtrl()
	{
		if( wnd_ != NULL ) {
			::DestroyWindow(*wnd_);
			delete wnd_;
		}
		if( m_pUnkSite != NULL ) m_pUnkSite->Release();
		if( m_pViewObject != NULL ) m_pViewObject->Release();
		if( m_pInPlaceObject != NULL ) m_pInPlaceObject->Release();
	}

	STDMETHODIMP ActiveXCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
	{
		*ppvObject = NULL;
		if( riid == IID_IUnknown )                       *ppvObject = static_cast<IOleWindow*>(this);
		else if( riid == IID_IOleClientSite )            *ppvObject = static_cast<IOleClientSite*>(this);
		else if( riid == IID_IOleInPlaceSiteWindowless ) *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
		else if( riid == IID_IOleInPlaceSiteEx )         *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
		else if( riid == IID_IOleInPlaceSite )           *ppvObject = static_cast<IOleInPlaceSite*>(this);
		else if( riid == IID_IOleWindow )                *ppvObject = static_cast<IOleWindow*>(this);
		else if( riid == IID_IOleControlSite )           *ppvObject = static_cast<IOleControlSite*>(this);
		else if( riid == IID_IOleContainer )             *ppvObject = static_cast<IOleContainer*>(this);
		else if( riid == IID_IObjectWithSite )           *ppvObject = static_cast<IObjectWithSite*>(this);
		if( *ppvObject != NULL ) AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}

	STDMETHODIMP_(ULONG) ActiveXCtrl::AddRef()
	{
		return ++m_dwRef;
	}

	STDMETHODIMP_(ULONG) ActiveXCtrl::Release()
	{
		LONG lRef = --m_dwRef;
		if( lRef == 0 ) delete this;
		return lRef;
	}

	STDMETHODIMP ActiveXCtrl::SetSite(IUnknown *pUnkSite)
	{
		if( m_pUnkSite != NULL ) {
			m_pUnkSite->Release();
			m_pUnkSite = NULL;
		}
		if( pUnkSite != NULL ) {
			m_pUnkSite = pUnkSite;
			m_pUnkSite->AddRef();
		}
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::GetSite(REFIID riid, LPVOID* ppvSite)
	{
		if( ppvSite == NULL ) return E_POINTER;
		*ppvSite = NULL;
		if( m_pUnkSite == NULL ) return E_FAIL;
		return m_pUnkSite->QueryInterface(riid, ppvSite);
	}

	STDMETHODIMP ActiveXCtrl::SaveObject(void)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP ActiveXCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
	{
		if( ppmk != NULL ) *ppmk = NULL;
		return E_NOTIMPL;
	}

	STDMETHODIMP ActiveXCtrl::GetContainer(IOleContainer** ppContainer)
	{
		if( ppContainer == NULL ) return E_POINTER;
		*ppContainer = NULL;
		HRESULT Hr = E_NOTIMPL;
		if( m_pUnkSite != NULL ) Hr = m_pUnkSite->QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
		if( FAILED(Hr) ) Hr = QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
		return Hr;
	}

	STDMETHODIMP ActiveXCtrl::ShowObject(void)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		HDC hDC = ::GetDC(owner_->hwnd_host_);
		if( hDC == NULL ) return E_FAIL;
		if( m_pViewObject != NULL ) m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &owner_->rect_, (RECTL*) &owner_->rect_, NULL, NULL);
		::ReleaseDC(owner_->hwnd_host_, hDC);
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::OnShowWindow(BOOL fShow)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP ActiveXCtrl::RequestNewObjectLayout(void)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP ActiveXCtrl::CanWindowlessActivate(void)
	{
		return S_OK;  // Yes, we can!!
	}

	STDMETHODIMP ActiveXCtrl::GetCapture(void)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		return m_bCaptured ? S_OK : S_FALSE;
	}

	STDMETHODIMP ActiveXCtrl::SetCapture(BOOL fCapture)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		m_bCaptured = (fCapture == TRUE);
		if( fCapture ) ::SetCapture(owner_->hwnd_host_); else ::ReleaseCapture();
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::GetFocus(void)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		return m_bFocused ? S_OK : S_FALSE;
	}

	STDMETHODIMP ActiveXCtrl::SetFocus(BOOL fFocus)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		if( fFocus ) owner_->SetFocus();
		m_bFocused = (fFocus == TRUE);
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
	{
		if( phDC == NULL ) return E_POINTER;
		if( owner_ == NULL ) return E_UNEXPECTED;
		if( m_bWindowless ) return S_FALSE;
		*phDC = ::GetDC(owner_->hwnd_host_);
		if( (grfFlags & OLEDC_PAINTBKGND) != 0 ) {
			CDuiRect rcItem = owner_->GetPos();
			if( !m_bWindowless ) rcItem.ResetOffset();
			::FillRect(*phDC, &rcItem, (HBRUSH) (COLOR_WINDOW + 1));
		}
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::ReleaseDC(HDC hDC)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		::ReleaseDC(owner_->hwnd_host_, hDC);
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::InvalidateRect(LPCRECT pRect, BOOL fErase)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		if( owner_->hwnd_host_ == NULL ) return E_FAIL;
		return ::InvalidateRect(owner_->hwnd_host_, pRect, fErase) ? S_OK : E_FAIL;
	}

	STDMETHODIMP ActiveXCtrl::InvalidateRgn(HRGN hRGN, BOOL fErase)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		return ::InvalidateRgn(owner_->hwnd_host_, hRGN, fErase) ? S_OK : E_FAIL;
	}

	STDMETHODIMP ActiveXCtrl::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
	{
	return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::AdjustRect(LPRECT prc)
	{
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		*plResult = ::DefWindowProc(owner_->hwnd_host_, msg, wParam, lParam);
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::OnInPlaceActivateEx(BOOL* pfNoRedraw, DWORD dwFlags)        
	{
		ASSERT(m_pInPlaceObject==NULL);
		if( owner_ == NULL ) return E_UNEXPECTED;
		if( owner_->unkown_ptr_ == NULL ) return E_UNEXPECTED;
		::OleLockRunning(owner_->unkown_ptr_, TRUE, FALSE);
		HWND hWndFrame = owner_->GetManager()->GetPaintWindow();
		HRESULT Hr = E_FAIL;
		if( (dwFlags & ACTIVATE_WINDOWLESS) != 0 ) {
			m_bWindowless = true;
			Hr = owner_->unkown_ptr_->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*) &m_pInPlaceObject);
			owner_->hwnd_host_ = hWndFrame;
			owner_->GetManager()->AddMessageFilter(owner_);
		}
		if( FAILED(Hr) ) {
			m_bWindowless = false;
			Hr = CreateActiveXWnd();
			if( FAILED(Hr) ) return Hr;
			Hr = owner_->unkown_ptr_->QueryInterface(IID_IOleInPlaceObject, (LPVOID*) &m_pInPlaceObject);
		}
		if( m_pInPlaceObject != NULL && !owner_->IsMFC() ) {
			CDuiRect rcItem = owner_->rect_;
			if( !m_bWindowless ) rcItem.ResetOffset();
			m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
		}
		m_bInPlaceActive = SUCCEEDED(Hr);
		return Hr;
	}

	STDMETHODIMP ActiveXCtrl::OnInPlaceDeactivateEx(BOOL fNoRedraw)       
	{
		m_bInPlaceActive = false;
		if( m_pInPlaceObject != NULL ) {
			m_pInPlaceObject->Release();
			m_pInPlaceObject = NULL;
		}
		if( wnd_ != NULL ) {
			::DestroyWindow(*wnd_);
			delete wnd_;
			wnd_ = NULL;
		}
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::RequestUIActivate(void)
	{
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::CanInPlaceActivate(void)       
	{
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::OnInPlaceActivate(void)
	{
		BOOL bDummy = FALSE;
		return OnInPlaceActivateEx(&bDummy, 0);
	}

	STDMETHODIMP ActiveXCtrl::OnUIActivate(void)
	{
		m_bUIActivated = true;
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{
		if( ppDoc == NULL ) return E_POINTER;
		if( ppFrame == NULL ) return E_POINTER;
		if( lprcPosRect == NULL ) return E_POINTER;
		if( lprcClipRect == NULL ) return E_POINTER;
		if (wnd_)
		{
			::GetClientRect(wnd_->GetHWND(),lprcPosRect);
			::GetClientRect(wnd_->GetHWND(),lprcClipRect);
		}
		*ppFrame = new CActiveXFrameWnd(owner_);
		*ppDoc = NULL;
		ACCEL ac = { 0 };
		HACCEL hac = ::CreateAcceleratorTable(&ac, 1);
		lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
		lpFrameInfo->fMDIApp = FALSE;
		lpFrameInfo->hwndFrame = owner_->GetManager()->GetPaintWindow();
		lpFrameInfo->haccel = hac;
		lpFrameInfo->cAccelEntries = 1;
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::Scroll(SIZE scrollExtant)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP ActiveXCtrl::OnUIDeactivate(BOOL fUndoable)
	{
		m_bUIActivated = false;
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::OnInPlaceDeactivate(void)
	{
		return OnInPlaceDeactivateEx(TRUE);
	}

	STDMETHODIMP ActiveXCtrl::DiscardUndoState(void)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP ActiveXCtrl::DeactivateAndUndo(void)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP ActiveXCtrl::OnPosRectChange(LPCRECT lprcPosRect)
	{
	return E_NOTIMPL;
	}

	STDMETHODIMP ActiveXCtrl::GetWindow(HWND* phwnd)
	{
		if( owner_ == NULL ) return E_UNEXPECTED;
		if( owner_->hwnd_host_ == NULL ) CreateActiveXWnd();
		if( owner_->hwnd_host_ == NULL ) return E_FAIL;
		*phwnd = owner_->hwnd_host_;
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::ContextSensitiveHelp(BOOL fEnterMode)
	{
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::OnControlInfoChanged(void)      
	{
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::LockInPlaceActive(BOOL fLock)       
	{
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::GetExtendedControl(IDispatch** ppDisp)        
	{
		if( ppDisp == NULL ) return E_POINTER;   
		if( owner_ == NULL ) return E_UNEXPECTED;
		if( owner_->unkown_ptr_ == NULL ) return E_UNEXPECTED;
		return owner_->unkown_ptr_->QueryInterface(IID_IDispatch, (LPVOID*) ppDisp);
	}

	STDMETHODIMP ActiveXCtrl::TransformCoords(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags)       
	{
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
	{
	return S_FALSE;
	}

	STDMETHODIMP ActiveXCtrl::OnFocus(BOOL fGotFocus)
	{
		m_bFocused = (fGotFocus == TRUE);
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::ShowPropertyFrame(void)
	{

		return E_NOTIMPL;
	}

	STDMETHODIMP ActiveXCtrl::EnumObjects(DWORD grfFlags, IEnumUnknown** ppenum)
	{
		if( ppenum == NULL ) return E_POINTER;
		if( owner_ == NULL ) return E_UNEXPECTED;
		*ppenum = new CActiveXEnum(owner_->unkown_ptr_);
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::LockContainer(BOOL fLock)
	{
		m_bLocked = fLock != FALSE;
		return S_OK;
	}

	STDMETHODIMP ActiveXCtrl::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut)
	{
		return E_NOTIMPL;
	}

	HRESULT ActiveXCtrl::CreateActiveXWnd()
	{
		if( wnd_ != NULL ) return S_OK;
		wnd_ = new CActiveXWnd;
		if( wnd_ == NULL ) return E_OUTOFMEMORY;
		owner_->hwnd_host_ = wnd_->Init(this, owner_->GetManager()->GetPaintWindow());
		return S_OK;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	HWND CActiveXWnd::Init(ActiveXCtrl* pOwner, HWND hWndParent)
	{
		owner_ = pOwner;
		UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		Create(hWndParent, _T("UIActiveX"), uStyle, 0L, 0,0,0,0, NULL);
		return wnd_;
	}

	std::wstring CActiveXWnd::GetWindowClassName() const
	{
		return _T("ActiveXWnd");
	}

	void CActiveXWnd::OnFinalMessage(HWND hWnd)
	{
		if( owner_->owner_->GetManager()->IsLayered() ) {
			owner_->owner_->GetManager()->RemovePaintChildWnd(hWnd);
		}
	}

	void CActiveXWnd::DoVerb(LONG iVerb)
	{
		if( owner_ == NULL ) return;
		if( owner_->owner_ == NULL ) return;
		IOleObject* pUnk = NULL;
		owner_->owner_->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
		if( pUnk == NULL ) return;
		SafeRelease<IOleObject> RefOleObject = pUnk;
		IOleClientSite* pOleClientSite = NULL;
		owner_->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
		SafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
		pUnk->DoVerb(iVerb, NULL, pOleClientSite, 0, wnd_, &owner_->owner_->GetPos());
	}

	LRESULT CActiveXWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes=0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_TIMER:         lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
		case WM_PAINT:         lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_PRINT:		   lRes = OnPrint(uMsg, wParam, lParam, bHandled); break;
		case WM_SETFOCUS:      lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_ERASEBKGND:    lRes = OnEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEACTIVATE: lRes = OnMouseActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEWHEEL: break;
		default:
			bHandled = FALSE;
		}
		if( !bHandled ) return Window::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CActiveXWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( owner_->owner_->GetManager()->IsLayered() ) {
			::SetTimer(wnd_, CARET_TIMERID, ::GetCaretBlinkTime(), NULL);
		}
		return 0;
	}

	LRESULT CActiveXWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == CARET_TIMERID) {
			if (owner_->owner_->GetManager()->IsLayered()) {
				owner_->owner_->GetManager()->AddPaintChildWnd(wnd_);
				m_iLayeredTick += 1;
				if (m_iLayeredTick >= 10) {
					m_iLayeredTick = 0;
					m_bDrawCaret = !m_bDrawCaret;
				}
			}
			return 0;
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT CActiveXWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( owner_->m_pViewObject == NULL ) bHandled = FALSE;
		return 1;
	}

	LRESULT CActiveXWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		IOleObject* pUnk = NULL;
		owner_->owner_->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
		if( pUnk == NULL ) return 0;
		SafeRelease<IOleObject> RefOleObject = pUnk;
		DWORD dwMiscStatus = 0;
		pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
		if( (dwMiscStatus & OLEMISC_NOUIACTIVATE) != 0 ) return 0;
		if( !owner_->m_bInPlaceActive ) DoVerb(OLEIVERB_INPLACEACTIVATE);
		bHandled = FALSE;
		return 0;
	}

	LRESULT CActiveXWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		owner_->m_bFocused = true;
		if( !owner_->m_bUIActivated ) DoVerb(OLEIVERB_UIACTIVATE);
		return 0;
	}

	LRESULT CActiveXWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		owner_->m_bFocused = false;
		return 0;
	}

	LRESULT CActiveXWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PAINTSTRUCT ps = { 0 };
		::BeginPaint(wnd_, &ps);
		::EndPaint(wnd_, &ps);
		return 1;
	}

	LRESULT CActiveXWnd::OnPrint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		RECT rcClient;
		::GetClientRect(wnd_, &rcClient);
		owner_->m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, (HDC)wParam, (RECTL*) &rcClient, NULL, NULL, NULL); 

		if (m_bDrawCaret ) {
			RECT rcPos = owner_->owner_->GetPos();
			GUITHREADINFO guiThreadInfo;
			guiThreadInfo.cbSize = sizeof(GUITHREADINFO);
			::GetGUIThreadInfo(NULL, &guiThreadInfo);
			if (guiThreadInfo.hwndCaret) {
				POINT ptCaret;
				ptCaret.x = guiThreadInfo.rcCaret.left;
				ptCaret.y = guiThreadInfo.rcCaret.top;
				::ClientToScreen(guiThreadInfo.hwndCaret, &ptCaret);
				::ScreenToClient(owner_->owner_->GetManager()->GetPaintWindow(), &ptCaret);
				if( ::PtInRect(&rcPos, ptCaret) ) {
					RECT rcCaret;
					rcCaret = guiThreadInfo.rcCaret;
					rcCaret.right = rcCaret.left;
					RenderEngine::DrawLine((HDC)wParam, rcCaret, 1, 0xFF000000);
				}
			}
		}

		return 1;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ActiveX)

	ActiveX::ActiveX() : unkown_ptr_(NULL), control_(NULL), hwnd_host_(NULL), is_created_(false), is_delay_created_(true), is_mfc_(false)
	{
		clsid_ = IID_NULL;
	}

	ActiveX::~ActiveX()
	{
		ReleaseControl();
	}

	std::wstring ActiveX::GetClass() const
	{
		return _T("ActiveX");
	}

	LPVOID ActiveX::GetInterface(std::wstring pstrName)
	{
		if( _tcscmp(pstrName.c_str(), DUI_CTR_ACTIVEX) == 0 ) return static_cast<ActiveX*>(this);
		return Control::GetInterface(pstrName);
	}

	HWND ActiveX::GetHostWindow() const
	{
		return hwnd_host_;
	}

	static void PixelToHiMetric(const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric)
	{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
		int nPixelsPerInchX;    // Pixels per logical inch along width
		int nPixelsPerInchY;    // Pixels per logical inch along height
		HDC hDCScreen = ::GetDC(NULL);
		nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
		nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
		::ReleaseDC(NULL, hDCScreen);
		lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, nPixelsPerInchX);
		lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, nPixelsPerInchY);
	}

	void ActiveX::SetVisible(bool bVisible)
	{
		Control::SetVisible(bVisible);
		if( hwnd_host_ != NULL && !control_->m_bWindowless ) 
			::ShowWindow(hwnd_host_, IsVisible() ? SW_SHOW : SW_HIDE);
	}

	void ActiveX::SetInternVisible(bool bVisible)
	{
		Control::SetInternVisible(bVisible);
		if( hwnd_host_ != NULL && !control_->m_bWindowless ) 
			::ShowWindow(hwnd_host_, IsVisible() ? SW_SHOW : SW_HIDE);
	}

	void ActiveX::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);

		if( !is_created_ ) DoCreateControl();

		if( unkown_ptr_ == NULL ) return;
		if( control_ == NULL ) return;

		SIZEL hmSize = { 0 };
		SIZEL pxSize = { 0 };
		pxSize.cx = rect_.right - rect_.left;
		pxSize.cy = rect_.bottom - rect_.top;
		PixelToHiMetric(&pxSize, &hmSize);

		if( unkown_ptr_ != NULL ) {
			unkown_ptr_->SetExtent(DVASPECT_CONTENT, &hmSize);
		}
		if( control_->m_pInPlaceObject != NULL ) {
			CDuiRect rcItem = rect_;
			if( !control_->m_bWindowless ) rcItem.ResetOffset();
			control_->m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
		}
		if( !control_->m_bWindowless ) {
			ASSERT(control_->wnd_);
			::MoveWindow(*control_->wnd_, rect_.left, rect_.top, rect_.right - rect_.left, rect_.bottom - rect_.top, TRUE);
		}
	}

	void ActiveX::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		Control::Move(szOffset, bNeedInvalidate);
		if( !control_->m_bWindowless ) {
			ASSERT(control_->wnd_);
			::MoveWindow(*control_->wnd_, rect_.left, rect_.top, rect_.right - rect_.left, rect_.bottom - rect_.top, TRUE);
		}
	}

	void ActiveX::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&paint_rect_, &rcPaint, &rect_) ) return;

		if( control_ != NULL && control_->m_bWindowless && control_->m_pViewObject != NULL )
		{
			control_->m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &rect_, (RECTL*) &rect_, NULL, NULL); 
		}
	}

	void ActiveX::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcscmp(pstrName, _T("clsid")) == 0 ) CreateControl(pstrValue);
		else if( _tcscmp(pstrName, _T("modulename")) == 0 ) SetModuleName(pstrValue);
		else if( _tcscmp(pstrName, _T("delaycreate")) == 0 ) SetDelayCreate(_tcscmp(pstrValue, _T("true")) == 0);
		//else if( _tcscmp(pstrName, _T("mfc")) == 0 ) SetMFC(_tcscmp(pstrValue, _T("true")) == 0);
		else Control::SetAttribute(pstrName, pstrValue);
	}

	LRESULT ActiveX::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
	{
		if( control_ == NULL ) return 0;
		ASSERT(control_->m_bWindowless);
		if( !control_->m_bInPlaceActive ) return 0;
		if( control_->m_pInPlaceObject == NULL ) return 0;
		if( !IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
		bool bWasHandled = true;
		if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
			// Mouse message only go when captured or inside rect
			DWORD dwHitResult = control_->m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;
			if( dwHitResult == HITRESULT_OUTSIDE && control_->m_pViewObject != NULL ) {
				IViewObjectEx* pViewEx = NULL;
				control_->m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*) &pViewEx);
				if( pViewEx != NULL ) {
					POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					pViewEx->QueryHitPoint(DVASPECT_CONTENT, &rect_, ptMouse, 0, &dwHitResult);
					pViewEx->Release();
				}
			}
			if( dwHitResult != HITRESULT_HIT ) return 0;
			if( uMsg == WM_SETCURSOR ) bWasHandled = false;
		}
		else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
			// Keyboard messages just go when we have focus
			if( !IsFocused() ) return 0;
		}
		else {
			switch( uMsg ) {
			case WM_HELP:
			case WM_CONTEXTMENU:
				bWasHandled = false;
				break;
			default:
				return 0;
			}
		}
		LRESULT lResult = 0;
		HRESULT Hr = control_->m_pInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lResult);
		if( Hr == S_OK ) bHandled = bWasHandled;
		return lResult;
	}

	bool ActiveX::IsDelayCreate() const
	{
		return is_delay_created_;
	}

	void ActiveX::SetDelayCreate(bool bDelayCreate)
	{
		if( is_delay_created_ == bDelayCreate ) return;
		if( bDelayCreate == false ) {
			if( is_created_ == false && clsid_ != IID_NULL ) DoCreateControl();
		}
		is_delay_created_ = bDelayCreate;
	}
	
	bool ActiveX::IsMFC() const
	{
		return is_mfc_;
	}

	void ActiveX::SetMFC(bool bMFC/* = false*/)
	{
		if( is_mfc_ == bMFC ) return;
		is_mfc_ = bMFC;
	}

	bool ActiveX::CreateControl(std::wstring strCLSID)
	{
		auto pstrCLSID = strCLSID.c_str();
		CLSID clsid = { 0 };
		OLECHAR szCLSID[100] = { 0 };
#ifndef _UNICODE
		::MultiByteToWideChar(::GetACP(), 0, pstrCLSID, -1, szCLSID, lengthof(szCLSID) - 1);
#else
		_tcsncpy(szCLSID, pstrCLSID, lengthof(szCLSID) - 1);
#endif
		if( pstrCLSID[0] == '{' ) ::CLSIDFromString(szCLSID, &clsid);
		else ::CLSIDFromProgID(szCLSID, &clsid);
		return CreateControl(clsid);
	}

	bool ActiveX::CreateControl(const CLSID clsid)
	{
		ASSERT(clsid!=IID_NULL);
		if( clsid == IID_NULL ) return false;
		is_created_ = false;
		clsid_ = clsid;
		if( !is_delay_created_ ) DoCreateControl();
		return true;
	}

	void ActiveX::ReleaseControl()
	{
		// ÒÆ³ýÏûÏ¢Á´
		pm_->RemoveMessageFilter(this);

		if( unkown_ptr_ != NULL ) {
			IObjectWithSite* pSite = NULL;
			unkown_ptr_->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
			if( pSite != NULL ) {
				pSite->SetSite(NULL);
				pSite->Release();
			}
			if(!IsMFC()) {
				unkown_ptr_->Close(OLECLOSE_NOSAVE);
			}
			unkown_ptr_->SetClientSite(NULL);
			unkown_ptr_->Release(); 
			unkown_ptr_ = NULL;
		}		
		// Ïú»ÙCActiveXCtrl
		if( control_ != NULL ) {
			control_->owner_ = NULL;
			control_->Release();
			control_ = NULL;
		}

		hwnd_host_ = NULL;
	}

	typedef HRESULT (__stdcall *DllGetClassObjectFunc)(REFCLSID rclsid, REFIID riid, LPVOID* ppv); 

	bool ActiveX::DoCreateControl()
	{
		ReleaseControl();
		// At this point we'll create the ActiveX control
		is_created_ = true;
		IOleControl* pOleControl = NULL;

		HRESULT Hr = -1;
		if( !module_name_.empty() ) {
			HMODULE hModule = ::LoadLibrary(module_name_.c_str());
			if( hModule != NULL ) {
				IClassFactory* aClassFactory = NULL;
				DllGetClassObjectFunc aDllGetClassObjectFunc = (DllGetClassObjectFunc)::GetProcAddress(hModule, "DllGetClassObject");
				Hr = aDllGetClassObjectFunc(clsid_, IID_IClassFactory, (LPVOID*)&aClassFactory);
				if( SUCCEEDED(Hr) ) {
					Hr = aClassFactory->CreateInstance(NULL, IID_IOleObject, (LPVOID*)&pOleControl);
				}
				aClassFactory->Release();
			}
		}
		if( FAILED(Hr) ) {
			Hr = ::CoCreateInstance(clsid_, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*)&pOleControl);
		}
		ASSERT(SUCCEEDED(Hr));
		if( FAILED(Hr) ) return false;
		pOleControl->QueryInterface(IID_IOleObject, (LPVOID*) &unkown_ptr_);
		pOleControl->Release();
		if( unkown_ptr_ == NULL ) return false;
		// Create the host too
		control_ = new ActiveXCtrl();
		control_->owner_ = this;
		// More control creation stuff
		DWORD dwMiscStatus = 0;
		unkown_ptr_->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
		IOleClientSite* pOleClientSite = NULL;
		control_->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
		SafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
		// Initialize control
		if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) != 0 ) unkown_ptr_->SetClientSite(pOleClientSite);
		IPersistStreamInit* pPersistStreamInit = NULL;
		unkown_ptr_->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersistStreamInit);
		if( pPersistStreamInit != NULL ) {
			Hr = pPersistStreamInit->InitNew();
			pPersistStreamInit->Release();
		}
		if( FAILED(Hr) ) return false;
		if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) == 0 ) unkown_ptr_->SetClientSite(pOleClientSite);
		// Grab the view...
		Hr = unkown_ptr_->QueryInterface(IID_IViewObjectEx, (LPVOID*) &control_->m_pViewObject);
		if( FAILED(Hr) ) Hr = unkown_ptr_->QueryInterface(IID_IViewObject2, (LPVOID*) &control_->m_pViewObject);
		if( FAILED(Hr) ) Hr = unkown_ptr_->QueryInterface(IID_IViewObject, (LPVOID*) &control_->m_pViewObject);
		// Activate and done...
		unkown_ptr_->SetHostNames(OLESTR("ActiveX"), NULL);
		//if( pm_ != NULL ) pm_->SendNotify((Control*)this, DUI_MSGTYPE_SHOWACTIVEX, 0, 0, false);
		Notify(kEventShowActiveX);
		if( (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0 ) {
			try
			{
				Hr = unkown_ptr_->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pOleClientSite, 0, pm_->GetPaintWindow(), &rect_);
			}
			catch (...)
			{
			}
		}
		IObjectWithSite* pSite = NULL;
		unkown_ptr_->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
		if( pSite != NULL ) {
			pSite->SetSite(static_cast<IOleClientSite*>(control_));
			pSite->Release();
		}
		return SUCCEEDED(Hr);
	}

	HRESULT ActiveX::GetControl(const IID iid, LPVOID* ppRet)
	{
		ASSERT(ppRet!=NULL);
		ASSERT(*ppRet==NULL);
		if( ppRet == NULL ) return E_POINTER;
		if( unkown_ptr_ == NULL ) return E_PENDING;
		return unkown_ptr_->QueryInterface(iid, (LPVOID*) ppRet);
	}

	CLSID ActiveX::GetClisd() const
	{
		return clsid_;
	}

	std::wstring ActiveX::GetModuleName() const
	{
		return module_name_;
	}

	void ActiveX::SetModuleName(std::wstring pstrText)
	{
		module_name_ = pstrText;
	}

} // namespace DuiLib