#ifndef __UICHILDBOX_H__
#define __UICHILDBOX_H__

#pragma once

namespace ui
{
	class UILIB_API ChildBox : public Box
	{
		DECLARE_DUICONTROL(ChildBox)
	public:
		ChildBox();

		void Init();
		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		void SetChildLayoutXML(std::wstring pXML);
		std::wstring GetChildLayoutXML();
		virtual LPVOID GetInterface(std::wstring pstrName);
		virtual std::wstring GetClass() const;

	private:
		std::wstring xml_file_;
	};
} // namespace DuiLib
#endif // __UICHILDBOX_H__
