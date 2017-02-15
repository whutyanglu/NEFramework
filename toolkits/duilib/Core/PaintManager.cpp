#include "StdAfx.h"
#include <zmouse.h>

namespace ui {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	static void GetChildWndRect(HWND hWnd, HWND hChildWnd, RECT& rcChildWnd)
	{
		::GetWindowRect(hChildWnd, &rcChildWnd);

		POINT pt;
		pt.x = rcChildWnd.left;
		pt.y = rcChildWnd.top;
		::ScreenToClient(hWnd, &pt);
		rcChildWnd.left = pt.x;
		rcChildWnd.top = pt.y;

		pt.x = rcChildWnd.right;
		pt.y = rcChildWnd.bottom;
		::ScreenToClient(hWnd, &pt);
		rcChildWnd.right = pt.x;
		rcChildWnd.bottom = pt.y;
	}

	static UINT MapKeyState()
	{
		UINT uState = 0;
		if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
		if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
		if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
		if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
		if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
		return uState;
	}

	typedef struct tagFINDTABINFO
	{
		Control* pFocus;
		Control* pLast;
		bool bForward;
		bool bNextIsIt;
	} FINDTABINFO;

	typedef struct tagFINDSHORTCUT
	{
		TCHAR ch;
		bool bPickNext;
	} FINDSHORTCUT;

	typedef struct tagTIMERINFO
	{
		Control* pSender;
		UINT nLocalID;
		HWND hWnd;
		UINT uWinTimer;
		bool bKilled;
	} TIMERINFO;


