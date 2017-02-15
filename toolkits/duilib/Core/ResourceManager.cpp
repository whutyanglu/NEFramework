#include "StdAfx.h"
#include "ResourceManager.h"

namespace ui {
	
	ResourceManager::ResourceManager(void)
	{
		m_pQuerypInterface = NULL;
		
	}

	ResourceManager::~ResourceManager(void)
	{
		//重置语言文本缓冲map
		ResetTextMap();

		//重置图片资源map
		ResetResourceMap();
	}

	BOOL ResourceManager::LoadResource(STRINGorID xml, std::wstring type)
	{
		if( HIWORD(xml.m_lpstr) != NULL ) 
		{
			if( *(xml.m_lpstr) == _T('<') ) 
			{
				if( !xml_.Load(xml.m_lpstr) ) return NULL;
			}
			else 
			{
				if( !xml_.LoadFromFile(xml.m_lpstr) ) return NULL;
			}
		}
		else
		{
			HRSRC hResource = ::FindResource(PaintManager::GetResourceDll(), xml.m_lpstr, type.c_str());
			if( hResource == NULL ) return NULL;
			HGLOBAL hGlobal = ::LoadResource(PaintManager::GetResourceDll(), hResource);
			if( hGlobal == NULL ) 
			{
				FreeResource(hResource);
				return NULL;
			}

			if( !xml_.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(PaintManager::GetResourceDll(), hResource) )) 
				return NULL;

			::FreeResource(hResource);
		}

