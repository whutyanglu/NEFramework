#ifndef __UIGROUPBOX_H__
#define __UIGROUPBOX_H__

#pragma once

namespace ui
{

	class UILIB_API GroupBox : public VBox
	{
		DECLARE_DUICONTROL(GroupBox)
	public:
		GroupBox();
		~GroupBox();
		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		void SetFont(int index);
		int GetFont() const;

	protected:	
		//Paint
		virtual void PaintText(HDC hDC);
		virtual void PaintBorder(HDC hDC);
		virtual void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

	private:
		SIZE CalcrectSize(SIZE szAvailable);

	protected:
		DWORD text_color_;
		DWORD disabled_text_color_;
		int font_;
		UINT text_style_;
	};
}
#endif // __UIGROUPBOX_H__