#ifndef __DUILIB_BUTTON_H__
#define __DUILIB_BUTTON_H__

#pragma once

namespace ui
{
	class UILIB_API Button : public Label
	{
		DECLARE_DUICONTROL(Button)

	public:
		Button();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		void AttachClick(const EventCallback &cb);

		virtual int GetFixedWidth() override;
		virtual int GetFixedHeight() override;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void DoEvent(EventArgs& event);

		virtual std::wstring GetNormalImage();
		virtual void SetNormalImage(std::wstring pStrImage);
		virtual std::wstring GetHotImage();
		virtual void SetHotImage(std::wstring pStrImage);
		virtual std::wstring GetPushedImage();
		virtual void SetPushedImage(std::wstring pStrImage);
		virtual std::wstring GetFocusedImage();
		virtual void SetFocusedImage(std::wstring pStrImage);
		virtual std::wstring GetDisabledImage();
		virtual void SetDisabledImage(std::wstring pStrImage);
		virtual std::wstring GetHotForeImage();
		virtual void SetHotForeImage(std::wstring pStrImage);

		void BindTabIndex(int _BindTabIndex);
		void BindTabBoxName(std::wstring _TabLayoutName);
		void BindTriggerTabSel(int _SetSelectIndex = -1);
		void RemoveBindTabIndex();
		int	 GetBindTabBoxIndex();
		std::wstring GetBindTabBoxName();

		void SetHotBkColor(DWORD dwColor);
		DWORD GetHotBkColor() const;
		void SetPushedBkColor(DWORD dwColor);
		DWORD GetPushedBkColor() const;
		void SetHotTextColor(DWORD dwColor);
		DWORD GetHotTextColor() const;
		void SetPushedTextColor(DWORD dwColor);
		DWORD GetPushedTextColor() const;
		void SetFocusedTextColor(DWORD dwColor);
		DWORD GetFocusedTextColor() const;
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		void PaintText(HDC hDC);

		void PaintBkColor(HDC hDC);
		void PaintStatusImage(HDC hDC);
		void PaintForeImage(HDC hDC);

	protected:
		UINT state_;

		DWORD hot_bkcolor_;
		DWORD pushed_bkcolor_;
		DWORD hot_textcolor_;
		DWORD pushed_textcolor;
		DWORD focused_textcolor_;

		std::wstring normal_image_;
		std::wstring hot_image_;
		std::wstring hot_foreimage_;
		std::wstring pushed_image_;
		std::wstring pushed_fore_image_;
		std::wstring focused_image_;
		std::wstring disabled_image_;

		int			 bind_tab_index_;
		std::wstring bind_box_name_;
	};

}	// namespace ui

#endif // __DUILIB_BUTTON_H__