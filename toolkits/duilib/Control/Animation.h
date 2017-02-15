#ifndef __DUILIB_ANIMATION_H__
#define __DUILIB_ANIMATION_H__

#include "Button.h"
#pragma once

namespace ui {

	class UILIB_API IAnimation
	{
	public:
		virtual ~IAnimation() { NULL; }

		virtual BOOL StartAnimation(int nElapse, int nTotalFrame, int nAnimationID = 0, BOOL bLoop = FALSE) = 0;
		virtual void StopAnimation(int nAnimationID = 0) = 0;
		virtual BOOL IsAnimationRunning(int nAnimationID) = 0;
		virtual int GetCurrentFrame(int nAnimationID = 0) = 0;
		virtual BOOL SetCurrentFrame(int nFrame, int nAnimationID = 0) = 0;

		virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) = 0;
		virtual void OnAnimationStart(int nAnimationID, BOOL bFirstLoop) = 0;
		virtual void OnAnimationStop(int nAnimationID) = 0;

		virtual void OnAnimationElapse(int nAnimationID) = 0;
	};

	class UILIB_API AnimationData
	{
	public:
		AnimationData(int nElipse, int nFrame, int nID, BOOL bLoop)
		{
			is_first_loop_ = TRUE;
			cur_frame_ = 0;
			elapse_ = nElipse;
			total_frame_ = nFrame;
			loop_floag_ = bLoop;
			id_ = nID;
		}

	//protected:
	public:
		friend class Animation;

		int id_;
		int elapse_;
		int total_frame_;
		int cur_frame_;

		BOOL loop_floag_;
		BOOL is_first_loop_;
	};

	class UILIB_API Animation: public IAnimation
	{
		struct Imp;
	public:
		Animation(Control* pOwner);
		~Animation();

		virtual BOOL StartAnimation(int nElapse, int nTotalFrame, int nAnimationID = 0, BOOL bLoop = FALSE);
		virtual void StopAnimation(int nAnimationID = 0);
		virtual BOOL IsAnimationRunning(int nAnimationID);
		virtual int GetCurrentFrame(int nAnimationID = 0);
		virtual BOOL SetCurrentFrame(int nFrame, int nAnimationID = 0);

		virtual void OnAnimationStart(int nAnimationID, BOOL bFirstLoop) {};
		virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) {};
		virtual void OnAnimationStop(int nAnimationID) {};
		virtual void OnAnimationElapse(int nAnimationID);

	protected:
		AnimationData* GetAnimationDataByID(int nAnimationID);

	protected:
		Control* control_;
		Imp * imp_;
	};

} // namespace ui

#endif // __DUILIB_ANIMATION_H__