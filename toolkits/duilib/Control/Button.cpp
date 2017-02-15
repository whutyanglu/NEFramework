#include "StdAfx.h"
#include "Button.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(Button)

	Button::Button()
		: state_(0)
		, hot_textcolor_(0)
		, pushed_textcolor(0)
		, focused_textcolor_(0)
		, hot_bkcolor_(0)
		, pushed_bkcolor_(0)
		, bind_tab_index_(-1)
	{
		text_style_ = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	}

	std::wstring Button::GetClass() const
	{
		return _T("Button");
	}

	LPVOID Button::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_BUTTON) == 0 ) return static_cast<Button*>(this);
		return Label::GetInterface(pstrName);
	}

	UINT Button::GetControlFlags() const
	{
		return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	void Button::AttachClick(const EventCallback & cb)
	{
		on_event_[kEventClick] += cb;
	}

	int Button::GetFixedWidth()
	{
		std::wstring image_path;
		image_path = normal_image_.empty() ? fore_image_ : normal_image_;
		image_path = image_path.empty() ? (fore_image_.empty() ? bkimage_ : fore_image_) : image_path;

		if (cxy_fired_sz_.cx == -1 && !image_path.empty()) {
			int width = 0;
			const TImageInfo *image_info = pm_->GetImageEx(image_path);
			if (image_info == NULL) {
				TDrawInfo draw_info;
				draw_info.Parse(image_path);
				if (draw_info.IsValidDest()) {
					width = draw_info.rcDest.right - draw_info.rcDest.left;
				}
				else {
					image_info = pm_->AddImage(draw_info.sImageName, draw_info.sResType, draw_info.dwMask, draw_info.bHSL);
					width = image_info->nX;
				}
			}
			else {
				width = image_info->nX;
			}
			
			cxy_fired_sz_.cx = width;	
		}
		
		return __super::GetFixedWidth();
		
	}

	int Button::GetFixedHeight()
	{
		std::wstring image_path;
		image_path = normal_image_.empty() ? fore_image_ : normal_image_;
		image_path = image_path.empty() ? (fore_image_.empty() ? bkimage_ : fore_image_) : image_path;

		if (cxy_fired_sz_.cy == -1 && !image_path.empty()) {
			int height = 0;
			const TImageInfo *image_info = pm_->GetImageEx(image_path);
			if (image_info == NULL) {
				TDrawInfo draw_info;
				draw_info.Parse(image_path);
				if (draw_info.IsValidDest()) {
					height = draw_info.rcDest.bottom - draw_info.rcDest.top;
				}
				else {
					image_info = pm_->AddImage(draw_info.sImageName, draw_info.sResType, draw_info.dwMask, draw_info.bHSL);
					height = image_info->nY;
				}
			}
			else {
				height = image_info->nY;
			}

			cxy_fired_sz_.cy = height;
		}
		return __super::GetFixedHeight();
	}

	void Button::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Label::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetFocus ) {
			Invalidate();
		}
		if( event.type_ == kEventKillFocus ) {
			Invalidate();
		}
		if( event.type_ == kEventKeyDown ){
			if (IsKeyboardEnabled()) {
				if( event.key_ == VK_SPACE || event.key_ == VK_RETURN ) {
					Activate();
					return;
				}
			}
		}		
		if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick){
			if( ::PtInRect(&rect_, event.mouse_point_) && IsEnabled() ) {
				state_ |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}	
		if( event.type_ == kEventMouseMove ){
			if( (state_ & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&rect_, event.mouse_point_) ) state_ |= UISTATE_PUSHED;
				else state_ &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseButtonUp ){
			if( (state_ & UISTATE_CAPTURED) != 0 ) {
				state_ &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
				if( ::PtInRect(&rect_, event.mouse_point_) ) Activate();				
			}
			return;
		}
		if( event.type_ == kEventMouseMenu ){
			if( IsContextMenuUsed() ) {
				Notify(kEventMouseMenu, event.w_param_, event.l_param_);
			}
			return;
		}
		if( event.type_ == kEventMouseEnter ){
			if( IsEnabled() ) {
				state_ |= UISTATE_HOT;
				Invalidate();
			}
		}
		if( event.type_ == kEventMouseLeave ){
			if( IsEnabled() ) {
				state_ &= ~UISTATE_HOT;
				Invalidate();
			}
		}
		Label::DoEvent(event);
	}

	bool Button::Activate()
	{
		if( !Control::Activate() ) return false;
		if( pm_ != NULL ){
			Notify(kEventClick);
			BindTriggerTabSel();
		}
		return true;
	}

	void Button::SetEnabled(bool bEnable)
	{
		Control::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			state_ = 0;
		}
	}

	void Button::SetHotBkColor( DWORD dwColor )
	{
		hot_bkcolor_ = dwColor;
	}

	DWORD Button::GetHotBkColor() const
	{
		return hot_bkcolor_;
	}
	
	void Button::SetPushedBkColor( DWORD dwColor )
	{
		pushed_bkcolor_ = dwColor;
	}

	DWORD Button::GetPushedBkColor() const
	{
		return pushed_bkcolor_;
	}

	void Button::SetHotTextColor(DWORD dwColor)
	{
		hot_textcolor_ = dwColor;
	}

	DWORD Button::GetHotTextColor() const
	{
		return hot_textcolor_;
	}

	void Button::SetPushedTextColor(DWORD dwColor)
	{
		pushed_textcolor = dwColor;
	}

	DWORD Button::GetPushedTextColor() const
	{
		return pushed_textcolor;
	}

	void Button::SetFocusedTextColor(DWORD dwColor)
	{
		focused_textcolor_ = dwColor;
	}

	DWORD Button::GetFocusedTextColor() const
	{
		return focused_textcolor_;
	}

	std::wstring Button::GetNormalImage()
	{
		return normal_image_;
	}

	void Button::SetNormalImage(std::wstring pStrImage)
	{
		normal_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Button::GetHotImage()
	{
		return hot_image_;
	}

	void Button::SetHotImage(std::wstring pStrImage)
	{
		hot_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Button::GetPushedImage()
	{
		return pushed_image_;
	}

	void Button::SetPushedImage(std::wstring pStrImage)
	{
		pushed_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Button::GetFocusedImage()
	{
		return focused_image_;
	}

	void Button::SetFocusedImage(std::wstring pStrImage)
	{
		focused_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Button::GetDisabledImage()
	{
		return disabled_image_;
	}

	void Button::SetDisabledImage(std::wstring pStrImage)
	{
		disabled_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Button::GetHotForeImage()
	{
		return hot_foreimage_;
	}

	void Button::SetHotForeImage(std::wstring pStrImage )
	{
		hot_foreimage_ = pStrImage;
		Invalidate();
	}

	void Button::BindTabIndex(int _BindTabIndex )
	{
		if( _BindTabIndex >= 0)
			bind_tab_index_	= _BindTabIndex;
	}

	void Button::BindTabBoxName(std::wstring _TabLayoutName )
	{
		if(!_TabLayoutName.empty())
			bind_box_name_ = _TabLayoutName;
	}

	void Button::BindTriggerTabSel( int _SetSelectIndex /*= -1*/ )
	{
		std::wstring pstrName = GetBindTabBoxName();
		if(pstrName.empty() || (GetBindTabBoxIndex() < 0 && _SetSelectIndex < 0))
			return;

		TabBox* pTabBox = static_cast<TabBox*>(GetManager()->FindControl(pstrName));
		if(!pTabBox) return;
		pTabBox->SelectItem(_SetSelectIndex >=0?_SetSelectIndex:GetBindTabBoxIndex());
	}

	void Button::RemoveBindTabIndex()
	{
		bind_tab_index_	= -1;
		bind_box_name_.clear();
	}

	int Button::GetBindTabBoxIndex()
	{
		return bind_tab_index_;
	}

	std::wstring Button::GetBindTabBoxName()
	{
		return bind_box_name_;
	}

	void Button::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("hotforeimage")) == 0 ) SetHotForeImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("bindtabindex")) == 0 ) BindTabIndex(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("bindtablayoutname")) == 0 ) BindTabBoxName(pstrValue);
		else if( _tcsicmp(pstrName, _T("hotbkcolor")) == 0 )
		{
			SetHotBkColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("pushedbkcolor")) == 0 )
		{
			SetPushedBkColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("hottextcolor")) == 0 )
		{
			SetHotTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("pushedtextcolor")) == 0 )
		{
			SetPushedTextColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("focusedtextcolor")) == 0 )
		{
			SetFocusedTextColor(GlobalManager::GetColor(strValue));
		}
		else Label::SetAttribute(pstrName, pstrValue);
	}

	void Button::PaintText(HDC hDC)
	{
		if( IsFocused() ) state_ |= UISTATE_FOCUSED;
		else state_ &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) state_ |= UISTATE_DISABLED;
		else state_ &= ~ UISTATE_DISABLED;

		if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();
		if( disabled_text_color_ == 0 ) disabled_text_color_ = GlobalManager::GetDefaultDisabledColor();
		
		std::wstring sText = GetText();
		if( sText.empty() ) return;

		RECT text_padding_rect_ = Button::text_padding_rect_;
		GetManager()->GetDPIObj()->Scale(&text_padding_rect_);
		int nLinks = 0;
		RECT rc = rect_;
		rc.left += text_padding_rect_.left;
		rc.right -= text_padding_rect_.right;
		rc.top += text_padding_rect_.top;
		rc.bottom -= text_padding_rect_.bottom;

		DWORD clrColor = IsEnabled()?text_color_:disabled_text_color_;

		if( ((state_ & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0) )
			clrColor = GetPushedTextColor();
		else if( ((state_ & UISTATE_HOT) != 0) && (GetHotTextColor() != 0) )
			clrColor = GetHotTextColor();
		else if( ((state_ & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0) )
			clrColor = GetFocusedTextColor();

		if( show_html_ )
			RenderEngine::DrawHtmlText(hDC, pm_, rc, sText, clrColor, \
			NULL, NULL, nLinks, text_style_);
		else
			RenderEngine::DrawText(hDC, pm_, rc, sText, clrColor, \
			font_, text_style_);
	}

	void Button::PaintBkColor(HDC hDC)
	{
		if( (state_ & UISTATE_PUSHED) != 0 ) {
			if(pushed_bkcolor_ != 0) {
				RenderEngine::DrawColor(hDC, paint_rect_, GetAdjustColor(pushed_bkcolor_));
				return;
			}
		}
		else if( (state_ & UISTATE_HOT) != 0 ) {
			if(hot_bkcolor_ != 0) {
				RenderEngine::DrawColor(hDC, paint_rect_, GetAdjustColor(hot_bkcolor_));
				return;
			}
		}

		return Control::PaintBkColor(hDC);
	}

	void Button::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) state_ |= UISTATE_FOCUSED;
		else state_ &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) state_ |= UISTATE_DISABLED;
		else state_ &= ~ UISTATE_DISABLED;
		if(!::IsWindowEnabled(pm_->GetPaintWindow())) {
			state_ &= UISTATE_DISABLED;
		}
		if( (state_ & UISTATE_DISABLED) != 0 ) {
			if( !disabled_image_.empty() ) {
				if( !DrawImage(hDC, disabled_image_) ) {}
				else return;
			}
		}
		else if( (state_ & UISTATE_PUSHED) != 0 ) {
			if( !pushed_image_.empty() ) {
				if( !DrawImage(hDC, pushed_image_) ) {}
				else return;
			}
		}
		else if( (state_ & UISTATE_HOT) != 0 ) {
			if( !hot_image_.empty() ) {
				if( !DrawImage(hDC, hot_image_) ) {}
				else return;
			}
		}
		else if( (state_ & UISTATE_FOCUSED) != 0 ) {
			if( !focused_image_.empty() ) {
				if( !DrawImage(hDC, focused_image_) ) {}
				else return;
			}
		}

		if( !normal_image_.empty() ) {
			if( !DrawImage(hDC, normal_image_) ) {}
		}
	}

	void Button::PaintForeImage(HDC hDC)
	{
		if( (state_ & UISTATE_PUSHED) != 0 ) {
			if( !pushed_fore_image_.empty() ) {
				if( !DrawImage(hDC, pushed_fore_image_) ) {}
				else return;
			}
		}
		else if( (state_ & UISTATE_HOT) != 0 ) {
			if( !hot_foreimage_.empty() ) {
				if( !DrawImage(hDC, hot_foreimage_) ) {}
				else return;
			}
		}
		if(!fore_image_.empty() ) {
			if( !DrawImage(hDC, fore_image_) ) {}
		}
	}
}