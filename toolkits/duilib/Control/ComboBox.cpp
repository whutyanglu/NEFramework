#include "StdAfx.h"
#include "ComboBox.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(ComboBox)

	ComboBox::ComboBox()
	{
		arrow_width_ = 0;
	}

	std::wstring ComboBox::GetClass() const
	{
		return _T("ComboBox");
	}

	void ComboBox::SetAttribute(std::wstring pstrName, std::wstring pstrValue)
	{
		if (_tcsicmp(pstrName.c_str(), _T("arrowimage")) == 0)
			arrow_image_ = pstrValue;
		else
			Combo::SetAttribute(pstrName, pstrValue);
	}

	void ComboBox::PaintStatusImage(HDC hDC)
	{
		if (arrow_image_.empty())
			Combo::PaintStatusImage(hDC);
		else
		{
			// get index
			if( IsFocused() ) btn_state_ |= UISTATE_FOCUSED;
			else btn_state_ &= ~ UISTATE_FOCUSED;
			if( !IsEnabled() ) btn_state_ |= UISTATE_DISABLED;
			else btn_state_ &= ~ UISTATE_DISABLED;

			int nIndex = 0;
			if ((btn_state_ & UISTATE_DISABLED) != 0)
				nIndex = 4;
			else if ((btn_state_ & UISTATE_PUSHED) != 0)
				nIndex = 2;
			else if ((btn_state_ & UISTATE_HOT) != 0)
				nIndex = 1;
			else if ((btn_state_ & UISTATE_FOCUSED) != 0)
				nIndex = 3;

			// make modify string
			std::wstring sModify = arrow_image_;

			size_t nPos1 = sModify.find(_T("source"));
			size_t nPos2 = sModify.find(_T("'"), nPos1 + 7);
			if (nPos2 == std::string::npos) return; //first
			int nPos3 = sModify.find(_T("'"), nPos2 + 1);
			if (nPos3 == -1) return; //second

			CDuiRect rcBmpPart;
			LPTSTR lpszValue = NULL;
			rcBmpPart.left = _tcstol(sModify.c_str() + nPos2 + 1, &lpszValue, 10);  ASSERT(lpszValue);
			rcBmpPart.top = _tcstol(lpszValue + 1, &lpszValue, 10);    ASSERT(lpszValue);    
			rcBmpPart.right = _tcstol(lpszValue + 1, &lpszValue, 10);  ASSERT(lpszValue);    
			rcBmpPart.bottom = _tcstol(lpszValue + 1, &lpszValue, 10); ASSERT(lpszValue); 

			arrow_width_ = rcBmpPart.GetWidth() / 5;
			rcBmpPart.left += nIndex * arrow_width_;
			rcBmpPart.right = rcBmpPart.left + arrow_width_;

			CDuiRect rcDest(0, 0, rect_.right - rect_.left, rect_.bottom - rect_.top);
			rcDest.Deflate(GetBorderSize(), GetBorderSize());
			rcDest.left = rcDest.right - arrow_width_;

			std::wstring sSource = sModify.substr(nPos1, nPos3 + 1 - nPos1);
			std::wstring sReplace;
			sReplace = nbase::StringPrintf(_T("source='%d,%d,%d,%d' dest='%d,%d,%d,%d'"),
				rcBmpPart.left, rcBmpPart.top, rcBmpPart.right, rcBmpPart.bottom,
				rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);
			
			nbase::StringReplaceAll(sSource, sReplace, sModify);

			// draw image
			if (!DrawImage(hDC, arrow_image_, sModify))
				{}
		}
	}

	void ComboBox::PaintText(HDC hDC)
	{
		RECT rcText = rect_;
		rcText.left += text_padding_rc_.left;
		rcText.right -= text_padding_rc_.right;
		rcText.top += text_padding_rc_.top;
		rcText.bottom -= text_padding_rc_.bottom;

		rcText.right -= arrow_width_; // add this line than CComboUI::PaintText(HDC hDC)

		if( cur_sel_ >= 0 ) {
			Control* pControl = static_cast<Control*>(items_[cur_sel_]);
			IListItem* pElement = static_cast<IListItem*>(pControl->GetInterface(_T("ListItem")));
			if( pElement != NULL ) {
				pElement->DrawItemText(hDC, rcText);
			}
			else {
				RECT rcOldPos = pControl->GetPos();
				pControl->SetPos(rcText);
				pControl->DoPaint(hDC, rcText);
				pControl->SetPos(rcOldPos);
			}
		}
	}
}
