#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace ui
{
	class DateTimeWnd;

	/// 时间选择控件
	class UILIB_API DateTime : public Label
	{
		DECLARE_DUICONTROL(DateTime)
		friend class DateTimeWnd;
	public:
		DateTime();
		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		SYSTEMTIME& GetTime();
		void SetTime(SYSTEMTIME* pst);

		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;

		void UpdateText();

		void DoEvent(EventArgs& event);

	protected:
		SYSTEMTIME sys_time_;
		int        update_flag_;
		bool       is_readonly_;

		DateTimeWnd* wnd_;
	};
}
#endif // __UIDATETIME_H__