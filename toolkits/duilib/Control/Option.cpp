#include "StdAfx.h"
#include "Option.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(Option)
	Option::Option() : selected_(false), selected_text_color_(0), selected_bkcolor_(0)
	{
	}

	Option::~Option()
	{
		if( !group_name_.empty() && pm_ ) pm_->RemoveOptionGroup(group_name_, this);
	}

	std::wstring Option::GetClass() const
	{
		return DUI_CTR_OPTION;
	}

	LPVOID Option::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_OPTION) == 0 ) return static_cast<Option*>(this);
		return Button::GetInterface(pstrName);
	}

	void Option::AttachSelectedChanged(const EventCallback & cb)
	{
		on_event_[kEventSelecteChanged] += cb;
	}

	void Option::SetManager(PaintManager* pManager, Control* pParent, bool bInit)
	{
		Control::SetManager(pManager, pParent, bInit);
		if( bInit && !group_name_.empty() ) {
			if (pm_) pm_->AddOptionGroup(group_name_, this);
		}
	}

	std::wstring Option::GetGroup() const
	{
		return group_name_;
	}

	void Option::SetGroup(std::wstring pStrGroupName)
	{
		if( pStrGroupName.empty()) {
			if( group_name_.empty() ) return;
			group_name_.clear();
		}
		else {
			if( group_name_ == pStrGroupName ) return;
			if (!group_name_.empty() && pm_) pm_->RemoveOptionGroup(group_name_, this);
			group_name_ = pStrGroupName;
		}

		if( !group_name_.empty() ) {
			if (pm_) pm_->AddOptionGroup(group_name_, this);
		}
		else {
			if (pm_) pm_->RemoveOptionGroup(group_name_, this);
		}

		Selected(selected_);
	}

	bool Option::IsSelected() const
	{
		return selected_;
	}

	void Option::Selected(bool bSelected)
	{
		if(selected_ == bSelected) return;

		selected_ = bSelected;
		if( selected_ ) state_ |= UISTATE_SELECTED;
		else state_ &= ~UISTATE_SELECTED;

		if( pm_ != NULL ) {
			if( !group_name_.empty() ) {
				if( selected_ ) {
					CStdPtrArray* aOptionGroup = pm_->GetOptionGroup(group_name_);
					for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
						Option* pControl = static_cast<Option*>(aOptionGroup->GetAt(i));
						if( pControl != this ) {
							pControl->Selected(false);
						}
					}
					Notify(kEventSelecteChanged, selected_);
				}
			}
			else {
				Notify(kEventSelecteChanged, selected_);
			}
		}

		Invalidate();
	}

	bool Option::Activate()
	{
		if( !Button::Activate() ) return false;
		if( !group_name_.empty() ) Selected(true);
		else Selected(!selected_);

		return true;
	}

	void Option::SetEnabled(bool bEnable)
	{
		Control::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			if( selected_ ) state_ = UISTATE_SELECTED;
			else state_ = 0;
		}
	}

	std::wstring Option::GetSelectedImage()
	{
		return selected_image_;
	}

	void Option::SetSelectedImage(std::wstring pStrImage)
	{
		selected_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Option::GetSelectedHotImage()
	{
		return selected_hot_image_;
	}

	void Option::SetSelectedHotImage( std::wstring pStrImage )
	{
		selected_hot_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Option::GetSelectedPushedImage()
	{
		return selected_pushed_image_;
	}

	void Option::SetSelectedPushedImage(std::wstring pStrImage)
	{
		selected_pushed_image_ = pStrImage;
		Invalidate();
	}

	void Option::SetSelectedTextColor(DWORD dwTextColor)
	{
		selected_text_color_ = dwTextColor;
	}

	DWORD Option::GetSelectedTextColor()
	{
		if (selected_text_color_ == 0) selected_text_color_ = GlobalManager::GetDefaultFontColor();
		return selected_text_color_;
	}

	void Option::SetSelectedBkColor( DWORD dwBkColor )
	{
		selected_bkcolor_ = dwBkColor;
	}

	DWORD Option::GetSelectBkColor()
	{
		return selected_bkcolor_;
	}

	std::wstring Option::GetSelectedForedImage()
	{
		return selected_fore__image_;
	}

	void Option::SetSelectedForedImage(std::wstring pStrImage)
	{
		selected_fore__image_ = pStrImage;
		Invalidate();
	}

	void Option::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("group")) == 0 ) SetGroup(pstrValue);
		else if( _tcsicmp(pstrName, _T("selected")) == 0 ) Selected(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("selectedimage")) == 0 ) SetSelectedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedhotimage")) == 0 ) SetSelectedHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedpushedimage")) == 0 ) SetSelectedPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedforeimage")) == 0 ) SetSelectedForedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedbkcolor")) == 0 ) {
			SetSelectedBkColor(GlobalManager::GetColor(strValue));
		}
		else if( _tcsicmp(pstrName, _T("selectedtextcolor")) == 0 ) {
			SetSelectedTextColor(GlobalManager::GetColor(strValue));
		}
		else Button::SetAttribute(strName, strValue);
	}

	void Option::PaintBkColor(HDC hDC)
	{
		if(IsSelected()) {
			if(selected_bkcolor_ != 0) {
				RenderEngine::DrawColor(hDC, paint_rect_, GetAdjustColor(selected_bkcolor_));
			}
		}
		else {
			return Button::PaintBkColor(hDC);
		}
	}

	void Option::PaintStatusImage(HDC hDC)
	{
		if(IsSelected()) {
			if( (state_ & UISTATE_PUSHED) != 0 && !selected_pushed_image_.empty()) {
				if( !DrawImage(hDC, selected_pushed_image_ )) {}
				else return;
			}
			else if( (state_ & UISTATE_HOT) != 0 && !selected_hot_image_.empty()) {
				if( !DrawImage(hDC, selected_hot_image_) ) {}
				else return;
			}

			if( !selected_image_.empty() ) {
				if( !DrawImage(hDC, selected_image_) ) {}
			}
		}
		else {
			Button::PaintStatusImage(hDC);
		}
	}

	void Option::PaintForeImage(HDC hDC)
	{
		if(IsSelected()) {
			if( !selected_fore__image_.empty() ) {
				if( !DrawImage(hDC, selected_fore__image_) ) {}
				else return;
			}
		}

		return Button::PaintForeImage(hDC);
	}

	void Option::PaintText(HDC hDC)
	{
		if( (state_ & UISTATE_SELECTED) != 0 )
		{
			DWORD oldTextColor = text_color_;
			if( selected_text_color_ != 0 ) text_color_ = selected_text_color_;

			if( text_color_ == 0 ) text_color_ = GlobalManager::GetDefaultFontColor();
			if( disabled_text_color_ == 0 ) disabled_text_color_ = GlobalManager::GetDefaultDisabledColor();

			std::wstring sText = GetText();
			if( sText.empty() ) return;
			int nLinks = 0;
			RECT rc = rect_;
			RECT text_padding_rect_ = Button::text_padding_rect_;
			GetManager()->GetDPIObj()->Scale(&text_padding_rect_);
			rc.left += text_padding_rect_.left;
			rc.right -= text_padding_rect_.right;
			rc.top += text_padding_rect_.top;
			rc.bottom -= text_padding_rect_.bottom;
			
			if( show_html_ )
				RenderEngine::DrawHtmlText(hDC, pm_, rc, sText, IsEnabled()?text_color_:disabled_text_color_, \
				NULL, NULL, nLinks, text_style_);
			else
				RenderEngine::DrawText(hDC, pm_, rc, sText, IsEnabled()?text_color_:disabled_text_color_, \
				font_, text_style_);

			text_color_ = oldTextColor;
		}
		else
			Button::PaintText(hDC);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_DUICONTROL(CheckBox)

	std::wstring CheckBox::GetClass() const
	{
		return DUI_CTR_CHECKBOX;
	}
	LPVOID CheckBox::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_CHECKBOX) == 0 ) return static_cast<CheckBox*>(this);
		return Option::GetInterface(pstrName);
	}

	int CheckBox::GetFixedWidth()
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

			width += (text_padding_rect_.right + text_padding_rect_.left);
			if (!text_.empty()) {
				width += GlobalManager::GetTextPixelWidth(pm_->GetPaintDC(), GetFont(), text_);
			}

			cxy_fired_sz_.cx = width;
		}

		return __super::GetFixedWidth();
	}

	void CheckBox::AttachCheckClick(const EventCallback & cb)
	{
		on_event_[kEventCheckClick] += cb;
	}

	void CheckBox::SetCheck(bool bCheck)
	{
		Selected(bCheck);
	}

	bool  CheckBox::GetCheck() const
	{
		return IsSelected();
	}

	CheckBox::CheckBox() : auto_check(FALSE)
	{

	}
	void CheckBox::SetAttribute(std::wstring pstrName, std::wstring pstrValue)
	{
		if( _tcsicmp(pstrName.c_str(), _T("EnableAutoCheck")) == 0 ) SetAutoCheck(_tcsicmp(pstrValue.c_str(), _T("true")) == 0);
		
		Option::SetAttribute(pstrName, pstrValue);
	}
	void CheckBox::SetAutoCheck(bool bEnable)
	{
		auto_check = bEnable;
	}
	void CheckBox::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Option::DoEvent(event);
			return;
		}
		if( auto_check && (event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick))
		{
			if( ::PtInRect(&rect_, event.mouse_point_) && IsEnabled() ){
				SetCheck(!GetCheck()); 
				Notify(kEventCheckClick, 0, 0);
				Invalidate();
			}
			return;
		}
		Option::DoEvent(event);
	}
	void CheckBox::Selected(bool bSelected, bool send_notify)
	{
		if( selected_ == bSelected ) return;
		selected_ = bSelected;
		if( selected_ ) state_ |= UISTATE_SELECTED;
		else state_ &= ~UISTATE_SELECTED;

		if( pm_ != NULL ) {
			if( !group_name_.empty() ) {
				if( selected_ ) {
					CStdPtrArray* aOptionGroup = pm_->GetOptionGroup(group_name_);
					for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
						Option* pControl = static_cast<Option*>(aOptionGroup->GetAt(i));
						if( pControl != this ) {
							pControl->Selected(FALSE);
						}
					}
					if (send_notify) {
						Notify(kEventSelecteChanged, selected_, 0);
					}
				}
			}
			else {
				if (send_notify) {
					Notify(kEventSelecteChanged, selected_, 0);
				}
			}
		}

		Invalidate();
	}
}