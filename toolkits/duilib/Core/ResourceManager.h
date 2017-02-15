#ifndef __UIRESOURCEMANAGER_H__
#define __UIRESOURCEMANAGER_H__
#pragma once

namespace ui {
	// 控件文字查询接口
	class UILIB_API IQueryControlText
	{
	public:
		virtual std::wstring QueryControlText(std::wstring lpstrId, std::wstring lpstrType) = 0;
	};

	class UILIB_API ResourceManager
	{
	private:
		ResourceManager(void);
		~ResourceManager(void);

	public:
		static ResourceManager* GetInstance()
		{
			static ResourceManager * p = new ResourceManager;
			return p;
		};	
		void Release(void) { delete this; }

	public:
		BOOL LoadResource(STRINGorID xml, std::wstring type = L"");
		BOOL LoadResource(MarkupNode Root);
		void ResetResourceMap();
		std::wstring GetImagePath(std::wstring lpstrId);
		std::wstring GetXmlPath(std::wstring lpstrId);

	public:
		void SetLanguage(std::wstring pstrLanguage) { m_sLauguage = pstrLanguage; }
		std::wstring GetLanguage() { return m_sLauguage; }
		BOOL LoadLanguage(std::wstring pstrXml);
		
	public:
		void SetTextQueryInterface(IQueryControlText* pInterface) { m_pQuerypInterface = pInterface; }
		std::wstring GetText(std::wstring lpstrId, std::wstring lpstrType = L"");
		void ReloadText();
		void ResetTextMap();

	private:
		CStdStringPtrMap m_mTextResourceHashMap;
		IQueryControlText*	m_pQuerypInterface;
		CStdStringPtrMap m_mImageHashMap;
		CStdStringPtrMap m_mXmlHashMap;
		Markup xml_;
		std::wstring m_sLauguage;
		CStdStringPtrMap m_mTextHashMap;
	};

} // namespace DuiLib

#endif // __UIRESOURCEMANAGER_H__