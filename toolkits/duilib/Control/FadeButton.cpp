#include "StdAfx.h"
#include "FadeButton.h"

namespace ui {
	IMPLEMENT_DUICONTROL(FadeButton)

	FadeButton::FadeButton() : Animation( &(*this) ), mose_hove_( FALSE ), mouse_leave_( FALSE )
	{
	}

	FadeButton::~FadeButton()
	{
		StopAnimation();
	}

	std::wstring FadeButton::GetClass() const
	{
		return _T("FadeButton");
	}

	LPVOID FadeButton::GetInterface(std::wstring pstrName)
	{
		if( _tcscmp(pstrName.c_str(), _T("FadeButton")) == 0 ) 
			return static_cast<FadeButton*>(this);
		return Button::GetInterface(pstrName);
	}

	void FadeButton::SetNormalImage(std::wstring pStrImage)
	{
		normal_image_ = pStrImage;
		last_image_ = normal_image_;
	}

	void FadeButton::DoEvent(EventArgs& event)
	{
		if( event.type_ == kEventMouseEnter && !IsAnimationRunning( FADE_IN_ID ) ){
			fade_alpha_ = 0;
			mose_hove_ = TRUE;
			StopAnimation( FADE_OUT_ID );
			StartAnimation( FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_IN_ID );
			Invalidate();
			return;
		}

		if( event.type_ == kEventMouseLeave && !IsAnimationRunning( FADE_OUT_ID ) ){
			fade_alpha_ = 0;
			mouse_leave_ = TRUE;
			StopAnimation(FADE_IN_ID);
			StartAnimation(FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_OUT_ID);
			Invalidate();
			return;
		}

		if( event.type_ == kEventTimer ) {
			OnTimer(  event.w_param_ );
		}

		Button::DoEvent( event );
	}

	void FadeButton::OnTimer( int nTimerID )
	{
		OnAnimationElapse( nTimerID );
	}

	void FadeButton::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) state_ |= UISTATE_FOCUSED;
		else state_ &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) state_ |= UISTATE_DISABLED;
		else state_ &= ~ UISTATE_DISABLED;

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
		else if( (state_ & UISTATE_FOCUSED) != 0 ) {
			if( !focused_image_.empty() ) {
				if( !DrawImage(hDC, focused_image_) ) {}
				else return;
			}
		}

		if( !normal_image_.empty() ) {
			if( IsAnimationRunning(FADE_IN_ID) || IsAnimationRunning(FADE_OUT_ID)) {
				if( mose_hove_ ) {
					mose_hove_ = FALSE;
					last_image_ = hot_image_;
					if( !DrawImage(hDC, normal_image_) ) {}
					return;
				}

				if( mouse_leave_ ) {
					mouse_leave_ = FALSE;
					last_image_ = normal_image_;
					if( !DrawImage(hDC, hot_image_) ) {}
					return;
				}

				old_image_ = normal_image_;
				new_image_ = hot_image_;
				if( IsAnimationRunning(FADE_OUT_ID) ) {
					old_image_ = hot_image_;
					new_image_ = normal_image_;
				}
				std::wstring sFadeOut, sFadeIn;
				sFadeOut = nbase::StringPrintf(_T("fade='%d'"), 255 - fade_alpha_);
				sFadeIn  = nbase::StringPrintf(_T("fade='%d'"), fade_alpha_);
				if( !DrawImage(hDC, old_image_, sFadeOut) ) {}
				if( !DrawImage(hDC, new_image_, sFadeIn) ) {}
				return;
			}
			else {
				if(last_image_.empty()) last_image_ = normal_image_;
				if( !DrawImage(hDC, last_image_) ) {}
				return;
			}
		}
	}

	void FadeButton::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
	{
		fade_alpha_ = (BYTE)((nCurFrame / (double)nTotalFrame) * 255);
		fade_alpha_ = fade_alpha_ == 0 ? 10 : fade_alpha_;
		Invalidate();
	}

} // namespace DuiLib