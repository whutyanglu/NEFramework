#include "StdAfx.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(GroupBox)

	//////////////////////////////////////////////////////////////////////////
	//
	GroupBox::GroupBox(): text_style_(DT_SINGLELINE | DT_VCENTER | DT_CENTER), text_color_(0), 
		disabled_text_color_(0), font_(-1)
	{
		SetInset(CDuiRect(20, 25, 20, 20));
	}

	GroupBox::~GroupBox()
	{
	}

	std::wstring GroupBox::GetClass() const
	{
		return _T("GroupBox");
	}

	LPVOID GroupBox::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("GroupBox")) == 0 ) return static_cast<GroupBox*>(this);
		return VBox::GetInterface(pstrName);
	}
	void GroupBox::SetTextColor(DWORD dwTextColor)
	{
		text_color_ = dwTextColor;
		Invalidate();
	}

	DWORD GroupBox::GetTextColor() const
	{
		return text_color_;
	}
	void GroupBox::SetDisabledTextColor(DWORD dwTextColor)
	{
		disabled_text_color_ = dwTextColor;
		Invalidate();
	}

	DWORD GroupBox::GetDisabledTextColor() const
	{
		return disabled_text_color_;
	}
	void GroupBox::SetFont(int index)
	{
		font_ = index;
		Invalidate();
	}

	int GroupBox::GetFont() const
	{
		return font_;
	}
	void GroupBox::PaintText(HDC hDC)
	{
		std::wstring sText = GetText();
		if( sText.empty() ) {
			return;
		}

		if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();
		if( disabled_text_color_ == 0 ) disabled_text_color_ = GlobalManager::GetDefaultDisabledColor();
		if( sText.empty() ) return;

		CDuiRect rcText = rect_;
		rcText.Deflate(5,5);
		SIZE szAvailable = { rcText.right - rcText.left, rcText.bottom - rcText.top };
		SIZE sz = CalcrectSize(szAvailable);

		//¼ÆËãÎÄ×ÖÇøÓò
		rcText.left = rcText.left + 15;
		rcText.top = rcText.top - 5;
		rcText.right = rcText.left + sz.cx;
		rcText.bottom = rcText.top + sz.cy;

		DWORD dwTextColor = text_color_;
		if(!IsEnabled()) dwTextColor = disabled_text_color_;
		RenderEngine::DrawText(hDC, pm_, rcText, sText, dwTextColor, font_, text_style_, GetAdjustColor(back_color_));
	}
	void GroupBox::PaintBorder(HDC hDC)
	{
		int nBorderSize;
		SIZE cxyBorderRound;
		RECT rcBorderSize;
		if (pm_) {
			nBorderSize = GetManager()->GetDPIObj()->Scale(border_size_);
			cxyBorderRound = GetManager()->GetDPIObj()->Scale(border_round_sz_);
			rcBorderSize = GetManager()->GetDPIObj()->Scale(border_size_rect_);
		}
		else {
			nBorderSize = border_size_;
			cxyBorderRound = border_round_sz_;
			rcBorderSize = border_size_rect_;
		}

		if( nBorderSize > 0 )
		{
			CDuiRect rcItem = rect_;
			rcItem.Deflate(5, 5);
			
			if( cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0 )//»­Ô²½Ç±ß¿ò
			{
				if (IsFocused() && focused_border_color_ != 0)
					RenderEngine::DrawRoundRect(hDC, rcItem, nBorderSize, cxyBorderRound.cx, cxyBorderRound.cy, GetAdjustColor(focused_border_color_));
				else
					RenderEngine::DrawRoundRect(hDC, rcItem, nBorderSize, cxyBorderRound.cx, cxyBorderRound.cy, GetAdjustColor(border_color_));
			}
			else
			{
				if (IsFocused() && focused_border_color_ != 0)
					RenderEngine::DrawRect(hDC, rcItem, nBorderSize, GetAdjustColor(focused_border_color_));
				else
					RenderEngine::DrawRect(hDC, rcItem, nBorderSize, GetAdjustColor(border_color_));
			}
		}

		PaintText(hDC);
	}

	SIZE GroupBox::CalcrectSize(SIZE szAvailable)
	{
		SIZE cxyFixed = GetFixedXY();
		RECT rcText = { 0, 0, MAX(szAvailable.cx, cxyFixed.cx), 20 };
		
		std::wstring sText = GetText();

		RenderEngine::DrawText(pm_->GetPaintDC(), pm_, rcText, sText, text_color_, font_, DT_CALCRECT | text_style_);
		SIZE cXY = {rcText.right - rcText.left, rcText.bottom - rcText.top};
		return cXY;
	}
	void GroupBox::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("textcolor")) == 0 ) {
			SetTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("disabledtextcolor")) == 0 ) {
			SetDisabledTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("font")) == 0 ) 
		{
			SetFont(_ttoi(pstrValue));
		}

		VBox::SetAttribute(strName, strValue);
	}
}
