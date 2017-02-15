#include "stdafx.h"
#include "HotKey.h"
namespace ui{
	HotKeyWnd::HotKeyWnd(void) : owner_(NULL), bk_brush_(NULL), is_init_(false)
	{
	}
	void HotKeyWnd::Init(HotKey * pOwner)
	{
		owner_ = pOwner;
		do 
		{
			if (NULL == owner_)
			{
				break;
			}
			RECT rcPos = CalPos();
			UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;
			HWND hWnd = Create(owner_->GetManager()->GetPaintWindow(), L"", uStyle, 0, rcPos);
			if (!IsWindow(hWnd))
			{
				break;
			}
			SetWindowFont(wnd_, GlobalManager::GetFontInfo(owner_->GetFont(), ::GetDC(wnd_))->handle_, TRUE);
			SetHotKey(owner_->virtual_keycode_, owner_->modifiers_);
			owner_->text_ = GetHotKeyName();
			::EnableWindow(wnd_, owner_->IsEnabled() == true);
			::ShowWindow(wnd_, SW_SHOWNOACTIVATE);
			::SetFocus(wnd_);
			is_init_ = true;   
		} while (0); 
	}


	RECT HotKeyWnd::CalPos()
	{
		CDuiRect rcPos = owner_->GetPos();
		RECT rcInset = owner_->GetTextPadding();
		rcPos.left += rcInset.left;
		rcPos.top += rcInset.top;
		rcPos.right -= rcInset.right;
		rcPos.bottom -= rcInset.bottom;
		LONG lHeight = GlobalManager::GetFontInfo(owner_->GetFont(), ::GetDC(wnd_))->tm_.tmHeight;
		if( lHeight < rcPos.GetHeight() ) {
			rcPos.top += (rcPos.GetHeight() - lHeight) / 2;
			rcPos.bottom = rcPos.top + lHeight;
		}
		return rcPos;
	}


	std::wstring HotKeyWnd::GetWindowClassName() const
	{
		return _T("HotKeyClass");
	}