	/////////////////////////////////////////////////////////////////////////////////////
	typedef BOOL (__stdcall *PFUNCUPDATELAYEREDWINDOW)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);
	PFUNCUPDATELAYEREDWINDOW g_fUpdateLayeredWindow = NULL;

	HPEN m_hUpdateRectPen = NULL;

	HINSTANCE PaintManager::resource_instance_ = NULL;
	std::wstring PaintManager::resource_path_;
	std::wstring PaintManager::resource_zip_;
	std::wstring PaintManager::resource_zip_wnd_;  //Garfield 20160325 带密码zip包解密
	HANDLE PaintManager::resource_zip_handle_ = NULL;
	bool PaintManager::resource_zip_cached_ = true;
	int PaintManager::res_type_ = UILIB_FILE;
	TResInfo PaintManager::shared_res_info_;
	HINSTANCE PaintManager::instance_ = NULL;
	bool PaintManager::use_hsl_ = false;
	short PaintManager::H_ = 180;
	short PaintManager::S_ = 100;
	short PaintManager::L_ = 100;
	CStdPtrArray PaintManager::pre_messages_;
	CStdPtrArray PaintManager::plugins_;

	PaintManager::PaintManager(Window *wnd):
		host_wnd_(wnd),
		paint_wnd_(NULL),
		paint_dc_(NULL),
		dc_off_screen_(NULL),
		dc_background_(NULL),
		off_screen_paint_(true),
		bmp_off_screen_(NULL),
		off_screen_bits_(NULL),
		bmp_background_(NULL),
		bits_background_(NULL),
		tooltip_wnd_(NULL),
		timer_id_(0x1000),
		root_(NULL),
		focus_(NULL),
		event_hover_(NULL),
		event_click_(NULL),
		event_key_(NULL),
		is_first_layout_(true),
		need_focus_(false),
		need_update_(false),
		mouse_tracking_(false),
		mouse_captured_(false),
		used_virtual_wnd_(false),
		use_forced_shared_res_(false),
		opacity_(0xFF),
		layered_(false),
		layered_changed_(false),
		show_update_rect_(false),
		caret_active_(false),
		caret_showing_(false),
		current_caret_object_(NULL),
		use_gdiplus_text_(false),
		trh_(0),
		drag_mode_(false),
		drag_bitmap_(NULL),
		DPI_(NULL)
	{
		if (shared_res_info_.m_DefaultFontInfo.name_.empty())
		{
			shared_res_info_.m_dwDefaultDisabledColor = 0xFFA7A6AA;
			shared_res_info_.m_dwDefaultFontColor = 0xFF000000;
			shared_res_info_.m_dwDefaultLinkFontColor = 0xFF0000FF;
			shared_res_info_.m_dwDefaultLinkHoverFontColor = 0xFFD3215F;
			shared_res_info_.m_dwDefaultSelectedBkColor = 0xFFBAE4FF;

			LOGFONT lf = { 0 };
			::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
			lf.lfCharSet = DEFAULT_CHARSET;
			HFONT hDefaultFont = ::CreateFontIndirect(&lf);
			shared_res_info_.m_DefaultFontInfo.handle_ = hDefaultFont;
			shared_res_info_.m_DefaultFontInfo.name_ = lf.lfFaceName;
			shared_res_info_.m_DefaultFontInfo.size_ = -lf.lfHeight;
			shared_res_info_.m_DefaultFontInfo.bold_ = (lf.lfWeight >= FW_BOLD);
			shared_res_info_.m_DefaultFontInfo.underline_ = (lf.lfUnderline == TRUE);
			shared_res_info_.m_DefaultFontInfo.italic_ = (lf.lfItalic == TRUE);
			::ZeroMemory(&shared_res_info_.m_DefaultFontInfo.tm_, sizeof(shared_res_info_.m_DefaultFontInfo.tm_));
		}

		res_info_.m_dwDefaultDisabledColor = shared_res_info_.m_dwDefaultDisabledColor;
		res_info_.m_dwDefaultFontColor = shared_res_info_.m_dwDefaultFontColor;
		res_info_.m_dwDefaultLinkFontColor = shared_res_info_.m_dwDefaultLinkFontColor;
		res_info_.m_dwDefaultLinkHoverFontColor = shared_res_info_.m_dwDefaultLinkHoverFontColor;
		res_info_.m_dwDefaultSelectedBkColor = shared_res_info_.m_dwDefaultSelectedBkColor;

		if( m_hUpdateRectPen == NULL ) {
			m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
			// Boot Windows Common Controls (for the ToolTip control)
			::InitCommonControls();
			::LoadLibrary(_T("msimg32.dll"));
		}

		min_wnd_sz_.cx = 0;
		min_wnd_sz_.cy = 0;
		max_wnd_sz_.cx = 0;
		max_wnd_sz_.cy = 0;
		init_wnd_sz_.cx = 0;
		init_wnd_sz_.cy = 0;
		round_corner_sz_.cx = round_corner_sz_.cy = 0;
		::ZeroMemory(&size_box_sz_, sizeof(size_box_sz_));
		::ZeroMemory(&caption_rect_, sizeof(caption_rect_));
		::ZeroMemory(&layered_inset_rect_, sizeof(layered_inset_rect_));
		::ZeroMemory(&layered_update_rect_, sizeof(layered_update_rect_));
		last_mouse_pt_.x = last_mouse_pt_.y = -1;

		gduplus_startup_input_ = new Gdiplus::GdiplusStartupInput;
		Gdiplus::GdiplusStartup( &gdi_plus_token_, gduplus_startup_input_, NULL); // 加载GDI接口

		ShadowUI::Initialize(instance_);
	}

	PaintManager::~PaintManager()
	{
		Cleanup(); // TODO : luyang CEF BUG
	}

	void PaintManager::Init(HWND hWnd, std::wstring pstrName)
	{
		ASSERT(::IsWindow(hWnd));
		
		name_hash_.Resize();
		RemoveAllImages();
		RemoveAllDefaultAttributeList();
		RemoveAllWindowCustomAttribute();
		RemoveAllOptionGroups();
		RemoveAllTimers();

		name_.clear();
		if( !pstrName.empty() ) name_ = pstrName;

		if( paint_wnd_ != hWnd ) {
			paint_wnd_ = hWnd;
			paint_dc_ = ::GetDC(hWnd);
			pre_messages_.Add(this);
		}

		SetTargetWnd(hWnd);
		InitDragDrop();
	}

	void PaintManager::Cleanup()
	{
		// Delete the control-tree structures
		for (int i = 0; i < delayed_cleanup_.GetSize(); i++) delete static_cast<Control*>(delayed_cleanup_[i]);
		for (int i = 0; i < async_notify_.GetSize(); i++) delete static_cast<EventArgs*>(async_notify_[i]);
		name_hash_.Resize(0);
		if (root_ != NULL) delete root_;

		::DeleteObject(res_info_.m_DefaultFontInfo.handle_);
		RemoveAllImages();
		RemoveAllDefaultAttributeList();
		RemoveAllWindowCustomAttribute();
		RemoveAllOptionGroups();
		RemoveAllTimers();
		RemoveAllDrawInfos(); // TODO : luyang CEF BUG

		if (tooltip_wnd_ != NULL) {
			::DestroyWindow(tooltip_wnd_);
			tooltip_wnd_ = NULL;
		}
		if (dc_off_screen_ != NULL) ::DeleteDC(dc_off_screen_);
		if (dc_background_ != NULL) ::DeleteDC(dc_background_);
		if (bmp_off_screen_ != NULL) ::DeleteObject(bmp_off_screen_);
		if (bmp_background_ != NULL) ::DeleteObject(bmp_background_);
		if (paint_dc_ != NULL) ::ReleaseDC(paint_wnd_, paint_dc_);
		pre_messages_.Remove(pre_messages_.Find(this));
		// 销毁拖拽图片
		if (drag_bitmap_ != NULL) ::DeleteObject(drag_bitmap_);
		//卸载GDIPlus
		Gdiplus::GdiplusShutdown(gdi_plus_token_);
		delete gduplus_startup_input_;
		// DPI管理对象
		if (DPI_ != NULL) {
			delete DPI_;
			DPI_ = NULL;
		}
	}

	void PaintManager::DeletePtr(void* ptr)
	{
		if(ptr) {delete ptr; ptr = NULL;}
	}

	HINSTANCE PaintManager::GetInstance()
	{
		return instance_;
	}

	std::wstring PaintManager::GetInstancePath()
	{
		if( instance_ == NULL ) return _T('\0');

		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetModuleFileName(instance_, tszModule, MAX_PATH);
		std::wstring sInstancePath = tszModule;
		int pos = sInstancePath.rfind(_T('\\'));
		if( pos >= 0 ) sInstancePath = sInstancePath.substr(0, pos + 1);
		return sInstancePath;
	}

	std::wstring PaintManager::GetCurrentPath()
	{
		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetCurrentDirectory(MAX_PATH, tszModule);
		return tszModule;
	}

	HINSTANCE PaintManager::GetResourceDll()
	{
		if( resource_instance_ == NULL ) return instance_;
		return resource_instance_;
	}

	std::wstring PaintManager::GetResourcePath()
	{
		return resource_path_;
	}

	std::wstring PaintManager::GetResourceZip()
	{
		return resource_zip_;
	}

	std::wstring PaintManager::GetResourceZipPwd()  //Garfield 20160325 带密码zip包解密
	{
		return resource_zip_wnd_;
	}
	
	bool PaintManager::IsCachedResourceZip()
	{
		return resource_zip_cached_;
	}

	HANDLE PaintManager::GetResourceZipHandle()
	{
		return resource_zip_handle_;
	}

	void PaintManager::SetInstance(HINSTANCE hInst)
	{
		instance_ = hInst;
	}

	void PaintManager::SetCurrentPath(std::wstring pStrPath)
	{
		::SetCurrentDirectory(pStrPath.c_str());
	}

	void PaintManager::SetResourceDll(HINSTANCE hInst)
	{
		resource_instance_ = hInst;
	}

	void PaintManager::SetResourcePath(std::wstring pStrPath)
	{
		resource_path_ = pStrPath;
		if( resource_path_.empty() ) return;
		TCHAR cEnd = resource_path_.at(resource_path_.length() - 1);
		if( cEnd != _T('\\') && cEnd != _T('/') ) resource_path_ += _T('\\');
	}

	void PaintManager::SetResourceZip(LPVOID pVoid, unsigned int len, std::wstring password)
	{
		if( resource_zip_ == _T("membuffer") ) return;
		if( resource_zip_cached_ && resource_zip_handle_ != NULL ) {
			CloseZip((HZIP)resource_zip_handle_);
			resource_zip_handle_ = NULL;
		}
		resource_zip_ = _T("membuffer");
		resource_zip_cached_ = true;
		resource_zip_wnd_ = password;  //Garfield 20160325 带密码zip包解密
		if( resource_zip_cached_ ) 
		{
#ifdef UNICODE
			char* pwd = w2a((wchar_t*)password.c_str());
			resource_zip_handle_ = (HANDLE)OpenZip(pVoid, len, pwd);
			if(pwd) delete[] pwd;
#else
			m_hResourceZip = (HANDLE)OpenZip(pVoid, len, password);
#endif
		}
	}

	void PaintManager::SetResourceZip2(std::wstring pStrPath, bool bCachedResourceZip, std::wstring password)
	{
		if( resource_zip_ == pStrPath && resource_zip_cached_ == bCachedResourceZip ) return;
		if( resource_zip_cached_ && resource_zip_handle_ != NULL ) {
			CloseZip((HZIP)resource_zip_handle_);
			resource_zip_handle_ = NULL;
		}
		resource_zip_ = pStrPath;
		resource_zip_cached_ = bCachedResourceZip;
		resource_zip_wnd_ = password;
		if( resource_zip_cached_ ) {
			std::wstring sFile = PaintManager::GetResourcePath();
			sFile += PaintManager::GetResourceZip();
#ifdef UNICODE
			char* pwd = w2a((wchar_t*)password.c_str());
			resource_zip_handle_ = (HANDLE)OpenZip(sFile.c_str(), pwd);
			if(pwd) if(pwd) delete[] pwd;
#else
			m_hResourceZip = (HANDLE)OpenZip(sFile.GetData(), password);
#endif
		}
	}
	
	void PaintManager::SetResourceType(int nType)
	{
		res_type_ = nType;
	}

	int PaintManager::GetResourceType()
	{
		return res_type_;
	}

	bool PaintManager::GetHSL(short* H, short* S, short* L)
	{
		*H = H_;
		*S = S_;
		*L = L_;
		return use_hsl_;
	}

	void PaintManager::SetHSL(bool bUseHSL, short H, short S, short L)
	{
		if( use_hsl_ || use_hsl_ != bUseHSL ) {
			use_hsl_ = bUseHSL;
			if( H == H_ && S == S_ && L == L_ ) return;
			H_ = CLAMP(H, 0, 360);
			S_ = CLAMP(S, 0, 200);
			L_ = CLAMP(L, 0, 200);
			AdjustSharedImagesHSL();
			for( int i = 0; i < pre_messages_.GetSize(); i++ ) {
				PaintManager* pManager = static_cast<PaintManager*>(pre_messages_[i]);
				if( pManager != NULL ) pManager->AdjustImagesHSL();
			}
		}
	}

	void PaintManager::ReloadSkin()
	{
		ReloadSharedImages();
		for( int i = 0; i < pre_messages_.GetSize(); i++ ) {
			PaintManager* pManager = static_cast<PaintManager*>(pre_messages_[i]);
			pManager->ReloadImages();
		}
	}

	PaintManager* PaintManager::GetPaintManager(std::wstring pstrName)
	{
		if( pstrName.empty()) return NULL;
		std::wstring sName = pstrName;
		if( sName.empty() ) return NULL;
		for( int i = 0; i < pre_messages_.GetSize(); i++ ) {
			PaintManager* pManager = static_cast<PaintManager*>(pre_messages_[i]);
			if( pManager != NULL && sName == pManager->GetName() ) return pManager;
		}
		return NULL;
	}

	CStdPtrArray* PaintManager::GetPaintManagers()
	{
		return &pre_messages_;
	}

	bool PaintManager::LoadPlugin(std::wstring pstrModuleName)
	{
		ASSERT( !::IsBadStringPtr(pstrModuleName.c_str(), -1) || pstrModuleName.empty() );
		if( pstrModuleName.empty() ) return false;
		HMODULE hModule = ::LoadLibrary(pstrModuleName.c_str());
		if( hModule != NULL ) {
			LPCREATECONTROL lpCreateControl = (LPCREATECONTROL)::GetProcAddress(hModule, "CreateControl");
			if( lpCreateControl != NULL ) {
				if( plugins_.Find(lpCreateControl) >= 0 ) return true;
				plugins_.Add(lpCreateControl);
				return true;
			}
		}
		return false;
	}

	CStdPtrArray* PaintManager::GetPlugins()
	{
		return &plugins_;
	}

	HWND PaintManager::GetPaintWindow() const
	{
		return paint_wnd_;
	}

	HWND PaintManager::GetTooltipWindow() const
	{
		return tooltip_wnd_;
	}

	std::wstring PaintManager::GetName() const
	{
		return name_.c_str();
	}

	HDC PaintManager::GetPaintDC() const
	{
		return paint_dc_;
	}

	POINT PaintManager::GetMousePos() const
	{
		return last_mouse_pt_;
	}

	SIZE PaintManager::GetClientSize() const
	{
		RECT rcClient = { 0 };
		::GetClientRect(paint_wnd_, &rcClient);
		return CDuiSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	}

	SIZE PaintManager::GetInitSize()
	{
		return init_wnd_sz_;
	}

	void PaintManager::SetInitSize(int cx, int cy)
	{
		init_wnd_sz_.cx = cx;
		init_wnd_sz_.cy = cy;
		if( root_ == NULL && paint_wnd_ != NULL ) {
			::SetWindowPos(paint_wnd_, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}

	RECT& PaintManager::GetSizeBox()
	{
		return size_box_sz_;
	}

	void PaintManager::SetSizeBox(RECT& rcSizeBox)
	{
		size_box_sz_ = rcSizeBox;
	}

	RECT& PaintManager::GetCaptionRect()
	{
		return caption_rect_;
	}

	void PaintManager::SetCaptionRect(RECT& rcCaption)
	{
		caption_rect_ = rcCaption;
	}

	SIZE PaintManager::GetRoundCorner() const
	{
		return round_corner_sz_;
	}

	void PaintManager::SetRoundCorner(int cx, int cy)
	{
		round_corner_sz_.cx = cx;
		round_corner_sz_.cy = cy;
	}

	SIZE PaintManager::GetMinInfo() const
	{
		return min_wnd_sz_;
	}

	void PaintManager::SetMinInfo(int cx, int cy)
	{
		ASSERT(cx>=0 && cy>=0);
		min_wnd_sz_.cx = cx;
		min_wnd_sz_.cy = cy;
	}

	SIZE PaintManager::GetMaxInfo() const
	{
		return max_wnd_sz_;
	}

	void PaintManager::SetMaxInfo(int cx, int cy)
	{
		ASSERT(cx>=0 && cy>=0);
		max_wnd_sz_.cx = cx;
		max_wnd_sz_.cy = cy;
	}

	bool PaintManager::IsShowUpdateRect() const
	{
		return show_update_rect_;
	}

	void PaintManager::SetShowUpdateRect(bool show)
	{
		show_update_rect_ = show;
	}

	BYTE PaintManager::GetOpacity() const
	{
		return opacity_;
	}

	void PaintManager::SetOpacity(BYTE nOpacity)
	{
		opacity_ = nOpacity;
		if( paint_wnd_ != NULL ) {
			typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
			PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes;

			HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
			if (hUser32)
			{
				fSetLayeredWindowAttributes = 
					(PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
				if( fSetLayeredWindowAttributes == NULL ) return;
			}

			DWORD dwStyle = ::GetWindowLong(paint_wnd_, GWL_EXSTYLE);
			DWORD dwNewStyle = dwStyle;
			if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
			else dwNewStyle &= ~WS_EX_LAYERED;
			if(dwStyle != dwNewStyle) ::SetWindowLong(paint_wnd_, GWL_EXSTYLE, dwNewStyle);
			fSetLayeredWindowAttributes(paint_wnd_, 0, nOpacity, LWA_ALPHA);
		}
	}

	bool PaintManager::IsLayered()
	{
		return layered_;
	}

	void PaintManager::SetLayered(bool bLayered)
	{
		if( paint_wnd_ != NULL && bLayered != layered_ ) {
			UINT uStyle = GetWindowStyle(paint_wnd_);
			if( (uStyle & WS_CHILD) != 0 ) return;
			if( g_fUpdateLayeredWindow == NULL ) {
				HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
				if (hUser32) {
					g_fUpdateLayeredWindow = 
						(PFUNCUPDATELAYEREDWINDOW)::GetProcAddress(hUser32, "UpdateLayeredWindow");
					if( g_fUpdateLayeredWindow == NULL ) return;
				}
			}
			layered_ = bLayered;
			if( root_ != NULL ) root_->NeedUpdate();
			Invalidate();
		}
	}

	RECT& PaintManager::GetLayeredInset()
	{
		return layered_inset_rect_;
	}

	void PaintManager::SetLayeredInset(RECT& rcLayeredInset)
	{
		layered_inset_rect_ = rcLayeredInset;
		layered_changed_ = true;
		Invalidate();
	}

	BYTE PaintManager::GetLayeredOpacity()
	{
		return opacity_;
	}

	void PaintManager::SetLayeredOpacity(BYTE nOpacity)
	{
		opacity_ = nOpacity;
		layered_changed_ = true;
		Invalidate();
	}

	//std::wstring CPaintManagerUI::GetLayeredImage()
	//{
	//	return m_diLayered.sDrawString;
	//}

	//void CPaintManagerUI::SetLayeredImage(std::wstring pstrImage)
	//{
	//	m_diLayered.sDrawString = pstrImage;
	//	RECT rcNull = {0};
	//	CRenderEngine::DrawImage(NULL, this, rcNull, rcNull, m_diLayered);
	//	m_bLayeredChanged = true;
	//	Invalidate();
	//}

	bool PaintManager::ShowCaret(bool bShow)
	{
		if(caret_showing_ == bShow)
			return true;

		caret_showing_ = bShow;
		if(!bShow) {	
			::KillTimer(paint_wnd_, CARET_TIMERID);
			if(caret_active_) {
				Invalidate(caret_rect_);
			}
			caret_active_ = false;
		}
		else {
			::SetTimer(paint_wnd_, CARET_TIMERID, ::GetCaretBlinkTime(), NULL);
			if(!caret_active_) {
				Invalidate(caret_rect_);
				caret_active_ = true;
			}
		}

		return true;
	}

	bool PaintManager::SetCaretPos(RichEdit* obj, int x, int y)
	{
		if(!::SetCaretPos(x, y)) 
			return false;

		current_caret_object_ = obj;
		RECT tempRt = caret_rect_;
		int w = caret_rect_.right - caret_rect_.left;
		int h = caret_rect_.bottom - caret_rect_.top;
		caret_rect_.left = x;
		caret_rect_.top = y;
		caret_rect_.right = x + w;
		caret_rect_.bottom = y + h;
		Invalidate(tempRt);
		Invalidate(caret_rect_);

		return true;
	}

	RichEdit* PaintManager::GetCurrentCaretObject()
	{
		return current_caret_object_;
	}

	bool PaintManager::CreateCaret(HBITMAP hBmp, int nWidth, int nHeight)
	{
		::CreateCaret(paint_wnd_, hBmp, nWidth, nHeight);
		//TODO hBmp处理位图光标
		caret_rect_.right = caret_rect_.left + nWidth;
		caret_rect_.bottom = caret_rect_.top + nHeight;
		return true;
	}

	void PaintManager::DrawCaret(HDC hDC, const RECT& rcPaint)
	{
		if(current_caret_object_ && (!current_caret_object_->IsFocused() || paint_wnd_ != ::GetFocus()))
		{
			::KillTimer(paint_wnd_, CARET_TIMERID);
			if(caret_active_)
			{
				Invalidate(caret_rect_);
			}
			caret_active_ = false;
			return;
		}

		if(caret_active_ && caret_showing_ && current_caret_object_)
		{
			RECT temp = {};
			if(::IntersectRect(&temp, &rcPaint, &caret_rect_))
			{
				DWORD dwColor = current_caret_object_->GetTextColor();
				if(dwColor == 0) dwColor = res_info_.m_dwDefaultFontColor;
				RenderEngine::DrawColor(hDC, temp, dwColor);
			}
		}
	}

	ShadowUI* PaintManager::GetShadow()
	{
		return &shadow_;
	}

	void PaintManager::SetUseGdiplusText(bool bUse)
	{
		use_gdiplus_text_ = bUse;
	}

	bool PaintManager::IsUseGdiplusText() const
	{
		return use_gdiplus_text_;
	}

	void PaintManager::SetGdiplusTextRenderingHint(int trh)
	{
		trh_ = trh;
	}

	int PaintManager::GetGdiplusTextRenderingHint() const
	{
		return trh_;
	}

	bool PaintManager::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
	{
		for( int i = 0; i < pre_message_filters_.GetSize(); i++ ) 
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<IMessageFilterUI*>(pre_message_filters_[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
			if( bHandled ) {
				return true;
			}
		}
		switch( uMsg ) {
		case WM_KEYDOWN:
			{
				// Tabbing between controls
				if( wParam == VK_TAB ) {
					if( focus_ && focus_->IsVisible() && focus_->IsEnabled() && _tcsstr(focus_->GetClass().c_str(), _T("RichEdit")) != NULL ) {
						if( static_cast<RichEdit*>(focus_)->IsWantTab() ) return false;
					}
					if( focus_ && focus_->IsVisible() && focus_->IsEnabled() && _tcsstr(focus_->GetClass().c_str(), _T("WkeWebkit")) != NULL ) {
						return false;
					}
					SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
					return true;
				}
			}
			break;
		case WM_SYSCHAR:
			{
				// Handle ALT-shortcut key-combinations
				FINDSHORTCUT fs = { 0 };
				fs.ch = toupper((int)wParam);
				Control* pControl = root_->FindControl(__FindControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
				if( pControl != NULL ) {
					pControl->SetFocus();
					pControl->Activate();
					return true;
				}
			}
			break;
		case WM_SYSKEYDOWN:
			{
				if( focus_ != NULL ) {
					EventArgs event;
					event.type_ = kEventSyskey;
					event.key_ = (TCHAR)wParam;
					event.mouse_point_ = last_mouse_pt_;
					event.key_state_ = MapKeyState();
					event.timestamp_ = ::GetTickCount();
					focus_->Event(event);
				}
			}
			break;
		}
		return false;
	}

	bool PaintManager::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		if( paint_wnd_ == NULL ) return false;
		// Cycle through listeners
		for( int i = 0; i < message_filters_.GetSize(); i++ ) 
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<IMessageFilterUI*>(message_filters_[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
			if( bHandled ) {
				lRes = lResult;
				return true;
			}
		}
		// Custom handling of events
		switch( uMsg ) {
			break;
		case WM_CLOSE:
			{
				// Make sure all matching "closing" events are sent
				EventArgs event;
				event.mouse_point_ = last_mouse_pt_;
				event.key_state_ = MapKeyState();
				event.timestamp_ = ::GetTickCount();
				if( event_hover_ != NULL ) {
					event.type_ = kEventMouseLeave;
					event.sender_ = event_hover_;
					event_hover_->Event(event);
				}
				if( event_click_ != NULL ) {
					event.type_ = kEventMouseButtonUp;
					event.sender_ = event_click_;
					event_click_->Event(event);
				}

				SetFocus(NULL);

				if( ::GetActiveWindow() == paint_wnd_ ) {
					HWND hwndParent = ::GetWindowOwner(paint_wnd_);
					if( hwndParent != NULL ) ::SetFocus(hwndParent);
				}

				if (tooltip_wnd_ != NULL) {
					::DestroyWindow(tooltip_wnd_);
					tooltip_wnd_ = NULL;
				}
			}
			break;
		case WM_ERASEBKGND:
			{
				// We'll do the painting here...
				lRes = 1;
			}
			return true;
		case WM_PAINT:
			{
				RECT rcPaint = { 0 };
				if( !::GetUpdateRect(paint_wnd_, &rcPaint, FALSE) ) return true;

				if( root_ == NULL ) {
					PAINTSTRUCT ps = { 0 };
					::BeginPaint(paint_wnd_, &ps);
					::EndPaint(paint_wnd_, &ps);
					return true;
				}

				bool bNeedSizeMsg = false;
				RECT rcClient = { 0 };
				::GetClientRect(paint_wnd_, &rcClient);
				DWORD dwWidth = rcClient.right - rcClient.left;
				DWORD dwHeight = rcClient.bottom - rcClient.top;

				SetPainting(true);
				if( need_update_ ) {
					need_update_ = false;
					if( !::IsRectEmpty(&rcClient) && !::IsIconic(paint_wnd_) ) {
						if( root_->IsUpdateNeeded() ) {
							if( dc_off_screen_ != NULL ) ::DeleteDC(dc_off_screen_);
							if( bmp_off_screen_ != NULL ) ::DeleteObject(bmp_off_screen_);
							dc_off_screen_ = NULL;
							bmp_off_screen_ = NULL;
							root_->SetPos(rcClient, true);
							bNeedSizeMsg = true;
						}
						else {
							Control* pControl = NULL;
							found_controls_.Empty();
							root_->FindControl(__FindControlsFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST | UIFIND_UPDATETEST);
							for( int it = 0; it < found_controls_.GetSize(); it++ ) {
								pControl = static_cast<Control*>(found_controls_[it]);
								if( !pControl->IsFloat() ) pControl->SetPos(pControl->GetPos(), true);
								else pControl->SetPos(pControl->GetRelativePos(), true);
							}
						}
						// We'll want to notify the window when it is first initialized
						// with the correct layout. The window form would take the time
						// to submit swipes/animations.
						if( is_first_layout_ ) {
							is_first_layout_ = false;
							//root_->HandleEvent(kEventWindowInit);
							//SendNotify(root_, DUI_MSGTYPE_WINDOWINIT,  0, 0, false);
							// 更新阴影窗口显示
							if (host_wnd_) {
								host_wnd_->OnWindowInit();
							}

							shadow_.Update(paint_wnd_);
						}
					}
				}

				// Set focus to first control?
				if( need_focus_ ) {
					SetNextTabControl();
				}

				if( layered_ ) {
					DWORD dwExStyle = ::GetWindowLong(paint_wnd_, GWL_EXSTYLE);
					DWORD dwNewExStyle = dwExStyle | WS_EX_LAYERED;
					if(dwExStyle != dwNewExStyle) ::SetWindowLong(paint_wnd_, GWL_EXSTYLE, dwNewExStyle);
					off_screen_paint_ = true;
					UnionRect(&rcPaint, &rcPaint, &layered_update_rect_);
					if( rcPaint.right > rcClient.right ) rcPaint.right = rcClient.right;
					if( rcPaint.bottom > rcClient.bottom ) rcPaint.bottom = rcClient.bottom;
					::ZeroMemory(&layered_update_rect_, sizeof(layered_update_rect_));
				}

				if( off_screen_paint_ && bmp_off_screen_ == NULL ) {
					dc_off_screen_ = ::CreateCompatibleDC(paint_dc_);
					bmp_off_screen_ = RenderEngine::CreateARGB32Bitmap(paint_dc_, dwWidth, dwHeight, (LPBYTE*)&off_screen_bits_); 
					ASSERT(dc_off_screen_);
					ASSERT(bmp_off_screen_);
				}
				// Begin Windows paint
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(paint_wnd_, &ps);
				if( off_screen_paint_ ) {
					HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(dc_off_screen_, bmp_off_screen_);
					int iSaveDC = ::SaveDC(dc_off_screen_);
					if (layered_) {
						for( LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y ) {
							for( LONG x = rcPaint.left; x < rcPaint.right; ++x ) {
								int i = (y * dwWidth + x) * 4;
								*(DWORD*)(&off_screen_bits_[i]) = 0;
							}
						}
					}
					root_->DoPaint(dc_off_screen_, rcPaint);
					DrawCaret(dc_off_screen_, rcPaint);
					for( int i = 0; i < post_paint_controls_.GetSize(); i++ ) {
						Control* pPostPaintControl = static_cast<Control*>(post_paint_controls_[i]);
						pPostPaintControl->DoPostPaint(dc_off_screen_, rcPaint);
					}
					if( layered_ ) {
						for( LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y ) {
							for( LONG x = rcPaint.left; x < rcPaint.right; ++x ) {
								int i = (y * dwWidth + x) * 4;
								if((off_screen_bits_[i + 3] == 0)&& (off_screen_bits_[i + 0] != 0 || off_screen_bits_[i + 1] != 0|| off_screen_bits_[i + 2] != 0))
									off_screen_bits_[i + 3] = 255;
							}
						}

						for( int i = 0; i < child_wnds_.GetSize(); ) {
							HWND hChildWnd = static_cast<HWND>(child_wnds_[i]);
							if (!::IsWindow(hChildWnd)) {
								child_wnds_.Remove(i);
								continue;
							}
							++i;
							if (!::IsWindowVisible(hChildWnd)) continue;
							RECT rcChildWnd;
							GetChildWndRect(paint_wnd_, hChildWnd, rcChildWnd);

							RECT rcTemp = { 0 };
							if( !::IntersectRect(&rcTemp, &rcPaint, &rcChildWnd) ) continue;

							COLORREF* pChildBitmapBits = NULL;
							HDC hChildMemDC = ::CreateCompatibleDC(dc_off_screen_);
							HBITMAP hChildBitmap = RenderEngine::CreateARGB32Bitmap(hChildMemDC, rcChildWnd.right-rcChildWnd.left, rcChildWnd.bottom-rcChildWnd.top, (LPBYTE*)&pChildBitmapBits); 
							::ZeroMemory(pChildBitmapBits, (rcChildWnd.right - rcChildWnd.left)*(rcChildWnd.bottom - rcChildWnd.top)*4);
							HBITMAP hOldChildBitmap = (HBITMAP) ::SelectObject(hChildMemDC, hChildBitmap);
							::SendMessage(hChildWnd, WM_PRINT, (WPARAM)hChildMemDC,(LPARAM)(PRF_CHECKVISIBLE|PRF_CHILDREN|PRF_CLIENT|PRF_OWNED));
							COLORREF* pChildBitmapBit;
							for( LONG y = 0; y < rcChildWnd.bottom-rcChildWnd.top; y++ ) {
								for( LONG x = 0; x < rcChildWnd.right-rcChildWnd.left; x++ ) {
									pChildBitmapBit = pChildBitmapBits+y*(rcChildWnd.right-rcChildWnd.left) + x;
									if (*pChildBitmapBit != 0x00000000) *pChildBitmapBit |= 0xff000000;
								}
							}
							::BitBlt(dc_off_screen_, rcChildWnd.left, rcChildWnd.top, rcChildWnd.right - rcChildWnd.left, rcChildWnd.bottom - rcChildWnd.top, hChildMemDC, 0, 0, SRCCOPY);
							::SelectObject(hChildMemDC, hOldChildBitmap);
							::DeleteObject(hChildBitmap);
							::DeleteDC(hChildMemDC);
						}
					}
					::RestoreDC(dc_off_screen_, iSaveDC);

					if( layered_ ) {
						RECT rcWnd = { 0 };
						::GetWindowRect(paint_wnd_, &rcWnd);
						BLENDFUNCTION bf = { AC_SRC_OVER, 0, opacity_, AC_SRC_ALPHA };
						POINT ptPos   = { rcWnd.left, rcWnd.top };
						SIZE sizeWnd  = { dwWidth, dwHeight };
						POINT ptSrc   = { 0, 0 };
						g_fUpdateLayeredWindow(paint_wnd_, paint_dc_, &ptPos, &sizeWnd, dc_off_screen_, &ptSrc, 0, &bf, ULW_ALPHA);
					}
					else {
						::BitBlt(paint_dc_, rcPaint.left, rcPaint.top, rcPaint.right - rcPaint.left, rcPaint.bottom - rcPaint.top, dc_off_screen_, rcPaint.left, rcPaint.top, SRCCOPY);
					}
					::SelectObject(dc_off_screen_, hOldBitmap);

					if( show_update_rect_ ) {
						RenderEngine::DrawRect(paint_dc_, rcPaint, 1, 0xFFFF0000);
					}
				}
				else {
					// A standard paint job
					int iSaveDC = ::SaveDC(paint_dc_);
					root_->DoPaint(paint_dc_, rcPaint);
					for( int i = 0; i < post_paint_controls_.GetSize(); i++ ) {
						Control* pPostPaintControl = static_cast<Control*>(post_paint_controls_[i]);
						pPostPaintControl->DoPostPaint(paint_dc_, rcPaint);
					}
					::RestoreDC(paint_dc_, iSaveDC);
				}
				// All Done!
				::EndPaint(paint_wnd_, &ps);
				if( need_update_ ) Invalidate();
				SetPainting(false);
				// 发送窗口大小改变消息
				if(bNeedSizeMsg) {
					if (host_wnd_) {
						host_wnd_->OnWindowSize();
					}
					//this->SendNotify(root_, DUI_MSGTYPE_WINDOWSIZE, 0, 0, true);
				}
				return true;
			}
		case WM_PRINTCLIENT:
			{
				if( root_ == NULL ) break;
				RECT rcClient;
				::GetClientRect(paint_wnd_, &rcClient);
				HDC hDC = (HDC) wParam;
				int save = ::SaveDC(hDC);
				root_->DoPaint(hDC, rcClient);
				if( (lParam & PRF_CHILDREN) != 0 ) {
					HWND hWndChild = ::GetWindow(paint_wnd_, GW_CHILD);
					while( hWndChild != NULL ) {
						RECT rcPos = { 0 };
						::GetWindowRect(hWndChild, &rcPos);
						::MapWindowPoints(HWND_DESKTOP, paint_wnd_, reinterpret_cast<LPPOINT>(&rcPos), 2);
						::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
						::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
						hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
					}
				}
				::RestoreDC(hDC, save);
			}
			break;
		case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
				if( min_wnd_sz_.cx > 0 ) lpMMI->ptMinTrackSize.x = min_wnd_sz_.cx;
				if( min_wnd_sz_.cy > 0 ) lpMMI->ptMinTrackSize.y = min_wnd_sz_.cy;
				if( max_wnd_sz_.cx > 0 ) lpMMI->ptMaxTrackSize.x = max_wnd_sz_.cx;
				if( max_wnd_sz_.cy > 0 ) lpMMI->ptMaxTrackSize.y = max_wnd_sz_.cy;
				if( max_wnd_sz_.cx > 0 ) lpMMI->ptMaxSize.x = max_wnd_sz_.cx;
				if( max_wnd_sz_.cy > 0 ) lpMMI->ptMaxSize.y = max_wnd_sz_.cy;
			}
			break;
		case WM_SIZE:
			{
				if( focus_ != NULL ) {
					EventArgs event;
					event.type_ = kEventWindowSize;
					event.sender_ = focus_;
					event.timestamp_ = ::GetTickCount();
					focus_->Event(event);
				}
				if( root_ != NULL ) root_->NeedUpdate();
			}
			return true;
		case WM_TIMER:
			{
				if(CARET_TIMERID == LOWORD(wParam)){
					Invalidate(caret_rect_);
					caret_active_ = !caret_active_;
				}
				else{
					for( int i = 0; i < timers_.GetSize(); i++ ) {
						const TIMERINFO* pTimer = static_cast<TIMERINFO*>(timers_[i]);
						if(pTimer->hWnd == paint_wnd_ && 
							pTimer->uWinTimer == LOWORD(wParam) && 
							pTimer->bKilled == false)
						{
							EventArgs event;
							event.type_ = kEventTimer;
							event.sender_ = pTimer->pSender;
							event.timestamp_ = ::GetTickCount();
							event.mouse_point_ = last_mouse_pt_;
							event.key_state_ = MapKeyState();
							event.w_param_ = pTimer->nLocalID;
							event.l_param_ = lParam;
							pTimer->pSender->Event(event);
							break;
						}
					}
				}

			}
			break;
		case WM_MOUSEHOVER:
			{
				mouse_tracking_ = false;
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				Control* pHover = FindControl(pt);
				if( pHover == NULL ) break;
				// Generate mouse hover event
				if( event_hover_ != NULL ) {
					EventArgs event;
					event.type_ = kEventMouseHover;
					event.sender_ = event_hover_;
					event.w_param_ = wParam;
					event.l_param_ = lParam;
					event.timestamp_ = ::GetTickCount();
					event.mouse_point_ = pt;
					event.key_state_ = MapKeyState();
					event_hover_->Event(event);
				}
				// Create tooltip information
				std::wstring sToolTip = pHover->GetToolTip();
				if( sToolTip.empty() ) return true;
				::ZeroMemory(&tool_tip_, sizeof(TOOLINFO));
				tool_tip_.cbSize = sizeof(TOOLINFO);
				tool_tip_.uFlags = TTF_IDISHWND;
				tool_tip_.hwnd = paint_wnd_;
				tool_tip_.uId = (UINT_PTR) paint_wnd_;
				tool_tip_.hinst = instance_;
				tool_tip_.lpszText = const_cast<LPTSTR>( sToolTip.c_str() );
				tool_tip_.rect = pHover->GetPos();
				if( tooltip_wnd_ == NULL ) {
					tooltip_wnd_ = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, paint_wnd_, NULL, instance_, NULL);
					::SendMessage(tooltip_wnd_, TTM_ADDTOOL, 0, (LPARAM) &tool_tip_);
					::SendMessage(tooltip_wnd_,TTM_SETMAXTIPWIDTH,0, pHover->GetToolTipWidth());
				}
				if(!::IsWindowVisible(tooltip_wnd_))
				{
					::SendMessage(tooltip_wnd_, TTM_SETTOOLINFO, 0, (LPARAM)&tool_tip_);
					::SendMessage(tooltip_wnd_, TTM_TRACKACTIVATE, TRUE, (LPARAM)&tool_tip_);
				}
			}
			return true;
		case WM_MOUSELEAVE:
			{
				if( tooltip_wnd_ != NULL ) ::SendMessage(tooltip_wnd_, TTM_TRACKACTIVATE, FALSE, (LPARAM) &tool_tip_);
				if( mouse_tracking_ ) {
					POINT pt = { 0 };
					RECT rcWnd = { 0 };
					::GetCursorPos(&pt);
					::GetWindowRect(paint_wnd_, &rcWnd);
					if( !::IsIconic(paint_wnd_) && ::GetActiveWindow() == paint_wnd_ && ::PtInRect(&rcWnd, pt) ) {
						if( ::SendMessage(paint_wnd_, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y)) == HTCLIENT ) {
							::ScreenToClient(paint_wnd_, &pt);
							::SendMessage(paint_wnd_, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
						}
						else 
							::SendMessage(paint_wnd_, WM_MOUSEMOVE, 0, (LPARAM)-1);
					}
					else 
						::SendMessage(paint_wnd_, WM_MOUSEMOVE, 0, (LPARAM)-1);
				}
				mouse_tracking_ = false;
			}
			break;
		case WM_MOUSEMOVE:
			{
				// Start tracking this entire window again...
				if( !mouse_tracking_ ) {
					TRACKMOUSEEVENT tme = { 0 };
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_HOVER | TME_LEAVE;
					tme.hwndTrack = paint_wnd_;
					tme.dwHoverTime = tooltip_wnd_ == NULL ? 400UL : (DWORD) ::SendMessage(tooltip_wnd_, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
					_TrackMouseEvent(&tme);
					mouse_tracking_ = true;
				}

				// Generate the appropriate mouse messages
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				// 是否移动
				bool bNeedDrag = true;
				if(last_mouse_pt_.x == pt.x && last_mouse_pt_.y == pt.y) {
					bNeedDrag = false;
				}
				// 记录鼠标位置
				last_mouse_pt_ = pt;
				Control* pControl = FindControl(pt);
				if( pControl != NULL && pControl->GetManager() != this ) break;

				// 拖拽事件
				if(bNeedDrag && drag_mode_ && wParam == MK_LBUTTON)
				{
					::ReleaseCapture();
					CIDropSource* pdsrc = new CIDropSource;
					if(pdsrc == NULL) return 0;
					pdsrc->AddRef();

					CIDataObject* pdobj = new CIDataObject(pdsrc);
					if(pdobj == NULL) return 0;
					pdobj->AddRef();

					FORMATETC fmtetc = {0};
					STGMEDIUM medium = {0};
					fmtetc.dwAspect = DVASPECT_CONTENT;
					fmtetc.lindex = -1;
					//////////////////////////////////////
					fmtetc.cfFormat = CF_BITMAP;
					fmtetc.tymed = TYMED_GDI;			
					medium.tymed = TYMED_GDI;
					HBITMAP hBitmap = (HBITMAP)OleDuplicateData(drag_bitmap_, fmtetc.cfFormat, NULL);
					medium.hBitmap = hBitmap;
					pdobj->SetData(&fmtetc,&medium,FALSE);
					//////////////////////////////////////
					BITMAP bmap;
					GetObject(hBitmap, sizeof(BITMAP), &bmap);
					RECT rc={0, 0, bmap.bmWidth, bmap.bmHeight};
					fmtetc.cfFormat = CF_ENHMETAFILE;
					fmtetc.tymed = TYMED_ENHMF;
					HDC hMetaDC = CreateEnhMetaFile(paint_dc_, NULL, NULL, NULL);
					HDC hdcMem = CreateCompatibleDC(paint_dc_);
					HGDIOBJ hOldBmp = ::SelectObject(hdcMem, hBitmap);
					::BitBlt(hMetaDC, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
					::SelectObject(hdcMem, hOldBmp);
					medium.hEnhMetaFile = CloseEnhMetaFile(hMetaDC);
					DeleteDC(hdcMem);
					medium.tymed = TYMED_ENHMF;
					pdobj->SetData(&fmtetc, &medium, TRUE);
					//////////////////////////////////////
					DragSourceHelper dragSrcHelper;
					POINT ptDrag = {0};
					ptDrag.x = bmap.bmWidth / 2;
					ptDrag.y = bmap.bmHeight / 2;
					dragSrcHelper.InitializeFromBitmap(hBitmap, ptDrag, rc, pdobj); //will own the bmp
					DWORD dwEffect;
					HRESULT hr = ::DoDragDrop(pdobj, pdsrc, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
					pdsrc->Release();
					pdobj->Release();
					drag_mode_ = false;
					break;
				}
				EventArgs event;
				event.mouse_point_ = pt;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.timestamp_ = ::GetTickCount();
				if( pControl != event_hover_ && event_hover_ != NULL ) {
					event.type_ = kEventMouseLeave;
					event.sender_ = event_hover_;
					event_hover_->Event(event);
					event_hover_ = NULL;
					if( tooltip_wnd_ != NULL ) ::SendMessage(tooltip_wnd_, TTM_TRACKACTIVATE, FALSE, (LPARAM) &tool_tip_);
				}
				if( pControl != event_hover_ && pControl != NULL ) {
					event.type_ = kEventMouseEnter;
					event.sender_ = pControl;
					pControl->Event(event);
					event_hover_ = pControl;
				}
				if( event_click_ != NULL ) {
					event.type_ = kEventMouseMove;
					event.sender_ = event_click_;
					event_click_->Event(event);
				}
				else if( pControl != NULL ) {
					event.type_ = kEventMouseMove;
					event.sender_ = pControl;
					pControl->Event(event);
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				// We alway set focus back to our app (this helps
				// when Win32 child windows are placed on the dialog
				// and we need to remove them on focus change).
				::SetFocus(paint_wnd_);
				// 查找控件
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				last_mouse_pt_ = pt;
				Control* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;

				// 准备拖拽
				if(pControl->IsDragEnabled()) {
					drag_mode_ = true;
					if( drag_bitmap_ != NULL ) {
						::DeleteObject(drag_bitmap_);
						drag_bitmap_ = NULL;
					}
					drag_bitmap_ = RenderEngine::GenerateBitmap(this, pControl, pControl->GetPos());
				}

				// 开启捕获
				SetCapture();
				// 事件处理
				event_click_ = pControl;
				pControl->SetFocus();

				EventArgs event;
				event.type_ = kEventMouseButtonDown;
				event.sender_ = pControl;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.mouse_point_ = pt;
				event.key_state_ = (WORD)wParam;
				event.timestamp_ = ::GetTickCount();
				pControl->Event(event);
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				::SetFocus(paint_wnd_);
				SetCapture();
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				last_mouse_pt_ = pt;
				Control* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;

				EventArgs event;
				event.type_ = kEventMouseDoubleClick;
				event.sender_ = pControl;
				event.mouse_point_ = pt;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.key_state_ = (WORD)wParam;
				event.timestamp_ = ::GetTickCount();
				pControl->Event(event);
				event_click_ = pControl;
			}
			break;
		case WM_LBUTTONUP:
			{
				ReleaseCapture();
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				last_mouse_pt_ = pt;
				if( event_click_ == NULL ) break;

				EventArgs event;
				event.type_ = kEventMouseButtonUp;
				event.sender_ = event_click_;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.mouse_point_ = pt;
				event.key_state_ = (WORD)wParam;
				event.timestamp_ = ::GetTickCount();
				Control* pClick = event_click_;
				event_click_ = NULL;
				pClick->Event(event);
			}
			break;
		case WM_RBUTTONDOWN:
			{
				::SetFocus(paint_wnd_);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				last_mouse_pt_ = pt;
				Control* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				pControl->SetFocus();
				SetCapture();
				EventArgs event;
				event.type_ = kEventMouseRightButtonDown;
				event.sender_ = pControl;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.mouse_point_ = pt;
				event.key_state_ = (WORD)wParam;
				event.timestamp_ = ::GetTickCount();
				pControl->Event(event);
				event_click_ = pControl;
			}
			break;
		case WM_RBUTTONUP:
			{
				ReleaseCapture();

				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				last_mouse_pt_ = pt;
				event_click_ = FindControl(pt);
				if(event_click_ == NULL) break;
				EventArgs event;
				event.type_ = kEventMouseRightButtonUp;
				event.sender_ = event_click_;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.mouse_point_ = pt;
				event.key_state_ = (WORD)wParam;
				event.timestamp_ = ::GetTickCount();
				event_click_->Event(event);
			}
			break;
		case WM_MBUTTONDOWN:
			{
				::SetFocus(paint_wnd_);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				last_mouse_pt_ = pt;
				Control* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				pControl->SetFocus();
				SetCapture();
				EventArgs event;
				event.type_ = kEventMouseMiddleButtonDown;
				event.sender_ = pControl;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.mouse_point_ = pt;
				event.key_state_ = (WORD)wParam;
				event.timestamp_ = ::GetTickCount();
				pControl->Event(event);
				event_click_ = pControl;
			}
			break;
		case WM_MBUTTONUP:
			{
				ReleaseCapture();

				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				last_mouse_pt_ = pt;
				event_click_ = FindControl(pt);
				if(event_click_ == NULL) break;
				EventArgs event;
				event.type_ = kEventMouseMiddleButtonUp;
				event.sender_ = event_click_;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.mouse_point_ = pt;
				event.key_state_ = (WORD)wParam;
				event.timestamp_ = ::GetTickCount();
				event_click_->Event(event);
			}
			break;
		case WM_CONTEXTMENU:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ScreenToClient(paint_wnd_, &pt);
				last_mouse_pt_ = pt;
				if( event_click_ == NULL ) break;
				ReleaseCapture();
				EventArgs event;
				event.type_ = kEventMouseMenu;
				event.sender_ = event_click_;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.mouse_point_ = pt;
				event.key_state_ = (WORD)wParam;
				event.l_param_ = (LPARAM)event_click_;
				event.timestamp_ = ::GetTickCount();
				event_click_->Event(event);
				event_click_ = NULL;
			}
			break;
		case WM_MOUSEWHEEL:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ScreenToClient(paint_wnd_, &pt);
				last_mouse_pt_ = pt;
				Control* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				int zDelta = (int) (short) HIWORD(wParam);
				EventArgs event;
				event.type_ = kMouseScrollWheel;
				event.sender_ = pControl;
				event.w_param_ = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
				event.l_param_ = lParam;
				event.mouse_point_ = pt;
				event.key_state_ = MapKeyState();
				event.timestamp_ = ::GetTickCount();
				pControl->Event(event);

				// Let's make sure that the scroll item below the cursor is the same as before...
				::SendMessage(paint_wnd_, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(last_mouse_pt_.x, last_mouse_pt_.y));
			}
			break;
		case WM_CHAR:
			{
				if( focus_ == NULL ) break;
				EventArgs event;
				event.type_ = kEventChar;
				event.sender_ = focus_;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.key_ = (TCHAR)wParam;
				event.mouse_point_ = last_mouse_pt_;
				event.key_state_ = MapKeyState();
				event.timestamp_ = ::GetTickCount();
				focus_->Event(event);
			}
			break;
		case WM_KEYDOWN:
			{
				if( focus_ == NULL ) break;
				EventArgs event;
				event.type_ = kEventKeyDown;
				event.sender_ = focus_;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.key_ = (TCHAR)wParam;
				event.mouse_point_ = last_mouse_pt_;
				event.key_state_ = MapKeyState();
				event.timestamp_ = ::GetTickCount();
				focus_->Event(event);
				event_key_ = focus_;
			}
			break;
		case WM_KEYUP:
			{
				if( event_key_ == NULL ) break;
				EventArgs event;
				event.type_ = kEventKeyUp;
				event.sender_ = event_key_;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.key_ = (TCHAR)wParam;
				event.mouse_point_ = last_mouse_pt_;
				event.key_state_ = MapKeyState();
				event.timestamp_ = ::GetTickCount();
				event_key_->Event(event);
				event_key_ = NULL;
			}
			break;
		case WM_SETCURSOR:
			{
				if( LOWORD(lParam) != HTCLIENT ) break;
				if( mouse_captured_ ) return true;

				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(paint_wnd_, &pt);
				Control* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
				EventArgs event;
				event.type_ = kEventSetCursor;
				event.sender_ = pControl;
				event.w_param_ = wParam;
				event.l_param_ = lParam;
				event.mouse_point_ = pt;
				event.key_state_ = MapKeyState();
				event.timestamp_ = ::GetTickCount();
				pControl->Event(event);
			}
			return true;
		case WM_SETFOCUS:
			{
				if( focus_ != NULL) {
					EventArgs event ;
					event.type_ = kEventSetFocus;
					event.w_param_ = wParam;
					event.l_param_ = lParam;
					event.sender_ = focus_;
					event.timestamp_ = ::GetTickCount();
					//focus_->Event(event);
				}
				break;
			}
		case WM_NOTIFY:
			{
				if( lParam == 0 ) break;
				LPNMHDR lpNMHDR = (LPNMHDR) lParam;
				if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
				return true;
			}
			break;
		case WM_COMMAND:
			{
				if( lParam == 0 ) break;
				HWND hWndChild = (HWND) lParam;
				lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
				if(lRes != 0) return true;
			}
			break;
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
			{
				if( lParam == 0 ) break;
				HWND hWndChild = (HWND) lParam;
				lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
				if(lRes != 0) return true;
			}
			break;
		default:
			break;
		}
		return false;
	}

	bool PaintManager::IsUpdateNeeded() const
	{
		return need_update_;
	}

	void PaintManager::NeedUpdate()
	{
		need_update_ = true;
	}

	void PaintManager::Invalidate()
	{
		RECT rcClient = { 0 };
		::GetClientRect(paint_wnd_, &rcClient);
		::UnionRect(&layered_update_rect_, &layered_update_rect_, &rcClient);
		::InvalidateRect(paint_wnd_, NULL, FALSE);
	}

	void PaintManager::Invalidate(RECT& rcItem)
	{
		if( rcItem.left < 0 ) rcItem.left = 0;
		if( rcItem .top < 0 ) rcItem.top = 0;
		if( rcItem.right < rcItem.left ) rcItem.right = rcItem.left;
		if( rcItem.bottom < rcItem.top ) rcItem.bottom = rcItem.top;
		::UnionRect(&layered_update_rect_, &layered_update_rect_, &rcItem);
		::InvalidateRect(paint_wnd_, &rcItem, FALSE);
	}

	bool PaintManager::AttachDialog(Control* pControl)
	{
		ASSERT(::IsWindow(paint_wnd_));
		// 创建阴影窗口
		shadow_.Create(this);

		// Reset any previous attachment
		SetFocus(NULL);
		event_key_ = NULL;
		event_hover_ = NULL;
		event_click_ = NULL;
		// Remove the existing control-tree. We might have gotten inside this function as
		// a result of an event fired or similar, so we cannot just delete the objects and
		// pull the internal memory of the calling code. We'll delay the cleanup.
		if( root_ != NULL ) {
			post_paint_controls_.Empty();
			AddDelayedCleanup(root_);
		}
		// Set the dialog root element
		root_ = pControl;
		// Go ahead...
		need_update_ = true;
		is_first_layout_ = true;
		need_focus_ = true;
		// Initiate all control
		return InitControls(pControl);
	}

	bool PaintManager::InitControls(Control* pControl, Control* pParent /*= NULL*/)
	{
		ASSERT(pControl);
		if( pControl == NULL ) return false;
		pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent(), true);
		pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
		return true;
	}

	void PaintManager::ReapObjects(Control* pControl)
	{
		if( pControl == event_key_ ) event_key_ = NULL;
		if( pControl == event_hover_ ) event_hover_ = NULL;
		if( pControl == event_click_ ) event_click_ = NULL;
		if( pControl == focus_ ) focus_ = NULL;
		KillTimer(pControl);
		std::wstring sName = pControl->GetName();
		if( !sName.empty() ) {
			if( pControl == FindControl(sName.c_str()) ) name_hash_.Remove(sName);
		}
	}

	bool PaintManager::AddOptionGroup(std::wstring pStrGroupName, Control* pControl)
	{
		LPVOID lp = option_group_.Find(pStrGroupName);
		if( lp ) {
			CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<Control*>(aOptionGroup->GetAt(i)) == pControl ) {
					return false;
				}
			}
			aOptionGroup->Add(pControl);
		}
		else {
			CStdPtrArray* aOptionGroup = new CStdPtrArray(6);
			aOptionGroup->Add(pControl);
			option_group_.Insert(pStrGroupName, aOptionGroup);
		}
		return true;
	}

	CStdPtrArray* PaintManager::GetOptionGroup(std::wstring pStrGroupName)
	{
		LPVOID lp = option_group_.Find(pStrGroupName);
		if( lp ) return static_cast<CStdPtrArray*>(lp);
		return NULL;
	}

	void PaintManager::RemoveOptionGroup(std::wstring pStrGroupName, Control* pControl)
	{
		LPVOID lp = option_group_.Find(pStrGroupName);
		if( lp ) {
			CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
			if( aOptionGroup == NULL ) return;
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<Control*>(aOptionGroup->GetAt(i)) == pControl ) {
					aOptionGroup->Remove(i);
					break;
				}
			}
			if( aOptionGroup->IsEmpty() ) {
				delete aOptionGroup;
				option_group_.Remove(pStrGroupName);
			}
		}
	}

	void PaintManager::RemoveAllOptionGroups()
	{
		CStdPtrArray* aOptionGroup;
		for( int i = 0; i< option_group_.GetSize(); i++ ) {
			std::wstring key = option_group_.GetAt(i);
			if(!key.empty()) {
				aOptionGroup = static_cast<CStdPtrArray*>(option_group_.Find(key));
				delete aOptionGroup;
			}
		}
		option_group_.RemoveAll();
	}

	void PaintManager::MessageLoop()
	{
		MSG msg = { 0 };
		while( ::GetMessage(&msg, NULL, 0, 0) ) {
			if( !PaintManager::TranslateMessage(&msg) ) {
				::TranslateMessage(&msg);
				try{
					::DispatchMessage(&msg);
				} catch(...) {
					// TODO:
				}
			}
		}
	}

	void PaintManager::Term()
	{
		if( resource_zip_cached_ && resource_zip_handle_ != NULL ) {
			CloseZip((HZIP)resource_zip_handle_);
			resource_zip_handle_ = NULL;
		}
	}

	CDPI * ui::PaintManager::GetDPIObj()
	{
		if (DPI_ == NULL) {
			DPI_ = new CDPI;
		}
		return DPI_;
	}

	void ui::PaintManager::SetDPI(int iDPI)
	{
		int scale1 = GetDPIObj()->GetScale();
		GetDPIObj()->SetScale(iDPI);
		int scale2 = GetDPIObj()->GetScale();
		ResetDPIAssets();
		RECT rcWnd = {0};
		::GetWindowRect(GetPaintWindow(), &rcWnd);
		RECT*  prcNewWindow = &rcWnd;
		if (!::IsZoomed(GetPaintWindow())) {
			RECT rc = rcWnd;
			rc.right = rcWnd.left + (rcWnd.right - rcWnd.left) * scale2 / scale1;
			rc.bottom = rcWnd.top + (rcWnd.bottom - rcWnd.top) * scale2 / scale1;
			prcNewWindow = &rc;
		}
		SetWindowPos(GetPaintWindow(), NULL, prcNewWindow->left, prcNewWindow->top, prcNewWindow->right - prcNewWindow->left, prcNewWindow->bottom - prcNewWindow->top, SWP_NOZORDER | SWP_NOACTIVATE);
		if (GetRoot() != NULL) GetRoot()->NeedUpdate();
		::PostMessage(GetPaintWindow(), WM_USER_SET_DPI, 0, 0);
	}

	void ui::PaintManager::SetAllDPI(int iDPI)
	{
		for (int i = 0; i < pre_messages_.GetSize(); i++) {
			PaintManager* pManager = static_cast<PaintManager*>(pre_messages_[i]);
			pManager->SetDPI(iDPI);
		}
	}

	void ui::PaintManager::ResetDPIAssets()
	{
		RemoveAllDrawInfos();
		RemoveAllImages();;
		
		for (int it = 0; it < res_info_.m_CustomFonts.GetSize(); it++) {
			TFontInfo* pFontInfo = static_cast<TFontInfo*>(res_info_.m_CustomFonts.Find(res_info_.m_CustomFonts[it]));
			RebuildFont(pFontInfo);
		}
		RebuildFont(&res_info_.m_DefaultFontInfo);

		for (int it = 0; it < shared_res_info_.m_CustomFonts.GetSize(); it++) {
			TFontInfo* pFontInfo = static_cast<TFontInfo*>(shared_res_info_.m_CustomFonts.Find(shared_res_info_.m_CustomFonts[it]));
			RebuildFont(pFontInfo);
		}
		RebuildFont(&shared_res_info_.m_DefaultFontInfo);

		CStdPtrArray *richEditList = FindSubControlsByClass(GetRoot(), L"RichEdit");
		for (int i = 0; i < richEditList->GetSize(); i++)
		{
			RichEdit* pT = static_cast<RichEdit*>((*richEditList)[i]);
			pT->SetFont(pT->GetFont());

		}
	}

	void ui::PaintManager::RebuildFont(TFontInfo * pFontInfo)
	{
		::DeleteObject(pFontInfo->handle_);
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		_tcsncpy(lf.lfFaceName, pFontInfo->name_.c_str(), LF_FACESIZE);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -GetDPIObj()->Scale(pFontInfo->size_);
		lf.lfQuality = CLEARTYPE_QUALITY;
		if (pFontInfo->bold_) lf.lfWeight += FW_BOLD;
		if (pFontInfo->underline_) lf.lfUnderline = TRUE;
		if (pFontInfo->italic_) lf.lfItalic = TRUE;
		HFONT hFont = ::CreateFontIndirect(&lf);
		pFontInfo->handle_ = hFont;
		::ZeroMemory(&(pFontInfo->tm_), sizeof(pFontInfo->tm_));
		if (paint_dc_) {
			HFONT hOldFont = (HFONT) ::SelectObject(paint_dc_, hFont);
			::GetTextMetrics(paint_dc_, &pFontInfo->tm_);
			::SelectObject(paint_dc_, hOldFont);
		}
	}

	Control* PaintManager::GetFocus() const
	{
		return focus_;
	}

	void PaintManager::SetFocus(Control* pControl)
	{
		// Paint manager window has focus?
		HWND hFocusWnd = ::GetFocus();
		if( hFocusWnd != paint_wnd_ && pControl != focus_ ) ::SetFocus(paint_wnd_);
		// Already has focus?
		if( pControl == focus_ ) return;
		// Remove focus from old control
		if( focus_ != NULL ) 
		{
			if (focus_->IsFocused()) {
				EventArgs event;
				event.type_ = kEventKillFocus;
				event.sender_ = pControl;
				event.timestamp_ = ::GetTickCount();
				focus_->Event(event);
				//SendNotify(focus_, DUI_MSGTYPE_KILLFOCUS);
				focus_ = NULL;
			}
			
		}
		if( pControl == NULL ) return;
		// Set focus to new control
		if( pControl != NULL 
			&& pControl->GetManager() == this 
			&& pControl->IsVisible() 
			&& pControl->IsEnabled() ) 
		{
			focus_ = pControl;
			EventArgs event;
			event.type_ = kEventSetFocus;
			event.sender_ = pControl;
			event.timestamp_ = ::GetTickCount();
			focus_->Event(event);
			//SendNotify(focus_, DUI_MSGTYPE_SETFOCUS);
		}
	}

	void PaintManager::SetFocusNeeded(Control* pControl)
	{
		::SetFocus(paint_wnd_);
		if( pControl == NULL ) return;
		if( focus_ != NULL ) {
			EventArgs event;
			event.type_ = kEventKillFocus;
			event.sender_ = pControl;
			event.timestamp_ = ::GetTickCount();
			focus_->Event(event);
			focus_ = NULL;
		}
		FINDTABINFO info = { 0 };
		info.pFocus = pControl;
		info.bForward = false;
		focus_ = root_->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		need_focus_ = true;
		if( root_ != NULL ) root_->NeedUpdate();
	}

	bool PaintManager::SetTimer(Control* pControl, UINT nTimerID, UINT uElapse)
	{
		ASSERT(pControl!=NULL);
		ASSERT(uElapse>0);
		for( int i = 0; i< timers_.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(timers_[i]);
			if( pTimer->pSender == pControl
				&& pTimer->hWnd == paint_wnd_
				&& pTimer->nLocalID == nTimerID ) {
					if( pTimer->bKilled == true ) {
						if( ::SetTimer(paint_wnd_, pTimer->uWinTimer, uElapse, NULL) ) {
							pTimer->bKilled = false;
							return true;
						}
						return false;
					}
					return false;
			}
		}

		timer_id_ = (++timer_id_) % 0xF0; //0xf1-0xfe特殊用途
		if( !::SetTimer(paint_wnd_, timer_id_, uElapse, NULL) ) return FALSE;
		TIMERINFO* pTimer = new TIMERINFO;
		if( pTimer == NULL ) return FALSE;
		pTimer->hWnd = paint_wnd_;
		pTimer->pSender = pControl;
		pTimer->nLocalID = nTimerID;
		pTimer->uWinTimer = timer_id_;
		pTimer->bKilled = false;
		return timers_.Add(pTimer);
	}

	bool PaintManager::KillTimer(Control* pControl, UINT nTimerID)
	{
		ASSERT(pControl!=NULL);
		for( int i = 0; i< timers_.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(timers_[i]);
			if( pTimer->pSender == pControl
				&& pTimer->hWnd == paint_wnd_
				&& pTimer->nLocalID == nTimerID )
			{
				if( pTimer->bKilled == false ) {
					if( ::IsWindow(paint_wnd_) ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
					pTimer->bKilled = true;
					return true;
				}
			}
		}
		return false;
	}

	void PaintManager::KillTimer(Control* pControl)
	{
		ASSERT(pControl!=NULL);
		int count = timers_.GetSize();
		for( int i = 0, j = 0; i < count; i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(timers_[i - j]);
			if( pTimer->pSender == pControl && pTimer->hWnd == paint_wnd_ ) {
				if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
				delete pTimer;
				timers_.Remove(i - j);
				j++;
			}
		}
	}

	void PaintManager::RemoveAllTimers()
	{
		for( int i = 0; i < timers_.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(timers_[i]);
			if( pTimer->hWnd == paint_wnd_ ) {
				if( pTimer->bKilled == false ) {
					if( ::IsWindow(paint_wnd_) ) ::KillTimer(paint_wnd_, pTimer->uWinTimer);
				}
				delete pTimer;
			}
		}

		timers_.Empty();
	}

	void PaintManager::SetCapture()
	{
		::SetCapture(paint_wnd_);
		mouse_captured_ = true;
	}

	void PaintManager::ReleaseCapture()
	{
		::ReleaseCapture();
		mouse_captured_ = false;
		drag_mode_ = false;
	}

	bool PaintManager::IsCaptured()
	{
		return mouse_captured_;
	}

	bool PaintManager::IsPainting()
	{
		return is_painting_;
	}

	void PaintManager::SetPainting(bool bIsPainting)
	{
		is_painting_ = bIsPainting;
	}

	bool PaintManager::SetNextTabControl(bool bForward)
	{
		// If we're in the process of restructuring the layout we can delay the
		// focus calulation until the next repaint.
		if( need_update_ && bForward ) {
			need_focus_ = true;
			::InvalidateRect(paint_wnd_, NULL, FALSE);
			return true;
		}
		// Find next/previous tabbable control
		FINDTABINFO info1 = { 0 };
		info1.pFocus = focus_;
		info1.bForward = bForward;
		Control* pControl = root_->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		if( pControl == NULL ) {  
			if( bForward ) {
				// Wrap around
				FINDTABINFO info2 = { 0 };
				info2.pFocus = bForward ? NULL : info1.pLast;
				info2.bForward = bForward;
				pControl = root_->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
			}
			else {
				pControl = info1.pLast;
			}
		}
		if( pControl != NULL ) SetFocus(pControl);
		need_focus_ = false;
		return true;
	}

	bool PaintManager::AddPreMessageFilter(IMessageFilterUI* pFilter)
	{
		ASSERT(pre_message_filters_.Find(pFilter)<0);
		return pre_message_filters_.Add(pFilter);
	}

	bool PaintManager::RemovePreMessageFilter(IMessageFilterUI* pFilter)
	{
		for( int i = 0; i < pre_message_filters_.GetSize(); i++ ) {
			if( static_cast<IMessageFilterUI*>(pre_message_filters_[i]) == pFilter ) {
				return pre_message_filters_.Remove(i);
			}
		}
		return false;
	}

	bool PaintManager::AddMessageFilter(IMessageFilterUI* pFilter)
	{
		ASSERT(message_filters_.Find(pFilter)<0);
		return message_filters_.Add(pFilter);
	}

	bool PaintManager::RemoveMessageFilter(IMessageFilterUI* pFilter)
	{
		for( int i = 0; i < message_filters_.GetSize(); i++ ) {
			if( static_cast<IMessageFilterUI*>(message_filters_[i]) == pFilter ) {
				return message_filters_.Remove(i);
			}
		}
		return false;
	}

	int PaintManager::GetPostPaintCount() const
	{
		return post_paint_controls_.GetSize();
	}

	bool PaintManager::IsPostPaint(Control* pControl)
	{
		return post_paint_controls_.Find(pControl) >= 0;
	}

	bool PaintManager::AddPostPaint(Control* pControl)
	{
		ASSERT(post_paint_controls_.Find(pControl) < 0);
		return post_paint_controls_.Add(pControl);
	}

	bool PaintManager::RemovePostPaint(Control* pControl)
	{
		for( int i = 0; i < post_paint_controls_.GetSize(); i++ ) {
			if( static_cast<Control*>(post_paint_controls_[i]) == pControl ) {
				return post_paint_controls_.Remove(i);
			}
		}
		return false;
	}

	bool PaintManager::SetPostPaintIndex(Control* pControl, int iIndex)
	{
		RemovePostPaint(pControl);
		return post_paint_controls_.InsertAt(iIndex, pControl);
	}

	int PaintManager::GetPaintChildWndCount() const
	{
		return child_wnds_.GetSize();
	}

	bool PaintManager::AddPaintChildWnd(HWND hChildWnd)
	{
		RECT rcChildWnd;
		GetChildWndRect(paint_wnd_, hChildWnd, rcChildWnd);
		Invalidate(rcChildWnd);

		if (child_wnds_.Find(hChildWnd) >= 0) return false;
		return child_wnds_.Add(hChildWnd);
	}

	bool PaintManager::RemovePaintChildWnd(HWND hChildWnd)
	{
		for( int i = 0; i < child_wnds_.GetSize(); i++ ) {
			if( static_cast<HWND>(child_wnds_[i]) == hChildWnd ) {
				return child_wnds_.Remove(i);
			}
		}
		return false;
	}

	void PaintManager::AddDelayedCleanup(Control* pControl)
	{
		pControl->SetManager(this, NULL, false);
		delayed_cleanup_.Add(pControl);
		::PostMessage(paint_wnd_, WM_APP + 1, 0, 0L);
	}

	bool PaintManager::IsForceUseSharedRes() const
	{
		return use_forced_shared_res_;
	}

	void PaintManager::SetForceUseSharedRes(bool bForce)
	{
		use_forced_shared_res_ = bForce;
	}

	const TImageInfo* PaintManager::GetImage(std::wstring bitmap)
	{
		TImageInfo* data = static_cast<TImageInfo*>(res_info_.m_ImageHash.Find(bitmap));
		if (!data) data = static_cast<TImageInfo*>(shared_res_info_.m_ImageHash.Find(bitmap));
		return data;
	}

	const TImageInfo* PaintManager::GetImageEx(std::wstring bitmap, std::wstring type, DWORD mask, bool bUseHSL, HINSTANCE instance)
	{
		const TImageInfo* data = GetImage(bitmap);
		if (!data) {
			if (AddImage(bitmap, type, mask, bUseHSL, false, instance)) {
				if (use_forced_shared_res_) data = static_cast<TImageInfo*>(shared_res_info_.m_ImageHash.Find(bitmap));
				else data = static_cast<TImageInfo*>(res_info_.m_ImageHash.Find(bitmap));
			}
		}

		return data;
	}

	const TImageInfo* PaintManager::AddImage(std::wstring bitmap, std::wstring type, DWORD mask, bool bUseHSL, bool bShared, HINSTANCE instance)
	{
		if (bitmap.empty()) return NULL;

		TImageInfo* data = NULL;
		if (!type.empty()) {
			int iIndex;
			if (nbase::StringToInt(bitmap, &iIndex)) {
				data = RenderEngine::LoadImage(iIndex, type, mask, instance);
			}
		}
		else {
			data = RenderEngine::LoadImage(bitmap, L"", mask, instance);
		}

		if (data == NULL) {
			return NULL;
		}

		data->bUseHSL = bUseHSL;
		if (!type.empty()) { 
			data->sResType = type; 
		}

		data->dwMask = mask;
		if (data->bUseHSL) {
			data->pSrcBits = new BYTE[data->nX * data->nY * 4];
			::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
		}
		else { 
			data->pSrcBits = NULL; 
		}

		if (use_hsl_) { 
			RenderEngine::AdjustImage(true, data, H_, S_, L_); 
		}

		if (data){
			if (bShared || use_forced_shared_res_){
				TImageInfo* pOldImageInfo = static_cast<TImageInfo*>(shared_res_info_.m_ImageHash.Find(bitmap));
				if (pOldImageInfo){
					RenderEngine::FreeImage(pOldImageInfo);
					shared_res_info_.m_ImageHash.Remove(bitmap);
				}

				if (!shared_res_info_.m_ImageHash.Insert(bitmap, data)) {
					RenderEngine::FreeImage(data);
					data = NULL;
				}
			}
			else{
				TImageInfo* pOldImageInfo = static_cast<TImageInfo*>(res_info_.m_ImageHash.Find(bitmap));
				if (pOldImageInfo)
				{
					RenderEngine::FreeImage(pOldImageInfo);
					res_info_.m_ImageHash.Remove(bitmap);
				}

				if (!res_info_.m_ImageHash.Insert(bitmap, data)) {
					RenderEngine::FreeImage(data);
					data = NULL;
				}
			}
		}

		return data;
	}

	const TImageInfo* PaintManager::AddImage(std::wstring bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha, bool bShared)
	{
		// 因无法确定外部HBITMAP格式，不能使用hsl调整
		if( bitmap.empty()) return NULL;
		if( hBitmap == NULL || iWidth <= 0 || iHeight <= 0 ) return NULL;

		TImageInfo* data = new TImageInfo;
		data->pBits = NULL;
		data->pSrcBits = NULL;
		data->hBitmap = hBitmap;
		data->pBits = NULL;
		data->nX = iWidth;
		data->nY = iHeight;
		data->bAlpha = bAlpha;
		data->bUseHSL = false;
		data->pSrcBits = NULL;
		data->dwMask = 0;

		if (bShared || use_forced_shared_res_)
		{
			if( !shared_res_info_.m_ImageHash.Insert(bitmap, data) ) {
				RenderEngine::FreeImage(data);
				data = NULL;
			}
		}
		else
		{
			if( !res_info_.m_ImageHash.Insert(bitmap, data) ) {
				RenderEngine::FreeImage(data);
				data = NULL;
			}
		}

		return data;
	}

	void PaintManager::RemoveImage(std::wstring bitmap, bool bShared)
	{
		TImageInfo* data = NULL;
		if (bShared) 
		{
			data = static_cast<TImageInfo*>(shared_res_info_.m_ImageHash.Find(bitmap));
			if (data)
			{
				RenderEngine::FreeImage(data) ;
				shared_res_info_.m_ImageHash.Remove(bitmap);
			}
		}
		else
		{
			data = static_cast<TImageInfo*>(res_info_.m_ImageHash.Find(bitmap));
			if (data)
			{
				RenderEngine::FreeImage(data) ;
				res_info_.m_ImageHash.Remove(bitmap);
			}
		}
	}

	void PaintManager::RemoveAllImages(bool bShared)
	{
		if (bShared)
		{
			TImageInfo* data;
			for( int i = 0; i< shared_res_info_.m_ImageHash.GetSize(); i++ ) {
				std::wstring key = shared_res_info_.m_ImageHash.GetAt(i);
				if(!key.empty()) {
					data = static_cast<TImageInfo*>(shared_res_info_.m_ImageHash.Find(key, false));
					if (data) {
						RenderEngine::FreeImage(data);
					}
				}
			}
			shared_res_info_.m_ImageHash.RemoveAll();
		}
		else
		{
			TImageInfo* data;
			for( int i = 0; i< res_info_.m_ImageHash.GetSize(); i++ ) {
				std::wstring key = res_info_.m_ImageHash.GetAt(i);
				if(!key.empty()) {
					data = static_cast<TImageInfo*>(res_info_.m_ImageHash.Find(key, false));
					if (data) {
						RenderEngine::FreeImage(data);
					}
				}
			}
			res_info_.m_ImageHash.RemoveAll();
		}
	}

	void PaintManager::AdjustSharedImagesHSL()
	{
		TImageInfo* data;
		for( int i = 0; i< shared_res_info_.m_ImageHash.GetSize(); i++ ) {
			std::wstring key = shared_res_info_.m_ImageHash.GetAt(i);
			if(!key.empty()) {
				data = static_cast<TImageInfo*>(shared_res_info_.m_ImageHash.Find(key));
				if( data && data->bUseHSL ) {
					RenderEngine::AdjustImage(use_hsl_, data, H_, S_, L_);
				}
			}
		}
	}

	void PaintManager::AdjustImagesHSL()
	{
		TImageInfo* data;
		for( int i = 0; i< res_info_.m_ImageHash.GetSize(); i++ ) {
			std::wstring key = res_info_.m_ImageHash.GetAt(i);
			if(!key.empty()) {
				data = static_cast<TImageInfo*>(res_info_.m_ImageHash.Find(key));
				if( data && data->bUseHSL ) {
					RenderEngine::AdjustImage(use_hsl_, data, H_, S_, L_);
				}
			}
		}
		Invalidate();
	}

	void PaintManager::ReloadSharedImages()
	{
		TImageInfo* data;
		TImageInfo* pNewData;
		for( int i = 0; i< shared_res_info_.m_ImageHash.GetSize(); i++ ) {
			std::wstring bitmap = shared_res_info_.m_ImageHash.GetAt(i);
			if(!bitmap.empty()) {
				data = static_cast<TImageInfo*>(shared_res_info_.m_ImageHash.Find(bitmap));
				if( data != NULL ) {
					if( !data->sResType.empty() ) {
						int iIndex;
						if(nbase::StringToInt(bitmap, &iIndex)) {
							pNewData = RenderEngine::LoadImage(iIndex, data->sResType, data->dwMask);
						}
					}
					else {
						pNewData = RenderEngine::LoadImage(bitmap, L"", data->dwMask);
					}
					if( pNewData == NULL ) continue;

					RenderEngine::FreeImage(data, false);
					data->hBitmap = pNewData->hBitmap;
					data->pBits = pNewData->pBits;
					data->nX = pNewData->nX;
					data->nY = pNewData->nY;
					data->bAlpha = pNewData->bAlpha;
					data->pSrcBits = NULL;
					if( data->bUseHSL ) {
						data->pSrcBits = new BYTE[data->nX * data->nY * 4];
						::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
					}
					else data->pSrcBits = NULL;
					if( use_hsl_ ) RenderEngine::AdjustImage(true, data, H_, S_, L_);

					delete pNewData;
				}
			}
		}
	}

	void PaintManager::ReloadImages()
	{
		RemoveAllDrawInfos();

		TImageInfo* data;
		TImageInfo* pNewData;
		for( int i = 0; i< res_info_.m_ImageHash.GetSize(); i++ ) {
			std::wstring bitmap = res_info_.m_ImageHash.GetAt(i);
			if(!bitmap.empty()) {
				data = static_cast<TImageInfo*>(res_info_.m_ImageHash.Find(bitmap));
				if( data != NULL ) {
					if( !data->sResType.empty() ) {
						int iIndex;
						if( nbase::StringToInt(bitmap, &iIndex)) {
							pNewData = RenderEngine::LoadImage(iIndex, data->sResType.c_str(), data->dwMask);
						}
					}
					else {
						pNewData = RenderEngine::LoadImage(bitmap, L"", data->dwMask);
					}

					RenderEngine::FreeImage(data, false);
					if( pNewData == NULL ) {
						res_info_.m_ImageHash.Remove(bitmap);
						continue;
					}
					data->hBitmap = pNewData->hBitmap;
					data->pBits = pNewData->pBits;
					data->nX = pNewData->nX;
					data->nY = pNewData->nY;
					data->bAlpha = pNewData->bAlpha;
					data->pSrcBits = NULL;
					if( data->bUseHSL ) {
						data->pSrcBits = new BYTE[data->nX * data->nY * 4];
						::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
					}
					else data->pSrcBits = NULL;
					if( use_hsl_ ) RenderEngine::AdjustImage(true, data, H_, S_, L_);

					delete pNewData;
				}
			}
		}

		if( root_ ) root_->Invalidate();
	}

	const TDrawInfo* PaintManager::GetDrawInfo(std::wstring pStrImage, std::wstring pStrModify)
	{
		std::wstring sStrImage = pStrImage;
		std::wstring sStrModify = pStrModify;
		std::wstring sKey = sStrImage + sStrModify;
		TDrawInfo* pDrawInfo = static_cast<TDrawInfo*>(res_info_.m_DrawInfoHash.Find(sKey));
		if(pDrawInfo == NULL && !sKey.empty()) {
			pDrawInfo = new TDrawInfo();
			pDrawInfo->Parse(pStrImage, pStrModify,this);
			res_info_.m_DrawInfoHash.Insert(sKey, pDrawInfo);
		}
		return pDrawInfo;
	}

	void PaintManager::RemoveDrawInfo(std::wstring pStrImage, std::wstring pStrModify)
	{
		std::wstring sStrImage = pStrImage;
		std::wstring sStrModify = pStrModify;
		std::wstring sKey = sStrImage + sStrModify;
		TDrawInfo* pDrawInfo = static_cast<TDrawInfo*>(res_info_.m_DrawInfoHash.Find(sKey));
		if(pDrawInfo != NULL) {
			res_info_.m_DrawInfoHash.Remove(sKey);
			delete pDrawInfo;
			pDrawInfo = NULL;
		}
	}

	void PaintManager::RemoveAllDrawInfos()
	{
		TDrawInfo* pDrawInfo = NULL;
		for( int i = 0; i< res_info_.m_DrawInfoHash.GetSize(); i++ ) {
			std::wstring key = res_info_.m_DrawInfoHash.GetAt(i);
			if(!key.empty()) {
				pDrawInfo = static_cast<TDrawInfo*>(res_info_.m_DrawInfoHash.Find(key, false));
				if (pDrawInfo) {
					delete pDrawInfo;
					pDrawInfo = NULL;
				}
			}
		}
		res_info_.m_DrawInfoHash.RemoveAll();
	}

	void PaintManager::AddDefaultAttributeList(std::wstring pStrControlName, std::wstring pStrControlAttrList, bool bShared)
	{
		if (bShared || use_forced_shared_res_){
			std::wstring* pDefaultAttr = new std::wstring(pStrControlAttrList);
			if (pDefaultAttr != NULL)
			{
				std::wstring* pOldDefaultAttr = static_cast<std::wstring*>(shared_res_info_.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
				if (pOldDefaultAttr) delete pOldDefaultAttr;
			}
		}
		else{
			std::wstring* pDefaultAttr = new std::wstring(pStrControlAttrList);
			if (pDefaultAttr != NULL)
			{
				std::wstring* pOldDefaultAttr = static_cast<std::wstring*>(res_info_.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
				if (pOldDefaultAttr) delete pOldDefaultAttr;
			}
		}
	}

	std::wstring PaintManager::GetDefaultAttributeList(std::wstring pStrControlName) const
	{
		std::wstring* pDefaultAttr = static_cast<std::wstring*>(res_info_.m_AttrHash.Find(pStrControlName));
		if( !pDefaultAttr ) pDefaultAttr = static_cast<std::wstring*>(shared_res_info_.m_AttrHash.Find(pStrControlName));
		if (pDefaultAttr) return pDefaultAttr->c_str();
		return L"";
	}

	bool PaintManager::RemoveDefaultAttributeList(std::wstring pStrControlName, bool bShared)
	{
		if (bShared){
			std::wstring* pDefaultAttr = static_cast<std::wstring*>(shared_res_info_.m_AttrHash.Find(pStrControlName));
			if( !pDefaultAttr ) return false;

			delete pDefaultAttr;
			return shared_res_info_.m_AttrHash.Remove(pStrControlName);
		}
		else{
			std::wstring* pDefaultAttr = static_cast<std::wstring*>(res_info_.m_AttrHash.Find(pStrControlName));
			if( !pDefaultAttr ) return false;

			delete pDefaultAttr;
			return res_info_.m_AttrHash.Remove(pStrControlName);
		}
	}

	void PaintManager::RemoveAllDefaultAttributeList(bool bShared)
	{
		if (bShared)
		{
			std::wstring* pDefaultAttr;
			for( int i = 0; i< shared_res_info_.m_AttrHash.GetSize(); i++ ) {
				std::wstring key = shared_res_info_.m_AttrHash.GetAt(i);
				if(!key.empty()) {
					pDefaultAttr = static_cast<std::wstring*>(shared_res_info_.m_AttrHash.Find(key));
					if (pDefaultAttr) delete pDefaultAttr;
				}
			}
			shared_res_info_.m_AttrHash.RemoveAll();
		}
		else
		{
			std::wstring* pDefaultAttr;
			for( int i = 0; i< res_info_.m_AttrHash.GetSize(); i++ ) {
				std::wstring key = res_info_.m_AttrHash.GetAt(i);
				if(!key.empty()) {
					pDefaultAttr = static_cast<std::wstring*>(res_info_.m_AttrHash.Find(key));
					if (pDefaultAttr) delete pDefaultAttr;
				}
			}
			res_info_.m_AttrHash.RemoveAll();
		}
	}

	void PaintManager::AddWindowCustomAttribute(std::wstring pstrName, std::wstring pstrAttr)
	{
		if( pstrName.empty() || pstrAttr.empty()) return;
		std::wstring* pCostomAttr = new std::wstring(pstrAttr);
		if (pCostomAttr != NULL) {
			if (custom_wnd_attr_hash_.Find(pstrName) == NULL)
				custom_wnd_attr_hash_.Set(pstrName, (LPVOID)pCostomAttr);
			else
				delete pCostomAttr;
		}
	}

	std::wstring PaintManager::GetWindowCustomAttribute(std::wstring pstrName) const
	{
		if( pstrName.empty()) return L"";
		std::wstring* pCostomAttr = static_cast<std::wstring*>(custom_wnd_attr_hash_.Find(pstrName));
		if( pCostomAttr ) return pCostomAttr->c_str();
		return L"";
	}

	bool PaintManager::RemoveWindowCustomAttribute(std::wstring pstrName)
	{
		if( pstrName.empty()) return NULL;
		std::wstring* pCostomAttr = static_cast<std::wstring*>(custom_wnd_attr_hash_.Find(pstrName));
		if( !pCostomAttr ) return false;

		delete pCostomAttr;
		return custom_wnd_attr_hash_.Remove(pstrName);
	}

	void PaintManager::RemoveAllWindowCustomAttribute()
	{
		std::wstring* pCostomAttr;
		for( int i = 0; i< custom_wnd_attr_hash_.GetSize(); i++ ) {
			std::wstring key = custom_wnd_attr_hash_.GetAt(i);
			if(!key.empty()) {
				pCostomAttr = static_cast<std::wstring*>(custom_wnd_attr_hash_.Find(key));
				delete pCostomAttr;
			}
		}
		custom_wnd_attr_hash_.Resize();
	}

	Control* PaintManager::GetRoot() const
	{
		ASSERT(root_);
		return root_;
	}

	Control* PaintManager::FindControl(POINT pt) const
	{
		ASSERT(root_);
		return root_->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	}

	Control* PaintManager::FindControl(std::wstring pstrName) const
	{
		ASSERT(root_);
		return static_cast<Control*>(name_hash_.Find(pstrName));
	}

	Control* PaintManager::FindSubControlByPoint(Control* pParent, POINT pt) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	}

	Control* PaintManager::FindSubControlByName(Control* pParent, std::wstring pstrName) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName.c_str(), UIFIND_ALL);
	}

	Control* PaintManager::FindSubControlByClass(Control* pParent, std::wstring pstrClass, int iIndex)
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		found_controls_.Resize(iIndex + 1);
		return pParent->FindControl(__FindControlFromClass, (LPVOID)pstrClass.c_str(), UIFIND_ALL);
	}

	CStdPtrArray* PaintManager::FindSubControlsByClass(Control* pParent, std::wstring pstrClass)
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		found_controls_.Empty();
		pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass.c_str(), UIFIND_ALL);
		return &found_controls_;
	}

	Window * PaintManager::GetWindow()
	{
		return host_wnd_;
	}

	CStdPtrArray* PaintManager::GetFoundControls()
	{
		return &found_controls_;
	}

	Control* CALLBACK PaintManager::__FindControlFromNameHash(Control* pThis, LPVOID pData)
	{
		PaintManager* pManager = static_cast<PaintManager*>(pData);
		std::wstring sName = pThis->GetName();
		if( sName.empty() ) return NULL;
		// Add this control to the hash list
		pManager->name_hash_.Set(sName, pThis);
		return NULL; // Attempt to add all controls
	}

	Control* CALLBACK PaintManager::__FindControlFromCount(Control* /*pThis*/, LPVOID pData)
	{
		int* pnCount = static_cast<int*>(pData);
		(*pnCount)++;
		return NULL;  // Count all controls
	}

	Control* CALLBACK PaintManager::__FindControlFromPoint(Control* pThis, LPVOID pData)
	{
		LPPOINT pPoint = static_cast<LPPOINT>(pData);
		return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : NULL;
	}

	Control* CALLBACK PaintManager::__FindControlFromTab(Control* pThis, LPVOID pData)
	{
		FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
		if( pInfo->pFocus == pThis ) {
			if( pInfo->bForward ) pInfo->bNextIsIt = true;
			return pInfo->bForward ? NULL : pInfo->pLast;
		}
		if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
		pInfo->pLast = pThis;
		if( pInfo->bNextIsIt ) return pThis;
		if( pInfo->pFocus == NULL ) return pThis;
		return NULL;  // Examine all controls
	}

	Control* CALLBACK PaintManager::__FindControlFromShortcut(Control* pThis, LPVOID pData)
	{
		if( !pThis->IsVisible() ) return NULL; 
		FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
		if( pFS->ch == toupper(pThis->GetShortcut()) ) pFS->bPickNext = true;
		if( _tcsstr(pThis->GetClass().c_str(), _T("Label")) != NULL ) return NULL;   // Labels never get focus!
		return pFS->bPickNext ? pThis : NULL;
	}

	Control* CALLBACK PaintManager::__FindControlFromName(Control* pThis, LPVOID pData)
	{
		LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
		std::wstring sName = pThis->GetName();
		if( sName.empty() ) return NULL;
		return (_tcsicmp(sName.c_str(), pstrName) == 0) ? pThis : NULL;
	}

	Control* CALLBACK PaintManager::__FindControlFromClass(Control* pThis, LPVOID pData)
	{
		LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
		std::wstring pType = pThis->GetClass();
		CStdPtrArray* pFoundControls = pThis->GetManager()->GetFoundControls();
		if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType.c_str()) == 0 ) {
			int iIndex = -1;
			while( pFoundControls->GetAt(++iIndex) != NULL ) ;
			if( iIndex < pFoundControls->GetSize() ) pFoundControls->SetAt(iIndex, pThis);
		}
		if( pFoundControls->GetAt(pFoundControls->GetSize() - 1) != NULL ) return pThis; 
		return NULL;
	}

	Control* CALLBACK PaintManager::__FindControlsFromClass(Control* pThis, LPVOID pData)
	{
		LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
		std::wstring pType = pThis->GetClass();
		if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType.c_str()) == 0 ) 
			pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
		return NULL;
	}

	Control* CALLBACK PaintManager::__FindControlsFromUpdate(Control* pThis, LPVOID pData)
	{
		if( pThis->IsUpdateNeeded() ) {
			pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
			return pThis;
		}
		return NULL;
	}

	bool PaintManager::TranslateAccelerator(LPMSG pMsg)
	{
		for (int i = 0; i < translate_accelerator_.GetSize(); i++)
		{
			LRESULT lResult = static_cast<ITranslateAccelerator *>(translate_accelerator_[i])->TranslateAccelerator(pMsg);
			if( lResult == S_OK ) return true;
		}
		return false;
	}

	bool PaintManager::TranslateMessage(const LPMSG pMsg)
	{
		// Pretranslate Message takes care of system-wide messages, such as
		// tabbing and shortcut key-combos. We'll look for all messages for
		// each window and any child control attached.
		UINT uStyle = GetWindowStyle(pMsg->hwnd);
		UINT uChildRes = uStyle & WS_CHILD;	
		LRESULT lRes = 0;
		if (uChildRes != 0)
		{
			HWND hWndParent = ::GetParent(pMsg->hwnd);

			for( int i = 0; i < pre_messages_.GetSize(); i++ ) 
			{
				PaintManager* pT = static_cast<PaintManager*>(pre_messages_[i]);        
				HWND hTempParent = hWndParent;
				while(hTempParent)
				{
					if(pMsg->hwnd == pT->GetPaintWindow() || hTempParent == pT->GetPaintWindow())
					{
						if (pT->TranslateAccelerator(pMsg))
							return true;

						pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes);
					}
					hTempParent = GetParent(hTempParent);
				}
			}
		}
		else
		{
			for( int i = 0; i < pre_messages_.GetSize(); i++ ) 
			{
				PaintManager* pT = static_cast<PaintManager*>(pre_messages_[i]);
				if(pMsg->hwnd == pT->GetPaintWindow())
				{
					if (pT->TranslateAccelerator(pMsg))
						return true;

					if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
						return true;

					return false;
				}
			}
		}
		return false;
	}

	bool PaintManager::AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
	{
		ASSERT(translate_accelerator_.Find(pTranslateAccelerator) < 0);
		return translate_accelerator_.Add(pTranslateAccelerator);
	}

	bool PaintManager::RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
	{
		for (int i = 0; i < translate_accelerator_.GetSize(); i++)
		{
			if (static_cast<ITranslateAccelerator *>(translate_accelerator_[i]) == pTranslateAccelerator)
			{
				return translate_accelerator_.Remove(i);
			}
		}
		return false;
	}

	void PaintManager::UsedVirtualWnd(bool bUsed)
	{
		used_virtual_wnd_ = bUsed;
	}

	const TImageInfo* PaintManager::GetImageString(std::wstring strImage, std::wstring pStrModify)
	{
		auto sImageName = strImage.c_str();
		auto pStrImage = strImage.c_str();

		std::wstring sImageResType = _T("");
		DWORD dwMask = 0;
		std::wstring sItem;
		std::wstring sValue;
		LPTSTR pstr = NULL;

		for( int i = 0; i < 2; ++i) {
			if( i == 1)
				pStrImage = pStrModify.c_str();

			if( !pStrImage ) continue;

			while( *pStrImage != _T('\0') ) {
				sItem.clear();
				sValue.clear();
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
					LPTSTR pstrTemp = ::CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sItem += *pStrImage++;
					}
				}
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				if( *pStrImage++ != _T('=') ) break;
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				if( *pStrImage++ != _T('\'') ) break;
				while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
					LPTSTR pstrTemp = ::CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sValue += *pStrImage++;
					}
				}
				if( *pStrImage++ != _T('\'') ) break;
				if( !sValue.empty() ) {
					if( sItem == _T("file") || sItem == _T("res") ) {
						sImageName = sValue.c_str();
					}
					else if( sItem == _T("restype") ) {
						sImageResType = sValue;
					}
					else if( sItem == _T("mask") ) 
					{
						if( sValue[0] == _T('#')) dwMask = _tcstoul(sValue.c_str() + 1, &pstr, 16);
						else dwMask = _tcstoul(sValue.c_str(), &pstr, 16);
					}

				}
				if( *pStrImage++ != _T(' ') ) break;
			}
		}
		return GetImageEx(sImageName, sImageResType, dwMask);
	}

	bool PaintManager::InitDragDrop()
	{
		AddRef();

		if(FAILED(RegisterDragDrop(paint_wnd_, this))){
			DWORD dwError = GetLastError();
			return false;
		}
		else { 
			Release(); 
		}

		FORMATETC ftetc={0};
		ftetc.cfFormat = CF_BITMAP;
		ftetc.dwAspect = DVASPECT_CONTENT;
		ftetc.lindex = -1;
		ftetc.tymed = TYMED_GDI;
		AddSuportedFormat(ftetc);
		ftetc.cfFormat = CF_DIB;
		ftetc.tymed = TYMED_HGLOBAL;
		AddSuportedFormat(ftetc);
		ftetc.cfFormat = CF_HDROP;
		ftetc.tymed = TYMED_HGLOBAL;
		AddSuportedFormat(ftetc);
		ftetc.cfFormat = CF_ENHMETAFILE;
		ftetc.tymed = TYMED_ENHMF;
		AddSuportedFormat(ftetc);
		return true;
	}
	static WORD DIBNumColors(void* pv) 
	{     
		int bits;     
		LPBITMAPINFOHEADER  lpbi;     
		LPBITMAPCOREHEADER  lpbc;      
		lpbi = ((LPBITMAPINFOHEADER)pv);     
		lpbc = ((LPBITMAPCOREHEADER)pv);      
		/*  With the BITMAPINFO format headers, the size of the palette 
		*  is in biClrUsed, whereas in the BITMAPCORE - style headers, it      
		*  is dependent on the bits per pixel ( = 2 raised to the power of      
		*  bits/pixel).
		*/     
		if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
		{         
			if (lpbi->biClrUsed != 0)
				return (WORD)lpbi->biClrUsed;         
			bits = lpbi->biBitCount;     
		}     
		else         
			bits = lpbc->bcBitCount;
		switch (bits)
		{         
		case 1:                 
			return 2;         
		case 4:                 
			return 16;         
		case 8:       
			return 256;
		default:
			/* A 24 bitcount DIB has no color table */                 
			return 0;
		} 
	} 
	//code taken from SEEDIB MSDN sample
	static WORD ColorTableSize(LPVOID lpv)
	{
		LPBITMAPINFOHEADER lpbih = (LPBITMAPINFOHEADER)lpv;

		if (lpbih->biSize != sizeof(BITMAPCOREHEADER))
		{
			if (((LPBITMAPINFOHEADER)(lpbih))->biCompression == BI_BITFIELDS)
				/* Remember that 16/32bpp dibs can still have a color table */
				return (sizeof(DWORD) * 3) + (DIBNumColors (lpbih) * sizeof (RGBQUAD));
			else
				return (WORD)(DIBNumColors (lpbih) * sizeof (RGBQUAD));
		}
		else
			return (WORD)(DIBNumColors (lpbih) * sizeof (RGBTRIPLE));
	}

	bool PaintManager::OnDrop(FORMATETC* pFmtEtc, STGMEDIUM& medium,DWORD *pdwEffect)
	{
		POINT ptMouse = {0};
		GetCursorPos(&ptMouse);
		::SendMessage(m_hTargetWnd, WM_LBUTTONUP, NULL, MAKELPARAM(ptMouse.x, ptMouse.y));

		if(pFmtEtc->cfFormat == CF_DIB && medium.tymed == TYMED_HGLOBAL)
		{
			if(medium.hGlobal != NULL)
			{
				LPBITMAPINFOHEADER  lpbi = (BITMAPINFOHEADER*)GlobalLock(medium.hGlobal);
				if(lpbi != NULL)
				{
					HBITMAP hbm;
					HDC hdc = GetDC(NULL);
					if(hdc != NULL)
					{
						int i = ((BITMAPFILEHEADER *)lpbi)->bfOffBits;
						hbm = CreateDIBitmap(hdc,(LPBITMAPINFOHEADER)lpbi,
							(LONG)CBM_INIT,
							(LPSTR)lpbi + lpbi->biSize + ColorTableSize(lpbi),
							(LPBITMAPINFO)lpbi,DIB_RGB_COLORS);

						::ReleaseDC(NULL,hdc);
					}
					GlobalUnlock(medium.hGlobal);
					if(hbm != NULL)
						hbm = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm);
					if(hbm != NULL)
						DeleteObject(hbm);
					return true; //release the medium
				}
			}
		}
		if(pFmtEtc->cfFormat == CF_BITMAP && medium.tymed == TYMED_GDI)
		{
			if(medium.hBitmap != NULL)
			{
				HBITMAP hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)medium.hBitmap);
				if(hBmp != NULL)
					DeleteObject(hBmp);
				return false; //don't free the bitmap
			}
		}
		if(pFmtEtc->cfFormat == CF_ENHMETAFILE && medium.tymed == TYMED_ENHMF)
		{
			ENHMETAHEADER emh;
			GetEnhMetaFileHeader(medium.hEnhMetaFile, sizeof(ENHMETAHEADER),&emh);
			RECT rc;//={0,0,EnhMetaHdr.rclBounds.right-EnhMetaHdr.rclBounds.left, EnhMetaHdr.rclBounds.bottom-EnhMetaHdr.rclBounds.top};
			HDC hDC= GetDC(m_hTargetWnd);
			//start code: taken from ENHMETA.EXE MSDN Sample
			//*ALSO NEED to GET the pallete (select and RealizePalette it, but i was too lazy*
			// Get the characteristics of the output device
			float PixelsX = (float)GetDeviceCaps( hDC, HORZRES );
			float PixelsY = (float)GetDeviceCaps( hDC, VERTRES );
			float MMX = (float)GetDeviceCaps( hDC, HORZSIZE );
			float MMY = (float)GetDeviceCaps( hDC, VERTSIZE );
			// Calculate the rect in which to draw the metafile based on the
			// intended size and the current output device resolution
			// Remember that the intended size is given in 0.01mm units, so
			// convert those to device units on the target device
			rc.top = (int)((float)(emh.rclFrame.top) * PixelsY / (MMY*100.0f));
			rc.left = (int)((float)(emh.rclFrame.left) * PixelsX / (MMX*100.0f));
			rc.right = (int)((float)(emh.rclFrame.right) * PixelsX / (MMX*100.0f));
			rc.bottom = (int)((float)(emh.rclFrame.bottom) * PixelsY / (MMY*100.0f));
			//end code: taken from ENHMETA.EXE MSDN Sample

			HDC hdcMem = CreateCompatibleDC(hDC);
			HGDIOBJ hBmpMem = CreateCompatibleBitmap(hDC, emh.rclBounds.right, emh.rclBounds.bottom);
			HGDIOBJ hOldBmp = ::SelectObject(hdcMem, hBmpMem);
			PlayEnhMetaFile(hdcMem,medium.hEnhMetaFile,&rc);
			HBITMAP hBmp = (HBITMAP)::SelectObject(hdcMem, hOldBmp);
			DeleteDC(hdcMem);
			ReleaseDC(m_hTargetWnd,hDC);
			hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
			if(hBmp != NULL)
				DeleteObject(hBmp);
			return true;
		}
		if(pFmtEtc->cfFormat == CF_HDROP && medium.tymed == TYMED_HGLOBAL)
		{
			HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
			if(hDrop != NULL)
			{
				TCHAR szFileName[MAX_PATH];
				UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); 
				if(cFiles > 0)
				{
					DragQueryFile(hDrop, 0, szFileName, sizeof(szFileName)); 
					HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, szFileName,IMAGE_BITMAP,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);
					if(hBitmap)
					{
						HBITMAP hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
						if(hBmp != NULL)
							DeleteObject(hBmp);
					}
				}
				//DragFinish(hDrop); // base class calls ReleaseStgMedium
			}
			GlobalUnlock(medium.hGlobal);
		}
		return true; //let base free the medium
	}
} // namespace DuiLib
