#include "StdAfx.h"
#include <math.h>

namespace ui {
#define HSLMAX   255	/* H,L, and S vary over 0-HSLMAX */
#define RGBMAX   255    /* R,G, and B vary over 0-RGBMAX */
#define HSLUNDEFINED (HSLMAX*2/3)

	/*
	* Convert hue value to RGB
	*/
	static float HueToRGB(float v1, float v2, float vH)
	{
		if (vH < 0.0f) vH += 1.0f;
		if (vH > 1.0f) vH -= 1.0f;
		if ((6.0f * vH) < 1.0f) return (v1 + (v2 - v1) * 6.0f * vH);
		if ((2.0f * vH) < 1.0f) return (v2);
		if ((3.0f * vH) < 2.0f) return (v1 + (v2 - v1) * ((2.0f / 3.0f) - vH) * 6.0f);
		return (v1);
	}

	/*
	* Convert color RGB to HSL
	* pHue HSL hue value			[0 - 1]
	* pSat HSL saturation value		[0 - 1]
	* pLue HSL luminance value		[0 - 1]
	*/

	static void RGBToHSL(DWORD clr, float *pHue, float *pSat, float *pLue)
	{
		float R = (float)(GetRValue(clr) / 255.0f);  //RGB from 0 to 255
		float G = (float)(GetGValue(clr) / 255.0f);
		float B = (float)(GetBValue(clr) / 255.0f);

		float H, S, L;

		float fMin = min(R, min(G, B));		//Min. value of RGB
		float fMax = max(R, max(G, B));		//Max. value of RGB
		float fDelta = fMax - fMin;				//Delta RGB value

		L = (fMax + fMin) / 2.0f;

		if (fDelta == 0)                     //This is a gray, no chroma...
		{
			H = 0.0f;                          //HSL results from 0 to 1
			S = 0.0f;
		}
		else                                   //Chromatic data...
		{
			float del_R, del_G, del_B;

			if (L < 0.5) S = fDelta / (fMax + fMin);
			else           S = fDelta / (2.0f - fMax - fMin);

			del_R = (((fMax - R) / 6.0f) + (fDelta / 2.0f)) / fDelta;
			del_G = (((fMax - G) / 6.0f) + (fDelta / 2.0f)) / fDelta;
			del_B = (((fMax - B) / 6.0f) + (fDelta / 2.0f)) / fDelta;

			if (R == fMax) H = del_B - del_G;
			else if (G == fMax) H = (1.0f / 3.0f) + del_R - del_B;
			else if (B == fMax) H = (2.0f / 3.0f) + del_G - del_R;

			if (H < 0.0f) H += 1.0f;
			if (H > 1.0f)  H -= 1.0f;
		}

		*pHue = H;
		*pSat = S;
		*pLue = L;
	}

	/*
	* Convert color HSL to RGB
	* H HSL hue value				[0 - 1]
	* S HSL saturation value		[0 - 1]
	* L HSL luminance value			[0 - 1]
	*/
	static DWORD HSLToRGB(float H, float S, float L)
	{
		BYTE R, G, B;
		float var_1, var_2;

		if (S == 0)                       //HSL from 0 to 1
		{
			R = G = B = (BYTE)(L * 255.0f);   //RGB results from 0 to 255
		}
		else
		{
			if (L < 0.5) var_2 = L * (1.0f + S);
			else           var_2 = (L + S) - (S * L);

			var_1 = 2.0f * L - var_2;

			R = (BYTE)(255.0f * HueToRGB(var_1, var_2, H + (1.0f / 3.0f)));
			G = (BYTE)(255.0f * HueToRGB(var_1, var_2, H));
			B = (BYTE)(255.0f * HueToRGB(var_1, var_2, H - (1.0f / 3.0f)));
		}

		return RGB(R, G, B);
	}

