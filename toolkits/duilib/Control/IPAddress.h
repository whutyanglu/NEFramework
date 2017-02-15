#ifndef __UIIPADDRESS_H__
#define __UIIPADDRESS_H__

#pragma once

//给该控件添加一个属性dtstyle

namespace ui
{
	class IPAddressWnd;

	/// 时间选择控件
	class UILIB_API IPAddress : public Label
	{
		DECLARE_DUICONTROL(IPAddress)

		friend class IPAddressWnd;
	public:
		IPAddress();
		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		DWORD GetIP();
		void SetIP(DWORD dwIP);

		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;

		void UpdateText();

		void DoEvent(EventArgs& event);

		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

	protected:
		DWORD	ip_;
		bool    read_only_;
		int		update_flag_;

		IPAddressWnd* wnd_;
	};
}
#endif // __UIIPADDRESS_H__