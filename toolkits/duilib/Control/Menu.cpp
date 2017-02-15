#include "StdAfx.h"

#include "Menu.h"

namespace ui {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_DUICONTROL(Menu)

		Menu::Menu():
		wnd_(NULL)
	{
		if (GetHeader() != NULL)
			GetHeader()->SetVisible(false);
	}

	Menu::~Menu()
	{

	}

	std::wstring Menu::GetClass() const
	{
		return _T("Menu");
	}

	LPVOID Menu::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("Menu")) == 0 ) return static_cast<Menu*>(this);
		return List::GetInterface(pstrName);
	}

	void Menu::DoEvent(EventArgs& event)
	{
		return __super::DoEvent(event);
	}

	bool Menu::Add(Control* pControl)
	{
		MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
		if (pMenuItem == NULL)
			return false;

		for (int i = 0; i < pMenuItem->GetCount(); ++i)
		{
			if (pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL)
			{
				(static_cast<MenuElement*>(pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(false);
			}
		}
		return List::Add(pControl);
	}

	bool Menu::AddAt(Control* pControl, int iIndex)
	{
		MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
		if (pMenuItem == NULL)
			return false;

		for (int i = 0; i < pMenuItem->GetCount(); ++i)
		{
			if (pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL)
			{
				(static_cast<MenuElement*>(pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(false);
			}
		}
		return List::AddAt(pControl, iIndex);
	}

	int Menu::GetItemIndex(Control* pControl) const
	{
		MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
		if (pMenuItem == NULL)
			return -1;

		return __super::GetItemIndex(pControl);
	}

	bool Menu::SetItemIndex(Control* pControl, int iIndex)
	{
		MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
		if (pMenuItem == NULL)
			return false;

		return __super::SetItemIndex(pControl, iIndex);
	}

	bool Menu::Remove(Control* pControl)
	{
		MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
		if (pMenuItem == NULL)
			return false;

		return __super::Remove(pControl);
	}

	SIZE Menu::EstimateSize(SIZE szAvailable)
	{
		int cxFixed = 0;
		int cyFixed = 0;
		for( int it = 0; it < GetCount(); it++ ) {
			Control* pControl = static_cast<Control*>(GetItemAt(it));
			if( !pControl->IsVisible() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;
			if( cxFixed < sz.cx )
				cxFixed = sz.cx;
		}

		for (int it = 0; it < GetCount(); it++) {
			Control* pControl = static_cast<Control*>(GetItemAt(it));
			if (!pControl->IsVisible()) continue;

			pControl->SetFixedWidth(MulDiv(cxFixed, 100, GetManager()->GetDPIObj()->GetScale()));
		}

		return CDuiSize(cxFixed, cyFixed);
	}

	void Menu::SetAttribute(std::wstring pstrName, std::wstring pstrValue)
	{
		List::SetAttribute(pstrName, pstrValue);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//

	MenuWnd::MenuWnd():
	owner_(NULL),
		box_(),
		xml_(_T("")),
		isClosing(false)
	{
		alignment_ = eMenuAlignment_Left | eMenuAlignment_Top;
	}

	MenuWnd::~MenuWnd()
	{
		
	}

	void MenuWnd::Close(UINT nRet)
	{
		ASSERT(::IsWindow(wnd_));
		if (!::IsWindow(wnd_)) return;
		PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);
		isClosing = true;
	}


	BOOL MenuWnd::Receive(ContextMenuParam param)
	{
		switch (param.wParam)
		{
		case 1:
			Close();
			break;
		case 2:
			{
				HWND hParent = GetParent(wnd_);
				while (hParent != NULL)
				{
					if (hParent == param.hWnd)
					{
						Close();
						break;
					}
					hParent = GetParent(hParent);
				}
			}
			break;
		default:
			break;
		}

		return TRUE;
	}

	MenuWnd* MenuWnd::CreateMenu(MenuElement* pOwner, STRINGorID xml, POINT point, PaintManager* pMainPaintManager, CStdStringPtrMap* pMenuCheckInfo /*= NULL*/, DWORD dwAlignment /*= eMenuAlignment_Left | eMenuAlignment_Top*/)
	{
		MenuWnd* pMenu = new MenuWnd;
		pMenu->Init(pOwner, xml, point, pMainPaintManager, pMenuCheckInfo, dwAlignment);
		return pMenu;
	}

	void MenuWnd::DestroyMenu()
	{
		CStdStringPtrMap* mCheckInfos = MenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
		if (mCheckInfos != NULL)
		{
			for(int i = 0; i < mCheckInfos->GetSize(); i++) {
				MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(mCheckInfos->GetAt(i));
				if(pItemInfo != NULL) {
					delete pItemInfo;
					pItemInfo = NULL;
				}
			}
		}
	}
	
	void MenuWnd::Init(MenuElement* pOwner, STRINGorID xml, POINT point,
		PaintManager* pMainPaintManager, CStdStringPtrMap* pMenuCheckInfo/* = NULL*/,
		DWORD dwAlignment/* = eMenuAlignment_Left | eMenuAlignment_Top*/)
	{

		base_point_ = point;
		owner_ = pOwner;
		box_ = NULL;
		xml_ = xml;
		alignment_ = dwAlignment;

		// 如果是一级菜单的创建
		if (pOwner == NULL){
			ASSERT(pMainPaintManager != NULL);
			MenuWnd::GetGlobalContextMenuObserver().SetManger(pMainPaintManager);
			if (pMenuCheckInfo != NULL)
				MenuWnd::GetGlobalContextMenuObserver().SetMenuCheckInfo(pMenuCheckInfo);
		}

		MenuWnd::GetGlobalContextMenuObserver().AddReceiver(this);

		Create((owner_ == NULL) ? pMainPaintManager->GetPaintWindow() : owner_->GetManager()->GetPaintWindow(), L"", WS_POPUP , WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());
		
		// HACK: Don't deselect the parent's caption
		HWND hWndParent = wnd_;
		while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);

		::ShowWindow(wnd_, SW_SHOW);
		::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
	}

	std::wstring MenuWnd::GetWindowClassName() const
	{
		return _T("DuiMenuWnd");
	}

	// TODO : luyang 2017/01/04
	/*void MenuWnd::Notify(EventArgs& msg)
	{
		if( MenuWnd::GetGlobalContextMenuObserver().GetManager() != NULL) {
			if( msg.sType == _T("click") || msg.sType == _T("valuechanged") ) {
				MenuWnd::GetGlobalContextMenuObserver().GetManager()->SendNotify(msg, false);
			}
		}
	}*/

	Control* MenuWnd::CreateControl( std::wstring pstrClassName )
	{
		if (_tcsicmp(pstrClassName.c_str(), _T("Menu")) == 0)
		{
			return new Menu();
		}
		else if (_tcsicmp(pstrClassName.c_str(), _T("MenuElement")) == 0)
		{
			return new MenuElement();
		}
		return NULL;
	}


	void MenuWnd::OnFinalMessage(HWND hWnd)
	{
		RemoveObserver();
		if( owner_ != NULL ) {
			for( int i = 0; i < owner_->GetCount(); i++ ) {
				if( static_cast<MenuElement*>(owner_->GetItemAt(i)->GetInterface(_T("MenuElement"))) != NULL ) {
					(static_cast<MenuElement*>(owner_->GetItemAt(i)))->SetOwner(owner_->GetParent());
					(static_cast<MenuElement*>(owner_->GetItemAt(i)))->SetVisible(false);
					(static_cast<MenuElement*>(owner_->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(false);
				}
			}
			owner_->wnd_ = NULL;
			owner_->btn_state_ &= ~ UISTATE_PUSHED;
			owner_->Invalidate();

			// 内部创建的内部删除
			delete this;
		}
	}

	LRESULT MenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bool bShowShadow = false;
		if( owner_ != NULL) {
			LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
			styleValue &= ~WS_CAPTION;
			::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			RECT rcClient;
			::GetClientRect(*this, &rcClient);
			::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
				rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

			pm_.Init(wnd_);
			pm_.GetDPIObj()->SetScale(owner_->GetManager()->GetDPIObj()->GetDPI());
			// The trick is to add the items to the new container. Their owner gets
			// reassigned by this operation - which is why it is important to reassign
			// the items back to the righfull owner/manager when the window closes.
			box_ = new Menu();
			pm_.SetForceUseSharedRes(true);
			box_->SetManager(&pm_, NULL, true);
			std::wstring pDefaultAttributes = owner_->GetManager()->GetDefaultAttributeList(_T("Menu"));
			if( !pDefaultAttributes.empty() ) {
				box_->ApplyAttributeList(pDefaultAttributes);
			}
			box_->SetAutoDestroy(false);

			for( int i = 0; i < owner_->GetCount(); i++ ) {
				if(owner_->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL ){
					(static_cast<MenuElement*>(owner_->GetItemAt(i)))->SetOwner(box_);
					box_->Add(static_cast<Control*>(owner_->GetItemAt(i)));
				}
			}

			ShadowUI *pShadow = owner_->GetManager()->GetShadow();
			pShadow->CopyShadow(pm_.GetShadow());
			bShowShadow = pm_.GetShadow()->IsShowShadow();
			pm_.GetShadow()->ShowShadow(false);
			pm_.SetLayered(owner_->GetManager()->IsLayered());
			pm_.AttachDialog(box_);

			ResizeSubMenu();
		}
		else {
			pm_.Init(wnd_);
			pm_.GetDPIObj()->SetScale(MenuWnd::GetGlobalContextMenuObserver().GetManager()->GetDPIObj()->GetDPI());
			DialogBuilder builder;

			Control* pRoot = builder.Create(xml_,UINT(0), &pm_);
			bShowShadow = pm_.GetShadow()->IsShowShadow();
			pm_.GetShadow()->ShowShadow(false);
			pm_.AttachDialog(pRoot);

			ResizeMenu();
		}
		GetMenuUI()->wnd_ = this;
		pm_.GetShadow()->ShowShadow(bShowShadow);
		pm_.GetShadow()->Create(&pm_);
		return 0;
	}

	Menu* MenuWnd::GetMenuUI()
	{
		return static_cast<Menu*>(pm_.GetRoot());
	}

	void MenuWnd::ResizeMenu()
	{
		Control* pRoot = pm_.GetRoot();

#if defined(WIN32) && !defined(UNDER_CE)
		MONITORINFO oMonitor = {}; 
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
#else
		CDuiRect rcWork;
		GetWindowRect(owner_->GetManager()->GetPaintWindow(), &rcWork);
#endif
		SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
		szAvailable = pRoot->EstimateSize(szAvailable);
		pm_.SetInitSize(szAvailable.cx, szAvailable.cy);

		//必须是Menu标签作为xml的根节点
		Menu *pMenuRoot = static_cast<Menu*>(pRoot);
		ASSERT(pMenuRoot);

		SIZE szInit = pm_.GetInitSize();
		CDuiRect rc;
		CDuiPoint point = base_point_;
		rc.left = point.x;
		rc.top = point.y;
		rc.right = rc.left + szInit.cx;
		rc.bottom = rc.top + szInit.cy;

		int nWidth = rc.GetWidth();
		int nHeight = rc.GetHeight();

		if (alignment_ & eMenuAlignment_Right)
		{
			rc.right = point.x;
			rc.left = rc.right - nWidth;
		}

		if (alignment_ & eMenuAlignment_Bottom)
		{
			rc.bottom = point.y;
			rc.top = rc.bottom - nHeight;
		}

		SetForegroundWindow(wnd_);
		MoveWindow(wnd_, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
		SetWindowPos(wnd_, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(), rc.GetHeight() + pMenuRoot->GetInset().bottom + pMenuRoot->GetInset().top, SWP_SHOWWINDOW);
	}

	void MenuWnd::ResizeSubMenu()
	{
		// Position the popup window in absolute space
		RECT rcOwner = owner_->GetPos();
		RECT rc = rcOwner;

		int cxFixed = 0;
		int cyFixed = 0;

#if defined(WIN32) && !defined(UNDER_CE)
		MONITORINFO oMonitor = {}; 
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
#else
		CDuiRect rcWork;
		GetWindowRect(owner_->GetManager()->GetPaintWindow(), &rcWork);
#endif
		SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

		for( int it = 0; it < owner_->GetCount(); it++ ) {
			if(owner_->GetItemAt(it)->GetInterface(_T("MenuElement")) != NULL ){
				Control* pControl = static_cast<Control*>(owner_->GetItemAt(it));
				SIZE sz = pControl->EstimateSize(szAvailable);
				cyFixed += sz.cy;
				if( cxFixed < sz.cx ) cxFixed = sz.cx;
			}
		}

		RECT rcWindow;
		GetWindowRect(owner_->GetManager()->GetPaintWindow(), &rcWindow);

		rc.top = rcOwner.top;
		rc.bottom = rc.top + cyFixed;
		::MapWindowRect(owner_->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
		rc.left = rcWindow.right;
		rc.right = rc.left + cxFixed;
		rc.right += 2;

		bool bReachBottom = false;
		bool bReachRight = false;
		LONG chRightAlgin = 0;
		LONG chBottomAlgin = 0;

		RECT rcPreWindow = {0};
		MenuObserverImpl::Iterator iterator(MenuWnd::GetGlobalContextMenuObserver());
		MenuMenuReceiverImplBase* pReceiver = iterator.next();
		while( pReceiver != NULL ) {
			MenuWnd* pContextMenu = dynamic_cast<MenuWnd*>(pReceiver);
			if( pContextMenu != NULL ) {
				GetWindowRect(pContextMenu->GetHWND(), &rcPreWindow);

				bReachRight = rcPreWindow.left >= rcWindow.right;
				bReachBottom = rcPreWindow.top >= rcWindow.bottom;
				if( pContextMenu->GetHWND() == owner_->GetManager()->GetPaintWindow() ||  bReachBottom || bReachRight )
					break;
			}
			pReceiver = iterator.next();
		}

		if (bReachBottom)
		{
			rc.bottom = rcWindow.top;
			rc.top = rc.bottom - cyFixed;
		}

		if (bReachRight)
		{
			rc.right = rcWindow.left;
			rc.left = rc.right - cxFixed;
		}

		if( rc.bottom > rcWork.bottom )
		{
			rc.bottom = rc.top;
			rc.top = rc.bottom - cyFixed;
		}

		if (rc.right > rcWork.right)
		{
			rc.right = rcWindow.left;
			rc.left = rc.right - cxFixed;
		}

		if( rc.top < rcWork.top )
		{
			rc.top = rcOwner.top;
			rc.bottom = rc.top + cyFixed;
		}

		if (rc.left < rcWork.left)
		{
			rc.left = rcWindow.right;
			rc.right = rc.left + cxFixed;
		}

		MoveWindow(wnd_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + box_->GetInset().top + box_->GetInset().bottom, FALSE);
	}

	void MenuWnd::setDPI(int DPI) {
		pm_.SetDPI(DPI);
	}


	LRESULT MenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HWND hFocusWnd = (HWND)wParam;

		BOOL bInMenuWindowList = FALSE;
		ContextMenuParam param;
		param.hWnd = GetHWND();

		MenuObserverImpl::Iterator iterator(MenuWnd::GetGlobalContextMenuObserver());
		MenuMenuReceiverImplBase* pReceiver = iterator.next();
		while( pReceiver != NULL ) {
			MenuWnd* pContextMenu = dynamic_cast<MenuWnd*>(pReceiver);
			if( pContextMenu != NULL && pContextMenu->GetHWND() ==  hFocusWnd ) {
				bInMenuWindowList = TRUE;
				break;
			}
			pReceiver = iterator.next();
		}

		if( !bInMenuWindowList ) {
			param.wParam = 1;
			MenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
			return 0;
		}
		return 0;
	}
	LRESULT MenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = pm_.GetRoundCorner();
		if( !::IsIconic(*this) ) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT MenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg )
		{
		case WM_CREATE:       
			lRes = OnCreate(uMsg, wParam, lParam, bHandled); 
			break;
		case WM_KILLFOCUS:       
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); 
			break;
		case WM_KEYDOWN:
			if( wParam == VK_ESCAPE || wParam == VK_LEFT)
				Close();
			break;
		case WM_SIZE:
			lRes = OnSize(uMsg, wParam, lParam, bHandled);
			break;
		case WM_CLOSE:
			if( owner_ != NULL )
			{
				owner_->SetManager(owner_->GetManager(), owner_->GetParent(), false);
				owner_->SetPos(owner_->GetPos());
				owner_->SetFocus();
			}
			break;
		case WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
			return 0L;
			break;
		default:
			bHandled = FALSE;
			break;
		}

		if( pm_.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return Window::HandleMessage(uMsg, wParam, lParam);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_DUICONTROL(MenuElement)

	MenuElement::MenuElement():
	wnd_(NULL),
		draw_line_(false),
		line_color_(DEFAULT_LINE_COLOR),
		checked_(false),
		show_expand_icon_(false)
	{
		cxy_fired_sz_.cy = ITEM_DEFAULT_HEIGHT;
		cxy_fired_sz_.cx = ITEM_DEFAULT_WIDTH;
		icon_size_.cy = ITEM_DEFAULT_ICON_SIZE;
		icon_size_.cx = ITEM_DEFAULT_ICON_SIZE;

		line_padding_.top = line_padding_.bottom = 0;
		line_padding_.left = DEFAULT_LINE_LEFT_INSET;
		line_padding_.right = DEFAULT_LINE_RIGHT_INSET;
	}

	MenuElement::~MenuElement()
	{}

	std::wstring MenuElement::GetClass() const
	{
		return _T("MenuElement");
	}

	LPVOID MenuElement::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("MenuElement")) == 0 ) return static_cast<MenuElement*>(this);
		return ListContainerElement::GetInterface(pstrName);
	}

	void MenuElement::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		SIZE m_cxyFixed = MenuElement::cxy_fired_sz_;
		m_cxyFixed.cx = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cx);
		m_cxyFixed.cy = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cy);
		RECT line_padding_ = MenuElement::line_padding_;
		GetManager()->GetDPIObj()->Scale(&line_padding_);

		if( !::IntersectRect(&paint_rect_, &rcPaint, &rect_) ) return;

		if(draw_line_)
		{
			RECT rcLine = { rect_.left +  line_padding_.left, rect_.top + m_cxyFixed.cy/2, rect_.right - line_padding_.right, rect_.top + m_cxyFixed.cy/2 };
			RenderEngine::DrawLine(hDC, rcLine, 1, line_color_);
		}
		else
		{
			MenuElement::DrawItemBk(hDC, rect_);
			DrawItemText(hDC, rect_);
			DrawItemIcon(hDC, rect_);
			DrawItemExpland(hDC, rect_);
			for (int i = 0; i < GetCount(); ++i)
			{
				if (GetItemAt(i)->GetInterface(_T("MenuElement")) == NULL) {
					GetItemAt(i)->DoPaint(hDC, rcPaint);
				}
			}
		}
	}

	void MenuElement::DrawItemIcon(HDC hDC, const RECT& rcItem)
	{
		if (!icon_path_.empty() && !(checked_ && !GetChecked()))
		{
			SIZE m_cxyFixed = MenuElement::cxy_fired_sz_;
			m_cxyFixed.cx = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cx);
			m_cxyFixed.cy = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cy);

			SIZE icon_size_ = MenuElement::icon_size_;
			icon_size_.cx = GetManager()->GetDPIObj()->Scale(icon_size_.cx);
			icon_size_.cy = GetManager()->GetDPIObj()->Scale(icon_size_.cy);
			TListInfo* pInfo = owner_->GetListInfo();
			RECT rcTextPadding = pInfo->rcTextPadding;
			GetManager()->GetDPIObj()->Scale(&rcTextPadding);
			int padding = (rcTextPadding.left - icon_size_.cx) / 2;
			RECT rcDest =
			{
				padding,
				(m_cxyFixed.cy - icon_size_.cy) / 2,
				padding + icon_size_.cx,
				(m_cxyFixed.cy - icon_size_.cy) / 2 + icon_size_.cy
			};
			GetManager()->GetDPIObj()->ScaleBack(&rcDest);
			std::wstring pStrImage;
			pStrImage = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);
			DrawImage(hDC, icon_path_, pStrImage);
		}
	}

	void MenuElement::DrawItemExpland(HDC hDC, const RECT& rcItem)
	{
		if (show_expand_icon_)
		{
			std::wstring strExplandIcon;
			strExplandIcon = GetManager()->GetDefaultAttributeList(_T("ExplandIcon"));
			if (strExplandIcon.empty()) {
				return;
			}
			SIZE m_cxyFixed = MenuElement::cxy_fired_sz_;
			m_cxyFixed.cx = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cx);
			m_cxyFixed.cy = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cy);
			int padding = GetManager()->GetDPIObj()->Scale(ITEM_DEFAULT_EXPLAND_ICON_WIDTH) / 3;
			const TDrawInfo* pDrawInfo = GetManager()->GetDrawInfo(strExplandIcon, L"");
			const TImageInfo *pImageInfo = GetManager()->GetImageEx(pDrawInfo->sImageName, L"", 0);
			if (!pImageInfo) {
				return;
			}
			RECT rcDest =
			{
				m_cxyFixed.cx - pImageInfo->nX - padding,
				(m_cxyFixed.cy - pImageInfo->nY) / 2,
				m_cxyFixed.cx - pImageInfo->nX - padding + pImageInfo->nX,
				(m_cxyFixed.cy - pImageInfo->nY) / 2 + pImageInfo->nY
			};
			GetManager()->GetDPIObj()->ScaleBack(&rcDest);
			std::wstring pStrImage;
			pStrImage = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"),
				rcDest.left,
				rcDest.top,
				rcDest.right,
				rcDest.bottom);

			DrawImage(hDC, strExplandIcon, pStrImage);
		}
	}


	void MenuElement::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		std::wstring sText = GetText();
		if( sText.empty() ) return;

		if( owner_ == NULL ) return;
		TListInfo* pInfo = owner_->GetListInfo();
		DWORD iTextColor = pInfo->dwTextColor;
		if( (btn_state_ & UISTATE_HOT) != 0 ) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) {
			iTextColor = pInfo->dwDisabledTextColor;
		}
		int nLinks = 0;
		RECT rcText = rcItem;
		RECT rcTextPadding = pInfo->rcTextPadding;
		GetManager()->GetDPIObj()->Scale(&rcTextPadding);
		rcText.left += rcTextPadding.left;
		rcText.right -= rcTextPadding.right;
		rcText.top += rcTextPadding.top;
		rcText.bottom -= rcTextPadding.bottom;

		if( pInfo->bShowHtml )
			RenderEngine::DrawHtmlText(hDC, pm_, rcText, sText, iTextColor, \
			NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
		else
			RenderEngine::DrawText(hDC, pm_, rcText, sText, iTextColor, \
			pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
	}


	SIZE MenuElement::EstimateSize(SIZE szAvailable)
	{
		SIZE m_cxyFixed = MenuElement::cxy_fired_sz_;
		m_cxyFixed.cx = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cx);
		m_cxyFixed.cy = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cy);
		SIZE cXY = {0};
		for( int it = 0; it < GetCount(); it++ ) {
			Control* pControl = static_cast<Control*>(GetItemAt(it));
			if( !pControl->IsVisible() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			cXY.cy += sz.cy;
			if( cXY.cx < sz.cx )
				cXY.cx = sz.cx;
		}
		if(cXY.cy == 0) {
			TListInfo* pInfo = owner_->GetListInfo();

			DWORD iTextColor = pInfo->dwTextColor;
			if( (btn_state_ & UISTATE_HOT) != 0 ) {
				iTextColor = pInfo->dwHotTextColor;
			}
			if( IsSelected() ) {
				iTextColor = pInfo->dwSelectedTextColor;
			}
			if( !IsEnabled() ) {
				iTextColor = pInfo->dwDisabledTextColor;
			}
			std::wstring sText = GetText();

			RECT rcText = { 0, 0, MAX(szAvailable.cx, m_cxyFixed.cx), 9999 };
			RECT rcTextPadding = pInfo->rcTextPadding;
			GetManager()->GetDPIObj()->Scale(&rcTextPadding);
			rcText.left += rcTextPadding.left;
			rcText.right -= rcTextPadding.right;
			if( pInfo->bShowHtml ) {   
				int nLinks = 0;
				RenderEngine::DrawHtmlText(pm_->GetPaintDC(), pm_, rcText, sText, iTextColor, NULL, NULL, nLinks, DT_CALCRECT | pInfo->uTextStyle);
			}
			else {
				RenderEngine::DrawText(pm_->GetPaintDC(), pm_, rcText, sText, iTextColor, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle);
			}
			cXY.cx = rcText.right - rcText.left + rcTextPadding.left + rcTextPadding.right ;
			cXY.cy = rcText.bottom - rcText.top + rcTextPadding.top + rcTextPadding.bottom;
		}

		if( m_cxyFixed.cy != 0 ) cXY.cy = m_cxyFixed.cy;
		if ( cXY.cx < m_cxyFixed.cx )
			cXY.cx =  m_cxyFixed.cx;

		MenuElement::cxy_fired_sz_.cy = MulDiv(cXY.cy, 100, GetManager()->GetDPIObj()->GetScale());
		MenuElement::cxy_fired_sz_.cx = MulDiv(cXY.cx, 100, GetManager()->GetDPIObj()->GetScale());
		return cXY;
	}

	void MenuElement::DoEvent(EventArgs& event)
	{
		if( event.type_ == kEventMouseEnter )
		{
			ListContainerElement::DoEvent(event);
			if( wnd_ ) return;
			bool hasSubMenu = false;
			for( int i = 0; i < GetCount(); ++i )
			{
				if( GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL )
				{
					(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
					(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);

					hasSubMenu = true;
				}
			}
			if( hasSubMenu )
			{
				owner_->SelectItem(GetIndex(), true);
				CreateMenuWnd();
			}
			else
			{
				ContextMenuParam param;
				param.hWnd = pm_->GetPaintWindow();
				param.wParam = 2;
				MenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
				owner_->SelectItem(GetIndex(), true);
			}
			return;
		}


		if (event.type_ == kEventMouseLeave) {

			bool hasSubMenu = false;
			for (int i = 0; i < GetCount(); ++i)
			{
				if (GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL)
				{
					
					hasSubMenu = true;
				}
			}

			if (!hasSubMenu) {
				owner_->SelectItem(-1, true);
			}
		}

		if( event.type_ == kEventMouseButtonUp )
		{
			if( IsEnabled() ){
				ListContainerElement::DoEvent(event);

				if( wnd_ ) return;

				bool hasSubMenu = false;
				for( int i = 0; i < GetCount(); ++i ) {
					if( GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL ) {
						(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
						(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);

						hasSubMenu = true;
					}
				}
				if( hasSubMenu )
				{
					CreateMenuWnd();
				}
				else
				{
					SetChecked(!GetChecked());

					
					bool isClosing = false;
					Menu* menuUI=static_cast<Menu*>(GetManager()->GetRoot());
					isClosing = (menuUI->wnd_->isClosing);
					if (IsWindow(GetManager()->GetPaintWindow()) && !isClosing) {
						if (MenuWnd::GetGlobalContextMenuObserver().GetManager() != NULL)
						{

							MenuCmd* pMenuCmd = new MenuCmd();
							lstrcpy(pMenuCmd->szName, GetName().c_str());
							lstrcpy(pMenuCmd->szUserData, GetUserData().c_str());
							lstrcpy(pMenuCmd->szText, GetText().c_str());
							pMenuCmd->bChecked = GetChecked();
							if (!PostMessage(MenuWnd::GetGlobalContextMenuObserver().GetManager()->GetPaintWindow(), WM_MENUCLICK, (WPARAM)pMenuCmd, (LPARAM)this))
							{
								delete pMenuCmd;
								pMenuCmd = NULL;
							}
						}
					}
					ContextMenuParam param;
					param.hWnd = pm_->GetPaintWindow();
					param.wParam = 1;
					MenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);

					
				}
			}

			return;
		}

		if ( event.type_ == kEventKeyDown && event.key_ == VK_RIGHT )
		{
			if( wnd_ ) return;
			bool hasSubMenu = false;
			for( int i = 0; i < GetCount(); ++i )
			{
				if( GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL )
				{
					(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
					(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);
					hasSubMenu = true;
				}
			}
			if( hasSubMenu )
			{
				owner_->SelectItem(GetIndex(), true);
				CreateMenuWnd();
			}
			else
			{
				ContextMenuParam param;
				param.hWnd = pm_->GetPaintWindow();
				param.wParam = 2;
				MenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
				owner_->SelectItem(GetIndex(), true);
			}

			return;
		}

		ListContainerElement::DoEvent(event);
	}

	MenuWnd* MenuElement::GetMenuWnd()
	{
		return wnd_;
	}

	void MenuElement::CreateMenuWnd()
	{
		if( wnd_ ) return;

		wnd_ = new MenuWnd();
		ASSERT(wnd_);

		ContextMenuParam param;
		param.hWnd = pm_->GetPaintWindow();
		param.wParam = 2;
		MenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);

		wnd_->Init(static_cast<MenuElement*>(this), _T(""), CDuiPoint(), NULL);
	}

	void MenuElement::SetLineType()
	{
		draw_line_ = true;
		if (cxy_fired_sz_.cy == 0 || cxy_fired_sz_.cy == ITEM_DEFAULT_HEIGHT)
			SetFixedHeight(DEFAULT_LINE_HEIGHT);

		SetMouseChildEnabled(false);
		SetMouseEnabled(false);
		SetEnabled(false);
	}

	void MenuElement::SetLineColor(DWORD color)
	{
		line_color_ = color;
	}

	DWORD MenuElement::GetLineColor() const
	{
		return line_color_;
	}
	void MenuElement::SetLinePadding(RECT rcInset)
	{
		line_padding_ = rcInset;
	}

	RECT MenuElement::GetLinePadding() const
	{
		return line_padding_;
	}

	void MenuElement::SetIcon(std::wstring strIcon)
	{
		if ( strIcon != _T("") )
			icon_path_ = strIcon;
	}

	void MenuElement::SetIconSize(LONG cx, LONG cy)
	{
		icon_size_.cx = cx;
		icon_size_.cy = cy;
	}

	void MenuElement::SetChecked(bool bCheck/* = true*/)
	{
		SetItemInfo(GetName(), bCheck);
	}

	bool MenuElement::GetChecked() const
	{
		std::wstring pstrName = GetName();
		if(pstrName.empty()) return false;

		CStdStringPtrMap* mCheckInfos = MenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
		if (mCheckInfos != NULL)
		{
			MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(pstrName);
			if(pItemInfo != NULL) {
				return pItemInfo->bChecked;
			}
		}
		return false;

	}

	void MenuElement::SetCheckItem(bool bCheckItem/* = false*/)
	{
		checked_ = bCheckItem;
	}

	bool MenuElement::GetCheckItem() const
	{
		return checked_;
	}

	void MenuElement::SetShowExplandIcon(bool bShow)
	{
		show_expand_icon_ = bShow;
	}

	void MenuElement::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("icon")) == 0){
			SetIcon(pstrValue);
		}
		else if( _tcsicmp(pstrName, _T("iconsize")) == 0 ) {
			LPTSTR pstr = NULL;
			LONG cx = 0, cy = 0;
			cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);   
			SetIconSize(cx, cy);
		}
		else if( _tcsicmp(pstrName, _T("checkitem")) == 0 ) {		
			SetCheckItem(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);		
		}
		else if( _tcsicmp(pstrName, _T("ischeck")) == 0 ) {		
			CStdStringPtrMap* mCheckInfos = MenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
			if (mCheckInfos != NULL)
			{
				bool bFind = false;
				for(int i = 0; i < mCheckInfos->GetSize(); i++) {
					MenuItemInfo* itemInfo = (MenuItemInfo*)mCheckInfos->GetAt(i).c_str();
					if(lstrcmpi(itemInfo->szName, GetName().c_str()) == 0) {
						bFind = true;
						break;
					}
				}
				if(!bFind) SetChecked(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);
			}
		}	
		else if( _tcsicmp(pstrName, _T("linetype")) == 0){
			if (_tcsicmp(pstrValue, _T("true")) == 0)
				SetLineType();
		}
		else if( _tcsicmp(pstrName, _T("expland")) == 0 ) {
			SetShowExplandIcon(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);
		}
		else if( _tcsicmp(pstrName, _T("linecolor")) == 0){
			SetLineColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("linepadding")) == 0 ) {
			RECT rcInset = { 0 };
			LPTSTR pstr = NULL;
			rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetLinePadding(rcInset);
		}
		else if	( _tcsicmp(pstrName, _T("height")) == 0){
			SetFixedHeight(_ttoi(pstrValue));
		}
		else
			ListContainerElement::SetAttribute(strName, strValue);
	}


	MenuItemInfo* MenuElement::GetItemInfo(std::wstring pstrName)
	{
		if(pstrName.empty()) return NULL;

		CStdStringPtrMap* mCheckInfos = MenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
		if (mCheckInfos != NULL)
		{
			MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(pstrName);
			if(pItemInfo != NULL) {
				return pItemInfo;
			}
		}

		return NULL;
	}

	MenuItemInfo* MenuElement::SetItemInfo(std::wstring pstrName, bool bChecked)
	{
		if(pstrName.empty()) return NULL;

		CStdStringPtrMap* mCheckInfos = MenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
		if (mCheckInfos != NULL)
		{
			MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(pstrName);
			if(pItemInfo == NULL) {
				pItemInfo = new MenuItemInfo;
				lstrcpy(pItemInfo->szName, pstrName.c_str());
				pItemInfo->bChecked = bChecked;
				mCheckInfos->Insert(pstrName, pItemInfo);
			}
			else {
				pItemInfo->bChecked = bChecked;
			}

			return pItemInfo;
		}
		return NULL;
	}
} // namespace DuiLib