	/*
	* _HSLToRGB color HSL value to RGB
	* clr  RGB color value
	* nHue HSL hue value			[0 - 360]
	* nSat HSL saturation value		[0 - 200]
	* nLue HSL luminance value		[0 - 200]
	*/
#define _HSLToRGB(h,s,l) (0xFF << 24 | HSLToRGB((float)h / 360.0f,(float)s / 200.0f,l / 200.0f))

	///////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(ColorPalette)

	ColorPalette::ColorPalette()
		: btn_state_(0)
		, in_bar_(false)
		, in_pallet_(false)
		, cur_h_(180)
		, cur_s_(200)
		, cur_b_(100)
		, pallet_height_(200)
		, bar_height_(10)
		, bits_(NULL)
	{
		memset(&bitmap_info_, 0, sizeof(bitmap_info_));
	
		mem_bitmap_=NULL;
	}

	ColorPalette::~ColorPalette()
	{
		if (bits_) free(bits_);

		if (mem_bitmap_) {
			::DeleteObject(mem_bitmap_);
		}

	}

	void ColorPalette::AttachColorChanged(const EventCallback & cb)
	{
		on_event_[kEventColorChanged] += cb;
	}

	DWORD ColorPalette::GetSelectColor()
	{
		DWORD dwColor = _HSLToRGB(cur_h_, cur_s_, cur_b_);
		return 0xFF << 24 | GetRValue(dwColor) << 16 | GetGValue(dwColor) << 8 | GetBValue(dwColor);
	}

	void ColorPalette::SetSelectColor(DWORD dwColor) 
	{
		float H = 0, S = 0, B = 0;
		COLORREF dwBkClr = RGB(GetBValue(dwColor),GetGValue(dwColor),GetRValue(dwColor));
		RGBToHSL(dwBkClr, &H, &S, &B);
		cur_h_ = (int)(H*360);
		cur_s_ = (int)(S*200);
		cur_b_ = (int)(B*200);
		NeedUpdate();
		UpdatePalletData();
	}

	std::wstring ColorPalette::GetClass() const
	{
		return _T("ColorPalette");
	}

	LPVOID ColorPalette::GetInterface(std::wstring pstrName)
	{
		if (_tcscmp(pstrName.c_str(), DUI_CTR_COLORPALETTE) == 0) return static_cast<ColorPalette*>(this);
		return Control::GetInterface(pstrName);
	}

	void ColorPalette::SetPalletHeight(int nHeight)
	{
		pallet_height_ = nHeight;
	}
	int	 ColorPalette::GetPalletHeight() const
	{
		return pallet_height_;
	}
	void ColorPalette::SetBarHeight(int nHeight)
	{
		if (nHeight>150) {
			nHeight = 150; //限制最大高度，由于当前设计，nheight超出190，程序会因越界访问崩溃
		}
		bar_height_ = nHeight;
	}
	int  ColorPalette::GetBarHeight() const
	{
		return bar_height_;
	}

	void ColorPalette::SetThumbImage(std::wstring pszImage)
	{
		if (thumb_image_ != pszImage)
		{
			thumb_image_ = pszImage;
			NeedUpdate();
		}
	}

	std::wstring ColorPalette::GetThumbImage() const
	{
		return thumb_image_;
	}

