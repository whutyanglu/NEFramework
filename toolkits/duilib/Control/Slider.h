#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#pragma once

namespace ui
{
	class UILIB_API Slider : public Progress
	{
		DECLARE_DUICONTROL(Slider)
	public:
		Slider();

		std::wstring GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(std::wstring pstrName);

		void AttachValueChanged(const EventCallback & cb);
		void AttachValueChangedMove(const EventCallback &cb);

		void SetEnabled(bool bEnable = true);

		int GetChangeStep();
		void SetChangeStep(int step);
		void SetThumbSize(SIZE szXY);
		RECT GetThumbRect() const;
		std::wstring GetThumbImage() const;
		void SetThumbImage(std::wstring pStrImage);
		std::wstring GetThumbHotImage() const;
		void SetThumbHotImage(std::wstring pStrImage);
		std::wstring GetThumbPushedImage() const;
		void SetThumbPushedImage(std::wstring pStrImage);

		void DoEvent(EventArgs& event);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		void PaintForeImage(HDC hDC);

		void SetValue(int nValue);
		void SetCanSendMove(bool bCanSend);
		bool GetCanSendMove() const;
	protected:
		SIZE thumb_sz_;
		UINT btn_state_;
		int step_;

		std::wstring thumb_image_;
		std::wstring thumb_hot_image_;
		std::wstring thumb_pushed_image_;

		std::wstring image_modify_;
		bool	   send_move_;
	};
}

#endif // __UISLIDER_H__