#include "StdAfx.h"

namespace ui {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(List)

	List::List() : m_pCallback(NULL), m_bScrollSelect(false), m_iCurSel(-1), m_iExpandedItem(-1), m_bMultiSel(false)
	{
		m_pList = new ListBody(this);
		m_pHeader = new ListHeader;

		Add(m_pHeader);
		VBox::Add(m_pList);

		m_ListInfo.nColumns = 0;
		m_ListInfo.nFont = -1;
		m_ListInfo.uTextStyle = DT_VCENTER | DT_SINGLELINE;
		m_ListInfo.dwTextColor = 0xFF000000;
		m_ListInfo.dwBkColor = 0;
		m_ListInfo.bAlternateBk = false;
		m_ListInfo.dwSelectedTextColor = 0xFF000000;
		m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
		m_ListInfo.dwHotTextColor = 0xFF000000;
		m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
		m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
		m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
		m_ListInfo.dwLineColor = 0;
		m_ListInfo.bShowRowLine = false;
		m_ListInfo.bShowColumnLine = false;
		m_ListInfo.bShowHtml = false;
		m_ListInfo.bMultiExpandable = false;
		::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
		::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
	}

	std::wstring List::GetClass() const
	{
		return DUI_CTR_LIST;
	}

	UINT List::GetControlFlags() const
	{
		return UIFLAG_TABSTOP;
	}

