#ifndef __UIANIMATIONTABLAYOUT_H__
#define __UIANIMATIONTABLAYOUT_H__

namespace ui
{
	class UILIB_API AnimationTabBox : public TabBox, public Animation
	{
		DECLARE_DUICONTROL(AnimationTabBox)
	public:
		AnimationTabBox();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		bool SelectItem( int iIndex );
		void AnimationSwitch();
		void DoEvent(EventArgs& event);
		void OnTimer( int nTimerID );

		virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}
		virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
		virtual void OnAnimationStop(INT nAnimationID);

		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

	protected:
		bool is_vertical_direction_;
		int positive_direction_;
		RECT cur_pos_;
		RECT old_rect_;
		Control* current_control_;
		bool is_visible_;
		enum
		{
			TAB_ANIMATION_ID = 1,

			TAB_ANIMATION_ELLAPSE = 10,
			TAB_ANIMATION_FRAME_COUNT = 15,
		};
	};
}
#endif // __UIANIMATIONTABLAYOUT_H__