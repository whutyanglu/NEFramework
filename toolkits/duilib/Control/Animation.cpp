#include "StdAfx.h"
#include "Animation.h"
#include <vector>
#include <algorithm>

namespace ui {
	struct Animation::Imp
	{
		std::vector<AnimationData*> animations_;
	};
	
	Animation::Animation(Control* pOwner):imp_(new Animation::Imp())
	{
		ASSERT(pOwner != NULL);
		control_ = pOwner;
	}
	Animation:: ~Animation()
	{
		if(imp_)
		{
			delete imp_;
			imp_ = NULL;
		}
	}
	BOOL Animation::StartAnimation(int nElapse, int nTotalFrame, int nAnimationID /*= 0*/, BOOL bLoop/* = FALSE*/)
	{
		AnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL != pData 
			|| nElapse <= 0
			|| nTotalFrame <= 0
			|| NULL == control_ ){
			ASSERT(FALSE);
			return FALSE;
		}

		AnimationData* pAnimation = new AnimationData(nElapse, nTotalFrame, nAnimationID, bLoop);
		if( NULL == pAnimation ) return FALSE;
		
		if(control_->GetManager()->SetTimer(control_, nAnimationID, nElapse )){
			imp_->animations_.push_back(pAnimation);
			return TRUE;
		}
		return FALSE;
	}

	void Animation::StopAnimation(int nAnimationID /*= 0*/)
	{
		if(control_ == NULL) return;

		if(nAnimationID  != 0){
			AnimationData* pData = GetAnimationDataByID(nAnimationID);
			if( NULL != pData ){
				control_->GetManager()->KillTimer( control_, nAnimationID );
				imp_->animations_.erase(std::remove(imp_->animations_.begin(), imp_->animations_.end(), pData), imp_->animations_.end());
				if(pData != NULL){
					delete pData;
					pData = NULL;
				}
				return;
			}
		}
		else{
			int nCount = imp_->animations_.size();
			for(int i=0; i<nCount; ++i)
			{
				AnimationData* pData = imp_->animations_[i];
				if(pData) {
					control_->GetManager()->KillTimer(control_, pData->id_);
					if(pData != NULL){
						delete pData;
						pData = NULL;
					}
				}
			}
			imp_->animations_.clear();
		}
	}

	BOOL Animation::IsAnimationRunning(int nAnimationID)
	{
		AnimationData* pData = GetAnimationDataByID(nAnimationID);
		return NULL != pData;
	}

	int Animation::GetCurrentFrame(int nAnimationID/* = 0*/)
	{
		AnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL == pData ){
			ASSERT(FALSE);
			return -1;
		}
		return pData->cur_frame_;
	}

	BOOL Animation::SetCurrentFrame(int nFrame, int nAnimationID/* = 0*/)
	{
		AnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL == pData){
			ASSERT(FALSE);
			return FALSE;
		}

		if(nFrame >= 0 && nFrame <= pData->total_frame_){
			pData->cur_frame_ = nFrame;
			return TRUE;
		}
		else{
			ASSERT(FALSE);
		}
		return FALSE;
	}

	void Animation::OnAnimationElapse(int nAnimationID)
	{
		if(control_ == NULL) return;

		AnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL == pData ) return;

		int nCurFrame = pData->cur_frame_;
		if(nCurFrame == 0){
			OnAnimationStart(nAnimationID, pData->is_first_loop_);
			pData->is_first_loop_ = FALSE;
		}

		OnAnimationStep(pData->total_frame_, nCurFrame, nAnimationID);

		if(nCurFrame >= pData->total_frame_){
			OnAnimationStop(nAnimationID);
			if(pData->loop_floag_){
				pData->cur_frame_ = 0;
			}
			else{
				control_->GetManager()->KillTimer( control_, nAnimationID );
				imp_->animations_.erase(std::remove(imp_->animations_.begin(), imp_->animations_.end(), pData), imp_->animations_.end());
				delete pData;
				pData = NULL;
			}
		}

		if( NULL != pData ){
			++(pData->cur_frame_);
		}
	}

	AnimationData* Animation::GetAnimationDataByID(int nAnimationID)
	{
		AnimationData* pRet = NULL;
		int nCount = imp_->animations_.size();
		for(int i=0; i<nCount; ++i)
		{
			if(imp_->animations_[i]->id_ == nAnimationID){
				pRet = imp_->animations_[i];
				break;
			}
		}

		return pRet;
	}

} // namespace DuiLib