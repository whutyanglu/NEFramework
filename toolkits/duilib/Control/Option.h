#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace ui
{
	class UILIB_API Option : public Button
	{
		DECLARE_DUICONTROL(Option)
	public:
		Option();
		~Option();

		std::wstring GetClass() const;
		LPVOID GetInterface(std::wstring pstrName);
		
		void AttachSelectedChanged(const EventCallback &cb);
		void SetManager(PaintManager* pManager, Control* pParent, bool bInit = true);

		bool Activate();
		void SetEnabled(bool bEnable = true);

		std::wstring GetSelectedImage();
		void SetSelectedImage(std::wstring pStrImage);

		std::wstring GetSelectedHotImage();
		void SetSelectedHotImage(std::wstring pStrImage);

		std::wstring GetSelectedPushedImage();
		void SetSelectedPushedImage(std::wstring pStrImage);

		void SetSelectedTextColor(DWORD dwTextColor);
		DWORD GetSelectedTextColor();

		void SetSelectedBkColor(DWORD dwBkColor);
		DWORD GetSelectBkColor();

		std::wstring GetSelectedForedImage();
		void SetSelectedForedImage(std::wstring pStrImage);

		std::wstring GetGroup() const;
		void SetGroup(std::wstring pStrGroupName = L"");
		bool IsSelected() const;
		virtual void Selected(bool bSelected);

		void SetAttribute(std::wstring pstrName, std::wstring pstrValue);

		void PaintBkColor(HDC hDC);
		void PaintStatusImage(HDC hDC);
		void PaintForeImage(HDC hDC);
		void PaintText(HDC hDC);

	protected:
		bool			selected_;
		std::wstring	group_name_;

		DWORD			selected_bkcolor_;
		DWORD			selected_text_color_;

		std::wstring	selected_image_;
		std::wstring	selected_hot_image_;
		std::wstring	selected_pushed_image_;
		std::wstring	selected_fore__image_;
	};

	class UILIB_API CheckBox : public Option
	{
		DECLARE_DUICONTROL(CheckBox)

	public:
		virtual std::wstring GetClass() const override;
		virtual LPVOID GetInterface(std::wstring pstrName) override;
		virtual int GetFixedWidth() override;
		
		void AttachCheckClick(const EventCallback &cb);

		void SetCheck(bool bCheck);
		bool GetCheck() const;
	protected:
		bool auto_check; 

	public:
		CheckBox();
		virtual void SetAttribute(std::wstring pstrName, std::wstring pstrValue);
		void SetAutoCheck(bool bEnable);
		virtual void DoEvent(EventArgs& event);
		virtual void Selected(bool bSelected, bool send_notify = true);
	};
} // namespace DuiLib

#endif // __UIOPTION_H__