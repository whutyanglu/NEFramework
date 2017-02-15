#ifndef __UIDLGBUILDER_H__
#define __UIDLGBUILDER_H__

#pragma once

namespace ui {

	using ControlBuilderCallback = std::function<Control*(std::wstring cls)>;

	class UILIB_API DialogBuilder
	{
	public:
		Control* Create(STRINGorID xml, std::wstring type = L"", PaintManager* pManager = NULL, 
			Control* pParent = NULL, ControlBuilderCallback pCallback = nullptr);
		Control* Create(ControlBuilderCallback pCallback = nullptr, PaintManager* pManager = NULL,
			Control* pParent = NULL);

		Markup* GetMarkup();

		std::wstring GetLastErrorMessage() const;
		std::wstring GetLastErrorLocation() const;
	    void SetInstance(HINSTANCE instance){
			instance_ = instance;
		};

	private:
		Control* _Parse(MarkupNode* parent, Control* pParent = NULL, PaintManager* pManager = NULL);

	private:
		Markup			xml_;
		std::wstring	type_;
		HINSTANCE		instance_;
		ControlBuilderCallback callback_ = nullptr;
	};

} // namespace DuiLib

#endif // __UIDLGBUILDER_H__
