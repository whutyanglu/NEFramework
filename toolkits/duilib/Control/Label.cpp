#include "StdAfx.h"
#include "Label.h"

#include <atlconv.h>
namespace ui
{
	IMPLEMENT_DUICONTROL(Label)

	Label::Label() : text_style_(DT_VCENTER | DT_SINGLELINE), text_color_(0), 
		disabled_text_color_(0),
		font_(-1),
		show_html_(false),
		auto_calc_width_(false)
	{
		::ZeroMemory(&text_padding_rect_, sizeof(text_padding_rect_));
	}

	Label::~Label()
	{
	}

	std::wstring Label::GetClass() const
	{
		return _T("Label");
	}

	LPVOID Label::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("Label")) == 0 ) return static_cast<Label*>(this);
		return Control::GetInterface(pstrName);
	}
	
	UINT Label::GetControlFlags() const
	{
		return IsEnabled() ? UIFLAG_SETCURSOR : 0;
	}

	int Label::GetFixedHeight()
	{
		if (cxy_fired_sz_.cy == -1) {
			cxy_fired_sz_.cy = GlobalManager::GetTextPixelHeight(pm_->GetPaintDC(), GetFont());
			if (!bkimage_.empty()) {
				cxy_fired_sz_.cy = (pm_->GetImage(bkimage_)->nX > cxy_fired_sz_.cy ? pm_->GetImage(bkimage_)->nX : cxy_fired_sz_.cy);
			}
		}

		return __super::GetFixedHeight();
	}

	int Label::GetFixedWidth()
	{
		if (cxy_fired_sz_.cx == -1) {
			if (!text_.empty()) {
				cxy_fired_sz_.cx = GlobalManager::GetTextPixelWidth(pm_->GetPaintDC(), GetFont(), text_);
			}
			if (!bkimage_.empty()) {
				cxy_fired_sz_.cx = (pm_->GetImage(bkimage_)->nX > cxy_fired_sz_.cx ? pm_->GetImage(bkimage_)->nX : cxy_fired_sz_.cx);
			}
		}

		return __super::GetFixedWidth();
	}
	void Label::SetTextStyle(UINT uStyle)
	{
		text_style_ = uStyle;
		Invalidate();
	}

	UINT Label::GetTextStyle() const
	{
		return text_style_;
	}

	void Label::SetTextColor(DWORD dwTextColor)
	{
		text_color_ = dwTextColor;
		Invalidate();
	}

	DWORD Label::GetTextColor() const
	{
		return text_color_;
	}

	void Label::SetDisabledTextColor(DWORD dwTextColor)
	{
		disabled_text_color_ = dwTextColor;
		Invalidate();
	}

	DWORD Label::GetDisabledTextColor() const
	{
		return disabled_text_color_;
	}

	void Label::SetFont(int index)
	{
		font_ = index;
		Invalidate();
	}

	int Label::GetFont() const
	{
		return font_;
	}

	RECT Label::GetTextPadding() const
	{
		return text_padding_rect_;
	}

	void Label::SetTextPadding(RECT rc)
	{
		text_padding_rect_ = rc;
		Invalidate();
	}

	bool Label::IsShowHtml()
	{
		return show_html_;
	}

	void Label::SetShowHtml(bool bShowHtml)
	{
		if( show_html_ == bShowHtml ) return;

		show_html_ = bShowHtml;
		Invalidate();
	}

	SIZE Label::EstimateSize(SIZE szAvailable)
	{
		if (auto_calc_width_) {
			std::wstring sText = GetText();

			RECT rcText = {0, 0, szAvailable.cx, szAvailable.cy};
			int nLinks = 0;
			if( show_html_ ) RenderEngine::DrawHtmlText(pm_->GetPaintDC(), pm_, rcText, sText, text_color_, NULL, NULL, nLinks, DT_CALCRECT | text_style_);
			else RenderEngine::DrawText(pm_->GetPaintDC(), pm_, rcText, sText, text_color_, font_, DT_CALCRECT | text_style_);
			cxy_fired_sz_.cx = MulDiv(rcText.right - rcText.left + GetManager()->GetDPIObj()->Scale(text_padding_rect_.left) + GetManager()->GetDPIObj()->Scale(text_padding_rect_.right), 100, GetManager()->GetDPIObj()->GetScale());
		}

		if (cxy_fired_sz_.cy == 0) {
			int height = GlobalManager::GetFontInfo(GetFont(), pm_->GetPaintDC())->tm_.tmHeight + 4;
			return CDuiSize(GetManager()->GetDPIObj()->Scale(cxy_fired_sz_.cx),
				height);
		}
		return Control::EstimateSize(szAvailable);
	}

	void Label::DoEvent(EventArgs& event)
	{
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
		Control::DoEvent(event);
	}

	void Label::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("align")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				text_style_ &= ~(DT_CENTER | DT_RIGHT);
				text_style_ |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				text_style_ &= ~(DT_LEFT | DT_RIGHT );
				text_style_ |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				text_style_ &= ~(DT_LEFT | DT_CENTER);
				text_style_ |= DT_RIGHT;
			}
		}
		else if( _tcsicmp(pstrName, _T("valign")) == 0 ) {
			if( _tcsstr(pstrValue, _T("top")) != NULL ) {
				text_style_ &= ~(DT_BOTTOM | DT_VCENTER | DT_WORDBREAK);
				text_style_ |= (DT_TOP | DT_SINGLELINE);
			}
			if( _tcsstr(pstrValue, _T("vcenter")) != NULL ) {
				text_style_ &= ~(DT_TOP | DT_BOTTOM | DT_WORDBREAK);            
				text_style_ |= (DT_VCENTER | DT_SINGLELINE);
			}
			if( _tcsstr(pstrValue, _T("bottom")) != NULL ) {
				text_style_ &= ~(DT_TOP | DT_VCENTER | DT_WORDBREAK);
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
		else if( _tcsicmp(pstrName, _T("noprefix")) == 0 ) {
			if( _tcsicmp(pstrValue, _T("true")) == 0)
			{
				text_style_ |= DT_NOPREFIX;
			}
			else
			{
				text_style_ = text_style_ & ~DT_NOPREFIX;
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
		else if( _tcsicmp(pstrName, _T("autocalcwidth")) == 0 ) {
			SetAutoCalcWidth(_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else Control::SetAttribute(strName, strValue);
	}

	void Label::PaintText(HDC hDC)
	{
		if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();
		if( disabled_text_color_ == 0 ) disabled_text_color_ = GlobalManager::GetDefaultDisabledColor();

		RECT rc = rect_;
		RECT text_padding_rect_ = Label::text_padding_rect_;
		GetManager()->GetDPIObj()->Scale(&text_padding_rect_);
		rc.left += text_padding_rect_.left;
		rc.right -= text_padding_rect_.right;
		rc.top += text_padding_rect_.top;
		rc.bottom -= text_padding_rect_.bottom;

		std::wstring sText = GetText();
		if( sText.empty() ) return;
		int nLinks = 0;
		if( IsEnabled() ) {
			if( show_html_ )
				RenderEngine::DrawHtmlText(hDC, pm_, rc, sText, text_color_, \
				NULL, NULL, nLinks, text_style_);
			else
				RenderEngine::DrawText(hDC, pm_, rc, sText, text_color_, \
				font_, text_style_);
		}
		else {
			if( show_html_ )
				RenderEngine::DrawHtmlText(hDC, pm_, rc, sText, disabled_text_color_, \
				NULL, NULL, nLinks, text_style_);
			else
				RenderEngine::DrawText(hDC, pm_, rc, sText, disabled_text_color_, \
				font_, text_style_);
		}
	}

	bool Label::GetAutoCalcWidth() const
	{
		return auto_calc_width_;
	}

	void Label::SetAutoCalcWidth(bool bAutoCalcWidth)
	{
		auto_calc_width_ = bAutoCalcWidth;
	}

	void Label::SetText(std::wstring pstrText )
	{
		Control::SetText(pstrText);
		if(GetAutoCalcWidth()) {
			NeedParentUpdate();
		}
	}
}