#include "StdAfx.h"
#include "ChildBox.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(ChildBox)

	ChildBox::ChildBox()
	{

	}

	void ChildBox::Init()
	{
		if (!xml_file_.empty()){
			DialogBuilder builder;
			Box* pChildWindow = static_cast<Box*>(builder.Create(xml_file_.c_str(), L"", pm_));
			if (pChildWindow){
				this->Add(pChildWindow);
			}
			else{
				this->RemoveAll();
			}
		}
	}

	void ChildBox::SetAttribute( std::wstring pstrName, std::wstring pstrValue )
	{
		if( _tcsicmp(pstrName.c_str(), _T("xmlfile")) == 0 )
			SetChildLayoutXML(pstrValue);
		else
			Box::SetAttribute(pstrName,pstrValue);
	}

	void ChildBox::SetChildLayoutXML(std::wstring pXML )
	{
		xml_file_=pXML;
	}

	std::wstring ChildBox::GetChildLayoutXML()
	{
		return xml_file_;
	}

	LPVOID ChildBox::GetInterface( std::wstring pstrName )
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_CHILDBOX) == 0 ) return static_cast<ChildBox*>(this);
		return Control::GetInterface(pstrName);
	}

	std::wstring ChildBox::GetClass() const
	{
		return _T("ChildBox");
	}
} // namespace DuiLib
