#include "StdAfx.h"

namespace ui {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	class ComboWnd : public Window
	{
	public:
		ComboWnd() {}
		void Init(Combo* pOwner);
		std::wstring GetWindowClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void OnWindowInit();

		void EnsureVisible(int iIndex);
		void Scroll(int dx, int dy);

#if(_WIN32_WINNT >= 0x0501)
		virtual UINT GetClassStyle() const;
#endif
		bool IsHitItem(POINT ptMouse);
	public:
		PaintManager pm_;
		Combo* owner_;
		VBox* box_;
		int old_sel_;
		bool hit_item_;
	};

	void ComboWnd::Init(Combo* pOwner)
	{
		hit_item_ = false;
		owner_ = pOwner;
		box_ = NULL;
		old_sel_ = owner_->GetCurSel();

		// Position the popup window in absolute space
		SIZE szDrop = owner_->GetDropBoxSize();
		RECT rcOwner = pOwner->GetPos();
		RECT rc = rcOwner;
		rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
		rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
		if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		int cyFixed = 0;
		for( int it = 0; it < pOwner->GetCount(); it++ ) {
			Control* pControl = static_cast<Control*>(pOwner->GetItemAt(it));
			if( !pControl->IsVisible() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;
		}
		cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
		rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

		::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		if( rc.bottom > rcWork.bottom ) {
			rc.left = rcOwner.left;
			rc.right = rcOwner.right;
			if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
			rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
			rc.bottom = rcOwner.top;
			::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
		}

		Create(pOwner->GetManager()->GetPaintWindow(), L"", WS_POPUP, WS_EX_TOOLWINDOW, rc);
		// HACK: Don't deselect the parent's caption
		HWND hWndParent = wnd_;
		while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
		::ShowWindow(wnd_, SW_SHOW);
		::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
	}

	std::wstring ComboWnd::GetWindowClassName() const
	{
		return _T("ComboWnd");
	}

	void ComboWnd::OnFinalMessage(HWND hWnd)
	{
		owner_->combo_wnd_ = NULL;
		owner_->btn_state_ &= ~ UISTATE_PUSHED;
		owner_->Invalidate();
		delete this;
	}

	bool ComboWnd::IsHitItem(POINT ptMouse)
	{
		Control* pControl = pm_.FindControl(ptMouse);
		if(pControl != NULL) {
			LPVOID pInterface = pControl->GetInterface(DUI_CTR_SCROLLBAR);
			if(pInterface) return false;

			while(pControl != NULL) {
				IListItem* pListItem = (IListItem*)pControl->GetInterface(DUI_CTR_LISTITEM);
				if(pListItem != NULL ) {
					return true;
				}
				pControl = pControl->GetParent();
			}
		}
		
		return false;
	}

	LRESULT ComboWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if( uMsg == WM_CREATE ) {
			pm_.SetForceUseSharedRes(true);
			pm_.Init(wnd_);
			// The trick is to add the items to the new container. Their owner gets
			// reassigned by this operation - which is why it is important to reassign
			// the items back to the righfull owner/manager when the window closes.
			box_ = new VBox;
			box_->SetManager(&pm_, NULL, true);
			std::wstring pDefaultAttributes = owner_->GetManager()->GetDefaultAttributeList(_T("VBox"));
			if( !pDefaultAttributes.empty() ) {
				box_->ApplyAttributeList(pDefaultAttributes);
			}
			box_->SetInset(CDuiRect(1, 1, 1, 1));
			box_->SetBkColor(0xFFFFFFFF);
			box_->SetBorderColor(0xFFC6C7D2);
			box_->SetBorderSize(1);
			box_->SetAutoDestroy(false);
			box_->EnableScrollBar();
			box_->ApplyAttributeList(owner_->GetDropBoxAttributeList());
			for( int i = 0; i < owner_->GetCount(); i++ ) {
				box_->Add(static_cast<Control*>(owner_->GetItemAt(i)));
			}
			pm_.AttachDialog(box_);
			return 0;
		}
		else if( uMsg == WM_CLOSE ) {
			owner_->SetManager(owner_->GetManager(), owner_->GetParent(), false);
			RECT rcNull = { 0 };
			for( int i = 0; i < owner_->GetCount(); i++ ) static_cast<Control*>(owner_->GetItemAt(i))->SetPos(rcNull);
			owner_->SetFocus();
		}
		else if( uMsg == WM_LBUTTONDOWN ) {
			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(pm_.GetPaintWindow(), &pt);
			hit_item_ = IsHitItem(pt);
		}
		else if( uMsg == WM_LBUTTONUP ) {
			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(pm_.GetPaintWindow(), &pt);
			if(hit_item_ && IsHitItem(pt)) {
				PostMessage(WM_KILLFOCUS);
			}
			hit_item_ = false;
		}
		else if( uMsg == WM_KEYDOWN ) {
			switch( wParam ) {
			case VK_ESCAPE:
				owner_->SelectItem(old_sel_, true);
				EnsureVisible(old_sel_);
			case VK_RETURN:
				PostMessage(WM_KILLFOCUS);
				break;
			default:
				EventArgs event;
				event.type_ = kEventKeyDown;
				event.key_ = (TCHAR)wParam;
				owner_->DoEvent(event);
				EnsureVisible(owner_->GetCurSel());
				return 0;
			}
		}
		else if( uMsg == WM_MOUSEWHEEL ) {
			int zDelta = (int) (short) HIWORD(wParam);
			EventArgs event;
			event.type_ = kMouseScrollWheel;
			event.w_param_ = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
			event.l_param_ = lParam;
			event.timestamp_ = ::GetTickCount();
			if(owner_->GetScrollSelect()) {
				owner_->DoEvent(event);
				EnsureVisible(owner_->GetCurSel());
				return 0;
			}
			else {
				box_->DoEvent(event);
				return 0;
			}
		}
		else if( uMsg == WM_KILLFOCUS ) {
			if( wnd_ != (HWND) wParam ) PostMessage(WM_CLOSE);
		}

		LRESULT lRes = 0;
		if( pm_.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return Window::HandleMessage(uMsg, wParam, lParam);
	}

	void ComboWnd::OnWindowInit()
	{
		EnsureVisible(old_sel_);
	}

	void ComboWnd::EnsureVisible(int iIndex)
	{
		if( owner_->GetCurSel() < 0 ) return;
		box_->FindSelectable(owner_->GetCurSel(), false);
		RECT rcItem = box_->GetItemAt(iIndex)->GetPos();
		RECT rcList = box_->GetPos();
		ScrollBar* pHorizontalScrollBar = box_->GetHorizontalScrollBar();
		if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();
		int iPos = box_->GetScrollPos().cy;
		if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
		int dx = 0;
		if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
		if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
		Scroll(0, dx);
	}

	void ComboWnd::Scroll(int dx, int dy)
	{
		if( dx == 0 && dy == 0 ) return;
		SIZE sz = box_->GetScrollPos();
		box_->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
	}

#if(_WIN32_WINNT >= 0x0501)
	UINT ComboWnd::GetClassStyle() const
	{
		return __super::GetClassStyle() | CS_DROPSHADOW;
	}
#endif
	////////////////////////////////////////////////////////
	IMPLEMENT_DUICONTROL(Combo)

	Combo::Combo() : text_style_(DT_VCENTER | DT_SINGLELINE)
		, text_color_(0)
		, disabled_text_color_(0)
		, font_(-1)
		, is_show_html_(false)
		, combo_wnd_(NULL)
		, cur_sel_(-1)
		, btn_state_(0)
		, is_scroll_select_(true)
	{
		drop_box_size_ = CDuiSize(0, 150);
		::ZeroMemory(&text_padding_rc_, sizeof(text_padding_rc_));

		list_info_.nColumns = 0;
		list_info_.nFont = -1;
		list_info_.uTextStyle = DT_VCENTER;
		list_info_.dwTextColor = 0xFF000000;
		list_info_.dwBkColor = 0;
		list_info_.bAlternateBk = false;
		list_info_.dwSelectedTextColor = 0xFF000000;
		list_info_.dwSelectedBkColor = 0xFFC1E3FF;
		list_info_.dwHotTextColor = 0xFF000000;
		list_info_.dwHotBkColor = 0xFFE9F5FF;
		list_info_.dwDisabledTextColor = 0xFFCCCCCC;
		list_info_.dwDisabledBkColor = 0xFFFFFFFF;
		list_info_.dwLineColor = 0;
		list_info_.bShowHtml = false;
		list_info_.bMultiExpandable = false;
		::ZeroMemory(&list_info_.rcTextPadding, sizeof(list_info_.rcTextPadding));
		::ZeroMemory(&list_info_.rcColumn, sizeof(list_info_.rcColumn));
	}

	std::wstring Combo::GetClass() const
	{
		return DUI_CTR_COMBO;
	}

	LPVOID Combo::GetInterface(std::wstring strName)
	{
		auto pstrName = strName.c_str();
		if( _tcsicmp(pstrName, DUI_CTR_COMBO) == 0 ) return static_cast<Combo*>(this);
		if( _tcsicmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwner*>(this);
		return Box::GetInterface(pstrName);
	}

	void Combo::AttachDropDown(const EventCallback & cb)
	{
		on_event_[kEventDropDown] += cb;
	}

	void Combo::AttachItemSelected(const EventCallback & cb)
	{
		on_event_[kEventItemSelected] += cb;
	}

	UINT Combo::GetControlFlags() const
	{
		return UIFLAG_TABSTOP;
	}

	void Combo::DoInit()
	{
	}

	UINT Combo::GetListType()
	{
		return kListCombo;
	}

	TListInfo* Combo::GetListInfo()
	{
		return &list_info_;
	}

	int Combo::GetCurSel() const
	{
		return cur_sel_;
	}

	bool Combo::SelectItem(int iIndex, bool bTakeFocus, bool send_notify)
	{
		if( iIndex == cur_sel_ ) return true;
		int iOldSel = cur_sel_;
		if( cur_sel_ >= 0 ) {
			Control* pControl = static_cast<Control*>(items_[cur_sel_]);
			if( !pControl ) return false;
			IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->Select(false);
			cur_sel_ = -1;
		}
		if( iIndex < 0 ) return false;
		if( items_.GetSize() == 0 ) return false;
		if( iIndex >= items_.GetSize() ) iIndex = items_.GetSize() - 1;
		Control* pControl = static_cast<Control*>(items_[iIndex]);
		if( !pControl || !pControl->IsEnabled() ) return false;
		IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;
		cur_sel_ = iIndex;
		if( combo_wnd_ != NULL || bTakeFocus ) pControl->SetFocus();
		pListItem->Select(true);
		if (send_notify) {
			Notify(kEventItemSelected, cur_sel_, iOldSel);
		}
		
		Invalidate();

		return true;
	}

	bool Combo::SelectMultiItem(int iIndex, bool bTakeFocus)
	{
		return SelectItem(iIndex, bTakeFocus);
	}
	
	bool Combo::UnSelectItem(int iIndex, bool bOthers)
	{
		return false;
	}
		
	bool Combo::SetItemIndex(Control* pControl, int iIndex)
	{
		int iOrginIndex = GetItemIndex(pControl);
		if( iOrginIndex == -1 ) return false;
		if( iOrginIndex == iIndex ) return true;

		IListItem* pSelectedListItem = NULL;
		if( cur_sel_ >= 0 ) pSelectedListItem = 
			static_cast<IListItem*>(GetItemAt(cur_sel_)->GetInterface(_T("ListItem")));
		if( !Box::SetItemIndex(pControl, iIndex) ) return false;
		int iMinIndex = min(iOrginIndex, iIndex);
		int iMaxIndex = max(iOrginIndex, iIndex);
		for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
			Control* p = GetItemAt(i);
			IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		if( cur_sel_ >= 0 && pSelectedListItem != NULL ) cur_sel_ = pSelectedListItem->GetIndex();
		return true;
	}

	bool Combo::Add(Control* pControl)
	{
		IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) 
		{
			pListItem->SetOwner(this);
			pListItem->SetIndex(items_.GetSize());
		}
		return Box::Add(pControl);
	}

	bool Combo::AddAt(Control* pControl, int iIndex)
	{
		if (!Box::AddAt(pControl, iIndex)) return false;

		// The list items should know about us
		IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(iIndex);
		}

		for(int i = iIndex + 1; i < GetCount(); ++i) {
			Control* p = GetItemAt(i);
			pListItem = static_cast<IListItem*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		if( cur_sel_ >= iIndex ) cur_sel_ += 1;
		return true;
	}

	bool Combo::Remove(Control* pControl)
	{
		int iIndex = GetItemIndex(pControl);
		if (iIndex == -1) return false;

		if (!Box::RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < GetCount(); ++i) {
			Control* p = GetItemAt(i);
			IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}

		if( iIndex == cur_sel_ && cur_sel_ >= 0 ) {
			int iSel = cur_sel_;
			cur_sel_ = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( iIndex < cur_sel_ ) cur_sel_ -= 1;
		return true;
	}

	bool Combo::RemoveAt(int iIndex)
	{
		if (!Box::RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < GetCount(); ++i) {
			Control* p = GetItemAt(i);
			IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->SetIndex(i);
		}

		if( iIndex == cur_sel_ && cur_sel_ >= 0 ) {
			int iSel = cur_sel_;
			cur_sel_ = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( iIndex < cur_sel_ ) cur_sel_ -= 1;
		return true;
	}

	void Combo::RemoveAll()
	{
		cur_sel_ = -1;
		Box::RemoveAll();
	}

	void Combo::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Box::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetFocus ) {
			Invalidate();
		}
		if( event.type_ == kEventKillFocus ) {
			Invalidate();
		}
		if( event.type_ == kEventMouseButtonDown ){
			if( IsEnabled() ) {
				Activate();
				btn_state_ |= UISTATE_PUSHED | UISTATE_CAPTURED;
			}
			return;
		}
		if( event.type_ == kEventMouseButtonUp ){
			if( (btn_state_ & UISTATE_CAPTURED) != 0 ) {
				btn_state_ &= ~ UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseMove ){
			return;
		}
		if( event.type_ == kEventKeyDown ){
			switch( event.key_ ) {
			case VK_F4:
				Activate();
				return;
			case VK_UP:
				SelectItem(FindSelectable(cur_sel_ - 1, false));
				return;
			case VK_DOWN:
				SelectItem(FindSelectable(cur_sel_ + 1, true));
				return;
			case VK_PRIOR:
				SelectItem(FindSelectable(cur_sel_ - 1, false));
				return;
			case VK_NEXT:
				SelectItem(FindSelectable(cur_sel_ + 1, true));
				return;
			case VK_HOME:
				SelectItem(FindSelectable(0, false));
				return;
			case VK_END:
				SelectItem(FindSelectable(GetCount() - 1, true));
				return;
			}
		}
		if( event.type_ == kMouseScrollWheel ){
			if(GetScrollSelect()) {
				bool bDownward = LOWORD(event.w_param_) == SB_LINEDOWN;
				SelectItem(FindSelectable(cur_sel_ + (bDownward ? 1 : -1), bDownward));
			}
			return;
		}
		if( event.type_ == kEventMouseMenu ){
			return;
		}
		if( event.type_ == kEventMouseEnter ){
			if( ::PtInRect(&rect_, event.mouse_point_ ) ) {
				if( (btn_state_ & UISTATE_HOT) == 0  )
					btn_state_ |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseLeave ){
			if( (btn_state_ & UISTATE_HOT) != 0 ) {
				btn_state_ &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		Control::DoEvent(event);
	}

	SIZE Combo::EstimateSize(SIZE szAvailable)
	{
		if (cxy_fired_sz_.cy == 0) {
			return CDuiSize(cxy_fired_sz_.cx, GlobalManager::GetDefaultFontInfo(pm_->GetPaintDC())->tm_.tmHeight + 12);
		}
		return Control::EstimateSize(szAvailable);
	}

	bool Combo::Activate()
	{
		if( !Control::Activate() ) return false;
		if( combo_wnd_ ) return true;
		combo_wnd_ = new ComboWnd();
		ASSERT(combo_wnd_);
		combo_wnd_->Init(this);
		Notify(kEventDropDown);
		Invalidate();
		return true;
	}

	std::wstring Combo::GetText() const
	{
		if( cur_sel_ < 0 ) return _T("");
		Control* pControl = static_cast<Control*>(items_[cur_sel_]);
		return pControl->GetText();
	}

	void Combo::SetEnabled(bool bEnable)
	{
		Box::SetEnabled(bEnable);
		if( !IsEnabled() ) btn_state_ = 0;
	}

	std::wstring Combo::GetDropBoxAttributeList()
	{
		return drop_box_attributes_;
	}

	void Combo::SetDropBoxAttributeList(std::wstring pstrList)
	{
		drop_box_attributes_ = pstrList;
	}

	SIZE Combo::GetDropBoxSize() const
	{
		return drop_box_size_;
	}

	void Combo::SetDropBoxSize(SIZE szDropBox)
	{
		drop_box_size_ = szDropBox;
	}

	void Combo::SetTextStyle(UINT uStyle)
	{
		text_style_ = uStyle;
		Invalidate();
	}

	UINT Combo::GetTextStyle() const
	{
		return text_style_;
	}

	void Combo::SetTextColor(DWORD dwTextColor)
	{
		text_color_ = dwTextColor;
		Invalidate();
	}

	DWORD Combo::GetTextColor() const
	{
		return text_color_;
	}

	void Combo::SetDisabledTextColor(DWORD dwTextColor)
	{
		disabled_text_color_ = dwTextColor;
		Invalidate();
	}

	DWORD Combo::GetDisabledTextColor() const
	{
		return disabled_text_color_;
	}

	void Combo::SetFont(int index)
	{
		font_ = index;
		Invalidate();
	}

	int Combo::GetFont() const
	{
		return font_;
	}

	RECT Combo::GetTextPadding() const
	{
		return text_padding_rc_;
	}

	void Combo::SetTextPadding(RECT rc)
	{
		text_padding_rc_ = rc;
		Invalidate();
	}

	bool Combo::IsShowHtml()
	{
		return is_show_html_;
	}

	void Combo::SetShowHtml(bool bShowHtml)
	{
		if( is_show_html_ == bShowHtml ) return;

		is_show_html_ = bShowHtml;
		Invalidate();
	}

	std::wstring Combo::GetNormalImage() const
	{
		return normal_image_;
	}

	void Combo::SetNormalImage(std::wstring pStrImage)
	{
		normal_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Combo::GetHotImage() const
	{
		return hot_image_;
	}

	void Combo::SetHotImage(std::wstring pStrImage)
	{
		hot_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Combo::GetPushedImage() const
	{
		return pushed_image_;
	}

	void Combo::SetPushedImage(std::wstring pStrImage)
	{
		pushed_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Combo::GetFocusedImage() const
	{
		return focused_image_;
	}

	void Combo::SetFocusedImage(std::wstring pStrImage)
	{
		focused_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Combo::GetDisabledImage() const
	{
		return disabled_image_;
	}

	void Combo::SetDisabledImage(std::wstring pStrImage)
	{
		disabled_image_ = pStrImage;
		Invalidate();
	}

	bool Combo::GetScrollSelect()
	{
		return is_scroll_select_;
	}

	void Combo::SetScrollSelect(bool bScrollSelect)
	{
		is_scroll_select_ = bScrollSelect;
	}


	void Combo::SetItemFont(int index)
	{
		list_info_.nFont = index;
		Invalidate();
	}

	void Combo::SetItemTextStyle(UINT uStyle)
	{
		list_info_.uTextStyle = uStyle;
		Invalidate();
	}

	RECT Combo::GetItemTextPadding() const
	{
		return list_info_.rcTextPadding;
	}

	void Combo::SetItemTextPadding(RECT rc)
	{
		list_info_.rcTextPadding = rc;
		Invalidate();
	}

	void Combo::SetItemTextColor(DWORD dwTextColor)
	{
		list_info_.dwTextColor = dwTextColor;
		Invalidate();
	}

	void Combo::SetItemBkColor(DWORD dwBkColor)
	{
		list_info_.dwBkColor = dwBkColor;
	}

	void Combo::SetItemBkImage(std::wstring pStrImage)
	{
		list_info_.sBkImage = pStrImage;
	}

	DWORD Combo::GetItemTextColor() const
	{
		return list_info_.dwTextColor;
	}

	DWORD Combo::GetItemBkColor() const
	{
		return list_info_.dwBkColor;
	}

	std::wstring Combo::GetItemBkImage() const
	{
		return list_info_.sBkImage;
	}

	bool Combo::IsAlternateBk() const
	{
		return list_info_.bAlternateBk;
	}

	void Combo::SetAlternateBk(bool bAlternateBk)
	{
		list_info_.bAlternateBk = bAlternateBk;
	}

	void Combo::SetSelectedItemTextColor(DWORD dwTextColor)
	{
		list_info_.dwSelectedTextColor = dwTextColor;
	}

	void Combo::SetSelectedItemBkColor(DWORD dwBkColor)
	{
		list_info_.dwSelectedBkColor = dwBkColor;
	}

	void Combo::SetSelectedItemImage(std::wstring pStrImage)
	{
		list_info_.sSelectedImage = pStrImage;
	}

	DWORD Combo::GetSelectedItemTextColor() const
	{
		return list_info_.dwSelectedTextColor;
	}

	DWORD Combo::GetSelectedItemBkColor() const
	{
		return list_info_.dwSelectedBkColor;
	}

	std::wstring Combo::GetSelectedItemImage() const
	{
		return list_info_.sSelectedImage;
	}

	void Combo::SetHotItemTextColor(DWORD dwTextColor)
	{
		list_info_.dwHotTextColor = dwTextColor;
	}

	void Combo::SetHotItemBkColor(DWORD dwBkColor)
	{
		list_info_.dwHotBkColor = dwBkColor;
	}

	void Combo::SetHotItemImage(std::wstring pStrImage)
	{
		list_info_.sHotImage = pStrImage;
	}

	DWORD Combo::GetHotItemTextColor() const
	{
		return list_info_.dwHotTextColor;
	}
	DWORD Combo::GetHotItemBkColor() const
	{
		return list_info_.dwHotBkColor;
	}

	std::wstring Combo::GetHotItemImage() const
	{
		return list_info_.sHotImage;
	}

	void Combo::SetDisabledItemTextColor(DWORD dwTextColor)
	{
		list_info_.dwDisabledTextColor = dwTextColor;
	}

	void Combo::SetDisabledItemBkColor(DWORD dwBkColor)
	{
		list_info_.dwDisabledBkColor = dwBkColor;
	}

	void Combo::SetDisabledItemImage(std::wstring pStrImage)
	{
		list_info_.sDisabledImage = pStrImage;
	}

	DWORD Combo::GetDisabledItemTextColor() const
	{
		return list_info_.dwDisabledTextColor;
	}

	DWORD Combo::GetDisabledItemBkColor() const
	{
		return list_info_.dwDisabledBkColor;
	}

	std::wstring Combo::GetDisabledItemImage() const
	{
		return list_info_.sDisabledImage;
	}

	DWORD Combo::GetItemLineColor() const
	{
		return list_info_.dwLineColor;
	}

	void Combo::SetItemLineColor(DWORD dwLineColor)
	{
		list_info_.dwLineColor = dwLineColor;
	}

	bool Combo::IsItemShowHtml()
	{
		return list_info_.bShowHtml;
	}

	void Combo::SetItemShowHtml(bool bShowHtml)
	{
		if( list_info_.bShowHtml == bShowHtml ) return;

		list_info_.bShowHtml = bShowHtml;
		Invalidate();
	}

	void Combo::SetPos(RECT rc, bool bNeedInvalidate)
	{
		// 隐藏下拉窗口
		if(combo_wnd_ && ::IsWindow(combo_wnd_->GetHWND())) combo_wnd_->Close();
		// 所有元素大小置为0
		RECT rcNull = { 0 };
		for( int i = 0; i < items_.GetSize(); i++ ) static_cast<Control*>(items_[i])->SetPos(rcNull);
		// 调整位置
		Control::SetPos(rc, bNeedInvalidate);
	}

	void Combo::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		Control::Move(szOffset, bNeedInvalidate);
	}
	void Combo::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();
		if( _tcsicmp(pstrName, _T("align")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				text_style_ &= ~(DT_CENTER | DT_RIGHT | DT_SINGLELINE);
				text_style_ |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				text_style_ &= ~(DT_LEFT | DT_RIGHT );
				text_style_ |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				text_style_ &= ~(DT_LEFT | DT_CENTER | DT_SINGLELINE);
				text_style_ |= DT_RIGHT;
			}
		}
		else if( _tcsicmp(pstrName, _T("valign")) == 0 ) {
			if( _tcsstr(pstrValue, _T("top")) != NULL ) {
				text_style_ &= ~(DT_BOTTOM | DT_VCENTER);
				text_style_ |= (DT_TOP | DT_SINGLELINE);
			}
			if( _tcsstr(pstrValue, _T("vcenter")) != NULL ) {
				text_style_ &= ~(DT_TOP | DT_BOTTOM );            
				text_style_ |= (DT_VCENTER | DT_SINGLELINE);
			}
			if( _tcsstr(pstrValue, _T("bottom")) != NULL ) {
				text_style_ &= ~(DT_TOP | DT_VCENTER);
				text_style_ |= (DT_BOTTOM | DT_SINGLELINE);
			}
		}
		else if( _tcsicmp(pstrName, _T("endellipsis")) == 0 ) {
			if( _tcsicmp(pstrValue, _T("true")) == 0 ) text_style_ |= DT_END_ELLIPSIS;
			else text_style_ &= ~DT_END_ELLIPSIS;
		}   
		else if( _tcsicmp(pstrName, _T("wordbreak")) == 0 ) {
			if( _tcsicmp(pstrValue, _T("true")) == 0 ) {
				text_style_ &= ~DT_SINGLELINE;
				text_style_ |= DT_WORDBREAK | DT_EDITCONTROL;
			}
			else {
				text_style_ &= ~DT_WORDBREAK & ~DT_EDITCONTROL;
				text_style_ |= DT_SINGLELINE;
			}
		}    
		else if( _tcsicmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("textcolor")) == 0 ) {
			SetTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("disabledtextcolor")) == 0 ) {
			SetDisabledTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("textpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetTextPadding(rcTextPadding);
		}
		else if( _tcsicmp(pstrName, _T("showhtml")) == 0 ) SetShowHtml(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("scrollselect")) == 0 ) SetScrollSelect(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("dropbox")) == 0 ) SetDropBoxAttributeList(pstrValue);
		else if( _tcsicmp(pstrName, _T("dropboxsize")) == 0)
		{
			SIZE szDropBoxSize = { 0 };
			LPTSTR pstr = NULL;
			szDropBoxSize.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szDropBoxSize.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			SetDropBoxSize(szDropBoxSize);
		}
		else if( _tcsicmp(pstrName, _T("itemfont")) == 0 ) list_info_.nFont = _ttoi(pstrValue);
		else if( _tcsicmp(pstrName, _T("itemalign")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				list_info_.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				list_info_.uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				list_info_.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				list_info_.uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				list_info_.uTextStyle &= ~(DT_LEFT | DT_CENTER);
				list_info_.uTextStyle |= DT_RIGHT;
			}
		}
		else if( _tcsicmp(pstrName, _T("itemendellipsis")) == 0 ) {
			if( _tcsicmp(pstrValue, _T("true")) == 0 ) list_info_.uTextStyle |= DT_END_ELLIPSIS;
			else list_info_.uTextStyle &= ~DT_END_ELLIPSIS;
		}   
		else if( _tcsicmp(pstrName, _T("itemtextpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetItemTextPadding(rcTextPadding);
		}
		else if( _tcsicmp(pstrName, _T("itemtextcolor")) == 0 ) {
			SetItemTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("itembkcolor")) == 0 ) {
			SetItemBkColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("itembkimage")) == 0 ) SetItemBkImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("itemaltbk")) == 0 ) SetAlternateBk(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("itemselectedtextcolor")) == 0 ) {
			SetSelectedItemTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("itemselectedbkcolor")) == 0 ) {
			SetSelectedItemBkColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("itemselectedimage")) == 0 ) SetSelectedItemImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("itemhottextcolor")) == 0 ) {
			SetHotItemTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("itemhotbkcolor")) == 0 ) {
			SetHotItemBkColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("itemhotimage")) == 0 ) SetHotItemImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("itemdisabledtextcolor")) == 0 ) {
			SetDisabledItemTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("itemdisabledbkcolor")) == 0 ) {
			SetDisabledItemBkColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("itemdisabledimage")) == 0 ) SetDisabledItemImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("itemlinecolor")) == 0 ) {
			SetItemLineColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcsicmp(pstrValue, _T("true")) == 0);
		else Box::SetAttribute(strName, strValue);
	}

	void Combo::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		Control::DoPaint(hDC, rcPaint);
	}

	void Combo::PaintStatusImage(HDC hDC)
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
		else if( (btn_state_ & UISTATE_PUSHED) != 0 ) {
			if( !pushed_image_.empty() ) {
				if( !DrawImage(hDC, pushed_image_) ) {}
				else return;
			}
		}
		else if( (btn_state_ & UISTATE_HOT) != 0 ) {
			if( !hot_image_.empty() ) {
				if( !DrawImage(hDC, hot_image_) ) {}
				else return;
			}
		}
		else if( (btn_state_ & UISTATE_FOCUSED) != 0 ) {
			if( !focused_image_.empty() ) {
				if( !DrawImage(hDC, focused_image_) ) {}
				else return;
			}
		}

		if( !normal_image_.empty() ) {
			if( !DrawImage(hDC,normal_image_) ) {}
			else return;
		}
	}

	void Combo::PaintText(HDC hDC)
	{
		if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();
		if( disabled_text_color_ == 0 ) disabled_text_color_ = GlobalManager::GetDefaultDisabledColor();

		RECT rc = rect_;
		rc.left += text_padding_rc_.left;
		rc.right -= text_padding_rc_.right;
		rc.top += text_padding_rc_.top;
		rc.bottom -= text_padding_rc_.bottom;

		std::wstring sText = GetText();
		if( sText.empty() ) return;
		int nLinks = 0;
		if( IsEnabled() ) {
			if( is_show_html_ )
				RenderEngine::DrawHtmlText(hDC, pm_, rc, sText, text_color_, NULL, NULL, nLinks, text_style_);
			else
				RenderEngine::DrawText(hDC, pm_, rc, sText, text_color_, font_, text_style_);
		}
		else {
			if( is_show_html_ )
				RenderEngine::DrawHtmlText(hDC, pm_, rc, sText, disabled_text_color_, NULL, NULL, nLinks, text_style_);
			else
				RenderEngine::DrawText(hDC, pm_, rc, sText, disabled_text_color_, font_, text_style_);
		}
	}

} // namespace DuiLib
