#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace ui
{
	class UILIB_API Label : public Control
	{
		DECLARE_DUICONTROL(Label)
	public:
		Label();
		~Label();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		virtual int GetFixedHeight() override;
		virtual int GetFixedWidth() override;

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		void SetFont(int index);
		int GetFont() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);

		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(EventArgs& event);
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		void PaintText(HDC hDC);

		virtual bool GetAutoCalcWidth() const;
		virtual void SetAutoCalcWidth(bool bAutoCalcWidth);
		virtual void SetText(std::wstring pstrText);
		
	protected:
		DWORD	text_color_;
		DWORD	disabled_text_color_;
		int		font_;
		UINT	text_style_;
		RECT	text_padding_rect_;
		bool	show_html_;
		bool	auto_calc_width_;
	};
}

#endif // __UILABEL_H__