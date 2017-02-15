#ifndef __UIFADEBUTTON_H__
#define __UIFADEBUTTON_H__

#include "Animation.h"
#pragma once

namespace ui {

	class UILIB_API FadeButton : public Button, public Animation
	{
		DECLARE_DUICONTROL(FadeButton)
	public:
		FadeButton();
		virtual ~FadeButton();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		void SetNormalImage(std::wstring pStrImage);

		void DoEvent(EventArgs& event);
		void OnTimer( int nTimerID );
		void PaintStatusImage(HDC hDC);

		virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}
		virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
		virtual void OnAnimationStop(INT nAnimationID) {}
		
	protected:
		std::wstring old_image_;
		std::wstring new_image_;
		std::wstring last_image_;
		BYTE       fade_alpha_;
		BOOL       mose_hove_;
		BOOL       mouse_leave_;
		enum{
			FADE_IN_ID			= 8,
			FADE_OUT_ID			= 9,
			FADE_ELLAPSE		= 10,
			FADE_FRAME_COUNT	= 30,
		};
	};

} // namespace DuiLib

#endif // __UIFADEBUTTON_H__