	void ColorPalette::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if (_tcscmp(pstrName, _T("palletheight")) == 0) SetPalletHeight(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("barheight")) == 0) SetBarHeight(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("thumbimage")) == 0) SetThumbImage(pstrValue);
		else Control::SetAttribute(strName, strValue);
	}

	void ColorPalette::DoInit()
	{
		mem_dc_ = CreateCompatibleDC(GetManager()->GetPaintDC());
		mem_bitmap_ = CreateCompatibleBitmap(GetManager()->GetPaintDC(), 400, 360);
		HBITMAP pOldBit = (HBITMAP)SelectObject(mem_dc_, mem_bitmap_);

		::GetObject(mem_bitmap_, sizeof(bitmap_info_), &bitmap_info_);
		DWORD dwSize = bitmap_info_.bmHeight * bitmap_info_.bmWidthBytes;
		bits_ = (BYTE *)malloc(dwSize);
		::GetBitmapBits(mem_bitmap_, dwSize, bits_);

	}

	void ColorPalette::SetPos(RECT rc)
	{
		Control::SetPos(rc);

		last_pallet_mouse_.x = cur_h_ * (rect_.right - rect_.left) / 360 + rect_.left;
		last_pallet_mouse_.y = (200 - cur_b_) * pallet_height_ / 200 + rect_.top;

		UpdatePalletData();
		UpdateBarData();
	}

	void ColorPalette::DoEvent(EventArgs& event)
	{
		Control::DoEvent(event);

		if (event.type_ == kEventMouseButtonDown){
			if (event.mouse_point_.x >= rect_.left && event.mouse_point_.y >= rect_.top &&
				event.mouse_point_.x < rect_.right && event.mouse_point_.y < rect_.top + pallet_height_){
				int x = (event.mouse_point_.x - rect_.left) * 360 / (rect_.right - rect_.left);
				int y = (event.mouse_point_.y - rect_.top) * 200 / pallet_height_;
				x = min(max(x, 0), 360);
				y = min(max(y, 0), 200);

				last_pallet_mouse_ = event.mouse_point_;
				if (last_pallet_mouse_.x < rect_.left) last_pallet_mouse_.x = rect_.left;
				if (last_pallet_mouse_.x > rect_.right) last_pallet_mouse_.x = rect_.right;
				if (last_pallet_mouse_.y < rect_.top) last_pallet_mouse_.y = rect_.top;
				if (last_pallet_mouse_.y > rect_.top + pallet_height_) last_pallet_mouse_.y = rect_.top + pallet_height_;

				cur_h_ = x;
				cur_b_ = 200 - y;

				btn_state_ |= UISTATE_PUSHED;
				in_pallet_ = true;
				in_bar_ = false;

				UpdateBarData();
			}
			//::PtInRect(&m_rcItem, event.ptMouse)
			if (event.mouse_point_.x >= rect_.left && event.mouse_point_.y >= rect_.bottom - bar_height_ &&
				event.mouse_point_.x < rect_.right && event.mouse_point_.y < rect_.bottom){
				cur_s_ = (event.mouse_point_.x - rect_.left) * 200 / (rect_.right - rect_.left);
				btn_state_ |= UISTATE_PUSHED;
				in_bar_ = true;
				in_pallet_ = false;
				UpdatePalletData();
			}

			Invalidate();
			return;
		}
		if (event.type_ == kEventMouseButtonUp){
			DWORD color=0;
			if ((btn_state_ | UISTATE_PUSHED) && (IsEnabled())){
				color = GetSelectColor();
				Notify(kEventColorChanged, color, 0);
			}

			btn_state_ &= ~UISTATE_PUSHED;
			in_pallet_ = false;
			in_bar_ = false;

			Invalidate();
			return;
		}

		if (event.type_ == kEventMouseMove){

			if (!(btn_state_ &UISTATE_PUSHED)){
				in_bar_ = false;
				in_pallet_ = false;
			}
			if (in_pallet_ == true){
				POINT pt = event.mouse_point_;
				pt.x -= rect_.left;
				pt.y -= rect_.top;

				if (pt.x >= 0 && pt.y >= 0 && pt.x <= rect_.right && pt.y <= rect_.top + pallet_height_){
					int x = pt.x * 360 / (rect_.right - rect_.left);
					int y = pt.y * 200 / pallet_height_;
					x = min(max(x, 0), 360);
					y = min(max(y, 0), 200);

					last_pallet_mouse_ = event.mouse_point_;
					if (last_pallet_mouse_.x < rect_.left) last_pallet_mouse_.x = rect_.left;
					if (last_pallet_mouse_.x > rect_.right) last_pallet_mouse_.x = rect_.right;
					if (last_pallet_mouse_.y < rect_.top) last_pallet_mouse_.y = rect_.top;
					if (last_pallet_mouse_.y > rect_.top + pallet_height_) last_pallet_mouse_.y = rect_.top + pallet_height_;


					cur_h_ = x;
					cur_b_ = 200 - y;

					UpdateBarData();
				}
			}
			if (in_bar_ == true){
				cur_s_ = (event.mouse_point_.x - rect_.left) * 200 / (rect_.right - rect_.left);
				cur_s_ = min(max(cur_s_, 0), 200);

				UpdatePalletData();
			}

			Invalidate();
			return;
		}

	}

	void ColorPalette::PaintBkColor(HDC hDC)
	{
		PaintPallet(hDC);
	}
	
	void ColorPalette::PaintPallet(HDC hDC)
	{
		int nSaveDC = ::SaveDC(hDC);

		::SetStretchBltMode(hDC, HALFTONE);
		//拉伸模式将内存图画到控件上

		StretchBlt(hDC, rect_.left, rect_.top, rect_.right - rect_.left, pallet_height_, mem_dc_, 0, 0, 360, 199, SRCCOPY);
		StretchBlt(hDC, rect_.left, rect_.bottom - bar_height_, rect_.right - rect_.left, bar_height_, mem_dc_, 0, 210, 200, bar_height_, SRCCOPY);

		RECT rcCurSorPaint = { last_pallet_mouse_.x - 4, last_pallet_mouse_.y - 4, last_pallet_mouse_.x + 4, last_pallet_mouse_.y + 4 };
		RenderEngine::DrawImageString(hDC, pm_, rcCurSorPaint, paint_rect_, thumb_image_);

		rcCurSorPaint.left = rect_.left + cur_s_ * (rect_.right - rect_.left) / 200 - 4;
		rcCurSorPaint.right = rect_.left + cur_s_ * (rect_.right - rect_.left) / 200 + 4;
		rcCurSorPaint.top = rect_.bottom - bar_height_ / 2 - 4;
		rcCurSorPaint.bottom = rect_.bottom - bar_height_ / 2 + 4;
		RenderEngine::DrawImageString(hDC, pm_, rcCurSorPaint, paint_rect_, thumb_image_);
		::RestoreDC(hDC, nSaveDC);
	}

	void ColorPalette::UpdatePalletData()
	{
		int x, y;
		BYTE *pPiexl;
		DWORD dwColor;
		for (y = 0; y < 200; ++y) {
			for (x = 0; x < 360; ++x) {
				pPiexl = LPBYTE(bits_) + ((199 - y)*bitmap_info_.bmWidthBytes) + ((x*bitmap_info_.bmBitsPixel) / 8);
				dwColor = _HSLToRGB(x, cur_s_, y);
				pPiexl[0] = GetBValue(dwColor);
				pPiexl[1] = GetGValue(dwColor);
				pPiexl[2] = GetRValue(dwColor);
			}
		}

		SetBitmapBits(mem_bitmap_, bitmap_info_.bmWidthBytes * bitmap_info_.bmHeight, bits_);
	}


	void ColorPalette::UpdateBarData()
	{
		int x, y;
		BYTE *pPiexl;
		DWORD dwColor;
		//这里画出Bar
		for (y = 0; y < bar_height_; ++y) 
		{
			for (x = 0; x < 200; ++x) 
			{
				pPiexl = LPBYTE(bits_) + ((210 + y)*bitmap_info_.bmWidthBytes) + ((x*bitmap_info_.bmBitsPixel) / 8);
				//*(DWORD*)pPiexl = _HSLToRGB(m_nCurH, x , m_nCurB);
				dwColor = _HSLToRGB(cur_h_, x, cur_b_);
				pPiexl[0] = GetBValue(dwColor);
				pPiexl[1] = GetGValue(dwColor);
				pPiexl[2] = GetRValue(dwColor);
			}
		}

		SetBitmapBits(mem_bitmap_, bitmap_info_.bmWidthBytes * bitmap_info_.bmHeight, bits_);
	}

}