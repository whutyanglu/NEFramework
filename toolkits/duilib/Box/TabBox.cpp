#include "StdAfx.h"
#include "TabBox.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(TabBox)
	TabBox::TabBox() : cur_sel_(-1)
	{
	}

	std::wstring TabBox::GetClass() const
	{
		return _T("TabBox");
	}

	LPVOID TabBox::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_TABBOX) == 0 ) return static_cast<TabBox*>(this);
		return Box::GetInterface(pstrName.c_str());
	}

	bool TabBox::Add(Control* pControl)
	{
		bool ret = Box::Add(pControl);
		if( !ret ) return ret;

		if(cur_sel_ == -1 && pControl->IsVisible()){
			cur_sel_ = GetItemIndex(pControl);
		}
		else{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool TabBox::AddAt(Control* pControl, int iIndex)
	{
		bool ret = Box::AddAt(pControl, iIndex);
		if( !ret ) return ret;

		if(cur_sel_ == -1 && pControl->IsVisible()){
			cur_sel_ = GetItemIndex(pControl);
		}
		else if( cur_sel_ != -1 && iIndex <= cur_sel_ ){
			cur_sel_ += 1;
		}
		else{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool TabBox::Remove(Control* pControl)
	{
		if( pControl == NULL) return false;

		int index = GetItemIndex(pControl);
		bool ret = Box::Remove(pControl);
		if( !ret ) return false;

		if( cur_sel_ == index){
			if( GetCount() > 0 ){
				cur_sel_=0;
				GetItemAt(cur_sel_)->SetVisible(true);
			}
			else
				cur_sel_=-1;
			NeedParentUpdate();
		}
		else if( cur_sel_ > index ){
			cur_sel_ -= 1;
		}

		return ret;
	}

	void TabBox::RemoveAll()
	{
		cur_sel_ = -1;
		Box::RemoveAll();
		NeedParentUpdate();
	}

	int TabBox::GetCurSel() const
	{
		return cur_sel_;
	}

	bool TabBox::SelectItem(int iIndex)
	{
		if( iIndex < 0 || iIndex >= items_.GetSize() ) return false;
		if( iIndex == cur_sel_ ) return true;

		int iOldSel = cur_sel_;
		cur_sel_ = iIndex;
		for( int it = 0; it < items_.GetSize(); it++ )
		{
			if( it == iIndex ) {
				GetItemAt(it)->SetVisible(true);
				GetItemAt(it)->SetFocus();
				SetPos(rect_);
			}
			else GetItemAt(it)->SetVisible(false);
		}
		NeedParentUpdate();

		if( pm_ != NULL ) {
			pm_->SetNextTabControl();
			//pm_->SendNotify(this, DUI_MSGTYPE_TABSELECT, cur_sel_, iOldSel);
			Notify(kEventTab, cur_sel_, iOldSel);
		}
		return true;
	}

	bool TabBox::SelectItem( Control* pControl )
	{
		int iIndex=GetItemIndex(pControl);
		if (iIndex==-1)
			return false;
		else
			return SelectItem(iIndex);
	}

	void TabBox::SetAttribute(std::wstring pstrName, std::wstring pstrValue)
	{
		if( _tcsicmp(pstrName.c_str(), _T("selectedid")) == 0 ) SelectItem(_ttoi(pstrValue.c_str()));
		return Box::SetAttribute(pstrName, pstrValue);
	}

	void TabBox::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		rc = rect_;

		// Adjust for inset
		rc.left += rc_inset_.left;
		rc.top += rc_inset_.top;
		rc.right -= rc_inset_.right;
		rc.bottom -= rc_inset_.bottom;

		for( int it = 0; it < items_.GetSize(); it++ ) {
			Control* pControl = static_cast<Control*>(items_[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it);
				continue;
			}

			if( it != cur_sel_ ) continue;

			RECT rcPadding = pControl->GetPadding();
			rc.left += rcPadding.left;
			rc.top += rcPadding.top;
			rc.right -= rcPadding.right;
			rc.bottom -= rcPadding.bottom;

			SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cx == 0 ) {
				sz.cx = MAX(0, szAvailable.cx);
			}
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			if(sz.cy == 0) {
				sz.cy = MAX(0, szAvailable.cy);
			}
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy};
			pControl->SetPos(rcCtrl);
		}
	}
}
