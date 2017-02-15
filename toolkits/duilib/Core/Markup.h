#ifndef __UIMARKUP_H__
#define __UIMARKUP_H__

#pragma once

namespace ui {

	enum
	{
		XMLFILE_ENCODING_UTF8 = 0,
		XMLFILE_ENCODING_UNICODE = 1,
		XMLFILE_ENCODING_ASNI = 2,
	};

	class Markup;
	class MarkupNode;


	class UILIB_API Markup
	{
		friend class MarkupNode;
	public:
		Markup(std::wstring pstrXML = L"");
		~Markup();

		bool Load(std::wstring pstrXML);
		bool LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding = XMLFILE_ENCODING_UTF8);
		bool LoadFromFile(std::wstring pstrFilename, int encoding = XMLFILE_ENCODING_UTF8);
		void Release();
		bool IsValid() const;

		void SetPreserveWhitespace(bool bPreserve = true);
		std::wstring GetLastErrorMessage() const;
		std::wstring GetLastErrorLocation() const;

		MarkupNode GetRoot();

	private:
		typedef struct tagXMLELEMENT
		{
			ULONG iStart;
			ULONG iChild;
			ULONG iNext;
			ULONG iParent;
			ULONG iData;
		} XMLELEMENT;

		LPTSTR m_pstrXML;
		XMLELEMENT* m_pElements;
		ULONG m_nElements;
		ULONG m_nReservedElements;
		TCHAR m_szErrorMsg[100];
		TCHAR m_szErrorXML[50];
		bool m_bPreserveWhitespace;

	private:
		bool _Parse();
		bool _Parse(LPTSTR& pstrText, ULONG iParent);
		XMLELEMENT* _ReserveElement();
		inline void _SkipWhitespace(LPTSTR& pstr) const;
		inline void _SkipWhitespace(LPCTSTR& pstr) const;
		inline void _SkipIdentifier(LPTSTR& pstr) const;
		inline void _SkipIdentifier(LPCTSTR& pstr) const;
		bool _ParseData(LPTSTR& pstrText, LPTSTR& pstrData, char cEnd);
		void _ParseMetaChar(LPTSTR& pstrText, LPTSTR& pstrDest);
		bool _ParseAttributes(LPTSTR& pstrText);
		bool _Failed(LPCTSTR pstrError, LPCTSTR pstrLocation = L"");
	};


	class UILIB_API MarkupNode
	{
		friend class Markup;
	private:
		MarkupNode();
		MarkupNode(Markup* pOwner, int iPos);

	public:
		bool IsValid() const;

		MarkupNode GetParent();
		MarkupNode GetSibling();
		MarkupNode GetChild();
		MarkupNode GetChild(LPCTSTR pstrName);

		bool HasSiblings() const;
		bool HasChildren() const;
		LPCTSTR GetName() const;
		LPCTSTR GetValue() const;

		bool HasAttributes();
		bool HasAttribute(LPCTSTR pstrName);
		int GetAttributeCount();
		LPCTSTR GetAttributeName(int iIndex);
		LPCTSTR GetAttributeValue(int iIndex);
		LPCTSTR GetAttributeValue(LPCTSTR pstrName);
		bool GetAttributeValue(int iIndex, LPTSTR pstrValue, SIZE_T cchMax);
		bool GetAttributeValue(LPCTSTR pstrName, LPTSTR pstrValue, SIZE_T cchMax);

	private:
		void _MapAttributes();

		enum { MAX_XML_ATTRIBUTES = 64 };

		typedef struct
		{
			ULONG iName;
			ULONG iValue;
		} XMLATTRIBUTE;

		int pos_;
		int attribute_index_;
		XMLATTRIBUTE attributes_[MAX_XML_ATTRIBUTES];
		Markup* owner_;
	};

} // namespace DuiLib

#endif // __UIMARKUP_H__
