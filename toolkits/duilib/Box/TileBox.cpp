#include "StdAfx.h"
#include "TileBox.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(TileBox)
	TileBox::TileBox() : columns_(1)
	{
		item_size_.cx = item_size_.cy = 0;
	}

	std::wstring TileBox::GetClass() const
	{
		return _T("TileBox");
	}

	LPVOID TileBox::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_TILEBox) == 0 ) return static_cast<TileBox*>(this);
		return Box::GetInterface(pstrName);
	}

	SIZE TileBox::GetItemSize() const
	{
		return item_size_;
	}

	void TileBox::SetItemSize(SIZE szItem)
	{
		if( item_size_.cx != szItem.cx || item_size_.cy != szItem.cy ) {
			item_size_ = szItem;
			NeedUpdate();
		}
	}

	int TileBox::GetColumns() const
	{
		return columns_;
	}

	void TileBox::SetColumns(int nCols)
	{
		if( nCols <= 0 ) return;
		columns_ = nCols;
		NeedUpdate();
	}

	void TileBox::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("itemsize")) == 0 ) {
			SIZE szItem = { 0 };
			LPTSTR pstr = NULL;
			szItem.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szItem.cy = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);     
			SetItemSize(szItem);
		}
		else if( _tcsicmp(pstrName, _T("columns")) == 0 ) SetColumns(_ttoi(pstrValue));
		else Box::SetAttribute(pstrName, pstrValue);
	}

	void TileBox::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		rc = rect_;

		// Adjust for inset
		rc.left += rc_inset_.left;
		rc.top += rc_inset_.top;
		rc.right -= rc_inset_.right;
		rc.bottom -= rc_inset_.bottom;

		if( items_.GetSize() == 0) {
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) rc.right -= vertical_scrollbar_->GetFixedWidth();
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) rc.bottom -= horizontal_scrollbar_->GetFixedHeight();

		// Position the elements
		if( item_size_.cx > 0 ) columns_ = (rc.right - rc.left) / item_size_.cx;
		if( columns_ == 0 ) columns_ = 1;

		int cyNeeded = 0;
		int cxWidth = (rc.right - rc.left) / columns_;
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) 
			cxWidth = (rc.right - rc.left + horizontal_scrollbar_->GetScrollRange() ) / columns_; ;

		int cyHeight = 0;
		int iCount = 0;
		POINT ptTile = { rc.left, rc.top };
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
			ptTile.y -= vertical_scrollbar_->GetScrollPos();
		}
		int iPosX = rc.left;
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
			iPosX -= horizontal_scrollbar_->GetScrollPos();
			ptTile.x = iPosX;
		}
		for( int it1 = 0; it1 < items_.GetSize(); it1++ ) {
			Control* pControl = static_cast<Control*>(items_[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it1);
				continue;
			}

			// Determine size
			RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
			if( (iCount % columns_) == 0 ){
				int iIndex = iCount;
				for( int it2 = it1; it2 < items_.GetSize(); it2++ ) {
					Control* pLineControl = static_cast<Control*>(items_[it2]);
					if( !pLineControl->IsVisible() ) continue;
					if( pLineControl->IsFloat() ) continue;

					RECT rcPadding = pLineControl->GetPadding();
					SIZE szAvailable = { rcTile.right - rcTile.left - rcPadding.left - rcPadding.right, 9999 };
					if( iIndex == iCount || (iIndex + 1) % columns_ == 0 ) {
						szAvailable.cx -= child_padding_ / 2;
					}
					else {
						szAvailable.cx -= child_padding_;
					}

					if( szAvailable.cx < pControl->GetMinWidth() ) szAvailable.cx = pControl->GetMinWidth();
					if( szAvailable.cx > pControl->GetMaxWidth() ) szAvailable.cx = pControl->GetMaxWidth();

					SIZE szTile = pLineControl->EstimateSize(szAvailable);
					if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
					if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
					if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
					if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();

					cyHeight = MAX(cyHeight, szTile.cy + rcPadding.top + rcPadding.bottom);
					if( (++iIndex % columns_) == 0) break;
				}
			}

			RECT rcPadding = pControl->GetPadding();

			rcTile.left += rcPadding.left + child_padding_ / 2;
			rcTile.right -= rcPadding.right + child_padding_ / 2;
			if( (iCount % columns_) == 0 ) {
				rcTile.left -= child_padding_ / 2;
			}

			if( ( (iCount + 1) % columns_) == 0 ) {
				rcTile.right += child_padding_ / 2;
			}

			// Set position
			rcTile.top = ptTile.y + rcPadding.top;
			rcTile.bottom = ptTile.y + cyHeight;

			SIZE szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };
			SIZE szTile = pControl->EstimateSize(szAvailable);
			if( szTile.cx == 0 ) szTile.cx = szAvailable.cx;
			if( szTile.cy == 0 ) szTile.cy = szAvailable.cy;
			if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
			if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
			if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
			if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();
			RECT rcPos = {(rcTile.left + rcTile.right - szTile.cx) / 2, (rcTile.top + rcTile.bottom - szTile.cy) / 2,
				(rcTile.left + rcTile.right - szTile.cx) / 2 + szTile.cx, (rcTile.top + rcTile.bottom - szTile.cy) / 2 + szTile.cy};
			pControl->SetPos(rcPos);

			if( (++iCount % columns_) == 0 ) {
				ptTile.x = iPosX;
				ptTile.y += cyHeight + child_padding_;
				cyHeight = 0;
			}
			else {
				ptTile.x += cxWidth;
			}
			cyNeeded = rcTile.bottom - rc.top;
			if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) cyNeeded += vertical_scrollbar_->GetScrollPos();
		}

		// Process the scrollbar
		ProcessScrollBar(rc, 0, cyNeeded);
	}
}
