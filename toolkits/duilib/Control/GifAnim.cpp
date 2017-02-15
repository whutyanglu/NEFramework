#include "StdAfx.h"
#include "GifAnim.h"

///////////////////////////////////////////////////////////////////////////////////////
namespace ui
{
	IMPLEMENT_DUICONTROL(GifAnim)

	GifAnim::GifAnim(void)
	{
		gif_image_			=	NULL;
		property_item_		=	NULL;
		frame_count_		=	0;	
		frame_position_	=	0;	
		is_auto_play_		=	true;
		is_auto_size_		=	false;
		playing_		=	false;

	}


	GifAnim::~GifAnim(void)
	{
		DeleteGif();
		pm_->KillTimer( this, EVENT_TIEM_ID );

	}

	std::wstring GifAnim::GetClass() const
	{
		return DUI_CTR_GIFANIM;
	}

	LPVOID GifAnim::GetInterface(std::wstring pstrName )
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_GIFANIM) == 0 ) return static_cast<GifAnim*>(this);
		return Control::GetInterface(pstrName);
	}

	void GifAnim::DoInit()
	{
		InitGifImage();
	}

	void GifAnim::DoPaint( HDC hDC, const RECT& rcPaint )
	{
		if( !::IntersectRect( &paint_rect_, &rcPaint, &rect_ ) ) return;
		if ( NULL == gif_image_ )
		{		
			InitGifImage();
		}
		DrawFrame( hDC );
	}

	void GifAnim::DoEvent( EventArgs& event )
	{
		if( event.type_ == kEventTimer )
			OnTimer( (UINT_PTR)event.w_param_ );
	}

	void GifAnim::SetVisible(bool bVisible /* = true */)
	{
		Control::SetVisible(bVisible);
		if (bVisible)
			PlayGif();
		else
			StopGif();
	}

	void GifAnim::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();
		if( _tcsicmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("autoplay")) == 0 ) {
			SetAutoPlay(_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("autosize")) == 0 ) {
			SetAutoSize(_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else
			Control::SetAttribute(strName, strValue);
	}

	void GifAnim::SetBkImage(std::wstring pStrImage)
	{
		if( bk_image_ == pStrImage || pStrImage.empty()) return;

		bk_image_ = pStrImage;

		StopGif();
		DeleteGif();

		Invalidate();

	}

	std::wstring GifAnim::GetBkImage()
	{
		return bk_image_;
	}

	void GifAnim::SetAutoPlay(bool bIsAuto)
	{
		is_auto_play_ = bIsAuto;
	}

	bool GifAnim::IsAutoPlay() const
	{
		return is_auto_play_;
	}

	void GifAnim::SetAutoSize(bool bIsAuto)
	{
		is_auto_size_ = bIsAuto;
	}

	bool GifAnim::IsAutoSize() const
	{
		return is_auto_size_;
	}

	void GifAnim::PlayGif()
	{
		if (playing_ || gif_image_ == NULL)
		{
			return;
		}

		long lPause = ((long*) property_item_->value)[frame_position_] * 10;
		if ( lPause == 0 ) lPause = 100;
		pm_->SetTimer( this, EVENT_TIEM_ID, lPause );

		playing_ = true;
	}

	void GifAnim::PauseGif()
	{
		if (!playing_ || gif_image_ == NULL)
		{
			return;
		}

		pm_->KillTimer(this, EVENT_TIEM_ID);
		this->Invalidate();
		playing_ = false;
	}

	void GifAnim::StopGif()
	{
		if (!playing_)
		{
			return;
		}

		pm_->KillTimer(this, EVENT_TIEM_ID);
		frame_position_ = 0;
		this->Invalidate();
		playing_ = false;
	}

	void GifAnim::InitGifImage()
	{
		gif_image_ = RenderEngine::GdiplusLoadImage(GetBkImage());
		if ( NULL == gif_image_ ) return;
		UINT nCount	= 0;
		nCount	=	gif_image_->GetFrameDimensionsCount();
		GUID* pDimensionIDs	=	new GUID[ nCount ];
		gif_image_->GetFrameDimensionsList( pDimensionIDs, nCount );
		frame_count_	=	gif_image_->GetFrameCount( &pDimensionIDs[0] );
		int nSize		=	gif_image_->GetPropertyItemSize( PropertyTagFrameDelay );
		property_item_	=	(Gdiplus::PropertyItem*) malloc( nSize );
		gif_image_->GetPropertyItem( PropertyTagFrameDelay, nSize, property_item_ );
		delete  pDimensionIDs;
		pDimensionIDs = NULL;

		if (is_auto_size_)
		{
			SetFixedWidth(gif_image_->GetWidth());
			SetFixedHeight(gif_image_->GetHeight());
		}
		if (is_auto_play_)
		{
			PlayGif();
		}
	}

	void GifAnim::DeleteGif()
	{
		if ( gif_image_ != NULL )
		{
			delete gif_image_;
			gif_image_ = NULL;
		}

		if ( property_item_ != NULL )
		{
			free( property_item_ );
			property_item_ = NULL;
		}
		frame_count_		=	0;	
		frame_position_	=	0;	
	}

	void GifAnim::OnTimer( UINT_PTR idEvent )
	{
		if ( idEvent != EVENT_TIEM_ID )
			return;
		pm_->KillTimer( this, EVENT_TIEM_ID );
		this->Invalidate();

		frame_position_ = (++frame_position_) % frame_count_;

		long lPause = ((long*) property_item_->value)[frame_position_] * 10;
		if ( lPause == 0 ) lPause = 100;
		pm_->SetTimer( this, EVENT_TIEM_ID, lPause );
	}

	void GifAnim::DrawFrame( HDC hDC )
	{
		if ( NULL == hDC || NULL == gif_image_ ) return;
		GUID pageGuid = Gdiplus::FrameDimensionTime;
		Gdiplus::Graphics graphics( hDC );
		graphics.DrawImage( gif_image_, rect_.left, rect_.top, rect_.right-rect_.left, rect_.bottom-rect_.top );
		gif_image_->SelectActiveFrame( &pageGuid, frame_position_ );
	}
}