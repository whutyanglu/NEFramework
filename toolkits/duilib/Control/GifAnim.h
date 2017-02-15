#ifndef GifAnimUI_h__
#define GifAnimUI_h__

#pragma once

namespace ui
{
	class UILIB_API GifAnim : public Control
	{
		enum
		{ 
			EVENT_TIEM_ID = 100,
		};
		DECLARE_DUICONTROL(GifAnim)
	public:
		GifAnim(void);
		~GifAnim(void);

		std::wstring	GetClass() const;
		LPVOID	GetInterface(std::wstring pstrName);
		void	DoInit() override;
		void	DoPaint(HDC hDC, const RECT& rcPaint) override;
		void	DoEvent(EventArgs& event) override;
		void	SetVisible(bool bVisible = true ) override;
		void	SetAttribute(std::wstring pstrName, std::wstring pstrValue) override;
		void	SetBkImage(std::wstring pStrImage);
		std::wstring GetBkImage();

		void	SetAutoPlay(bool bIsAuto = true );
		bool	IsAutoPlay() const;
		void	SetAutoSize(bool bIsAuto = true );
		bool	IsAutoSize() const;
		void	PlayGif();
		void	PauseGif();
		void	StopGif();

	private:
		void	InitGifImage();
		void	DeleteGif();
		void    OnTimer( UINT_PTR idEvent );
		void	DrawFrame( HDC hDC );		// 绘制GIF每帧

	private:
		Gdiplus::Image	*gif_image_;
		UINT			frame_count_;				// gif图片总帧数
		UINT			frame_position_;			// 当前放到第几帧
		Gdiplus::PropertyItem*	property_item_;	// 帧与帧之间间隔时间

		std::wstring	bk_image_;
		bool			is_auto_play_;				// 是否自动播放gif
		bool			is_auto_size_;				// 是否自动根据图片设置大小
		bool			playing_;
	};
}

#endif // GifAnimUI_h__
