#ifndef _DUILIB_VBOX_H__
#define _DUILIB_VBOX_H__

#pragma once

namespace ui
{
	class UILIB_API VBox : public Box
	{
		DECLARE_DUICONTROL(VBox)
	public:
		VBox();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		UINT GetControlFlags() const;

		virtual int GetFixedHeight();

		void SetSepHeight(int iHeight);
		int GetSepHeight() const;
		void SetSepImmMode(bool bImmediately);
		bool IsSepImmMode() const;
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		void DoEvent(EventArgs& event);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoPostPaint(HDC hDC, const RECT& rcPaint);

		RECT GetThumbRect(bool bUseNew = false) const;

	protected:
		int sep_height_;
		UINT btn_state_;
		POINT last_mouse_pt_;
		RECT new_pos_;
		bool imm_mode_;
	};
}
#endif // _DUILIB_VBOX_H__