		return LoadResource(xml_.GetRoot());
	}

	BOOL ResourceManager::LoadResource(MarkupNode Root)
	{
		if( !Root.IsValid() ) return FALSE;

		LPCTSTR pstrClass = NULL;
		int nAttributes = 0;
		LPCTSTR pstrName = NULL;
		LPCTSTR pstrValue = NULL;
		LPTSTR pstr = NULL;

		//加载图片资源
		LPCTSTR pstrId = NULL;
		LPCTSTR pstrPath = NULL;
		for( MarkupNode node = Root.GetChild() ; node.IsValid(); node = node.GetSibling() ) 
		{
			pstrClass = node.GetName();
			MarkupNode ChildNode = node.GetChild();
			if(ChildNode.IsValid()) LoadResource(node);
			else if ((_tcsicmp(pstrClass,_T("Image")) == 0) && node.HasAttributes())
			{
				//加载图片资源
				nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) 
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = node.GetAttributeValue(i);

					if( _tcsicmp(pstrName, _T("id")) == 0 ) 
					{
						pstrId = pstrValue;
					}
					else if( _tcsicmp(pstrName, _T("path")) == 0 ) 
					{
						pstrPath = pstrValue;
					}
				}
				if( pstrId == NULL ||  pstrPath == NULL) continue;
				std::wstring * pstrFind = static_cast<std::wstring *>(m_mImageHashMap.Find(pstrId));
				if(pstrFind != NULL) continue;
				m_mImageHashMap.Insert(pstrId, (LPVOID)new std::wstring(pstrPath));
			}
			else if( _tcsicmp(pstrClass,_T("Xml")) == 0 && node.HasAttributes()) {
				//加载XML配置文件
				nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) 
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = node.GetAttributeValue(i);

					if( _tcsicmp(pstrName, _T("id")) == 0 ) 
					{
						pstrId = pstrValue;
					}
					else if( _tcsicmp(pstrName, _T("path")) == 0 ) 
					{
						pstrPath = pstrValue;
					}
				}
				if( pstrId == NULL ||  pstrPath == NULL) continue;
				std::wstring * pstrFind = static_cast<std::wstring *>(m_mXmlHashMap.Find(pstrId));
				if(pstrFind != NULL) continue;
				m_mXmlHashMap.Insert(pstrId, (LPVOID)new std::wstring(pstrPath));
			}
			else continue;
		}
		return TRUE;
	}

	std::wstring ResourceManager::GetImagePath(std::wstring lpstrId)
	{
		std::wstring * lpStr = static_cast<std::wstring *>(m_mImageHashMap.Find(lpstrId));
		return lpStr == NULL? L"":lpStr->c_str();
	}

	std::wstring ResourceManager::GetXmlPath(std::wstring lpstrId)
	{
		std::wstring * lpStr = static_cast<std::wstring *>(m_mXmlHashMap.Find(lpstrId));
		return lpStr == NULL? L"":lpStr->c_str();
	}

	void ResourceManager::ResetResourceMap()
	{
		std::wstring* lpStr;
		for( int i = 0; i< m_mImageHashMap.GetSize(); i++ )
		{
			std::wstring key = m_mImageHashMap.GetAt(i);
			if(!key.empty())
			{
				lpStr = static_cast<std::wstring *>(m_mImageHashMap.Find(key));
				delete lpStr;
			}
		}
		for( int i = 0; i< m_mXmlHashMap.GetSize(); i++ )
		{
			std::wstring key = m_mXmlHashMap.GetAt(i);
			if(!key.empty())
			{
				lpStr = static_cast<std::wstring *>(m_mXmlHashMap.Find(key));
				delete lpStr;
			}
		}
	}

	BOOL ResourceManager::LoadLanguage(std::wstring strXml)
	{
		auto pstrXml = strXml.c_str();
		Markup xml;
		if( *(pstrXml) == _T('<') ) {
			if( !xml.Load(pstrXml) ) return FALSE;
		}
		else {
			if( !xml.LoadFromFile(pstrXml) ) return FALSE;
		}
		MarkupNode Root = xml.GetRoot();
		if( !Root.IsValid() ) return FALSE;

		LPCTSTR pstrClass = NULL;
		int nAttributes = 0;
		LPCTSTR pstrName = NULL;
		LPCTSTR pstrValue = NULL;
		LPTSTR pstr = NULL;

		//加载图片资源
		LPCTSTR pstrId = NULL;
		LPCTSTR pstrText = NULL;
		for( MarkupNode node = Root.GetChild() ; node.IsValid(); node = node.GetSibling() ) 
		{
			pstrClass = node.GetName();
			if ((_tcsicmp(pstrClass,_T("Text")) == 0) && node.HasAttributes())
			{
				//加载图片资源
				nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) 
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = node.GetAttributeValue(i);

					if( _tcsicmp(pstrName, _T("id")) == 0 ) 
					{
						pstrId = pstrValue;
					}
					else if( _tcsicmp(pstrName, _T("value")) == 0 ) 
					{
						pstrText = pstrValue;
					}
				}
				if( pstrId == NULL ||  pstrText == NULL) continue;

				std::wstring *lpstrFind = static_cast<std::wstring *>(m_mTextResourceHashMap.Find(pstrId));
				if(lpstrFind != NULL) {
					lpstrFind->assign(pstrText);
				}
				else {
					m_mTextResourceHashMap.Insert(pstrId, (LPVOID)new std::wstring(pstrText));
				}
			}
			else continue;
		}

		return TRUE;
	}

	std::wstring ResourceManager::GetText(std::wstring lpstrId, std::wstring lpstrType)
	{
		if(lpstrId.empty()) return _T("");

		std::wstring *lpstrFind = static_cast<std::wstring *>(m_mTextResourceHashMap.Find(lpstrId));
		if (lpstrFind == NULL && m_pQuerypInterface)
		{
			lpstrFind = new std::wstring(m_pQuerypInterface->QueryControlText(lpstrId, lpstrType));
			m_mTextResourceHashMap.Insert(lpstrId, (LPVOID)lpstrFind);
		}
		return lpstrFind == NULL ? lpstrId : *lpstrFind;
	}

	void ResourceManager::ReloadText()
	{
		if(m_pQuerypInterface == NULL) return;
		//重载文字描述
		LPCTSTR lpstrId = NULL;
		std::wstring lpstrText;
		for( int i = 0; i < m_mTextResourceHashMap.GetSize(); i++ )
		{
			lpstrId = m_mTextResourceHashMap.GetAt(i).c_str();
			if (lpstrId == NULL) continue;
			lpstrText = m_pQuerypInterface->QueryControlText(lpstrId, NULL);
			if(!lpstrText.empty()) {
				std::wstring * lpStr = static_cast<std::wstring *>(m_mTextResourceHashMap.Find(lpstrId));
				lpStr->assign(lpstrText);
			}
		}
	}
	void ResourceManager::ResetTextMap()
	{
		std::wstring * lpStr;
		for( int i = 0; i< m_mTextResourceHashMap.GetSize(); i++ )
		{
			std::wstring key = m_mTextResourceHashMap.GetAt(i);
			if(!key.empty()){
				lpStr = static_cast<std::wstring *>(m_mTextResourceHashMap.Find(key));
				delete lpStr;
			}
		}
	}

	
} // namespace DuiLib