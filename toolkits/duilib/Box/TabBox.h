#ifndef _DUILIB_TABBOX_H__
#define _DUILIB_TABBOX_H__

#pragma once

namespace ui
{
	class UILIB_API TabBox : public Box
	{
		DECLARE_DUICONTROL(TabBox)
	public:
		TabBox();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);

		bool Add(Control* pControl);
		bool AddAt(Control* pControl, int iIndex);
		bool Remove(Control* pControl);
		void RemoveAll();
		int GetCurSel() const;
		virtual bool SelectItem(int iIndex);
		virtual bool SelectItem(Control* pControl);

		void SetPos(RECT rc, bool bNeedInvalidate = true);

		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

	protected:
		int cur_sel_;
	};
}
#endif // _DUILIB_TABBOX_H__
