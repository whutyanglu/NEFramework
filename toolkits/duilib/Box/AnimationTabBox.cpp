#include "StdAfx.h"
#include "AnimationTabBox.h"
#include "TabBox.h"

namespace ui {
	IMPLEMENT_DUICONTROL(AnimationTabBox)

	AnimationTabBox::AnimationTabBox() : 
		Animation( this ), 
		is_vertical_direction_( false ), 
		positive_direction_( 1 ),
		current_control_( NULL ),
		is_visible_( false )
	{
	}

	std::wstring  AnimationTabBox::GetClass() const
	{
		return _T("AnimationTabBox");
	}

	LPVOID AnimationTabBox::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("AnimationTabBox")) == 0 ) 
			return static_cast<AnimationTabBox*>(this);
		return TabBox::GetInterface(pstrName);
	}

	bool AnimationTabBox::SelectItem( int iIndex )
	{
		if( iIndex < 0 || iIndex >= items_.GetSize() ) return false;
		if( iIndex == cur_sel_ ) return true;
		if( iIndex > cur_sel_ ) positive_direction_ = -1;
		if( iIndex < cur_sel_ ) positive_direction_ = 1;

		int iOldSel = cur_sel_;
		cur_sel_ = iIndex;
		for( int it = 0; it < items_.GetSize(); it++ ) 
		{
			if( it == iIndex ) {
				GetItemAt(it)->SetVisible(true);
				GetItemAt(it)->SetFocus();
				is_visible_ = false;
				current_control_ = static_cast<Control*>(items_[it]);
				
			}
			else GetItemAt(it)->SetVisible(false);
		}

		NeedParentUpdate();
		if( NULL != current_control_ ) current_control_->SetVisible( false );
		AnimationSwitch();

		if( pm_ != NULL ) {
			pm_->SetNextTabControl();
			//pm_->SendNotify(this, _T("tabselect"), cur_sel_, iOldSel);
			Notify(kEventTab, cur_sel_, iOldSel);
		}
		return true;
	}

	void AnimationTabBox::AnimationSwitch()
	{
		old_rect_ = rect_;
		if( !is_vertical_direction_ ){
			cur_pos_.top = rect_.top;
			cur_pos_.bottom = rect_.bottom;
			cur_pos_.left = rect_.left - ( rect_.right - rect_.left ) * positive_direction_ + 52 * positive_direction_;
			cur_pos_.right = rect_.right - ( rect_.right - rect_.left ) * positive_direction_+ 52 * positive_direction_;		
		}
		else{
			cur_pos_.left = rect_.left;
			cur_pos_.right = rect_.right;
			cur_pos_.top = rect_.top - ( rect_.bottom - rect_.top ) * positive_direction_;
			cur_pos_.bottom = rect_.bottom - ( rect_.bottom - rect_.top ) * positive_direction_;		
		}

		StopAnimation( TAB_ANIMATION_ID );
		StartAnimation( TAB_ANIMATION_ELLAPSE, TAB_ANIMATION_FRAME_COUNT, TAB_ANIMATION_ID );
	}

	void AnimationTabBox::DoEvent(EventArgs& event)
	{
		if( event.type_ == kEventTimer ) 
		{
			OnTimer(  event.w_param_ );
		}
		__super::DoEvent( event );
	}

	void AnimationTabBox::OnTimer( int nTimerID )
	{
		OnAnimationElapse( nTimerID );
	}

	void AnimationTabBox::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
	{
		if( !is_visible_ ) {
			is_visible_ = true;
			current_control_->SetVisible( true );
		}

		int step_len = 0;
		if( !is_vertical_direction_ ){
			step_len = ( old_rect_.right - old_rect_.left ) * positive_direction_ / nTotalFrame;
			if( nCurFrame != nTotalFrame ){
				cur_pos_.left = cur_pos_.left + step_len;
				cur_pos_.right = cur_pos_.right +step_len;			
			}
			else{
				rect_ = cur_pos_ = old_rect_;
			}
		}
		else{
			step_len = ( old_rect_.bottom - old_rect_.top ) * positive_direction_ / nTotalFrame;
			if( nCurFrame != nTotalFrame ){
				cur_pos_.top = cur_pos_.top + step_len;
				cur_pos_.bottom = cur_pos_.bottom +step_len;			
			}
			else{
				rect_ = cur_pos_ = old_rect_;	
			}	
		}

		SetPos(cur_pos_);
	}

	void AnimationTabBox::OnAnimationStop(INT nAnimationID) 
	{
		SetPos(old_rect_);
		NeedParentUpdate();
	}

	void AnimationTabBox::SetAttribute(std::wstring pstrName, std::wstring pstrValue)
	{
		if (_tcsicmp(pstrName.c_str(), _T("animation_direction")) == 0 && _tcsicmp(pstrValue.c_str(), _T("vertical")) == 0) {
			is_vertical_direction_ = true; // pstrValue = "vertical" or "horizontal"
		}
		return TabBox::SetAttribute(pstrName, pstrValue);
	}
} // namespace DuiLib