	void HotKeyWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		// Clear reference and die
		if( bk_brush_ != NULL ) ::DeleteObject(bk_brush_);
		owner_->wnd_ = NULL;
		delete this;
	}

	LRESULT HotKeyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if( uMsg == WM_KILLFOCUS ) lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		else if( uMsg == OCM_COMMAND ) {
			if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
			else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
				RECT rcClient;
				::GetClientRect(wnd_, &rcClient);
				::InvalidateRect(wnd_, &rcClient, FALSE);
			}
		}
		else if( uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN ) {
			//owner_->GetManager()->SendNotify(owner_, _T("return"));
			owner_->Notify(kEventReturn);
		}
		else if ( (uMsg == WM_NCACTIVATE) || (uMsg == WM_NCACTIVATE) || (uMsg == WM_NCCALCSIZE) )
		{
			return 0;
		}
		else if (uMsg == WM_PAINT)
		{
			PAINTSTRUCT ps = { 0 };
			HDC hDC = ::BeginPaint(wnd_, &ps);
			DWORD dwTextColor = owner_->GetTextColor();
			DWORD dwBkColor = owner_->GetNativeBkColor();
			std::wstring strText = GetHotKeyName();
			::RECT rect;
			::GetClientRect(wnd_, &rect);
			::SetBkMode(hDC, TRANSPARENT);
			::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
			HBRUSH hBrush =  CreateSolidBrush( RGB(GetBValue(dwBkColor), GetGValue(dwBkColor), GetRValue(dwBkColor)) );
			::FillRect(hDC, &rect, hBrush);
			::DeleteObject(hBrush);
			HFONT hOldFont = (HFONT)SelectObject(hDC, GetWindowFont(wnd_));
			::SIZE size = { 0 };
			::GetTextExtentPoint32(hDC, strText.c_str(), strText.length(), &size) ;
			::DrawText(hDC, strText.c_str(), -1, &rect, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
			::SelectObject(hDC, hOldFont);
			::SetCaretPos(size.cx, 0);
			::EndPaint(wnd_, &ps);
			bHandled = TRUE;
		}
		else bHandled = FALSE;
		if( !bHandled ) return Window::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}


	std::wstring HotKeyWnd::GetSuperClassName() const
	{
		return HOTKEY_CLASS;
	}


	LRESULT HotKeyWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = ::DefWindowProc(wnd_, uMsg, wParam, lParam);
		::SendMessage(wnd_, WM_CLOSE, 0, 0);
		return lRes;
	}


	LRESULT HotKeyWnd::OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( !is_init_ ) return 0;
		if( owner_ == NULL ) return 0;
		GetHotKey(owner_->virtual_keycode_, owner_->modifiers_);
		if (owner_->virtual_keycode_ == 0)
		{
			owner_->text_ = _T("нч");
			owner_->modifiers_ = 0;
		}
		else
		{
			owner_->text_ = GetHotKeyName();
		}
		//owner_->GetManager()->SendNotify(owner_, _T("textchanged"));
		owner_->Notify(kEventTextChange);
		return 0;
	}


	void HotKeyWnd::SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
	{
		ASSERT(::IsWindow(wnd_));  
		::SendMessage(wnd_, HKM_SETHOTKEY, MAKEWORD(wVirtualKeyCode, wModifiers), 0L);
	}

	DWORD HotKeyWnd::GetHotKey() const
	{
		ASSERT(::IsWindow(wnd_));
		return (::SendMessage(wnd_, HKM_GETHOTKEY, 0, 0L));
	}

	void HotKeyWnd::GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const
	{
		//ASSERT(::IsWindow(m_hWnd));
		//LRESULT dw = ::SendMessage(m_hWnd, HKM_GETHOTKEY, 0, 0L);
		DWORD dw = GetHotKey();
		wVirtualKeyCode = LOBYTE(LOWORD(dw));
		wModifiers = HIBYTE(LOWORD(dw));
	}

	void HotKeyWnd::SetRules(WORD wInvalidComb, WORD wModifiers)
	{ 
		ASSERT(::IsWindow(wnd_));  
		::SendMessage(wnd_, HKM_SETRULES, wInvalidComb, MAKELPARAM(wModifiers, 0)); 
	}


	std::wstring HotKeyWnd::GetKeyName(UINT vk, BOOL fExtended)
	{
		UINT nScanCode = ::MapVirtualKeyEx( vk, 0, ::GetKeyboardLayout( 0 ) );
		switch( vk )
		{
			// Keys which are "extended" (except for Return which is Numeric Enter as extended)
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_NEXT: // Page down
		case VK_PRIOR: // Page up
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			nScanCode |= 0x100; // Add extended bit
		}
		if (fExtended)
			nScanCode |= 0x01000000L;

		TCHAR szStr[ MAX_PATH ] = {0};
		::GetKeyNameText( nScanCode << 16, szStr, MAX_PATH );

		return std::wstring(szStr);
	}


	std::wstring HotKeyWnd::GetHotKeyName()
	{
		ASSERT(::IsWindow(wnd_));

		std::wstring strKeyName;
		WORD wCode = 0;
		WORD wModifiers = 0;
		const TCHAR szPlus[] = _T(" + ");

		GetHotKey(wCode, wModifiers);
		if (wCode != 0 || wModifiers != 0)
		{
			if (wModifiers & HOTKEYF_CONTROL)
			{
				strKeyName += GetKeyName(VK_CONTROL, FALSE);
				strKeyName += szPlus;
			}


			if (wModifiers & HOTKEYF_SHIFT)
			{
				strKeyName += GetKeyName(VK_SHIFT, FALSE);
				strKeyName += szPlus;
			}


			if (wModifiers & HOTKEYF_ALT)
			{
				strKeyName += GetKeyName(VK_MENU, FALSE);
				strKeyName += szPlus;
			}


			strKeyName += GetKeyName(wCode, wModifiers & HOTKEYF_EXT);
		}

		return strKeyName;
	}


	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DUICONTROL(HotKey)

	HotKey::HotKey() : wnd_(NULL), virtual_keycode_(0), modifiers_(0), btn_state_(0), bk_color_(0xFFFFFFFF)
	{
		SetTextPadding(CDuiRect(4, 3, 4, 3));
		SetBkColor(0xFFFFFFFF);
	}

	std::wstring HotKey::GetClass() const
	{
		return _T("HotKey");
	}

	LPVOID HotKey::GetInterface(std::wstring pstrName)
	{
		if( _tcscmp(pstrName.c_str(), _T("HotKey")) == 0 ) return static_cast<HotKey *>(this);
		return Label::GetInterface(pstrName);
	}

	UINT HotKey::GetControlFlags() const
	{
		if( !IsEnabled() ) return Control::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	void HotKey::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Label::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetCursor && IsEnabled() ){
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}

		if( event.type_ == kEventWindowSize ){
			if( wnd_ != NULL ) pm_->SetFocusNeeded(this);
		}

		if( event.type_ == kMouseScrollWheel ){
			if( wnd_ != NULL ) return;
		}

		if( event.type_ == kEventSetFocus && IsEnabled() ) {
			if( wnd_ ) return;
			wnd_ = new HotKeyWnd();
			ASSERT(wnd_);
			wnd_->Init(this);
			Invalidate();
		}

		if( event.type_ == kEventKillFocus && IsEnabled() ) {
			Invalidate();
		}

		if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick || event.type_ == kEventMouseRightButtonDown) {
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				if( IsFocused() && wnd_ == NULL )
				{
					wnd_ = new HotKeyWnd();
					ASSERT(wnd_);
					wnd_->Init(this);
				}
			}
			return;
		}

		if( event.type_ == kEventMouseMove ) {
			return;
		}

		if( event.type_ == kEventMouseButtonUp ) 
		{
			return;
		}
		if( event.type_ == kEventMouseMenu )
		{
			return;
		}
		if( event.type_ == kEventMouseEnter )
		{
			if( IsEnabled() ) {
				btn_state_ |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseLeave )
		{
			if( IsEnabled() ) {
				btn_state_ &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		Label::DoEvent(event);
	}

	void HotKey::SetEnabled(bool bEnable)
	{
		Control::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			btn_state_ = 0;
		}
	}

	void HotKey::SetText(std::wstring pstrText)
	{
		text_ = pstrText;
		if( wnd_ != NULL ) Edit_SetText(*wnd_, text_.c_str());
		Invalidate();
	}

	std::wstring HotKey::GetNormalImage()
	{
		return normal_image_;
	}

	void HotKey::SetNormalImage(std::wstring pStrImage)
	{
		normal_image_ = pStrImage;
		Invalidate();
	}

	std::wstring HotKey::GetHotImage()
	{
		return hot_image_;
	}

	void HotKey::SetHotImage(std::wstring pStrImage)
	{
		hot_image_ = pStrImage;
		Invalidate();
	}

	std::wstring HotKey::GetFocusedImage()
	{
		return focused_image_;
	}

	void HotKey::SetFocusedImage(std::wstring pStrImage)
	{
		focused_image_ = pStrImage;
		Invalidate();
	}

	std::wstring HotKey::GetDisabledImage()
	{
		return disabled_image_;
	}

	void HotKey::SetDisabledImage(std::wstring pStrImage)
	{
		disabled_image_ = pStrImage;
		Invalidate();
	}

	void HotKey::SetNativeBkColor(DWORD dwBkColor)
	{
		bk_color_ = dwBkColor;
	}

	DWORD HotKey::GetNativeBkColor() const
	{
		return bk_color_;
	}

	void HotKey::SetPos(RECT rc)
	{
		Control::SetPos(rc);
		if( wnd_ != NULL ) {
			RECT rcPos = wnd_->CalPos();
			::SetWindowPos(wnd_->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
				rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

	void HotKey::SetVisible(bool bVisible)
	{
		Control::SetVisible(bVisible);
		if( !IsVisible() && wnd_ != NULL ) pm_->SetFocus(NULL);
	}

	void HotKey::SetInternVisible(bool bVisible)
	{
		if( !IsVisible() && wnd_ != NULL ) pm_->SetFocus(NULL);
	}

	SIZE HotKey::EstimateSize(SIZE szAvailable)
	{
		if (cxy_fired_sz_.cy == 0) {
			return CDuiSize(cxy_fired_sz_.cx,
				GlobalManager::GetFontInfo(GetFont(), ::GetDC(wnd_->GetHWND()))->tm_.tmHeight + 6);
		}
		return Control::EstimateSize(szAvailable);
	}

	void HotKey::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("nativebkcolor")) == 0 ) {
			SetNativeBkColor(GlobalManager::GetColor(strValue));
		}
		else Label::SetAttribute(strName, strValue);
	}

	void HotKey::PaintStatusImage(HDC hDC)
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
		else if( (btn_state_ & UISTATE_HOT) != 0 ) {
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

	void HotKey::PaintText(HDC hDC)
	{
		if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();
		if( disabled_text_color_ == 0 ) disabled_text_color_ = GlobalManager::GetDefaultDisabledColor();
		if( text_.empty() ) return;
		std::wstring sText = text_;
		RECT rc = rect_;
		rc.left += text_padding_rect_.left;
		rc.right -= text_padding_rect_.right;
		rc.top += text_padding_rect_.top;
		rc.bottom -= text_padding_rect_.bottom;
		if( IsEnabled() ) {
			RenderEngine::DrawText(hDC, pm_, rc, sText, text_color_, \
				font_, DT_SINGLELINE | text_style_);
		}
		else {
			RenderEngine::DrawText(hDC, pm_, rc, sText, disabled_text_color_, \
				font_, DT_SINGLELINE | text_style_);
		}
	}

	DWORD HotKey::GetHotKey() const
	{
		return (MAKEWORD(virtual_keycode_, modifiers_));
	}

	void HotKey::GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const
	{
		wVirtualKeyCode = virtual_keycode_;
		wModifiers = modifiers_;
	}

	void HotKey::SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
	{
		virtual_keycode_ = wVirtualKeyCode;
		modifiers_ = wModifiers;

		if( wnd_ ) return;
		wnd_ = new HotKeyWnd();
		ASSERT(wnd_);
		wnd_->Init(this);
		Invalidate();
	}

}// Duilib