#ifndef __UIIPADDRESSEX_H__
#define __UIIPADDRESSEX_H__

#pragma once

//给该控件添加一个属性dtstyle

namespace ui
{
	
    /// IP输入控件
    class UILIB_API IPAddressEx : public Edit
    {
		DECLARE_DUICONTROL(IPAddressEx)
    public:
        IPAddressEx();
        std::wstring GetClass() const;
        LPVOID GetInterface(std::wstring pstrName);
        UINT GetControlFlags() const;
        void DoEvent(EventArgs& event);
        void PaintText(HDC hDC);

        void SetIP(LPCWSTR lpIP);
        std::wstring GetIP();

    private:
        void CharToInt();
        void GetNumInput(TCHAR chKey);
        void UpdateText();
        void IncNum();
        void DecNum();

    protected:
        int first_;
        int second_;
        int third_;
        int fourth_;
        int active_section_;

        TCHAR num_char_;
        std::wstring num_string_;
    };
}
#endif // __UIIPADDRESSEX_H__