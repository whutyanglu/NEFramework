#ifndef __DUILIB_HBOX_H__
#define __DUILIB_HBOX_H__

#pragma once

namespace ui
{
	class UILIB_API HBox : public Box
	{
		DECLARE_DUICONTROL(HBox)
	public:
		HBox();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		virtual int GetFixedWidth() override;
		virtual int GetFixedHeight() override;
		
		void SetSepWidth(int iWidth);
		int GetSepWidth() const;
		void SetSepImmMode(bool bImmediately);
		bool IsSepImmMode() const;
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		void DoEvent(EventArgs& event);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoPostPaint(HDC hDC, const RECT& rcPaint);

		RECT GetThumbRect(bool bUseNew = false) const;

	protected:
		int sep_width_;
		UINT btn_state_;
		POINT last_mouse_pt_;
		RECT new_pos_;
		bool imm_mode_;
	};
}
#endif // __UIHORIZONTALLAYOUT_H__
