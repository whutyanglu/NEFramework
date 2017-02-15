#include "StdAfx.h"
#include "Ring.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(Ring)

	Ring::Ring() : m_fCurAngle(0.0f), m_pBkimage(NULL)
	{
	}

	Ring::~Ring()
	{
		if(pm_) pm_->KillTimer(this, RING_TIMERID);

		DeleteImage();
	}

	std::wstring Ring::GetClass() const
	{
		return _T("Ring");
	}

	LPVOID Ring::GetInterface( std::wstring pstrName )
	{
		if( _tcscmp(pstrName.c_str(), _T("Ring")) == 0 ) return static_cast<Ring*>(this);
		return Label::GetInterface(pstrName);
	}

	void Ring::SetAttribute( std::wstring pstrName, std::wstring pstrValue )
	{
		if( _tcscmp(pstrName.c_str(), _T("bkimage")) == 0 ) SetBkImage(pstrValue);
		else Label::SetAttribute(pstrName, pstrValue);
	}

	void Ring::SetBkImage( std::wstring pStrImage )
	{
		if (bkimage_ == pStrImage) return;
		bkimage_ = pStrImage;
		DeleteImage();
		Invalidate();
	}

	void Ring::PaintBkImage( HDC hDC )
	{
		if(m_pBkimage == NULL) {
			InitImage();
		}

		if(m_pBkimage != NULL) {
			RECT rcItem = rect_;
			int iWidth = rcItem.right - rcItem.left;
			int iHeight = rcItem.bottom - rcItem.top;
			Gdiplus::PointF centerPos(rcItem.left + iWidth/2, rcItem.top + iHeight/2);

			Gdiplus::Graphics graphics(hDC);
			graphics.TranslateTransform(centerPos.X,centerPos.Y);
			graphics.RotateTransform(m_fCurAngle);
			graphics.TranslateTransform(-centerPos.X, -centerPos.Y);//还原源点
			graphics.DrawImage(m_pBkimage,rcItem.left,rcItem.top,iWidth,iHeight);
		}
	}

	void Ring::DoEvent( EventArgs& event )
	{
		if( event.type_ == kEventTimer && event.w_param_ == RING_TIMERID ) {
			if(m_fCurAngle > 359) {
				m_fCurAngle = 0;
			}
			m_fCurAngle += 36.0;
			Invalidate();
		}
		else {
			Label::DoEvent(event);
		}
	}

	void Ring::InitImage()
	{
		m_pBkimage = RenderEngine::GdiplusLoadImage(GetBkImage());
		if ( NULL == m_pBkimage ) return;
		if(pm_) pm_->SetTimer(this, RING_TIMERID, 100);
	}

	void Ring::DeleteImage()
	{
		if ( m_pBkimage != NULL )
		{
			delete m_pBkimage;
			m_pBkimage = NULL;
		}
	}
}
