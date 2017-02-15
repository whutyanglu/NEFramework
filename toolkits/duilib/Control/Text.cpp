#include "StdAfx.h"
#include "Text.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(Text)

	Text::Text() : link_id_(0), hover_link_(-1)
	{
		text_style_ = DT_WORDBREAK;
		text_padding_rect_.left = 2;
		text_padding_rect_.right = 2;
		::ZeroMemory(link_rects_, sizeof(link_rects_));
	}

	Text::~Text()
	{
	}

	std::wstring Text::GetClass() const
	{
		return DUI_CTR_TEXT;
	}

	LPVOID Text::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_TEXT) == 0 ) return static_cast<Text*>(this);
		return Label::GetInterface(pstrName);
	}

	void Text::AttachCustomLinkClick(const EventCallback & cb)
	{
		on_event_[kEventCustomLinkClick] += cb;
	}

	UINT Text::GetControlFlags() const
	{
		if( IsEnabled() && link_id_ > 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	std::wstring* Text::GetLinkContent(int iIndex)
	{
		if( iIndex >= 0 && iIndex < link_id_) return &links_[iIndex];
		return NULL;
	}

	void Text::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Label::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetCursor ) {
			for( int i = 0; i < link_id_; i++ ) {
				if( ::PtInRect(&link_rects_[i], event.mouse_point_) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}
		}
		if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick && IsEnabled() ) {
			for( int i = 0; i < link_id_; i++ ) {
				if( ::PtInRect(&link_rects_[i], event.mouse_point_) ) {
					Invalidate();
					return;
				}
			}
		}
		if( event.type_ == kEventMouseButtonUp && IsEnabled() ) {
			for( int i = 0; i < link_id_; i++ ) {
				if( ::PtInRect(&link_rects_[i], event.mouse_point_) ) {
					Notify(kEventCustomLinkClick, i);
					return;
				}
			}
		}
		if( event.type_ == kEventMouseMenu )
		{
			return;
		}
		// When you move over a link
		if(link_id_ > 0 && event.type_ == kEventMouseMove && IsEnabled() ) {
			int nHoverLink = -1;
			for( int i = 0; i < link_id_; i++ ) {
				if( ::PtInRect(&link_rects_[i], event.mouse_point_) ) {
					nHoverLink = i;
					break;
				}
			}

			if(hover_link_ != nHoverLink) {
				hover_link_ = nHoverLink;
				Invalidate();
				return;
			}      
		}
		if( event.type_ == kEventMouseLeave ) {
			if(link_id_ > 0 && IsEnabled() ) {
				if(hover_link_ != -1) {
					hover_link_ = -1;
					Invalidate();
					return;
				}
			}
		}

		Label::DoEvent(event);
	}

	SIZE Text::EstimateSize(SIZE szAvailable)
	{
		std::wstring sText = GetText();
		RECT text_padding_rect_ = GetTextPadding();

		RECT rcText = { 0, 0, auto_calc_width_ ? 9999 : GetManager()->GetDPIObj()->Scale(cxy_fired_sz_.cx), 9999 };
		rcText.left += text_padding_rect_.left;
		rcText.right -= text_padding_rect_.right;

		if( show_html_ ) {   
			int nLinks = 0;
			RenderEngine::DrawHtmlText(pm_->GetPaintDC(), pm_, rcText, sText, text_color_, NULL, NULL, nLinks, DT_CALCRECT | text_style_);
		}
		else {
			RenderEngine::DrawText(pm_->GetPaintDC(), pm_, rcText, sText, text_color_, font_, DT_CALCRECT | text_style_);
		}
		SIZE cXY = {rcText.right - rcText.left + text_padding_rect_.left + text_padding_rect_.right,
			rcText.bottom - rcText.top + text_padding_rect_.top + text_padding_rect_.bottom};
		
		if (auto_calc_width_){
			cxy_fired_sz_.cx = MulDiv(cXY.cx, 100, GetManager()->GetDPIObj()->GetScale());
		}

		return Control::EstimateSize(szAvailable);
	}

	void Text::PaintText(HDC hDC)
	{
		std::wstring sText = GetText();
		if( sText.empty() ) {
			link_id_ = 0;
			return;
		}

		if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();
		if( disabled_text_color_ == 0 ) disabled_text_color_ = GlobalManager::GetDefaultDisabledColor();

		link_id_ = lengthof(link_rects_);
		RECT rc = rect_;
		rc.left += text_padding_rect_.left;
		rc.right -= text_padding_rect_.right;
		rc.top += text_padding_rect_.top;
		rc.bottom -= text_padding_rect_.bottom;
		if( IsEnabled() ) {
			if( show_html_ )
				RenderEngine::DrawHtmlText(hDC, pm_, rc, sText, text_color_, \
				link_rects_, links_, link_id_, text_style_);
			else
				RenderEngine::DrawText(hDC, pm_, rc, sText, text_color_, \
				font_, text_style_);
		}
		else {
			if( show_html_ )
				RenderEngine::DrawHtmlText(hDC, pm_, rc, sText, disabled_text_color_, \
				link_rects_, links_, link_id_, text_style_);
			else
				RenderEngine::DrawText(hDC, pm_, rc, sText, disabled_text_color_, \
				font_, text_style_);
		}
	}
}
