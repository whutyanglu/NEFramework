#include "StdAfx.h"
#ifdef USE_XIMAGE_EFFECT
#include "GifAnimEx.h"
#include "ximage.h"
//
namespace ui
{
	#define GIFANIMUIEX_EVENT_TIEM_ID	100
	IMPLEMENT_DUICONTROL(GifAnimEx)
	struct GifAnimEx::Imp
	{
		bool				m_bRealStop			;//外部停止了
		bool				m_bLoadImg			;//是否加载过图片
		bool				m_bTimer			;//是否启动定时器
		bool				m_bAutoStart		;//是否自动开始
		int					m_nDelay			;//循环毫秒数
		UINT				m_nFrameCount		;//gif图片总帧数
		UINT				m_nFramePosition	;//当前放到第几帧
		CxImage*			m_pGifImage			;//gif对象
		PaintManager*&	m_pManager			;
		GifAnimEx*			m_pOwer				;//拥有者
		Imp(PaintManager* & pManager):m_pManager(pManager),
			m_bLoadImg(false),m_bTimer(false),
			m_nDelay(100),m_pGifImage(NULL),m_nFrameCount(0U),
			m_nFramePosition(0U),
			m_pOwer(NULL),m_bRealStop(false),m_bAutoStart(true)
		{
		}
		void SetOwer(GifAnimEx *pOwer)
		{
			m_pOwer = pOwer;
		}
		~Imp()
		{
			if ( m_pGifImage != NULL )
			{
				delete m_pGifImage;
				m_pGifImage = NULL;
			}
		}
		inline void CheckLoadImage()
			{
			if(!m_bLoadImg)
				LoadGifImage();
		}
		inline bool IsLoadImage(){return m_bLoadImg;}
		virtual void LoadGifImage()
		{
			std::wstring sImag = m_pOwer->GetBkImage();
			m_bLoadImg = true;
			m_pGifImage	=	RenderEngine::LoadGifImageX(STRINGorID(sImag.c_str()),0, 0);
			if ( NULL == m_pGifImage ) return;
			m_nFrameCount	=	m_pGifImage->GetNumFrames();
			m_nFramePosition = 0;
			m_nDelay = m_pGifImage->GetFrameDelay();
			if (m_nDelay <= 0 ) 
				m_nDelay = 100;
			if(!m_bAutoStart)
				m_bRealStop = true;
			if(m_bAutoStart && m_pOwer->IsVisible())
				StartAnim();
		}
		void SetAutoStart(bool bAuto)
		{
			m_bAutoStart = bAuto;
		}
		void StartAnim()
		{
			if(!m_bTimer)
			{
				if(!IsLoadImage())
				{
					LoadGifImage();
					m_pOwer->Invalidate();
				}
				if(m_pGifImage)
				m_pManager->SetTimer( m_pOwer, GIFANIMUIEX_EVENT_TIEM_ID, m_nDelay );
				m_bTimer = true;
			}
		}
		void StopAnim(bool bGoFirstFrame)//bGoFirstFrame 是否跑到第一帧
		{
			if(m_bTimer)
			{
				if(bGoFirstFrame)
				{
					m_nFramePosition = 0U;
					m_pOwer->Invalidate();
				}
				m_pManager->KillTimer( m_pOwer, GIFANIMUIEX_EVENT_TIEM_ID );
				m_bTimer = false;
			}
		}
		void EventOnTimer(const WPARAM idEvent )
		{
			if ( idEvent != GIFANIMUIEX_EVENT_TIEM_ID )
				return;
			++m_nFramePosition;
			if(m_nFramePosition >= m_nFrameCount)
				m_nFramePosition = 0;
			if(!m_pOwer->IsVisible())return;
			m_pOwer->Invalidate();
		}
		void DrawFrame( HDC hDC,const RECT& rcPaint,const RECT &rcItem)
		{
			if ( NULL == hDC || NULL == m_pGifImage ) return;
			if(m_pGifImage)
			{
				if (CxImage* pImage = m_pGifImage->GetFrame(m_nFramePosition))
					pImage->Draw2(hDC,rcItem);
			}
		}
		void EventSetVisible(bool bVisible)
		{
			if(bVisible)
			{
				if(!m_bRealStop)
					StartAnim();
			}
			else
				StopAnim(true);
		}
	};
	GifAnimEx::GifAnimEx(void):imp_(new GifAnimEx::Imp(pm_))
	{
		this;
		imp_->SetOwer(this);
	}
	GifAnimEx::~GifAnimEx(void)
	{
		imp_->StopAnim(false);
		delete imp_;
		imp_ = nullptr;
	}
	std::wstring GifAnimEx::GetClass() const
	{
		return _T("GifAnim");
	}
	LPVOID GifAnimEx::GetInterface(std::wstring pstrName )
	{
			if( _tcscmp(pstrName.c_str(), _T("GifAnim")) == 0 ) 
				return static_cast<GifAnimEx*>(this);
			return Label::GetInterface(pstrName);
	}
	void GifAnimEx::SetAttribute(std::wstring pstrName, std::wstring pstrValue)
	{
		if( _tcscmp(pstrName.c_str(), _T("auto")) == 0 ) 
			imp_->SetAutoStart(_tcscmp(pstrValue.c_str(), _T("true")) == 0);
		else
			__super::SetAttribute(pstrName, pstrValue);
	}
	void GifAnimEx::Init()
	{
		__super::Init();
		imp_->CheckLoadImage();
	}
	void GifAnimEx::SetVisible(bool bVisible /*= true*/)
	{
		__super::SetVisible(bVisible);
		imp_->EventSetVisible(bVisible);
	}
	void GifAnimEx::SetInternVisible(bool bVisible/* = true*/)
	{
		__super::SetInternVisible(bVisible);
		imp_->EventSetVisible(bVisible);
	}
	void GifAnimEx::DoPaint( HDC hDC, const RECT& rcPaint )
	{
		if( !::IntersectRect( &paint_rect_, &rcPaint, &rect_) ) return;
		imp_->DrawFrame( hDC,rcPaint, rect_);
	}
	void GifAnimEx::DoEvent( EventArgs& event )
	{
		this;
		WPARAM nID = event.w_param_;
		if( event.type_ == kEventTimer )
			imp_->EventOnTimer(nID);
	}
	void GifAnimEx::StartAnim()
	{
		imp_->m_bRealStop = false;
		imp_->StartAnim();
	}
	void GifAnimEx::StopAnim()
	{
		imp_->m_bRealStop = true;
		imp_->StopAnim(true);
	}
}
#endif//USE_XIMAGE_EFFECT