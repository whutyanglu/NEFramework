#include "stdafx.h"
#include "ListEx.h"

namespace ui {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ListEx)

	ListEx::ListEx() : m_pEditUI(NULL), m_pComboBoxUI(NULL), m_bAddMessageFilter(FALSE),m_nRow(-1),m_nColum(-1),m_pXCallback(NULL)
	{
		on_event_[kEventHeaderClick] += nbase::Bind(&ListEx::OnHeaderItemClick, this, std::placeholders::_1);
		on_event_[kEventItemChecked] += nbase::Bind(&ListEx::OnItemChecked, this, std::placeholders::_1);
	}

	std::wstring ListEx::GetClass() const
	{
		return _T("ListEx");
	}

	UINT ListEx::GetControlFlags() const
	{
		return UIFLAG_TABSTOP;
	}

	LPVOID ListEx::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("ListEx")) == 0 ) return static_cast<IListOwner*>(this);
		return List::GetInterface(pstrName);
	}

	BOOL ListEx::CheckColumEditable(int nColum)
	{
		ListContainerHeaderItem* pHItem = static_cast<ListContainerHeaderItem*>(m_pHeader->GetItemAt(nColum));
		return pHItem != NULL? pHItem->GetColumeEditable() : FALSE;
	}

	void ListEx::InitListCtrl()
	{
		if (!m_bAddMessageFilter){
			m_bAddMessageFilter = TRUE;
		}
	}

	Edit* ListEx::GetEditUI()
	{
		if (m_pEditUI == NULL)
		{
			m_pEditUI = new Edit;
			m_pEditUI->SetName(_T("ListEx_Edit"));
			std::wstring pDefaultAttributes = GetManager()->GetDefaultAttributeList(_T("Edit"));
			if( !pDefaultAttributes.empty() ) {
				m_pEditUI->ApplyAttributeList(pDefaultAttributes);
			}

			Add(m_pEditUI);
		}
		if (m_pComboBoxUI)
		{
			RECT rc = {0,0,0,0};
			m_pComboBoxUI->SetPos(rc);
		}

		return m_pEditUI;
	}

	BOOL ListEx::CheckColumComboBoxable(int nColum)
	{
		ListContainerHeaderItem* pHItem = static_cast<ListContainerHeaderItem*>(m_pHeader->GetItemAt(nColum));
		return pHItem != NULL? pHItem->GetColumeComboable() : FALSE;
	}

	ComboBox* ListEx::GetComboBoxUI()
	{
		if (m_pComboBoxUI == NULL)
		{
			m_pComboBoxUI = new ComboBox;
			m_pComboBoxUI->SetName(_T("ListEx_Combo"));
			std::wstring pDefaultAttributes = GetManager()->GetDefaultAttributeList(_T("Combo"));
			if( !pDefaultAttributes.empty() ) {
				m_pComboBoxUI->ApplyAttributeList(pDefaultAttributes);
			}

			Add(m_pComboBoxUI);
		}
		if (m_pEditUI)
		{
			RECT rc = {0,0,0,0};
			m_pEditUI->SetPos(rc);
		}

		return m_pComboBoxUI;
	}

	BOOL ListEx::CheckColumCheckBoxable(int nColum)
	{
		Control* p = m_pHeader->GetItemAt(nColum);
		ListContainerHeaderItem* pHItem = static_cast<ListContainerHeaderItem*>(p->GetInterface(_T("ListContainerHeaderItem")));
		return pHItem != NULL? pHItem->GetColumeCheckable() : FALSE;
	}

	// TODO : luyang 2017/01/04
	//void ListEx::Notify(EventArgs& msg)
	//{	
	//	std::wstring strName = Msg.sender_->GetName();

	//	//编辑框、组合框
	//	if (_tcsicmp(strName.c_str(), _T("ListEx_Edit")) == 0 && m_pEditUI && m_nRow >= 0 && m_nColum >= 0)
	//	{
	//		if(_tcsicmp(msg.sType.c_str(), DUI_MSGTYPE_SETFOCUS) == 0)
	//		{

	//		}
	//		else if(_tcsicmp(msg.sType.c_str(), DUI_MSGTYPE_KILLFOCUS) == 0)
	//		{
	//			std::wstring sText = m_pEditUI->GetText();
	//			ListTextExtElement* pRowCtrl = (ListTextExtElement*)GetItemAt(m_nRow);
	//			if (pRowCtrl)
	//			{
	//				pRowCtrl->SetText(m_nColum, sText.c_str());
	//			}

	//			//重置当前行列
	//			SetEditRowAndColum(-1, -1);

	//			//隐藏编辑框
	//			RECT rc = {0,0,0,0};
	//			m_pEditUI->SetPos(rc);

	//		}
	//	}
	//	else if (_tcsicmp(strName.c_str(), _T("ListEx_Combo")) == 0 && m_pComboBoxUI && m_nRow >= 0 && m_nColum >= 0)
	//	{
	//		int  iCurSel, iOldSel;
	//		iCurSel = msg.wParam;
	//		iOldSel = msg.lParam;

	//		if(_tcsicmp(msg.sType.c_str(), DUI_MSGTYPE_SETFOCUS) == 0)
	//		{

	//		}
	//		else if(_tcsicmp(msg.sType.c_str(), DUI_MSGTYPE_KILLFOCUS) == 0)
	//		{
	//		}
	//		else if(_tcsicmp(msg.sType.c_str(), DUI_MSGTYPE_LISTITEMSELECT) == 0 && iOldSel >= 0)
	//		{
	//			ListTextExtElement* pRowCtrl = (ListTextExtElement*)GetItemAt(m_nRow);
	//			if (pRowCtrl)
	//			{
	//				pRowCtrl->SetText(m_nColum, m_pComboBoxUI->GetText().c_str());
	//			}

	//			//隐藏组合框
	//			RECT rc = {0,0,0,0};
	//			m_pComboBoxUI->SetPos(rc);
	//		}
	//	}
	//	else if(_tcsicmp(msg.sType.c_str(), _T("scroll")) == 0 && (m_pComboBoxUI || m_pEditUI) && m_nRow >= 0 && m_nColum >= 0)
	//	{
	//		HideEditAndComboCtrl();
	//	}
	//}
	void ListEx::HideEditAndComboCtrl()
	{
		//隐藏编辑框
		RECT rc = {0,0,0,0};
		if(m_pEditUI)
		{	
			m_pEditUI->SetPos(rc);
		}

		if(m_pComboBoxUI)
		{	
			m_pComboBoxUI->SetPos(rc);
		}
	}
	IListComboCallback* ListEx::GetTextArrayCallback() const
	{
		return m_pXCallback;
	}

	void ListEx::SetTextArrayCallback(IListComboCallback* pCallback)
	{
		m_pXCallback = pCallback;
	}
	void ListEx::OnListItemClicked(int nIndex, int nColum, RECT* lpRCColum, std::wstring lpstrText)
	{
		RECT rc = {0,0,0,0};
		if (nColum < 0)
		{
			if (m_pEditUI)
			{
				m_pEditUI->SetPos(rc);
			}
			if (m_pComboBoxUI)
			{
				m_pComboBoxUI->SetPos(rc);
			}
		}
		else
		{
			if (CheckColumEditable(nColum) && GetEditUI())
			{
				//保存当前行列
				SetEditRowAndColum(nIndex, nColum);

				//设置文字
				m_pEditUI->SetText(lpstrText);

				//移动位置
				m_pEditUI->SetVisible(TRUE);
				m_pEditUI->SetPos(*lpRCColum);
			}
			else if(CheckColumComboBoxable(nColum) && GetComboBoxUI())
			{
				//重置组合框
				m_pComboBoxUI->RemoveAll();

				//保存当前行列
				SetEditRowAndColum(nIndex, nColum);

				//设置文字
				m_pComboBoxUI->SetText(lpstrText);

				//获取
				if (m_pXCallback)
				{
					m_pXCallback->GetItemComboTextArray(m_pComboBoxUI, nIndex, nColum);
				}

				//移动位置
				m_pComboBoxUI->SetPos(*lpRCColum);
				m_pComboBoxUI->SetVisible(TRUE);
			}
			else
			{
				if (m_pEditUI)
				{
					m_pEditUI->SetPos(rc);
				}
				if (m_pComboBoxUI)
				{
					m_pComboBoxUI->SetPos(rc);
				}
			}
		}
	}
	void ListEx::OnListItemChecked(int nIndex, int nColum, BOOL bChecked)
	{
		Control* p = m_pHeader->GetItemAt(nColum);
		ListContainerHeaderItem* pHItem = static_cast<ListContainerHeaderItem*>(p->GetInterface(_T("ListContainerHeaderItem")));
		if (pHItem == NULL)
		{
			return;
		}

		//如果选中，那么检查是否全部都处于选中状态
		if (bChecked)
		{
			BOOL bCheckAll = TRUE;
			for(int i = 0; i < GetCount(); i++) 
			{
				Control* p = GetItemAt(i);
				ListTextExtElement* pLItem = static_cast<ListTextExtElement*>(p->GetInterface(_T("ListTextExElement")));
				if( pLItem != NULL && !pLItem->GetCheck()) 
				{
					bCheckAll = FALSE;
					break;
				}
			}
			if (bCheckAll)
			{
				pHItem->SetCheck(TRUE);
			}
			else
			{
				pHItem->SetCheck(FALSE);
			}
		}
		else
		{
			pHItem->SetCheck(FALSE);
		}
	}
	void ListEx::DoEvent(EventArgs& event)
	{
		if (event.type_ == kEventMouseButtonDown)
		{
			HideEditAndComboCtrl();
		}

		List::DoEvent(event);
	}

	bool ListEx::OnHeaderItemClick(const EventArgs & arg)
	{
		BOOL bCheck = (BOOL)arg.l_param_;
		int  nIndex = arg.w_param_;
		//判断是否是本LIST发送的notify
		ListHeader* pHeader = GetHeader();
		for (int i = 0; i < pHeader->GetCount(); i++)
		{
			if (pHeader->GetItemAt(i) == arg.sender_){
				for (int i = 0; i < GetCount(); ++i) {
					Control* p = GetItemAt(i);
					ListTextExtElement* pLItem = static_cast<ListTextExtElement*>(p->GetInterface(_T("ListTextExElement")));
					if (pLItem != NULL) {
						pLItem->SetCheck(bCheck);
					}
				}
				break;
			}
		}

		return true;
	}

	bool ListEx::OnItemChecked(const EventArgs & arg)
	{
		for (int i = 0; i < GetCount(); ++i) {
			Control* p = GetItemAt(i);
			ListTextExtElement* pLItem = static_cast<ListTextExtElement*>(p->GetInterface(_T("ListTextExElement")));
			if (pLItem != NULL && pLItem == arg.sender_)
			{
				OnListItemChecked(LOWORD(arg.w_param_), HIWORD(arg.w_param_), arg.l_param_);
				break;
			}
		}
		return false;
	}

	void ListEx::SetColumItemColor(int nIndex, int nColum, DWORD iBKColor)
	{
		Control* p = GetItemAt(nIndex);
		ListTextExtElement* pLItem = static_cast<ListTextExtElement*>(p->GetInterface(_T("ListTextExElement")));
		if( pLItem != NULL) 
		{
			DWORD iTextBkColor = 0xFFFF0000;
			pLItem->SetColumItemColor(nColum, iTextBkColor);
		}
	}

	BOOL ListEx::GetColumItemColor(int nIndex, int nColum, DWORD& iBKColor)
	{
		Control* p = GetItemAt(nIndex);
		ListTextExtElement* pLItem = static_cast<ListTextExtElement*>(p->GetInterface(_T("ListTextExElement")));
		if( pLItem == NULL) 
		{
			return FALSE;
		}
		pLItem->GetColumItemColor(nColum, iBKColor);
		return TRUE;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ListContainerHeaderItem)

	ListContainerHeaderItem::ListContainerHeaderItem() : dragable_(TRUE), btn_state_(0), sep_width_(4),
		text_style_(DT_VCENTER | DT_CENTER | DT_SINGLELINE), text_color_(0), font_(-1), show_html_(FALSE),
		editable_(FALSE),comboable_(FALSE),checkboxable_(FALSE),checkbox_state_(0),checked_(FALSE),owner_(NULL)
	{
		SetTextPadding(CDuiRect(2, 0, 2, 0));
		last_mouse_pt_.x = last_mouse_pt_.y = 0;
		SetMinWidth(16);
	}

	std::wstring ListContainerHeaderItem::GetClass() const
	{
		return _T("ListContainerHeaderItem");
	}

	LPVOID ListContainerHeaderItem::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("ListContainerHeaderItem")) == 0 ) return this;
		return Box::GetInterface(pstrName);
	}

	UINT ListContainerHeaderItem::GetControlFlags() const
	{
		if( IsEnabled() && sep_width_ != 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void ListContainerHeaderItem::SetEnabled(BOOL bEnable)
	{
		Box::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			btn_state_ = 0;
		}
	}

	BOOL ListContainerHeaderItem::IsDragable() const
	{
		return dragable_;
	}

	void ListContainerHeaderItem::SetDragable(BOOL bDragable)
	{
		dragable_ = bDragable;
		if ( !dragable_ ) btn_state_ &= ~UISTATE_CAPTURED;
	}

	DWORD ListContainerHeaderItem::GetSepWidth() const
	{
		return sep_width_;
	}

	void ListContainerHeaderItem::SetSepWidth(int iWidth)
	{
		sep_width_ = iWidth;
	}

	DWORD ListContainerHeaderItem::GetTextStyle() const
	{
		return text_style_;
	}

	void ListContainerHeaderItem::SetTextStyle(UINT uStyle)
	{
		text_style_ = uStyle;
		Invalidate();
	}

	DWORD ListContainerHeaderItem::GetTextColor() const
	{
		return text_color_;
	}


	void ListContainerHeaderItem::SetTextColor(DWORD dwTextColor)
	{
		text_color_ = dwTextColor;
	}

	RECT ListContainerHeaderItem::GetTextPadding() const
	{
		return text_padding_rect_;
	}

	void ListContainerHeaderItem::SetTextPadding(RECT rc)
	{
		text_padding_rect_ = rc;
		Invalidate();
	}

	void ListContainerHeaderItem::SetFont(int index)
	{
		font_ = index;
	}

	BOOL ListContainerHeaderItem::IsShowHtml()
	{
		return show_html_;
	}

	void ListContainerHeaderItem::SetShowHtml(BOOL bShowHtml)
	{
		if( show_html_ == bShowHtml ) return;

		show_html_ = bShowHtml;
		Invalidate();
	}

	std::wstring ListContainerHeaderItem::GetNormalImage() const
	{
		return normal_image_;
	}

	void ListContainerHeaderItem::SetNormalImage(std::wstring pStrImage)
	{
		normal_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ListContainerHeaderItem::GetHotImage() const
	{
		return hot_image_;
	}

	void ListContainerHeaderItem::SetHotImage(std::wstring pStrImage)
	{
		hot_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ListContainerHeaderItem::GetPushedImage() const
	{
		return pushed_image_;
	}

	void ListContainerHeaderItem::SetPushedImage(std::wstring pStrImage)
	{
		pushed_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ListContainerHeaderItem::GetFocusedImage() const
	{
		return focused_image_;
	}

	void ListContainerHeaderItem::SetFocusedImage(std::wstring pStrImage)
	{
		focused_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ListContainerHeaderItem::GetSepImage() const
	{
		return sep_image_;
	}

	void ListContainerHeaderItem::SetSepImage(std::wstring pStrImage)
	{
		sep_image_ = pStrImage;
		Invalidate();
	}

	void ListContainerHeaderItem::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("dragable")) == 0 ) SetDragable(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("align")) == 0 ) 
		{
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				text_style_ &= ~(DT_CENTER | DT_RIGHT);
				text_style_ |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				text_style_ &= ~(DT_LEFT | DT_RIGHT);
				text_style_ |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				text_style_ &= ~(DT_LEFT | DT_CENTER);
				text_style_ |= DT_RIGHT;
			}
		}
		else if( _tcsicmp(pstrName, _T("endellipsis")) == 0 ) 
		{
			if( _tcsicmp(pstrValue, _T("true")) == 0 ) text_style_ |= DT_END_ELLIPSIS;
			else text_style_ &= ~DT_END_ELLIPSIS;
		}    
		else if( _tcsicmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("textcolor")) == 0 ) 
		{
			SetTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("textpadding")) == 0 ) 
		{
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10);
			SetTextPadding(rcTextPadding);
		}
		else if( _tcsicmp(pstrName, _T("showhtml")) == 0 ) SetShowHtml(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("sepimage")) == 0 ) SetSepImage(pstrValue);

		else if( _tcsicmp(pstrName, _T("editable")) == 0 ) SetColumeEditable(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("comboable")) == 0 ) SetColumeComboable(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("checkable")) == 0 ) SetColumeCheckable(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("checkboxwidth")) == 0 ) SetCheckBoxWidth(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("checkboxheight")) == 0 ) SetCheckBoxHeight(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("checkboxnormalimage")) == 0 ) SetCheckBoxNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxhotimage")) == 0 ) SetCheckBoxHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxpushedimage")) == 0 ) SetCheckBoxPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxfocusedimage")) == 0 ) SetCheckBoxFocusedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxdisabledimage")) == 0 ) SetCheckBoxDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxselectedimage")) == 0 ) SetCheckBoxSelectedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxforeimage")) == 0 ) SetCheckBoxForeImage(pstrValue);

		else Box::SetAttribute(strName, strValue);
	}

	void ListContainerHeaderItem::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Box::DoEvent(event);
			return;
		}

		//CheckBoxAble
		if (checkboxable_)
		{
			RECT rcCheckBox;
			GetCheckBoxRect(rcCheckBox);

			if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick )
			{
				if( ::PtInRect(&rcCheckBox, event.mouse_point_)) 
				{
					checkbox_state_ |= UISTATE_PUSHED | UISTATE_CAPTURED;
					Invalidate();
				}
			}
			else if( event.type_ == kEventMouseMove )
			{
				if( (checkbox_state_ & UISTATE_CAPTURED) != 0 ) 
				{
					if( ::PtInRect(&rcCheckBox, event.mouse_point_) ) 
						checkbox_state_ |= UISTATE_PUSHED;
					else 
						checkbox_state_ &= ~UISTATE_PUSHED;
					Invalidate();
				}
				else if (::PtInRect(&rcCheckBox, event.mouse_point_))
				{
					checkbox_state_ |= UISTATE_HOT;
					Invalidate();
				}
				else
				{
					checkbox_state_ &= ~UISTATE_HOT;
					Invalidate();
				}
			}
			else if( event.type_ == kEventMouseButtonUp )
			{
				if( (checkbox_state_ & UISTATE_CAPTURED) != 0 )
				{
					if( ::PtInRect(&rcCheckBox, event.mouse_point_) ) {
						SetCheck(!GetCheck());
						Box* pOwner = (Box*)parent_;
						Notify(kEventHeaderItemClick, pOwner->GetItemIndex(this), checked_);
					}
					checkbox_state_ &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
					Invalidate();
				}
				else if (::PtInRect(&rcCheckBox, event.mouse_point_))
				{

				}
			}
			else if( event.type_ == kEventMouseEnter )
			{
				if( ::PtInRect(&rcCheckBox, event.mouse_point_) ) 
				{
					checkbox_state_ |= UISTATE_HOT;
					Invalidate();
				}
			}
			else if( event.type_ == kEventMouseLeave )
			{
				checkbox_state_ &= ~UISTATE_HOT;
				Invalidate();
			}
		}

		if( event.type_ == kEventSetFocus ) 
		{
			Invalidate();
		}
		if( event.type_ == kEventKillFocus ) 
		{
			Invalidate();
		}
		if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick )
		{
			if( !IsEnabled() ) return;
			RECT rcSeparator = GetThumbRect();
			if (sep_width_>=0)
				rcSeparator.left-=4;
			else
				rcSeparator.right+=4;
			if( ::PtInRect(&rcSeparator, event.mouse_point_) ) {
				if( dragable_ ) {
					btn_state_ |= UISTATE_CAPTURED;
					last_mouse_pt_ = event.mouse_point_;
				}
			}
			else {
				btn_state_ |= UISTATE_PUSHED;
				Notify(kEventHeaderClick);
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseButtonUp )
		{
			if( (btn_state_ & UISTATE_CAPTURED) != 0 ) {
				btn_state_ &= ~UISTATE_CAPTURED;
				if( GetParent() ) 
					GetParent()->NeedParentUpdate();
			}
			else if( (btn_state_ & UISTATE_PUSHED) != 0 ) {
				btn_state_ &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseMove )
		{
			if( (btn_state_ & UISTATE_CAPTURED) != 0 ) {
				RECT rc = rect_;
				if( sep_width_ >= 0 ) {
					rc.right -= last_mouse_pt_.x - event.mouse_point_.x;
				}
				else {
					rc.left -= last_mouse_pt_.x - event.mouse_point_.x;
				}

				if( rc.right - rc.left > GetMinWidth() ) {
					cxy_fired_sz_.cx = rc.right - rc.left;
					last_mouse_pt_ = event.mouse_point_;
					if( GetParent() ) 
						GetParent()->NeedParentUpdate();
				}
			}
			return;
		}
		if( event.type_ == kEventSetCursor )
		{
			RECT rcSeparator = GetThumbRect();
			if (sep_width_>=0)
				rcSeparator.left-=4;
			else
				rcSeparator.right+=4;
			if( IsEnabled() && dragable_ && ::PtInRect(&rcSeparator, event.mouse_point_) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
				return;
			}
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
		Box::DoEvent(event);
	}

	SIZE ListContainerHeaderItem::EstimateSize(SIZE szAvailable)
	{
		if( cxy_fired_sz_.cy == 0 ) return CDuiSize(cxy_fired_sz_.cx, 
			GlobalManager::GetDefaultFontInfo(pm_->GetPaintDC())->tm_.tmHeight + 14);
		return Box::EstimateSize(szAvailable);
	}

	RECT ListContainerHeaderItem::GetThumbRect() const
	{
		if( sep_width_ >= 0 ) return CDuiRect(rect_.right - sep_width_, rect_.top, rect_.right, rect_.bottom);
		else return CDuiRect(rect_.left, rect_.top, rect_.left - sep_width_, rect_.bottom);
	}

	void ListContainerHeaderItem::PaintStatusImage(HDC hDC)
	{
		//HeadItem Bkgnd
		if( IsFocused() ) btn_state_ |= UISTATE_FOCUSED;
		else btn_state_ &= ~ UISTATE_FOCUSED;

		if( (btn_state_ & UISTATE_PUSHED) != 0 ) {
			if( pushed_image_.empty() && !normal_image_.empty() ) DrawImage(hDC, normal_image_);
			if( !DrawImage(hDC, pushed_image_) ) {}
		}
		else if( (btn_state_ & UISTATE_HOT) != 0 ) {
			if( hot_image_.empty() && !normal_image_.empty() ) DrawImage(hDC, normal_image_);
			if( !DrawImage(hDC, hot_image_) ) {}
		}
		else if( (btn_state_ & UISTATE_FOCUSED) != 0 ) {
			if( focused_image_.empty() && !normal_image_.empty() ) DrawImage(hDC, normal_image_);
			if( !DrawImage(hDC, focused_image_) ) {}
		}
		else {
			if( !normal_image_.empty() ) {
				if( !DrawImage(hDC, normal_image_) ) {}
			}
		}

		if( !sep_image_.empty() ) {
			RECT rcThumb = GetThumbRect();
			rcThumb.left -= rect_.left;
			rcThumb.top -= rect_.top;
			rcThumb.right -= rect_.left;
			rcThumb.bottom -= rect_.top;

			sep_image_modify_.clear();
			sep_image_modify_ = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			if( !DrawImage(hDC, sep_image_, sep_image_modify_) ) {}
		}

		if(checkboxable_)
		{
			checkbox_state_ &= ~UISTATE_PUSHED;

			if( (checkbox_state_ & UISTATE_SELECTED) != 0 ) {
				if( !checkbox_selected_image_.empty() ) {
					if( !DrawCheckBoxImage(hDC, checkbox_selected_image_) ) {}
					else goto Label_ForeImage;
				}
			}

			if( IsFocused() ) checkbox_state_ |= UISTATE_FOCUSED;
			else checkbox_state_ &= ~ UISTATE_FOCUSED;
			if( !IsEnabled() ) checkbox_state_ |= UISTATE_DISABLED;
			else checkbox_state_ &= ~ UISTATE_DISABLED;

			if( (checkbox_state_ & UISTATE_DISABLED) != 0 ) {
				if( !checkbox_disabled_image_.empty() ) {
					if( !DrawCheckBoxImage(hDC, checkbox_disabled_image_) ) {}
					else return;
				}
			}
			else if( (checkbox_state_ & UISTATE_PUSHED) != 0 ) {
				if( !checkbox_pushed_image_.empty() ) {
					if( !DrawCheckBoxImage(hDC, checkbox_pushed_image_) ) {}
					else return;
				}
			}
			else if( (checkbox_state_ & UISTATE_HOT) != 0 ) {
				if( !checkbox_hot_image_.empty() ) {
					if( !DrawCheckBoxImage(hDC, checkbox_hot_image_) ) {}
					else return;
				}
			}
			else if( (checkbox_state_ & UISTATE_FOCUSED) != 0 ) {
				if( !checkbox_focused_image_.empty() ) {
					if( !DrawCheckBoxImage(hDC, checkbox_focused_image_) ) {}
					else return;
				}
			}

			if( !checkbox_normal_image_.empty() ) {
				if( !DrawCheckBoxImage(hDC, checkbox_normal_image_) ) {}
				else return;
			}

Label_ForeImage:
			if( !checkbox_fore_image_.empty() ) {
				if( !DrawCheckBoxImage(hDC, checkbox_fore_image_) ) {}
			}
		}
	}

	void ListContainerHeaderItem::PaintText(HDC hDC)
	{
		if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();

		RECT rcText = rect_;
		rcText.left += text_padding_rect_.left;
		rcText.top += text_padding_rect_.top;
		rcText.right -= text_padding_rect_.right;
		rcText.bottom -= text_padding_rect_.bottom;
		if (checkboxable_) {
			RECT rcCheck;
			GetCheckBoxRect(rcCheck);
			rcText.left += (rcCheck.right - rcCheck.left);
		}

		std::wstring sText = GetText();
		if( sText.empty() ) return;

		int nLinks = 0;
		if( show_html_ )
			RenderEngine::DrawHtmlText(hDC, pm_, rcText, sText, text_color_, \
			NULL, NULL, nLinks, DT_SINGLELINE | text_style_);
		else
			RenderEngine::DrawText(hDC, pm_, rcText, sText, text_color_, \
			font_, DT_SINGLELINE | text_style_);
	}

	BOOL ListContainerHeaderItem::GetColumeEditable()
	{
		return editable_;
	}

	void ListContainerHeaderItem::SetColumeEditable(BOOL bEnable)
	{
		editable_ = bEnable;
	}

	BOOL ListContainerHeaderItem::GetColumeComboable()
	{
		return comboable_;
	}

	void ListContainerHeaderItem::SetColumeComboable(BOOL bEnable)
	{
		comboable_ = bEnable;
	}

	BOOL ListContainerHeaderItem::GetColumeCheckable()
	{
		return checkboxable_;
	}
	void ListContainerHeaderItem::SetColumeCheckable(BOOL bEnable)
	{
		checkboxable_ = bEnable;
	}
	void ListContainerHeaderItem::SetCheck(BOOL bCheck)
	{
		if( checked_ == bCheck ) return;
		checked_ = bCheck;
		if( checked_ ) checkbox_state_ |= UISTATE_SELECTED;
		else checkbox_state_ &= ~UISTATE_SELECTED;
		Invalidate();
	}

	BOOL ListContainerHeaderItem::GetCheck()
	{
		return checked_;
	}
	BOOL ListContainerHeaderItem::DrawCheckBoxImage(HDC hDC, std::wstring pStrImage, std::wstring pStrModify)
	{
		RECT rcCheckBox;
		GetCheckBoxRect(rcCheckBox);
		return RenderEngine::DrawImageString(hDC, pm_, rcCheckBox, paint_rect_, pStrImage, pStrModify);
	}
	std::wstring ListContainerHeaderItem::GetCheckBoxNormalImage()
	{
		return checkbox_normal_image_;
	}

	void ListContainerHeaderItem::SetCheckBoxNormalImage(std::wstring pStrImage)
	{
		checkbox_normal_image_ = pStrImage;
	}

	std::wstring ListContainerHeaderItem::GetCheckBoxHotImage()
	{
		return checkbox_hot_image_;
	}

	void ListContainerHeaderItem::SetCheckBoxHotImage(std::wstring pStrImage)
	{
		checkbox_hot_image_ = pStrImage;
	}

	std::wstring ListContainerHeaderItem::GetCheckBoxPushedImage()
	{
		return checkbox_pushed_image_;
	}

	void ListContainerHeaderItem::SetCheckBoxPushedImage(std::wstring pStrImage)
	{
		checkbox_pushed_image_ = pStrImage;
	}

	std::wstring ListContainerHeaderItem::GetCheckBoxFocusedImage()
	{
		return checkbox_focused_image_;
	}

	void ListContainerHeaderItem::SetCheckBoxFocusedImage(std::wstring pStrImage)
	{
		checkbox_focused_image_ = pStrImage;
	}

	std::wstring ListContainerHeaderItem::GetCheckBoxDisabledImage()
	{
		return checkbox_disabled_image_;
	}

	void ListContainerHeaderItem::SetCheckBoxDisabledImage(std::wstring pStrImage)
	{
		checkbox_disabled_image_ = pStrImage;
	}
	std::wstring ListContainerHeaderItem::GetCheckBoxSelectedImage()
	{
		return checkbox_selected_image_;
	}

	void ListContainerHeaderItem::SetCheckBoxSelectedImage(std::wstring pStrImage)
	{
		checkbox_selected_image_ = pStrImage;
	}
	std::wstring ListContainerHeaderItem::GetCheckBoxForeImage()
	{
		return checkbox_fore_image_;
	}

	void ListContainerHeaderItem::SetCheckBoxForeImage(std::wstring pStrImage)
	{
		checkbox_fore_image_ = pStrImage;
	}
	int ListContainerHeaderItem::GetCheckBoxWidth() const
	{
		return cxy_checkbox_.cx;
	}

	void ListContainerHeaderItem::SetCheckBoxWidth(int cx)
	{
		if( cx < 0 ) return; 
		cxy_checkbox_.cx = cx;
	}

	int ListContainerHeaderItem::GetCheckBoxHeight()  const 
	{
		return cxy_checkbox_.cy;
	}

	void ListContainerHeaderItem::SetCheckBoxHeight(int cy)
	{
		if( cy < 0 ) return; 
		cxy_checkbox_.cy = cy;
	}
	void ListContainerHeaderItem::GetCheckBoxRect(RECT &rc)
	{
		memset(&rc, 0x00, sizeof(rc)); 
		int nItemHeight = rect_.bottom - rect_.top;
		rc.left = rect_.left + 6;
		rc.top = rect_.top + (nItemHeight - GetCheckBoxHeight()) / 2;
		rc.right = rc.left + GetCheckBoxWidth();
		rc.bottom = rc.top + GetCheckBoxHeight();
	}

	void ListContainerHeaderItem::SetOwner(Box* pOwner)
	{
		owner_ = pOwner;
	}
	Box* ListContainerHeaderItem::GetOwner()
	{
		return owner_;
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ListTextExtElement)

	ListTextExtElement::ListTextExtElement() : 
		link_id_(0), hover_link_(-1), owner_(NULL),checkbox_state_(0),checked_(FALSE)
	{
		::ZeroMemory(&link_rects_, sizeof(link_rects_));
		cxy_checkbox_.cx = cxy_checkbox_.cy = 0;

		::ZeroMemory(&ColumCorlorArray, sizeof(ColumCorlorArray));
	}

	ListTextExtElement::~ListTextExtElement()
	{
		std::wstring* pText;
		for( int it = 0; it < texts_.GetSize(); it++ ) {
			pText = static_cast<std::wstring*>(texts_[it]);
			if( pText ) delete pText;
		}
		texts_.Empty();
	}

	std::wstring ListTextExtElement::GetClass() const
	{
		return _T("ListTextExElement");
	}

	LPVOID ListTextExtElement::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("ListTextExElement")) == 0 ) return static_cast<ListTextExtElement*>(this);
		return ListLabelElement::GetInterface(pstrName);
	}

	UINT ListTextExtElement::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN | ( (IsEnabled() && links_ > 0) ? UIFLAG_SETCURSOR : 0);
	}

	std::wstring ListTextExtElement::GetText(int iIndex) const
	{
		std::wstring* pText = static_cast<std::wstring*>(texts_.GetAt(iIndex));
		if( pText ) return pText->c_str();
		return NULL;
	}

	void ListTextExtElement::SetText(int iIndex, std::wstring pstrText)
	{
		if( owner_ == NULL ) return;
		TListInfo* pInfo = owner_->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( texts_.GetSize() < pInfo->nColumns ) { texts_.Add(NULL); }

		std::wstring* pText = static_cast<std::wstring*>(texts_[iIndex]);
		if( (pText == NULL && pstrText.empty()) || (pText && *pText == pstrText) ) return;

		if ( pText )
			pText->assign(pstrText);
		else
			texts_.SetAt(iIndex, new std::wstring(pstrText));
		Invalidate();
	}

	void ListTextExtElement::SetOwner(Control* pOwner)
	{
		ListElement::SetOwner(pOwner);
		owner_ = static_cast<List*>(pOwner->GetInterface(_T("List")));
	}

	std::wstring* ListTextExtElement::GetLinkContent(int iIndex)
	{
		if( iIndex >= 0 && iIndex < link_id_) return &links_[iIndex];
		return NULL;
	}

	void ListTextExtElement::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( owner_ != NULL ) owner_->DoEvent(event);
			else ListLabelElement::DoEvent(event);
			return;
		}

		// When you hover over a link
		if( event.type_ == kEventSetCursor ) {
			for( int i = 0; i < link_id_; i++ ) {
				if( ::PtInRect(&link_rects_[i], event.mouse_point_) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}      
		}
		if( event.type_ == kEventMouseButtonUp && IsEnabled() ) {
			for( int i = 0; i < link_id_; i++ ) {
				if( ::PtInRect(&link_rects_[i], event.mouse_point_) ) {
					//pm_->SendNotify(this, DUI_MSGTYPE_LINK, i);
					Notify(kEventItemLinkClick, i);
					return;
				}
			}
		}
		if( links_ > 0 && event.type_ == kEventMouseMove ) {
			int nHoverLink = -1;
			for( int i = 0; i < link_id_; i++ ) {
				if( ::PtInRect(&link_rects_[i], event.mouse_point_) ) {
					nHoverLink = i;
					break;
				}
			}

			if(hover_link_ != nHoverLink) {
				Invalidate();
				hover_link_ = nHoverLink;
			}
		}
		if( links_ > 0 && event.type_ == kEventMouseLeave ) {
			if(hover_link_ != -1) {
				Invalidate();
				hover_link_ = -1;
			}
		}

		//检查是否需要显示编辑框或者组合框	
		ListEx * pListCtrl = (ListEx *)owner_;
		int nColum = HitTestColum(event.mouse_point_);
		if(event.type_ == kEventMouseButtonDown && owner_->IsFocused())
		{
			RECT rc = {0,0,0,0};
			if (nColum >= 0)
			{
				GetColumRect(nColum, rc);
				::InflateRect(&rc, -2, -2);
			}

			pListCtrl->OnListItemClicked(GetIndex(), nColum, &rc, GetText(nColum));
		}

		//检查是否需要显示CheckBox
		TListInfo* pInfo = owner_->GetListInfo();
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			if (pListCtrl->CheckColumCheckBoxable(i))
			{
				RECT rcCheckBox;
				GetCheckBoxRect(i, rcCheckBox);

				if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick )
				{
					if( ::PtInRect(&rcCheckBox, event.mouse_point_)) 
					{
						checkbox_state_ |= UISTATE_PUSHED | UISTATE_CAPTURED;
						Invalidate();
					}
				}
				else if( event.type_ == kEventMouseMove )
				{
					if( (checkbox_state_ & UISTATE_CAPTURED) != 0 ) 
					{
						if( ::PtInRect(&rcCheckBox, event.mouse_point_) ) 
							checkbox_state_ |= UISTATE_PUSHED;
						else 
							checkbox_state_ &= ~UISTATE_PUSHED;
						Invalidate();
					}
				}
				else if( event.type_ == kEventMouseButtonUp )
				{
					if( (checkbox_state_ & UISTATE_CAPTURED) != 0 ){
						if( ::PtInRect(&rcCheckBox, event.mouse_point_) ) {
							SetCheck(!GetCheck());
							Notify(kEventItemChecked, MAKEWPARAM(GetIndex(), 0), checked_);
						}
						checkbox_state_ &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
						Invalidate();
					}
				}
				else if( event.type_ == kEventMouseEnter )
				{
					if( ::PtInRect(&rcCheckBox, event.mouse_point_) ) {
						checkbox_state_ |= UISTATE_HOT;
						Invalidate();
					}
				}
				else if( event.type_ == kEventMouseLeave ){
					checkbox_state_ &= ~UISTATE_HOT;
					Invalidate();
				}
			}
		}

		ListLabelElement::DoEvent(event);
	}

	SIZE ListTextExtElement::EstimateSize(SIZE szAvailable)
	{
		TListInfo* pInfo = NULL;
		if( owner_ ) pInfo = owner_->GetListInfo();

		SIZE cXY = cxy_fired_sz_;
		if( cXY.cy == 0 && pm_ != NULL ) {
			cXY.cy = GlobalManager::GetFontInfo(pInfo->nFont, pm_->GetPaintDC())->tm_.tmHeight + 8;
			if( pInfo ) cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		return cXY;
	}

	void ListTextExtElement::DrawItemText(HDC hDC, const RECT& rcItem)
	{
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
		IListCallback* pCallback = owner_->GetTextCallback();
		//DUIASSERT(pCallback);
		//if( pCallback == NULL ) return;

		ListEx * pListCtrl = (ListEx *)owner_;
		link_id_ = 0;
		int nLinks = lengthof(link_rects_);
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			RECT rcItem = { pInfo->rcColumn[i].left, rect_.top, pInfo->rcColumn[i].right, rect_.bottom };

			DWORD iTextBkColor = 0;
			if (GetColumItemColor(i, iTextBkColor))
			{	
				RenderEngine::DrawColor(hDC, rcItem, iTextBkColor);
			}

			rcItem.left += pInfo->rcTextPadding.left;
			rcItem.right -= pInfo->rcTextPadding.right;
			rcItem.top += pInfo->rcTextPadding.top;
			rcItem.bottom -= pInfo->rcTextPadding.bottom;

			//检查是否需要显示CheckBox
			if (pListCtrl->CheckColumCheckBoxable(i))
			{
				RECT rcCheckBox;
				GetCheckBoxRect(i, rcCheckBox);
				rcItem.left += (rcCheckBox.right - rcCheckBox.left);
			}

			std::wstring strText;//不使用std::wstring，否则限制太多 by cddjr 2011/10/20
			if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);
			else strText.assign(GetText(i));
			if( pInfo->bShowHtml )
				RenderEngine::DrawHtmlText(hDC, pm_, rcItem, strText, iTextColor, \
				&link_rects_[link_id_], &links_[link_id_], nLinks, DT_SINGLELINE | pInfo->uTextStyle);
			else
				RenderEngine::DrawText(hDC, pm_, rcItem, strText, iTextColor, \
				pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);

			link_id_ += nLinks;
			nLinks = lengthof(link_rects_) - link_id_;
		}
		for( int i = link_id_; i < lengthof(link_rects_); i++ ) {
			::ZeroMemory(link_rects_ + i, sizeof(RECT));
			((std::wstring*)(links_ + i))->clear();
		}
	}
	void ListTextExtElement::PaintStatusImage(HDC hDC)
	{
		ListEx * pListCtrl = (ListEx *)owner_;
		TListInfo* pInfo = owner_->GetListInfo();
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			if (pListCtrl->CheckColumCheckBoxable(i))
			{
				RECT rcCheckBox;
				GetCheckBoxRect(i, rcCheckBox);

				checkbox_state_ &= ~UISTATE_PUSHED;

				if( (checkbox_state_ & UISTATE_SELECTED) != 0 ) {
					if( !checkbox_selected_image_.empty() ) {
						if( !DrawCheckBoxImage(hDC, checkbox_selected_image_, L"", rcCheckBox) ) {}
						else goto Label_ForeImage;
					}
				}

				if( IsFocused() ) checkbox_state_ |= UISTATE_FOCUSED;
				else checkbox_state_ &= ~ UISTATE_FOCUSED;
				if( !IsEnabled() ) checkbox_state_ |= UISTATE_DISABLED;
				else checkbox_state_ &= ~ UISTATE_DISABLED;

				if( (checkbox_state_ & UISTATE_DISABLED) != 0 ) {
					if( !checkbox_disabled_image_.empty() ) {
						if( !DrawCheckBoxImage(hDC, checkbox_disabled_image_, L"", rcCheckBox) ) {}
						else return;
					}
				}
				else if( (checkbox_state_ & UISTATE_PUSHED) != 0 ) {
					if( !checkbox_pushed_image_.empty() ) {
						if( !DrawCheckBoxImage(hDC, checkbox_pushed_image_, L"", rcCheckBox) ) {}
						else return;
					}
				}
				else if( (checkbox_state_ & UISTATE_HOT) != 0 ) {
					if( !checkbox_hot_image_.empty() ) {
						if( !DrawCheckBoxImage(hDC, checkbox_hot_image_, L"", rcCheckBox) ) {}
						else return;
					}
				}
				else if( (checkbox_state_ & UISTATE_FOCUSED) != 0 ) {
					if( !checkbox_focused_image_.empty() ) {
						if( !DrawCheckBoxImage(hDC, checkbox_focused_image_, L"", rcCheckBox) ) {}
						else return;
					}
				}

				if( !checkbox_normal_image_.empty() ) {
					if( !DrawCheckBoxImage(hDC, checkbox_normal_image_, L"", rcCheckBox) ) {}
					else return;
				}

Label_ForeImage:
				if( !checkbox_fore_image_.empty() ) {
					if( !DrawCheckBoxImage(hDC, checkbox_fore_image_, L"", rcCheckBox) ) {}
				}
			}
		}
	}
	BOOL ListTextExtElement::DrawCheckBoxImage(HDC hDC, std::wstring pStrImage, std::wstring pStrModify, RECT& rcCheckBox)
	{
		return RenderEngine::DrawImageString(hDC, pm_, rcCheckBox, paint_rect_, pStrImage, pStrModify);
	}
	void ListTextExtElement::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("checkboxwidth")) == 0 ) SetCheckBoxWidth(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("checkboxheight")) == 0 ) SetCheckBoxHeight(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("checkboxnormalimage")) == 0 ) SetCheckBoxNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxhotimage")) == 0 ) SetCheckBoxHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxpushedimage")) == 0 ) SetCheckBoxPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxfocusedimage")) == 0 ) SetCheckBoxFocusedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxdisabledimage")) == 0 ) SetCheckBoxDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxselectedimage")) == 0 ) SetCheckBoxSelectedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("checkboxforeimage")) == 0 ) SetCheckBoxForeImage(pstrValue);
		else ListLabelElement::SetAttribute(strName, strValue);
	}
	std::wstring ListTextExtElement::GetCheckBoxNormalImage()
	{
		return checkbox_normal_image_;
	}

	void ListTextExtElement::SetCheckBoxNormalImage(std::wstring pStrImage)
	{
		checkbox_normal_image_ = pStrImage;
	}

	std::wstring ListTextExtElement::GetCheckBoxHotImage()
	{
		return checkbox_hot_image_;
	}

	void ListTextExtElement::SetCheckBoxHotImage(std::wstring pStrImage)
	{
		checkbox_hot_image_ = pStrImage;
	}

	std::wstring ListTextExtElement::GetCheckBoxPushedImage()
	{
		return checkbox_pushed_image_;
	}

	void ListTextExtElement::SetCheckBoxPushedImage(std::wstring pStrImage)
	{
		checkbox_pushed_image_ = pStrImage;
	}

	std::wstring ListTextExtElement::GetCheckBoxFocusedImage()
	{
		return checkbox_focused_image_;
	}

	void ListTextExtElement::SetCheckBoxFocusedImage(std::wstring pStrImage)
	{
		checkbox_focused_image_ = pStrImage;
	}

	std::wstring ListTextExtElement::GetCheckBoxDisabledImage()
	{
		return checkbox_disabled_image_;
	}

	void ListTextExtElement::SetCheckBoxDisabledImage(std::wstring pStrImage)
	{
		checkbox_disabled_image_ = pStrImage;
	}
	std::wstring ListTextExtElement::GetCheckBoxSelectedImage()
	{
		return checkbox_selected_image_;
	}

	void ListTextExtElement::SetCheckBoxSelectedImage(std::wstring pStrImage)
	{
		checkbox_selected_image_ = pStrImage;
	}
	std::wstring ListTextExtElement::GetCheckBoxForeImage()
	{
		return checkbox_fore_image_;
	}

	void ListTextExtElement::SetCheckBoxForeImage(std::wstring pStrImage)
	{
		checkbox_fore_image_ = pStrImage;
	}

	void ListTextExtElement::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&paint_rect_, &rcPaint, &rect_) ) return;
		DrawItemBk(hDC, rect_);
		PaintStatusImage(hDC);
		DrawItemText(hDC, rect_);
	}
	void ListTextExtElement::GetCheckBoxRect(int nIndex, RECT &rc)
	{
		memset(&rc, 0x00, sizeof(rc));
		int nItemHeight = rect_.bottom - rect_.top;
		rc.left = rect_.left + 6;
		rc.top = rect_.top + (nItemHeight - GetCheckBoxHeight()) / 2;
		rc.right = rc.left + GetCheckBoxWidth();
		rc.bottom = rc.top + GetCheckBoxHeight();
	}
	int ListTextExtElement::GetCheckBoxWidth() const
	{
		return cxy_checkbox_.cx;
	}

	void ListTextExtElement::SetCheckBoxWidth(int cx)
	{
		if( cx < 0 ) return; 
		cxy_checkbox_.cx = cx;
	}

	int ListTextExtElement::GetCheckBoxHeight()  const 
	{
		return cxy_checkbox_.cy;
	}

	void ListTextExtElement::SetCheckBoxHeight(int cy)
	{
		if( cy < 0 ) return; 
		cxy_checkbox_.cy = cy;
	}

	void ListTextExtElement::SetCheck(BOOL bCheck)
	{
		if( checked_ == bCheck ) return;
		checked_ = bCheck;
		if( checked_ ) checkbox_state_ |= UISTATE_SELECTED;
		else checkbox_state_ &= ~UISTATE_SELECTED;
		Invalidate();
	}

	BOOL  ListTextExtElement::GetCheck() const
	{
		return checked_;
	}

	int ListTextExtElement::HitTestColum(POINT ptMouse)
	{
		TListInfo* pInfo = owner_->GetListInfo();
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			RECT rcItem = { pInfo->rcColumn[i].left, rect_.top, pInfo->rcColumn[i].right, rect_.bottom };
			rcItem.left += pInfo->rcTextPadding.left;
			rcItem.right -= pInfo->rcTextPadding.right;
			rcItem.top += pInfo->rcTextPadding.top;
			rcItem.bottom -= pInfo->rcTextPadding.bottom;

			if( ::PtInRect(&rcItem, ptMouse)) 
			{
				return i;
			}
		}
		return -1;
	}

	BOOL ListTextExtElement::CheckColumEditable(int nColum)
	{
		return owner_->CheckColumEditable(nColum);
	}
	void ListTextExtElement::GetColumRect(int nColum, RECT &rc)
	{
		TListInfo* pInfo = owner_->GetListInfo();
		rc.left = pInfo->rcColumn[nColum].left;
		rc.top  = rect_.top;
		rc.right = pInfo->rcColumn[nColum].right;
		rc.bottom = rect_.bottom;
	}

	void ListTextExtElement::SetColumItemColor(int nColum, DWORD iBKColor)
	{
		ColumCorlorArray[nColum].bEnable = TRUE;
		ColumCorlorArray[nColum].iBKColor = iBKColor;
		Invalidate();
	}
	BOOL ListTextExtElement::GetColumItemColor(int nColum, DWORD& iBKColor)
	{
		if (!ColumCorlorArray[nColum].bEnable)
		{
			return FALSE;
		}
		iBKColor = ColumCorlorArray[nColum].iBKColor;
		return TRUE;
	}

} // namespace DuiLib