	LPVOID List::GetInterface(std::wstring strName)
	{
		auto pstrName = strName.c_str();

		if( _tcsicmp(pstrName, DUI_CTR_LIST) == 0 ) return static_cast<List*>(this);
		if( _tcsicmp(pstrName, _T("IList")) == 0 ) return static_cast<IList*>(this);
		if( _tcsicmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwner*>(this);
		return VBox::GetInterface(strName);
	}

	void List::AttachItemSelected(const EventCallback & cb)
	{
		on_event_[kEventItemSelected] += cb;
	}

	void List::AttachItemLinkClick(const EventCallback & cb)
	{
		on_event_[kEventItemLinkClick] += cb;
	}

	Control* List::GetItemAt(int iIndex) const
	{
		return m_pList->GetItemAt(iIndex);
	}

	int List::GetItemIndex(Control* pControl) const
	{
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) return VBox::GetItemIndex(pControl);
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass().c_str(), _T("ListHeaderItem")) != NULL ) return m_pHeader->GetItemIndex(pControl);

		return m_pList->GetItemIndex(pControl);
	}

	bool List::SetItemIndex(Control* pControl, int iIndex)
	{
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) return VBox::SetItemIndex(pControl, iIndex);
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass().c_str(), _T("ListHeaderItem")) != NULL ) return m_pHeader->SetItemIndex(pControl, iIndex);

		int iOrginIndex = m_pList->GetItemIndex(pControl);
		if( iOrginIndex == -1 ) return false;
		if( iOrginIndex == iIndex ) return true;

		IListItem* pSelectedListItem = NULL;
		if( m_iCurSel >= 0 ) pSelectedListItem = 
			static_cast<IListItem*>(GetItemAt(m_iCurSel)->GetInterface(_T("ListItem")));
		if( !m_pList->SetItemIndex(pControl, iIndex) ) return false;
		int iMinIndex = min(iOrginIndex, iIndex);
		int iMaxIndex = max(iOrginIndex, iIndex);
		for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
			Control* p = m_pList->GetItemAt(i);
			IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
		return true;
	}

	int List::GetCount() const
	{
		return m_pList->GetCount();
	}

	bool List::Add(Control* pControl)
	{
		// Override the Add() method so we can add items specifically to
		// the intended widgets. Headers are assumed to be
		// answer the correct interface so we can add multiple list headers.
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) {
			if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
				VBox::Remove(m_pHeader);
				m_pHeader = static_cast<ListHeader*>(pControl);
			}
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return VBox::AddAt(pControl, 0);
		}
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass().c_str(), _T("ListHeaderItem")) != NULL ) {
			bool ret = m_pHeader->Add(pControl);
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return ret;
		}
		// The list items should know about us
		IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(GetCount());
		}
		return m_pList->Add(pControl);
	}

	bool List::AddAt(Control* pControl, int iIndex)
	{
		// Override the AddAt() method so we can add items specifically to
		// the intended widgets. Headers and are assumed to be
		// answer the correct interface so we can add multiple list headers.
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) {
			if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
				VBox::Remove(m_pHeader);
				m_pHeader = static_cast<ListHeader*>(pControl);
			}
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return VBox::AddAt(pControl, 0);
		}
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass().c_str(), _T("ListHeaderItem")) != NULL ) {
			bool ret = m_pHeader->AddAt(pControl, iIndex);
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return ret;
		}
		if (!m_pList->AddAt(pControl, iIndex)) return false;

		// The list items should know about us
		IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(iIndex);
		}

		for(int i = iIndex + 1; i < m_pList->GetCount(); ++i) {
			Control* p = m_pList->GetItemAt(i);
			pListItem = static_cast<IListItem*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		if( m_iCurSel >= iIndex ) m_iCurSel += 1;
		return true;
	}

	bool List::Remove(Control* pControl)
	{
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) return VBox::Remove(pControl);
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass().c_str(), _T("ListHeaderItem")) != NULL ) return m_pHeader->Remove(pControl);

		int iIndex = m_pList->GetItemIndex(pControl);
		if (iIndex == -1) return false;

		if (!m_pList->RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < m_pList->GetCount(); ++i) {
			Control* p = m_pList->GetItemAt(i);
			IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}

		if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
		return true;
	}

	bool List::RemoveAt(int iIndex)
	{
		if (!m_pList->RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < m_pList->GetCount(); ++i) {
			Control* p = m_pList->GetItemAt(i);
			IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->SetIndex(i);
		}

		if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
		return true;
	}

	void List::RemoveAll()
	{
		m_iCurSel = -1;
		m_iExpandedItem = -1;
		m_pList->RemoveAll();
	}

	void List::SetPos(RECT rc, bool bNeedInvalidate)
	{
		VBox::SetPos(rc, bNeedInvalidate);

		if( m_pHeader == NULL ) return;
		// Determine general list information and the size of header columns
		m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
		// The header/columns may or may not be visible at runtime. In either case
		// we should determine the correct dimensions...

		if( !m_pHeader->IsVisible() ) {
			for( int it = 0; it < m_pHeader->GetCount(); it++ ) {
				static_cast<Control*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
			}
			m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0), bNeedInvalidate);
		}

		for( int i = 0; i < m_ListInfo.nColumns; i++ ) {
			Control* pControl = static_cast<Control*>(m_pHeader->GetItemAt(i));
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			RECT rcPos = pControl->GetPos();
			m_ListInfo.rcColumn[i] = pControl->GetPos();
		}
		if( !m_pHeader->IsVisible() ) {
			for( int it = 0; it < m_pHeader->GetCount(); it++ ) {
				static_cast<Control*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
			}
		}
		m_pList->SetPos(m_pList->GetPos(), bNeedInvalidate);
	}

	void List::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		VBox::Move(szOffset, bNeedInvalidate);
		if( !m_pHeader->IsVisible() ) m_pHeader->Move(szOffset, false);
	}

	int List::GetMinSelItemIndex()
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;
		int min = (int)m_aSelItems.GetAt(0);
		int index;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			index = (int)m_aSelItems.GetAt(i);
			if (min > index)
				min = index;
		}
		return min;
	}

	int List::GetMaxSelItemIndex()
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;
		int max = (int)m_aSelItems.GetAt(0);
		int index;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			index = (int)m_aSelItems.GetAt(i);
			if (max < index)
				max = index;
		}
		return max;
	}

	void List::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else VBox::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetFocus ) 
		{
			focused_ = true;
			return;
		}
		if( event.type_ == kEventKillFocus ) 
		{
			focused_ = false;
			return;
		}

		switch( event.type_ ) {
		case kEventKeyDown:
			switch( event.key_ ) {
			case VK_UP:
				{
					if (m_aSelItems.GetSize() > 0) {					
						int index = GetMinSelItemIndex() - 1;
						UnSelectAllItems();
						index > 0 ? SelectItem(index, true) : SelectItem(0, true);					
					}
				}			
				return;
			case VK_DOWN:			
				{
					if (m_aSelItems.GetSize() > 0) {					
						int index = GetMaxSelItemIndex() + 1;
						UnSelectAllItems();
						index + 1 > m_pList->GetCount() ? SelectItem(GetCount() - 1, true) : SelectItem(index, true);					
					}
				}
				return;
			case VK_PRIOR:
				PageUp();
				return;
			case VK_NEXT:
				PageDown();
				return;
			case VK_HOME:
				SelectItem(FindSelectable(0, false), true);
				return;
			case VK_END:
				SelectItem(FindSelectable(GetCount() - 1, true), true);
				return;
			case VK_RETURN:
				if( m_iCurSel != -1 ) GetItemAt(m_iCurSel)->Activate();
				return;
			case 0x41:// Ctrl+A
				{
					if (IsMultiSelect() && (GetKeyState(VK_CONTROL) & 0x8000)) {
						SelectAllItems();
					}
					return;
				}
			}
			break;
		case kMouseScrollWheel:
			{
				switch( LOWORD(event.w_param_) ) {
				case SB_LINEUP:
					if( m_bScrollSelect && !IsMultiSelect() ) SelectItem(FindSelectable(m_iCurSel - 1, false), true);
					else LineUp();
					return;
				case SB_LINEDOWN:
					if( m_bScrollSelect && !IsMultiSelect() ) SelectItem(FindSelectable(m_iCurSel + 1, true), true);
					else LineDown();
					return;
				}
			}
			break;
		}
		VBox::DoEvent(event);
	}

	ListHeader* List::GetHeader() const
	{
		return m_pHeader;
	}

	Box* List::GetList() const
	{
		return m_pList;
	}

	bool List::GetScrollSelect()
	{
		return m_bScrollSelect;
	}

	void List::SetScrollSelect(bool bScrollSelect)
	{
		m_bScrollSelect = bScrollSelect;
	}

	int List::GetCurSelActivate() const
	{
		return m_iCurSelActivate;
	}

	bool List::SelectItemActivate(int iIndex)
	{
		if (!SelectItem(iIndex, true)){
			return false;
		}

		m_iCurSelActivate = iIndex;
		return true;
	}

	int List::GetCurSel() const
	{	
		if (m_aSelItems.GetSize() <= 0) {
			return -1;
		}
		else {
			return (int)m_aSelItems.GetAt(0);
		}

		return -1;
	}

	bool List::SelectItem(int iIndex, bool bTakeFocus, bool send_notify)
	{
		// 取消所有选择项
		UnSelectAllItems();
		// 判断是否合法列表项
		if( iIndex < 0 ) return false;
		Control* pControl = GetItemAt(iIndex);
		if( pControl == NULL ) return false;
		IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;
		if( !pListItem->Select(true) ) {
			return false;
		}
		int iLastSel = m_iCurSel;
		m_iCurSel = iIndex;
		m_aSelItems.Add((LPVOID)iIndex);
		EnsureVisible(iIndex);
		if( bTakeFocus ) pControl->SetFocus();
		if( pm_ != NULL && iLastSel != m_iCurSel && send_notify) {
			Notify(kEventItemSelected, iIndex);
		}

		return true;
	}
	
	bool List::SelectMultiItem(int iIndex, bool bTakeFocus)
	{
		if(!IsMultiSelect()) return SelectItem(iIndex, bTakeFocus);

		if( iIndex < 0 ) return false;
		Control* pControl = GetItemAt(iIndex);
		if( pControl == NULL ) return false;
		IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;
		if(m_aSelItems.Find((LPVOID)iIndex) >= 0) return false;
		if(!pListItem->SelectMulti(true)) return false;

		m_iCurSel = iIndex;
		m_aSelItems.Add((LPVOID)iIndex);
		EnsureVisible(iIndex);
		if( bTakeFocus ) pControl->SetFocus();
		if( pm_ != NULL ) {
			Notify(kEventItemSelected, iIndex);
		}
		return true;
	}

	void List::SetMultiSelect(bool bMultiSel)
	{
		m_bMultiSel = bMultiSel;
		if(!bMultiSel) UnSelectAllItems();
	}

	bool List::IsMultiSelect() const
	{
		return m_bMultiSel;
	}

	bool List::UnSelectItem(int iIndex, bool bOthers)
	{
		if(!IsMultiSelect()) return false;
		if(bOthers) {
			for (int i = m_aSelItems.GetSize() - 1; i >= 0; --i) {
				int iSelIndex = (int)m_aSelItems.GetAt(i);
				if(iSelIndex == iIndex) continue;
				Control* pControl = GetItemAt(iSelIndex);
				if(pControl == NULL) continue;
				if(!pControl->IsEnabled()) continue;
				IListItem* pSelListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
				if( pSelListItem == NULL ) continue;
				if( !pSelListItem->SelectMulti(false) ) continue;
				m_aSelItems.Remove(i);
			}
		}
		else {
			if( iIndex < 0 ) return false;
			Control* pControl = GetItemAt(iIndex);
			if( pControl == NULL ) return false;
			if( !pControl->IsEnabled() ) return false;
			IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem == NULL ) return false;
			int aIndex = m_aSelItems.Find((LPVOID)iIndex);
			if (aIndex < 0) return false;
			if( !pListItem->SelectMulti(false) ) return false;
			if(m_iCurSel == iIndex) m_iCurSel = -1;
			m_aSelItems.Remove(aIndex);
		}
		return true;
	}

	void List::SelectAllItems()
	{
		for (int i = 0; i < GetCount(); ++i) {
			Control* pControl = GetItemAt(i);
			if(pControl == NULL) continue;
			if(!pControl->IsVisible()) continue;
			if(!pControl->IsEnabled()) continue;
			IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
			if(pListItem == NULL) continue;
			if(!pListItem->SelectMulti(true)) continue;

			m_aSelItems.Add((LPVOID)i);
			m_iCurSel = i;
		}
	}

	void List::UnSelectAllItems()
	{
		for (int i = 0; i < m_aSelItems.GetSize(); ++i) {
			int iSelIndex = (int)m_aSelItems.GetAt(i);
			Control* pControl = GetItemAt(iSelIndex);
			if(pControl == NULL) continue;
			if(!pControl->IsEnabled()) continue;
			IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem == NULL ) continue;
			if( !pListItem->SelectMulti(false) ) continue;		
		}
		m_aSelItems.Empty();
		m_iCurSel = -1;
	}

	int List::GetSelectItemCount() const
	{
		return m_aSelItems.GetSize();
	}

	int List::GetNextSelItem(int nItem) const
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;

		if (nItem < 0) {
			return (int)m_aSelItems.GetAt(0);
		}
		int aIndex = m_aSelItems.Find((LPVOID)nItem);
		if (aIndex < 0) return -1;
		if (aIndex + 1 > m_aSelItems.GetSize() - 1)
			return -1;
		return (int)m_aSelItems.GetAt(aIndex + 1);
	}

	UINT List::GetListType()
	{
		return kListList;
	}

	TListInfo* List::GetListInfo()
	{
		return &m_ListInfo;
	}

	bool List::IsDelayedDestroy() const
	{
		return m_pList->IsDelayedDestroy();
	}

	void List::SetDelayedDestroy(bool bDelayed)
	{
		m_pList->SetDelayedDestroy(bDelayed);
	}

	int List::GetChildPadding() const
	{
		return m_pList->GetChildPadding();
	}

	void List::SetChildPadding(int iPadding)
	{
		m_pList->SetChildPadding(iPadding);
	}

	void List::SetItemFont(int index)
	{
		m_ListInfo.nFont = index;
		NeedUpdate();
	}

	void List::SetItemTextStyle(UINT uStyle)
	{
		m_ListInfo.uTextStyle = uStyle;
		NeedUpdate();
	}

	void List::SetItemTextPadding(RECT rc)
	{
		m_ListInfo.rcTextPadding = rc;
		NeedUpdate();
	}

	RECT List::GetItemTextPadding() const
	{
		RECT rect = m_ListInfo.rcTextPadding;
		GetManager()->GetDPIObj()->Scale(&rect);
		return rect;
	}

	void List::SetItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwTextColor = dwTextColor;
		Invalidate();
	}

	void List::SetItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwBkColor = dwBkColor;
		Invalidate();
	}

	void List::SetItemBkImage(std::wstring pStrImage)
	{
		m_ListInfo.sBkImage = pStrImage;
		Invalidate();
	}

	void List::SetAlternateBk(bool bAlternateBk)
	{
		m_ListInfo.bAlternateBk = bAlternateBk;
		Invalidate();
	}

	DWORD List::GetItemTextColor() const
	{
		return m_ListInfo.dwTextColor;
	}

	DWORD List::GetItemBkColor() const
	{
		return m_ListInfo.dwBkColor;
	}

	std::wstring List::GetItemBkImage() const
	{
		return m_ListInfo.sBkImage;
	}

	bool List::IsAlternateBk() const
	{
		return m_ListInfo.bAlternateBk;
	}

	void List::SetSelectedItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwSelectedTextColor = dwTextColor;
		Invalidate();
	}

	void List::SetSelectedItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwSelectedBkColor = dwBkColor;
		Invalidate();
	}

	void List::SetSelectedItemImage(std::wstring pStrImage)
	{
		m_ListInfo.sSelectedImage = pStrImage;
		Invalidate();
	}

	DWORD List::GetSelectedItemTextColor() const
	{
		return m_ListInfo.dwSelectedTextColor;
	}

	DWORD List::GetSelectedItemBkColor() const
	{
		return m_ListInfo.dwSelectedBkColor;
	}

	std::wstring List::GetSelectedItemImage() const
	{
		return m_ListInfo.sSelectedImage;
	}

	void List::SetHotItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwHotTextColor = dwTextColor;
		Invalidate();
	}

	void List::SetHotItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwHotBkColor = dwBkColor;
		Invalidate();
	}

	void List::SetHotItemImage(std::wstring pStrImage)
	{
		m_ListInfo.sHotImage = pStrImage;
		Invalidate();
	}

	DWORD List::GetHotItemTextColor() const
	{
		return m_ListInfo.dwHotTextColor;
	}
	DWORD List::GetHotItemBkColor() const
	{
		return m_ListInfo.dwHotBkColor;
	}

	std::wstring List::GetHotItemImage() const
	{
		return m_ListInfo.sHotImage;
	}

	void List::SetDisabledItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	void List::SetDisabledItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwDisabledBkColor = dwBkColor;
		Invalidate();
	}

	void List::SetDisabledItemImage(std::wstring pStrImage)
	{
		m_ListInfo.sDisabledImage = pStrImage;
		Invalidate();
	}

	DWORD List::GetDisabledItemTextColor() const
	{
		return m_ListInfo.dwDisabledTextColor;
	}

	DWORD List::GetDisabledItemBkColor() const
	{
		return m_ListInfo.dwDisabledBkColor;
	}

	std::wstring List::GetDisabledItemImage() const
	{
		return m_ListInfo.sDisabledImage;
	}

	DWORD List::GetItemLineColor() const
	{
		return m_ListInfo.dwLineColor;
	}

	void List::SetItemLineColor(DWORD dwLineColor)
	{
		m_ListInfo.dwLineColor = dwLineColor;
		Invalidate();
	}
	void List::SetItemShowRowLine(bool bShowLine)
	{
		m_ListInfo.bShowRowLine = bShowLine;
		Invalidate();
	}
	void List::SetItemShowColumnLine(bool bShowLine)
	{
		m_ListInfo.bShowColumnLine = bShowLine;
		Invalidate();
	}
	bool List::IsItemShowHtml()
	{
		return m_ListInfo.bShowHtml;
	}

	void List::SetItemShowHtml(bool bShowHtml)
	{
		if( m_ListInfo.bShowHtml == bShowHtml ) return;

		m_ListInfo.bShowHtml = bShowHtml;
		NeedUpdate();
	}

	void List::SetMultiExpanding(bool bMultiExpandable)
	{
		m_ListInfo.bMultiExpandable = bMultiExpandable;
	}

	bool List::ExpandItem(int iIndex, bool bExpand /*= true*/)
	{
		if( m_iExpandedItem >= 0 && !m_ListInfo.bMultiExpandable) {
			Control* pControl = GetItemAt(m_iExpandedItem);
			if( pControl != NULL ) {
				IListItem* pItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
				if( pItem != NULL ) pItem->Expand(false);
			}
			m_iExpandedItem = -1;
		}
		if( bExpand ) {
			Control* pControl = GetItemAt(iIndex);
			if( pControl == NULL ) return false;
			if( !pControl->IsVisible() ) return false;
			IListItem* pItem = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
			if( pItem == NULL ) return false;
			m_iExpandedItem = iIndex;
			if( !pItem->Expand(true) ) {
				m_iExpandedItem = -1;
				return false;
			}
		}
		NeedUpdate();
		return true;
	}

	int List::GetExpandedItem() const
	{
		return m_iExpandedItem;
	}

	void List::EnsureVisible(int iIndex)
	{
		if( m_iCurSel < 0 ) return;
		RECT rcItem = m_pList->GetItemAt(iIndex)->GetPos();
		RECT rcList = m_pList->GetPos();
		RECT rcListInset = m_pList->GetInset();

		rcList.left += rcListInset.left;
		rcList.top += rcListInset.top;
		rcList.right -= rcListInset.right;
		rcList.bottom -= rcListInset.bottom;

		ScrollBar* pHorizontalScrollBar = m_pList->GetHorizontalScrollBar();
		if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();

		int iPos = m_pList->GetScrollPos().cy;
		if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
		int dx = 0;
		if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
		if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
		Scroll(0, dx);
	}

	void List::Scroll(int dx, int dy)
	{
		if( dx == 0 && dy == 0 ) return;
		SIZE sz = m_pList->GetScrollPos();
		m_pList->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
	}

	void List::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("header")) == 0 ) GetHeader()->SetVisible(_tcsicmp(pstrValue, _T("hidden")) != 0);
		else if( _tcsicmp(pstrName, _T("headerbkimage")) == 0 ) GetHeader()->SetBkImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("scrollselect")) == 0 ) SetScrollSelect(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("multiexpanding")) == 0 ) SetMultiExpanding(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("itemfont")) == 0 ) m_ListInfo.nFont = _ttoi(pstrValue);
		else if( _tcsicmp(pstrName, _T("itemalign")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_ListInfo.uTextStyle |= DT_RIGHT;
			}
		}
		else if( _tcsicmp(pstrName, _T("itemvalign")) == 0 ) {
			if( _tcsstr(pstrValue, _T("top")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_VCENTER | DT_BOTTOM);
				m_ListInfo.uTextStyle |= DT_TOP;
			}
			if( _tcsstr(pstrValue, _T("vcenter")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM);
				m_ListInfo.uTextStyle |= DT_VCENTER;
			}
			if( _tcsstr(pstrValue, _T("bottom")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_TOP | DT_VCENTER);
				m_ListInfo.uTextStyle |= DT_BOTTOM;
			}
		}
		else if( _tcsicmp(pstrName, _T("itemendellipsis")) == 0 ) {
			if( _tcsicmp(pstrValue, _T("true")) == 0 ) m_ListInfo.uTextStyle |= DT_END_ELLIPSIS;
			else m_ListInfo.uTextStyle &= ~DT_END_ELLIPSIS;
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
		else if( _tcsicmp(pstrName, _T("itemshowrowline")) == 0 ) SetItemShowRowLine(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("itemshowcolumnline")) == 0 ) SetItemShowColumnLine(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcsicmp(pstrValue, _T("true")) == 0);
		else if ( _tcscmp(pstrName, _T("multiselect")) == 0 ) SetMultiSelect(_tcscmp(pstrValue, _T("true")) == 0);
		else VBox::SetAttribute(strName, strValue);
	}

	IListCallback* List::GetTextCallback() const
	{
		return m_pCallback;
	}

	void List::SetTextCallback(IListCallback* pCallback)
	{
		m_pCallback = pCallback;
	}

	SIZE List::GetScrollPos() const
	{
		return m_pList->GetScrollPos();
	}

	SIZE List::GetScrollRange() const
	{
		return m_pList->GetScrollRange();
	}

	void List::SetScrollPos(SIZE szPos, bool bMsg)
	{
		m_pList->SetScrollPos(szPos, bMsg);
	}

	void List::LineUp()
	{
		m_pList->LineUp();
	}

	void List::LineDown()
	{
		m_pList->LineDown();
	}

	void List::PageUp()
	{
		m_pList->PageUp();
	}

	void List::PageDown()
	{
		m_pList->PageDown();
	}

	void List::HomeUp()
	{
		m_pList->HomeUp();
	}

	void List::EndDown()
	{
		m_pList->EndDown();
	}

	void List::LineLeft()
	{
		m_pList->LineLeft();
	}

	void List::LineRight()
	{
		m_pList->LineRight();
	}

	void List::PageLeft()
	{
		m_pList->PageLeft();
	}

	void List::PageRight()
	{
		m_pList->PageRight();
	}

	void List::HomeLeft()
	{
		m_pList->HomeLeft();
	}

	void List::EndRight()
	{
		m_pList->EndRight();
	}

	void List::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
	}

	ScrollBar* List::GetVerticalScrollBar() const
	{
		return m_pList->GetVerticalScrollBar();
	}

	ScrollBar* List::GetHorizontalScrollBar() const
	{
		return m_pList->GetHorizontalScrollBar();
	}

	BOOL List::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData)
	{
		if (!m_pList)
			return FALSE;
		return m_pList->SortItems(pfnCompare, dwData);	
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	ListBody::ListBody(List* pOwner) : owner_(pOwner)
	{
		ASSERT(owner_);
	}

	BOOL ListBody::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData)
	{
		if (!pfnCompare)
			return FALSE;
		m_pCompareFunc = pfnCompare;
		m_compareData = dwData;
		Control **pData = (Control **)items_.GetData();
		qsort_s(items_.GetData(), items_.GetSize(), sizeof(Control*), ListBody::ItemComareFunc, this);	
		IListItem *pItem = NULL;
		for (int i = 0; i < items_.GetSize(); ++i)
		{
			pItem = (IListItem*)(static_cast<Control*>(items_[i])->GetInterface(TEXT("ListItem")));
			if (pItem)
			{
				pItem->SetIndex(i);
				pItem->Select(false);
			}
		}
		owner_->SelectItem(-1);
		if (pm_)
		{
			SetPos(GetPos());
			Invalidate();
		}

		return TRUE;
	}

	int __cdecl ListBody::ItemComareFunc(void *pvlocale, const void *item1, const void *item2)
	{
		ListBody *pThis = (ListBody*)pvlocale;
		if (!pThis || !item1 || !item2)
			return 0;
		return pThis->ItemComareFunc(item1, item2);
	}

	int __cdecl ListBody::ItemComareFunc(const void *item1, const void *item2)
	{
		Control *pControl1 = *(Control**)item1;
		Control *pControl2 = *(Control**)item2;
		return m_pCompareFunc((UINT_PTR)pControl1, (UINT_PTR)pControl2, m_compareData);
	}

	int ListBody::GetScrollStepSize() const
	{
		if(owner_ != NULL) return owner_->GetScrollStepSize();

		return VBox::GetScrollStepSize();
	}

	void ListBody::SetScrollPos(SIZE szPos, bool bMsg)
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

		RECT rcPos;
		for( int it2 = 0; it2 < items_.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(items_[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;

			rcPos = pControl->GetPos();
			rcPos.left -= cx;
			rcPos.right -= cx;
			rcPos.top -= cy;
			rcPos.bottom -= cy;
			pControl->SetPos(rcPos, true);
		}

		Invalidate();
		if( owner_ ) {
			ListHeader* pHeader = owner_->GetHeader();
			if( pHeader == NULL ) return;
			TListInfo* pInfo = owner_->GetListInfo();
			pInfo->nColumns = MIN(pHeader->GetCount(), UILIST_MAX_COLUMNS);

			if( !pHeader->IsVisible() ) {
				for( int it = 0; it < pHeader->GetCount(); it++ ) {
					static_cast<Control*>(pHeader->GetItemAt(it))->SetInternVisible(true);
				}
			}
			for( int i = 0; i < pInfo->nColumns; i++ ) {
				Control* pControl = static_cast<Control*>(pHeader->GetItemAt(i));
				if( !pControl->IsVisible() ) continue;
				if( pControl->IsFloat() ) continue;

				RECT rcPos = pControl->GetPos();
				rcPos.left -= cx;
				rcPos.right -= cx;
				pControl->SetPos(rcPos);
				pInfo->rcColumn[i] = pControl->GetPos();
			}
			if( !pHeader->IsVisible() ) {
				for( int it = 0; it < pHeader->GetCount(); it++ ) {
					static_cast<Control*>(pHeader->GetItemAt(it))->SetInternVisible(false);
				}
			}
		}
	}

	void ListBody::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		rc = rect_;

		// Adjust for inset
		rc.left += rc_inset_.left;
		rc.top += rc_inset_.top;
		rc.right -= rc_inset_.right;
		rc.bottom -= rc_inset_.bottom;
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) rc.right -= vertical_scrollbar_->GetFixedWidth();
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) rc.bottom -= horizontal_scrollbar_->GetFixedHeight();

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) 
			szAvailable.cx += horizontal_scrollbar_->GetScrollRange();

		int cxNeeded = 0;
		int nAdjustables = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < items_.GetSize(); it1++ ) {
			Control* pControl = static_cast<Control*>(items_[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cy == 0 ) {
				nAdjustables++;
			}
			// TODO : luyang 列表适应auto属性
		/*	else if (sz.cy == -1) {
				sz.cy = pControl->GetFixedHeight();
				if (sz.cy < pControl->GetMinHeight()) {
					sz.cy = pControl->GetMinHeight();
				}
				if (sz.cy > pControl->GetMaxHeight()) {
					sz.cy = pControl->GetMaxHeight();
				}

			}*/
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;

			RECT rcPadding = pControl->GetPadding();
			sz.cx = MAX(sz.cx, 0);
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			cxNeeded = MAX(cxNeeded, sz.cx);
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * child_padding_;

		if( owner_ ) {
			ListHeader* pHeader = owner_->GetHeader();
			if( pHeader != NULL && pHeader->GetCount() > 0 ) {
				cxNeeded = MAX(0, pHeader->EstimateSize(CDuiSize(rc.right - rc.left, rc.bottom - rc.top)).cx);
				if ( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible())
				{
					int nOffset = horizontal_scrollbar_->GetScrollPos();
					RECT rcHeader = pHeader->GetPos();
					rcHeader.left = rc.left - nOffset;
					pHeader->SetPos(rcHeader);
				}
			}
		}

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		if( nAdjustables > 0 ) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
			iPosY -= vertical_scrollbar_->GetScrollPos();
		}
		int iPosX = rc.left;
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
			iPosX -= horizontal_scrollbar_->GetScrollPos();
		}
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for( int it2 = 0; it2 < items_.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(items_[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}

			RECT rcPadding = pControl->GetPadding();
			szRemaining.cy -= rcPadding.top;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if( sz.cy == 0 ) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cy = MAX(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
				} 
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = MAX(cxNeeded, szAvailable.cx - rcPadding.left - rcPadding.right);

			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
			pControl->SetPos(rcCtrl);

			iPosY += sz.cy + child_padding_ + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + child_padding_ + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * child_padding_;

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
			else {
				if( horizontal_scrollbar_->IsVisible() ) {
					horizontal_scrollbar_->SetVisible(false);
					horizontal_scrollbar_->SetScrollRange(0);
					horizontal_scrollbar_->SetScrollPos(0);
					rc.bottom += horizontal_scrollbar_->GetFixedHeight();
				}
			}
		}

		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	void ListBody::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( owner_ != NULL ) owner_->DoEvent(event);
			else VBox::DoEvent(event);
			return;
		}

		VBox::DoEvent(event);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ListHeader)

	ListHeader::ListHeader():
	m_bIsScaleHeader(false)
	{
	}

	std::wstring ListHeader::GetClass() const
	{
		return DUI_CTR_LISTHEADER;
	}

	LPVOID ListHeader::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_LISTHEADER) == 0 ) return this;
		return HBox::GetInterface(pstrName);
	}

	SIZE ListHeader::EstimateSize(SIZE szAvailable)
	{
		SIZE cXY = {0, cxy_fired_sz_.cy};
		if( cXY.cy == 0 && pm_ != NULL ) {
			for( int it = 0; it < items_.GetSize(); it++ ) {
				cXY.cy = MAX(cXY.cy,static_cast<Control*>(items_[it])->EstimateSize(szAvailable).cy);
			}
			int nMin = GlobalManager::GetDefaultFontInfo(pm_->GetPaintDC())->tm_.tmHeight + 6;
			cXY.cy = MAX(cXY.cy,nMin);
		}

		for( int it = 0; it < items_.GetSize(); it++ ) {
			cXY.cx +=  static_cast<Control*>(items_[it])->EstimateSize(szAvailable).cx;
		}

		return cXY;
	}

	void ListHeader::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		rc = rect_;

		// Adjust for inset
		rc.left += rc_inset_.left;
		rc.top += rc_inset_.top;
		rc.right -= rc_inset_.right;
		rc.bottom -= rc_inset_.bottom;

		if( items_.GetSize() == 0) {
			return;
		}


		// Determine the width of elements that are sizeable
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

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

		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;

		int nHeaderWidth = GetWidth();
		List *pList = static_cast<List*>(GetParent());
		if (pList != NULL)
		{
			ScrollBar* pVScroll = pList->GetVerticalScrollBar();
			if (pVScroll != NULL)
				nHeaderWidth -= pVScroll->GetWidth();
		}
		for( int it2 = 0; it2 < items_.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(items_[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cx -= rcPadding.left;

			SIZE sz = {0,0};
			if (m_bIsScaleHeader)
			{
				ListHeaderItem* pHeaderItem = static_cast<ListHeaderItem*>(pControl);
				sz.cx = int(nHeaderWidth * (float)pHeaderItem->GetScale() / 100);
			}
			else
			{
				sz = pControl->EstimateSize(szRemaining);
			}

			if( sz.cx == 0 ) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
				}
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

				cxFixedRemaining -= sz.cx;
			}

			sz.cy = pControl->GetFixedHeight();
			if( sz.cy == 0 ) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
			if( sz.cy < 0 ) sz.cy = 0;
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy};
			pControl->SetPos(rcCtrl);
			iPosX += sz.cx + child_padding_ + rcPadding.left + rcPadding.right;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
			szRemaining.cx -= sz.cx + child_padding_ + rcPadding.right;
		}
		cxNeeded += (nEstimateNum - 1) * child_padding_;
	}

	void ListHeader::SetAttribute(std::wstring pstrName, std::wstring pstrValue)
	{
		if( _tcsicmp(pstrName.c_str(), _T("scaleheader")) == 0 ) SetScaleHeader(_tcsicmp(pstrValue.c_str(), _T("true")) == 0);
		else HBox::SetAttribute(pstrName, pstrValue);
	}

	void ListHeader::SetScaleHeader(bool bIsScale)
	{
		m_bIsScaleHeader = bIsScale;
	}

	bool ListHeader::IsScaleHeader() const
	{
		return m_bIsScaleHeader;
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ListHeaderItem)

		ListHeaderItem::ListHeaderItem() : dragable_(true), btn_state_(0), sep_width_(4),
		text_style_(DT_VCENTER | DT_CENTER | DT_SINGLELINE), text_color_(0), font_(-1), show_html_(false),m_nScale(0)
	{
		SetTextPadding(CDuiRect(2, 0, 2, 0));
		last_mouse_pt_.x = last_mouse_pt_.y = 0;
		SetMinWidth(16);
	}

	std::wstring ListHeaderItem::GetClass() const
	{
		return _T("ListHeaderItem");
	}

	LPVOID ListHeaderItem::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_LISTHEADERITEM) == 0 ) return this;
		return Box::GetInterface(pstrName);
	}

	UINT ListHeaderItem::GetControlFlags() const
	{
		if( IsEnabled() && sep_width_ != 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void ListHeaderItem::AttachHeaderClick(const EventCallback & cb)
	{
		on_event_[kEventHeaderClick] += cb;
	}

	void ListHeaderItem::SetEnabled(bool bEnable)
	{
		Box::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			btn_state_ = 0;
		}
	}

	bool ListHeaderItem::IsDragable() const
	{
		return dragable_;
	}

	void ListHeaderItem::SetDragable(bool bDragable)
	{
		dragable_ = bDragable;
		if ( !dragable_ ) btn_state_ &= ~UISTATE_CAPTURED;
	}

	DWORD ListHeaderItem::GetSepWidth() const
	{
		return sep_width_;
	}

	void ListHeaderItem::SetSepWidth(int iWidth)
	{
		sep_width_ = iWidth;
	}

	DWORD ListHeaderItem::GetTextStyle() const
	{
		return text_style_;
	}

	void ListHeaderItem::SetTextStyle(UINT uStyle)
	{
		text_style_ = uStyle;
		Invalidate();
	}

	DWORD ListHeaderItem::GetTextColor() const
	{
		return text_color_;
	}


	void ListHeaderItem::SetTextColor(DWORD dwTextColor)
	{
		text_color_ = dwTextColor;
	}

	RECT ListHeaderItem::GetTextPadding() const
	{
		return text_padding_rect_;
	}

	void ListHeaderItem::SetTextPadding(RECT rc)
	{
		text_padding_rect_ = rc;
		Invalidate();
	}

	void ListHeaderItem::SetFont(int index)
	{
		font_ = index;
	}

	bool ListHeaderItem::IsShowHtml()
	{
		return show_html_;
	}

	void ListHeaderItem::SetShowHtml(bool bShowHtml)
	{
		if( show_html_ == bShowHtml ) return;

		show_html_ = bShowHtml;
		Invalidate();
	}

	std::wstring ListHeaderItem::GetNormalImage() const
	{
		return normal_image_;
	}

	void ListHeaderItem::SetNormalImage(std::wstring pStrImage)
	{
		normal_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ListHeaderItem::GetHotImage() const
	{
		return hot_image_;
	}

	void ListHeaderItem::SetHotImage(std::wstring pStrImage)
	{
		hot_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ListHeaderItem::GetPushedImage() const
	{
		return pushed_image_;
	}

	void ListHeaderItem::SetPushedImage(std::wstring pStrImage)
	{
		pushed_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ListHeaderItem::GetFocusedImage() const
	{
		return focused_image_;
	}

	void ListHeaderItem::SetFocusedImage(std::wstring pStrImage)
	{
		focused_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ListHeaderItem::GetSepImage() const
	{
		return sep_image_;
	}

	void ListHeaderItem::SetSepImage(std::wstring pStrImage)
	{
		sep_image_ = pStrImage;
		Invalidate();
	}

	void ListHeaderItem::SetScale(int nScale)
	{
		m_nScale = nScale;
	}

	int ListHeaderItem::GetScale() const
	{
		return m_nScale;
	}

	void ListHeaderItem::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("dragable")) == 0 ) SetDragable(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("align")) == 0 ) {
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
		else if( _tcsicmp(pstrName, _T("endellipsis")) == 0 ) {
			if( _tcsicmp(pstrValue, _T("true")) == 0 ) text_style_ |= DT_END_ELLIPSIS;
			else text_style_ &= ~DT_END_ELLIPSIS;
		}    
		else if( _tcsicmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("textcolor")) == 0 ) {
			SetTextColor(GlobalManager::GetColor(strValue));
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
		else if( _tcsicmp(pstrName, _T("sepimage")) == 0 ) SetSepImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("scale")) == 0 ) {
			LPTSTR pstr = NULL;
			SetScale(_tcstol(pstrValue, &pstr, 10)); 

		}
		else Box::SetAttribute(strName, strValue);
	}

	void ListHeaderItem::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Box::DoEvent(event);
			return;
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
			if (sep_width_>=0)//111024 by cddjr, 增加分隔符区域，方便用户拖动
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

	SIZE ListHeaderItem::EstimateSize(SIZE szAvailable)
	{
		if( cxy_fired_sz_.cy == 0 ) return CDuiSize(cxy_fired_sz_.cx, 
			GlobalManager::GetDefaultFontInfo(pm_->GetPaintDC())->tm_.tmHeight + 14);
		return Box::EstimateSize(szAvailable);
	}

	RECT ListHeaderItem::GetThumbRect() const
	{
		if( sep_width_ >= 0 ) return CDuiRect(rect_.right - sep_width_, rect_.top, rect_.right, rect_.bottom);
		else return CDuiRect(rect_.left, rect_.top, rect_.left - sep_width_, rect_.bottom);
	}

	void ListHeaderItem::PaintStatusImage(HDC hDC)
	{
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
	}

	void ListHeaderItem::PaintText(HDC hDC)
	{
		if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();

		RECT rcText = rect_;
		rcText.left += text_padding_rect_.left;
		rcText.top += text_padding_rect_.top;
		rcText.right -= text_padding_rect_.right;
		rcText.bottom -= text_padding_rect_.bottom;

		std::wstring sText = GetText();
		if( sText.empty() ) return;
		int nLinks = 0;
		if( show_html_ )
			RenderEngine::DrawHtmlText(hDC, pm_, rcText, sText, text_color_, \
			NULL, NULL, nLinks, text_style_);
		else
			RenderEngine::DrawText(hDC, pm_, rcText, sText, text_color_, \
			font_, text_style_);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
		ListElement::ListElement() : m_iIndex(-1),
		owner_(NULL), 
		selected_(false),
		btn_state_(0)
	{
	}

	std::wstring ListElement::GetClass() const
	{
		return _T("ListElement");
	}

	UINT ListElement::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN;
	}

	LPVOID ListElement::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_LISTITEM) == 0 ) return static_cast<IListItem*>(this);
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_LISTELEMENT) == 0 ) return static_cast<ListElement*>(this);
		return Control::GetInterface(pstrName);
	}

	void ListElement::AttachActive(const EventCallback & cb)
	{
		on_event_[kEventItemActive] += cb;
	}

	IListOwner* ListElement::GetOwner()
	{
		return owner_;
	}

	void ListElement::SetOwner(Control* pOwner)
	{
		owner_ = static_cast<IListOwner*>(pOwner->GetInterface(_T("IListOwner")));
	}

	void ListElement::SetVisible(bool bVisible)
	{
		Control::SetVisible(bVisible);
		if( !IsVisible() && selected_)
		{
			selected_ = false;
			if( owner_ != NULL ) owner_->SelectItem(-1);
		}
	}

	void ListElement::SetEnabled(bool bEnable)
	{
		Control::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			btn_state_ = 0;
		}
	}

	int ListElement::GetIndex() const
	{
		return m_iIndex;
	}

	void ListElement::SetIndex(int iIndex)
	{
		m_iIndex = iIndex;
	}

	void ListElement::Invalidate()
	{
		if( !IsVisible() ) return;

		if( GetParent() ) {
			Box* pParentContainer = static_cast<Box*>(GetParent()->GetInterface(_T("Container")));
			if( pParentContainer ) {
				RECT rc = pParentContainer->GetPos();
				RECT rcInset = pParentContainer->GetInset();
				rc.left += rcInset.left;
				rc.top += rcInset.top;
				rc.right -= rcInset.right;
				rc.bottom -= rcInset.bottom;
				ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = rect_;
				if( !::IntersectRect(&invalidateRc, &rect_, &rc) ) 
				{
					return;
				}

				Control* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while( pParent = pParent->GetParent() )
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
					{
						return;
					}
				}

				if( pm_ != NULL ) pm_->Invalidate(invalidateRc);
			}
			else {
				Control::Invalidate();
			}
		}
		else {
			Control::Invalidate();
		}
	}

	bool ListElement::Activate()
	{
		if( !Control::Activate() ) return false;
		if (pm_ != NULL) {
			Notify(kEventItemActive);
		}
		return true;
	}

	bool ListElement::IsSelected() const
	{
		return selected_;
	}

	bool ListElement::Select(bool bSelect, bool send_notify)
	{
		if( !IsEnabled() ) return false;
		if( owner_ != NULL && selected_ ) owner_->UnSelectItem(m_iIndex, true);
		if( bSelect == selected_ ) return true;
		selected_ = bSelect;
		if( bSelect && owner_ != NULL ) owner_->SelectItem(m_iIndex, false, send_notify);
		Invalidate();

		return true;
	}

	bool ListElement::SelectMulti(bool bSelect)
	{
		if( !IsEnabled() ) return false;
		if( bSelect == selected_ ) return true;

		selected_ = bSelect;
		if( bSelect && owner_ != NULL ) owner_->SelectMultiItem(m_iIndex);
		Invalidate();
		return true;
	}

	bool ListElement::IsExpanded() const
	{
		return false;
	}

	bool ListElement::Expand(bool /*bExpand = true*/)
	{
		return false;
	}

	void ListElement::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( owner_ != NULL ) owner_->DoEvent(event);
			else Control::DoEvent(event);
			return;
		}

		if( event.type_ == kEventMouseDoubleClick )
		{
			if( IsEnabled() ) {
				Activate();
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventKeyDown && IsEnabled() )
		{
			if( event.key_ == VK_RETURN ) {
				Activate();
				Invalidate();
				return;
			}
		}
		// An important twist: The list-item will send the event not to its immediate
		// parent but to the "attached" list. A list may actually embed several components
		// in its path to the item, but key-presses etc. needs to go to the actual list.
		if( owner_ != NULL ) owner_->DoEvent(event); else Control::DoEvent(event);
	}

	void ListElement::SetAttribute(std::wstring pstrName, std::wstring pstrValue)
	{
		if( _tcsicmp(pstrName.c_str(), _T("selected")) == 0 ) Select();
		else Control::SetAttribute(pstrName, pstrValue);
	}

	void ListElement::DrawItemBk(HDC hDC, const RECT& rcItem)
	{
		ASSERT(owner_);
		if( owner_ == NULL ) return;
		TListInfo* pInfo = owner_->GetListInfo();
		DWORD iBackColor = 0;
		if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;
		if( (btn_state_ & UISTATE_HOT) != 0 ) {
			iBackColor = pInfo->dwHotBkColor;
		}
		if( IsSelected() ) {
			iBackColor = pInfo->dwSelectedBkColor;
		}
		if( !IsEnabled() ) {
			iBackColor = pInfo->dwDisabledBkColor;
		}

		if ( iBackColor != 0 ) {
			RenderEngine::DrawColor(hDC, rect_, GetAdjustColor(iBackColor));
		}

		if( !IsEnabled() ) {
			if( !pInfo->sDisabledImage.empty() ) {
				if( !DrawImage(hDC, pInfo->sDisabledImage) ) {}
				else return;
			}
		}
		if( IsSelected() ) {
			if( !pInfo->sSelectedImage.empty() ) {
				if( !DrawImage(hDC, pInfo->sSelectedImage) ) {}
				else return;
			}
		}
		if( (btn_state_ & UISTATE_HOT) != 0 ) {
			if( !pInfo->sHotImage.empty() ) {
				if( !DrawImage(hDC, pInfo->sHotImage) ) {}
				else return;
			}
		}

		if( !bkimage_.empty() ) {
			if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) {
				if( !DrawImage(hDC, bkimage_) ) {}
			}
		}

		if( bkimage_.empty() ) {
			if( !pInfo->sBkImage.empty() ) {
				if( !DrawImage(hDC, pInfo->sBkImage) ) {}
				else return;
			}
		}

		if ( pInfo->dwLineColor != 0 ) {
			if(pInfo->bShowRowLine) {
				RECT rcLine = { rect_.left, rect_.bottom - 1, rect_.right, rect_.bottom - 1 };
				RenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
			}
			if(pInfo->bShowColumnLine) {
				for( int i = 0; i < pInfo->nColumns; i++ ) {
					RECT rcLine = { pInfo->rcColumn[i].right-1, rect_.top, pInfo->rcColumn[i].right-1, rect_.bottom };
					RenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
				}
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ListLabelElement)

		ListLabelElement::ListLabelElement()
	{
	}

	std::wstring ListLabelElement::GetClass() const
	{
		return DUI_CTR_LISTLABELELEMENT;
	}

	LPVOID ListLabelElement::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_LISTLABELELEMENT) == 0 ) return static_cast<ListLabelElement*>(this);
		return ListElement::GetInterface(pstrName);
	}

	void ListLabelElement::AttachItemClick(const EventCallback & cb)
	{
		on_event_[kEventItemClick] += cb;
	}

	void ListLabelElement::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( owner_ != NULL ) owner_->DoEvent(event);
			else ListElement::DoEvent(event);
			return;
		}

		if( event.type_ == kEventMouseButtonDown )
		{
			if( IsEnabled() ){
				if((GetKeyState(VK_CONTROL) & 0x8000)) {
					SelectMulti(!IsSelected());
				}
				else {
					Select();
				}
			}
			return;
		}
		if( event.type_ == kEventMouseButtonUp ) 
		{
			if( IsEnabled() ){
				Notify(kEventItemClick);
			}
			return;
		}

		if( event.type_ == kEventMouseMove ) 
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
			if( (btn_state_ & UISTATE_HOT) != 0 ) {
				btn_state_ &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		ListElement::DoEvent(event);
	}

	SIZE ListLabelElement::EstimateSize(SIZE szAvailable)
	{
		if( owner_ == NULL ) return CDuiSize(0, 0);
		std::wstring sText = GetText();

		TListInfo* pInfo = owner_->GetListInfo();
		SIZE cXY = cxy_fired_sz_;
		if( cXY.cy == 0 && pm_ != NULL ) {
			cXY.cy = GlobalManager::GetFontInfo(pInfo->nFont, pm_->GetPaintDC())->tm_.tmHeight + 8;
			cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		if( cXY.cx == 0 && pm_ != NULL ) {
			RECT rcText = { 0, 0, 9999, cXY.cy };
			if( pInfo->bShowHtml ) {
				int nLinks = 0;
				RenderEngine::DrawHtmlText(pm_->GetPaintDC(), pm_, rcText, sText, 0, NULL, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
			}
			else {
				RenderEngine::DrawText(pm_->GetPaintDC(), pm_, rcText, sText, 0, pInfo->nFont, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
			}
			cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;        
		}

		return cXY;
	}

	void ListLabelElement::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&paint_rect_, &rcPaint, &rect_) ) return;
		DrawItemBk(hDC, rect_);
		DrawItemText(hDC, rect_);
	}

	void ListLabelElement::DrawItemText(HDC hDC, const RECT& rcItem)
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
		rcText.left += pInfo->rcTextPadding.left;
		rcText.right -= pInfo->rcTextPadding.right;
		rcText.top += pInfo->rcTextPadding.top;
		rcText.bottom -= pInfo->rcTextPadding.bottom;

		if( pInfo->bShowHtml )
			RenderEngine::DrawHtmlText(hDC, pm_, rcText, sText, iTextColor, \
			NULL, NULL, nLinks, pInfo->uTextStyle);
		else
			RenderEngine::DrawText(hDC, pm_, rcText, sText, iTextColor, \
			pInfo->nFont, pInfo->uTextStyle);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ListTextElement)

	ListTextElement::ListTextElement() : links_(0), hover_link_(-1), owner_(NULL)
	{
		::ZeroMemory(&link_rects_, sizeof(link_rects_));
	}

	ListTextElement::~ListTextElement()
	{
		std::wstring* pText;
		for( int it = 0; it < texts_.GetSize(); it++ ) {
			pText = static_cast<std::wstring*>(texts_[it]);
			if( pText ) delete pText;
		}
		texts_.Empty();
	}

	std::wstring ListTextElement::GetClass() const
	{
		return DUI_CTR_LISTTEXTELEMENT;
	}

	LPVOID ListTextElement::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_LISTTEXTELEMENT) == 0 ) return static_cast<ListTextElement*>(this);
		return ListLabelElement::GetInterface(pstrName);
	}

	UINT ListTextElement::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN | ( (IsEnabled() && links_array_ > 0) ? UIFLAG_SETCURSOR : 0);
	}

	void ListTextElement::AttachLinkItemClick(const EventCallback & cb)
	{
		on_event_[kEventItemLinkClick] += cb;
	}

	std::wstring ListTextElement::GetText(int iIndex) const
	{
		std::wstring* pText = static_cast<std::wstring*>(texts_.GetAt(iIndex));
		if( pText ) {
			if (!IsResourceText()) 
				return pText->c_str();
			return ResourceManager::GetInstance()->GetText(pText->c_str());
		}
		return L"";
	}

	void ListTextElement::SetText(int iIndex, std::wstring pstrText)
	{
		if( owner_ == NULL ) return;

		TListInfo* pInfo = owner_->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( texts_.GetSize() < pInfo->nColumns ) { texts_.Add(NULL); }

		std::wstring* pText = static_cast<std::wstring*>(texts_[iIndex]);
		if( (pText == NULL && pstrText.empty()) || (pText && *pText == pstrText) ) return;

		if ( pText ) {delete pText; pText = NULL;}
		texts_.SetAt(iIndex, new std::wstring(pstrText));

		Invalidate();
	}

	void ListTextElement::SetOwner(Control* pOwner)
	{
		ListElement::SetOwner(pOwner);
		owner_ = static_cast<IList*>(pOwner->GetInterface(_T("IList")));
	}

	std::wstring* ListTextElement::GetLinkContent(int iIndex)
	{
		if( iIndex >= 0 && iIndex < links_) return &links_array_[iIndex];
		return NULL;
	}

	void ListTextElement::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( owner_ != NULL ) owner_->DoEvent(event);
			else ListLabelElement::DoEvent(event);
			return;
		}

		// When you hover over a link
		if( event.type_ == kEventSetCursor ) {
			for( int i = 0; i < links_; i++ ) {
				if( ::PtInRect(&link_rects_[i], event.mouse_point_) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}      
		}
		if( event.type_ == kEventMouseButtonUp && IsEnabled() ) {
			for( int i = 0; i < links_; i++ ) {
				if( ::PtInRect(&link_rects_[i], event.mouse_point_) ) {
				Notify(kEventItemLinkClick, i);
					return;
				}
			}
		}
		if( links_array_ > 0 && event.type_ == kEventMouseMove ) {
			int nHoverLink = -1;
			for( int i = 0; i < links_; i++ ) {
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
		if( links_array_ > 0 && event.type_ == kEventMouseLeave ) {
			if(hover_link_ != -1) {
				Invalidate();
				hover_link_ = -1;
			}
		}
		ListLabelElement::DoEvent(event);
	}

	SIZE ListTextElement::EstimateSize(SIZE szAvailable)
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

	void ListTextElement::DrawItemText(HDC hDC, const RECT& rcItem)
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

		links_ = 0;
		int nLinks = lengthof(link_rects_);
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			RECT rcItem = { pInfo->rcColumn[i].left, rect_.top, pInfo->rcColumn[i].right, rect_.bottom };
			rcItem.left += pInfo->rcTextPadding.left;
			rcItem.right -= pInfo->rcTextPadding.right;
			rcItem.top += pInfo->rcTextPadding.top;
			rcItem.bottom -= pInfo->rcTextPadding.bottom;

			std::wstring strText;
			if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);
			else strText.assign(GetText(i));

			if( pInfo->bShowHtml )
				RenderEngine::DrawHtmlText(hDC, pm_, rcItem, strText, iTextColor, \
				&link_rects_[links_], &links_array_[links_], nLinks, pInfo->uTextStyle);
			else
				RenderEngine::DrawText(hDC, pm_, rcItem, strText, iTextColor, \
				pInfo->nFont, pInfo->uTextStyle);

			links_ += nLinks;
			nLinks = lengthof(link_rects_) - links_;
		}
		for( int i = links_; i < lengthof(link_rects_); i++ ) {
			::ZeroMemory(link_rects_ + i, sizeof(RECT));
			((std::wstring*)(links_array_ + i))->clear();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ListContainerElement)

	ListContainerElement::ListContainerElement() : 
		m_iIndex(-1),
		owner_(NULL), 
		selected_(false),
		btn_state_(0)
	{
	}

	std::wstring ListContainerElement::GetClass() const
	{
		return DUI_CTR_LISTCONTAINERELEMENT;
	}

	UINT ListContainerElement::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN;
	}

	LPVOID ListContainerElement::GetInterface(std::wstring strName)
	{
		auto pstrName = strName.c_str();
		if( _tcsicmp(pstrName, DUI_CTR_LISTITEM) == 0 ) return static_cast<IListItem*>(this);
		if( _tcsicmp(pstrName, DUI_CTR_LISTCONTAINERELEMENT) == 0 ) return static_cast<ListContainerElement*>(this);
		return Box::GetInterface(pstrName);
	}

	int ListContainerElement::GetFixedHeight()
	{
		return __super::GetFixedHeight();
	}

	void ListContainerElement::AttachItemActive(const EventCallback & cb)
	{
		on_event_[kEventItemActive] += cb;
	}

	void ListContainerElement::AttachItemClick(const EventCallback & cb)
	{
		on_event_[kEventItemClick] += cb;
	}

	void ListContainerElement::AttachTimer(const EventCallback & cb)
	{
		on_event_[kEventTimer] += cb;
	}

	void ListContainerElement::AttachMouseMenu(const EventCallback & cb)
	{
		on_event_[kEventMouseMenu] += cb;
	}

	IListOwner* ListContainerElement::GetOwner()
	{
		return owner_;
	}

	void ListContainerElement::SetOwner(Control* pOwner)
	{
		owner_ = static_cast<IListOwner*>(pOwner->GetInterface(_T("IListOwner")));
	}

	void ListContainerElement::SetVisible(bool bVisible)
	{
		Box::SetVisible(bVisible);
		if( !IsVisible() && selected_)
		{
			selected_ = false;
			if( owner_ != NULL ) owner_->SelectItem(-1);
		}
	}

	void ListContainerElement::SetEnabled(bool bEnable)
	{
		Control::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			btn_state_ = 0;
		}
	}

	int ListContainerElement::GetIndex() const
	{
		return m_iIndex;
	}

	void ListContainerElement::SetIndex(int iIndex)
	{
		m_iIndex = iIndex;
	}

	void ListContainerElement::Invalidate()
	{
		if( !IsVisible() ) return;

		if( GetParent() ) {
			Box* pParentContainer = static_cast<Box*>(GetParent()->GetInterface(_T("Container")));
			if( pParentContainer ) {
				RECT rc = pParentContainer->GetPos();
				RECT rcInset = pParentContainer->GetInset();
				rc.left += rcInset.left;
				rc.top += rcInset.top;
				rc.right -= rcInset.right;
				rc.bottom -= rcInset.bottom;
				ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = rect_;
				if( !::IntersectRect(&invalidateRc, &rect_, &rc) ) 
				{
					return;
				}

				Control* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while( pParent = pParent->GetParent() )
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
					{
						return;
					}
				}

				if( pm_ != NULL ) pm_->Invalidate(invalidateRc);
			}
			else {
				Box::Invalidate();
			}
		}
		else {
			Box::Invalidate();
		}
	}

	bool ListContainerElement::Activate()
	{
		if( !Box::Activate() ) return false;
		if (pm_ != NULL) {
			Notify(kEventItemActive);
		}
		return true;
	}

	bool ListContainerElement::IsSelected() const
	{
		return selected_;
	}

	bool ListContainerElement::Select(bool bSelect, bool send_notify)
	{
		if( !IsEnabled() ) return false;
		if( owner_ != NULL && selected_ ) owner_->UnSelectItem(m_iIndex, true);
		if( bSelect == selected_ ) return true;
		selected_ = bSelect;
		if( bSelect && owner_ != NULL ) owner_->SelectItem(m_iIndex, false, send_notify);
		Invalidate();

		return true;
	}

	bool ListContainerElement::SelectMulti(bool bSelect)
	{
		if( !IsEnabled() ) return false;
		if( bSelect == selected_ ) return true;

		selected_ = bSelect;
		if( bSelect && owner_ != NULL ) owner_->SelectMultiItem(m_iIndex);
		Invalidate();
		return true;
	}

	bool ListContainerElement::IsExpanded() const
	{
		return false;
	}

	bool ListContainerElement::Expand(bool /*bExpand = true*/)
	{
		return false;
	}

	void ListContainerElement::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( owner_ != NULL ) owner_->DoEvent(event);
			else Box::DoEvent(event);
			return;
		}

		if( event.type_ == kEventMouseDoubleClick )
		{
			if( IsEnabled() ) {
				Activate();
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventKeyDown && IsEnabled() )
		{
			if( event.key_ == VK_RETURN ) {
				Activate();
				Invalidate();
				return;
			}
		}
		if( event.type_ == kEventMouseButtonDown )
		{
			if( IsEnabled() ){
				if((GetKeyState(VK_CONTROL) & 0x8000)) {
					SelectMulti(!IsSelected());
				}
				else Select();
			}
			return;
		}
		if( event.type_ == kEventMouseButtonUp ) 
		{
			if( IsEnabled() ){
				Notify(kEventItemClick);
			}
			return;
		}
		if( event.type_ == kEventMouseMove )
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
			if( (btn_state_ & UISTATE_HOT) != 0 ) {
				btn_state_ &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventTimer ){
			Notify(kEventTimer, event.w_param_, event.l_param_);
			return;
		}

		if( event.type_ == kEventMouseMenu ){
			if( IsContextMenuUsed() ) {
				Notify(kEventMouseMenu, event.w_param_, event.l_param_);
				return;
			}
		}
		// An important twist: The list-item will send the event not to its immediate
		// parent but to the "attached" list. A list may actually embed several components
		// in its path to the item, but key-presses etc. needs to go to the actual list.
		if( owner_ != NULL ) owner_->DoEvent(event); else Control::DoEvent(event);
	}

	void ListContainerElement::SetAttribute(std::wstring pstrName, std::wstring pstrValue)
	{
		if( _tcsicmp(pstrName.c_str(), _T("selected")) == 0 ) Select();
		else Box::SetAttribute(pstrName, pstrValue);
	}

	void ListContainerElement::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&paint_rect_, &rcPaint, &rect_) ) return;

		RECT rcTemp = { 0 };
		if( !::IntersectRect(&rcTemp, &rcPaint, &rect_) ) return;

		RenderClip clip;
		RenderClip::GenerateClip(hDC, rcTemp, clip);
		Control::DoPaint(hDC, rcPaint);

		DrawItemBk(hDC, rect_);

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
	}

	void ListContainerElement::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		return;
	}

	void ListContainerElement::DrawItemBk(HDC hDC, const RECT& rcItem)
	{
		ASSERT(owner_);
		if (owner_ == NULL) {
			return;
		} 

		TListInfo* pInfo = owner_->GetListInfo();
		DWORD iBackColor = 0;
		if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;

		if( (btn_state_ & UISTATE_HOT) != 0 ) {
			iBackColor = pInfo->dwHotBkColor;
		}
		if( IsSelected() ) {
			iBackColor = pInfo->dwSelectedBkColor;
		}
		if( !IsEnabled() ) {
			iBackColor = pInfo->dwDisabledBkColor;
		}
		if ( iBackColor != 0 ) {
			RenderEngine::DrawColor(hDC, rect_, GetAdjustColor(iBackColor));
		}

		if( !IsEnabled() ) {
			if( !pInfo->sDisabledImage.empty() ) {
				if( !DrawImage(hDC, pInfo->sDisabledImage) ) {}
				else return;
			}
		}
		if( IsSelected() ) {
			if( !pInfo->sSelectedImage.empty() ) {
				if( !DrawImage(hDC, pInfo->sSelectedImage) ) {}
				else return;
			}
		}
		if( (btn_state_ & UISTATE_HOT) != 0 ) {
			if( !pInfo->sHotImage.empty() ) {
				if( !DrawImage(hDC, pInfo->sHotImage) ) {}
				else return;
			}
		}
		if( !bkimage_.empty() ) {
			if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) {
				if( !DrawImage(hDC, bkimage_) ) {}
			}
		}

		if( bkimage_.empty() ) {
			if( !pInfo->sBkImage.empty() ) {
				if( !DrawImage(hDC, pInfo->sBkImage) ) {}
				else return;
			}
		}

		if ( pInfo->dwLineColor != 0 ) {
			if(pInfo->bShowRowLine) {
				RECT rcLine = { rect_.left, rect_.bottom - 1, rect_.right, rect_.bottom - 1 };
				RenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
			}
			if(pInfo->bShowColumnLine) {
				for( int i = 0; i < pInfo->nColumns; i++ ) {
					RECT rcLine = { pInfo->rcColumn[i].right-1, rect_.top, pInfo->rcColumn[i].right-1, rect_.bottom };
					RenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
				}
			}
		}
	}

	void ListContainerElement::SetPos(RECT rc, bool bNeedInvalidate)
	{	
		HBox::SetPos(rc, bNeedInvalidate);
		if( owner_ == NULL ) return;
		UINT uListType = owner_->GetListType();
		if(uListType != kListList && uListType != kListTree) return;

		List* pList = static_cast<List*>(owner_);

 		if (uListType == kListTree)
 		{
 			pList = (List*)pList->Control::GetInterface(_T("List"));
			if (pList == NULL) return;
 		}
	
		ListHeader *pHeader = pList->GetHeader();
		if (pHeader == NULL || !pHeader->IsVisible()) return;
		int nCount = items_.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			Control *pListItem = static_cast<Control*>(items_[i]);
			Control *pHeaderItem = pHeader->GetItemAt(i);
			if (pHeaderItem == NULL) return;
			RECT rcHeaderItem = pHeaderItem->GetPos();
			if (pListItem != NULL && !(rcHeaderItem.left ==0 && rcHeaderItem.right ==0) )
			{
				RECT rt = pListItem->GetPos();
				rt.left = rcHeaderItem.left;
				rt.right = rcHeaderItem.right;
				pListItem->SetPos(rt);
			}
		}
	}
} // namespace DuiLib
