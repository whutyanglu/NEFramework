#include "StdAfx.h"
namespace ui {
///////////////////////////////////////////////////////////////////////////////////////
//
//
//
MarkupNode::MarkupNode() : owner_(NULL)
{
}

MarkupNode::MarkupNode(Markup* pOwner, int iPos) : owner_(pOwner), pos_(iPos), attribute_index_(0)
{
}

MarkupNode MarkupNode::GetSibling()
{
    if( owner_ == NULL ) return MarkupNode();
    ULONG iPos = owner_->m_pElements[pos_].iNext;
    if( iPos == 0 ) return MarkupNode();
    return MarkupNode(owner_, iPos);
}

bool MarkupNode::HasSiblings() const
{
    if( owner_ == NULL ) return false;
    ULONG iPos = owner_->m_pElements[pos_].iNext;
    return iPos > 0;
}

MarkupNode MarkupNode::GetChild()
{
    if( owner_ == NULL ) return MarkupNode();
    ULONG iPos = owner_->m_pElements[pos_].iChild;
    if( iPos == 0 ) return MarkupNode();
    return MarkupNode(owner_, iPos);
}

MarkupNode MarkupNode::GetChild(LPCTSTR pstrName)
{
    if( owner_ == NULL ) return MarkupNode();
    ULONG iPos = owner_->m_pElements[pos_].iChild;
    while( iPos != 0 ) {
        if( _tcsicmp(owner_->m_pstrXML + owner_->m_pElements[iPos].iStart, pstrName) == 0 ) {
            return MarkupNode(owner_, iPos);
        }
        iPos = owner_->m_pElements[iPos].iNext;
    }
    return MarkupNode();
}

bool MarkupNode::HasChildren() const
{
    if( owner_ == NULL ) return false;
    return owner_->m_pElements[pos_].iChild != 0;
}

MarkupNode MarkupNode::GetParent()
{
    if( owner_ == NULL ) return MarkupNode();
    ULONG iPos = owner_->m_pElements[pos_].iParent;
    if( iPos == 0 ) return MarkupNode();
    return MarkupNode(owner_, iPos);
}

bool MarkupNode::IsValid() const
{
    return owner_ != NULL;
}

LPCTSTR MarkupNode::GetName() const
{
    if( owner_ == NULL ) return NULL;
    return owner_->m_pstrXML + owner_->m_pElements[pos_].iStart;
}

LPCTSTR MarkupNode::GetValue() const
{
    if( owner_ == NULL ) return NULL;
    return owner_->m_pstrXML + owner_->m_pElements[pos_].iData;
}

LPCTSTR MarkupNode::GetAttributeName(int iIndex)
{
    if( owner_ == NULL ) return NULL;
    if( attribute_index_ == 0 ) _MapAttributes();
    if( iIndex < 0 || iIndex >= attribute_index_ ) return _T("");
    return owner_->m_pstrXML + attributes_[iIndex].iName;
}

LPCTSTR MarkupNode::GetAttributeValue(int iIndex)
{
    if( owner_ == NULL ) return NULL;
    if( attribute_index_ == 0 ) _MapAttributes();
    if( iIndex < 0 || iIndex >= attribute_index_ ) return _T("");
    return owner_->m_pstrXML + attributes_[iIndex].iValue;
}

LPCTSTR MarkupNode::GetAttributeValue(LPCTSTR pstrName)
{
    if( owner_ == NULL ) return NULL;
    if( attribute_index_ == 0 ) _MapAttributes();
    for( int i = 0; i < attribute_index_; i++ ) {
		if (_tcsicmp(owner_->m_pstrXML + attributes_[i].iName, pstrName) == 0) {
			return owner_->m_pstrXML + attributes_[i].iValue;
		}
    }
    return NULL;
}

bool MarkupNode::GetAttributeValue(int iIndex, LPTSTR pstrValue, SIZE_T cchMax)
{
    if( owner_ == NULL ) return false;
    if( attribute_index_ == 0 ) _MapAttributes();
    if( iIndex < 0 || iIndex >= attribute_index_ ) return false;
    _tcsncpy(pstrValue, owner_->m_pstrXML + attributes_[iIndex].iValue, cchMax);
    return true;
}

bool MarkupNode::GetAttributeValue(LPCTSTR pstrName, LPTSTR pstrValue, SIZE_T cchMax)
{
    if( owner_ == NULL ) return false;
    if( attribute_index_ == 0 ) _MapAttributes();
    for( int i = 0; i < attribute_index_; i++ ) {
        if( _tcsicmp(owner_->m_pstrXML + attributes_[i].iName, pstrName) == 0 ) {
            _tcsncpy(pstrValue, owner_->m_pstrXML + attributes_[i].iValue, cchMax);
            return true;
        }
    }
    return false;
}

int MarkupNode::GetAttributeCount()
{
    if( owner_ == NULL ) return 0;
    if( attribute_index_ == 0 ) _MapAttributes();
    return attribute_index_;
}

bool MarkupNode::HasAttributes()
{
    if( owner_ == NULL ) return false;
    if( attribute_index_ == 0 ) _MapAttributes();
    return attribute_index_ > 0;
}

bool MarkupNode::HasAttribute(LPCTSTR pstrName)
{
    if( owner_ == NULL ) return false;
    if( attribute_index_ == 0 ) _MapAttributes();
    for( int i = 0; i < attribute_index_; i++ ) {
        if( _tcsicmp(owner_->m_pstrXML + attributes_[i].iName, pstrName) == 0 ) return true;
    }
    return false;
}

void MarkupNode::_MapAttributes()
{
    attribute_index_ = 0;
    LPTSTR pstr = owner_->m_pstrXML + owner_->m_pElements[pos_].iStart;
    LPTSTR pstrEnd = owner_->m_pstrXML + owner_->m_pElements[pos_].iData;
    pstr += _tcslen(pstr) + 1;
    while( pstr < pstrEnd ) {
        owner_->_SkipWhitespace(pstr);
        attributes_[attribute_index_].iName = pstr - owner_->m_pstrXML;
        pstr += _tcslen(pstr) + 1;
        owner_->_SkipWhitespace(pstr);
        if( *pstr++ != _T('\"') ) return; // if( *pstr != _T('\"') ) { pstr = ::CharNext(pstr); return; }
        
        attributes_[attribute_index_++].iValue = pstr - owner_->m_pstrXML;
        if( attribute_index_ >= MAX_XML_ATTRIBUTES ) return;
        pstr += _tcslen(pstr) + 1;
    }
}


///////////////////////////////////////////////////////////////////////////////////////
//
//
//

Markup::Markup(std::wstring pstrXML)
{
    m_pstrXML = NULL;
    m_pElements = NULL;
    m_nElements = 0;
    m_bPreserveWhitespace = true;
    if( !pstrXML.empty() ) Load(pstrXML);
}

Markup::~Markup()
{
    Release();
}

bool Markup::IsValid() const
{
    return m_pElements != NULL;
}

void Markup::SetPreserveWhitespace(bool bPreserve)
{
    m_bPreserveWhitespace = bPreserve;
}

bool Markup::Load(std::wstring pstrXML)
{
    Release();
    SIZE_T cchLen = _tcslen(pstrXML.c_str()) + 1;
    m_pstrXML = static_cast<LPTSTR>(malloc(cchLen * sizeof(TCHAR)));
    ::CopyMemory(m_pstrXML, pstrXML.c_str(), cchLen * sizeof(TCHAR));
    bool bRes = _Parse();
    if( !bRes ) Release();
    return bRes;
}

bool Markup::LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding)
{
#ifdef _UNICODE
    if (encoding == XMLFILE_ENCODING_UTF8)
    {
        if( dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF ) 
        {
            pByte += 3; dwSize -= 3;
        }
        DWORD nWide = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

        m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
        ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
        m_pstrXML[nWide] = _T('\0');
    }
    else if (encoding == XMLFILE_ENCODING_ASNI)
    {
        DWORD nWide = ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

        m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
        ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
        m_pstrXML[nWide] = _T('\0');
    }
    else
    {
        if ( dwSize >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
        {
            dwSize = dwSize / 2 - 1;

            if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
            {
                pByte += 2;

                for ( DWORD nSwap = 0 ; nSwap < dwSize ; nSwap ++ )
                {
                    register CHAR nTemp = pByte[ ( nSwap << 1 ) + 0 ];
                    pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
                    pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
                }
            }
            else
            {
                pByte += 2;
            }

            m_pstrXML = static_cast<LPTSTR>(malloc((dwSize + 1)*sizeof(TCHAR)));
            ::CopyMemory( m_pstrXML, pByte, dwSize * sizeof(TCHAR) );
            m_pstrXML[dwSize] = _T('\0');

            pByte -= 2;
        }
    }
#else // !_UNICODE
    if (encoding == XMLFILE_ENCODING_UTF8)
    {
        if( dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF ) 
        {
            pByte += 3; dwSize -= 3;
        }
        DWORD nWide = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

        LPWSTR w_str = static_cast<LPWSTR>(malloc((nWide + 1)*sizeof(WCHAR)));
        ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, w_str, nWide );
        w_str[nWide] = L'\0';

        DWORD wide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, NULL, 0, NULL, NULL);

        m_pstrXML = static_cast<LPTSTR>(malloc((wide + 1)*sizeof(TCHAR)));
        ::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)w_str, nWide, m_pstrXML, wide, NULL, NULL);
        m_pstrXML[wide] = _T('\0');

        free(w_str);
    }
    else if (encoding == XMLFILE_ENCODING_UNICODE)
    {
        if ( dwSize >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
        {
            dwSize = dwSize / 2 - 1;

            if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
            {
                pByte += 2;

                for ( DWORD nSwap = 0 ; nSwap < dwSize ; nSwap ++ )
                {
                    register CHAR nTemp = pByte[ ( nSwap << 1 ) + 0 ];
                    pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
                    pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
                }
            }
            else
            {
                pByte += 2;
            }

            DWORD nWide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pByte, dwSize, NULL, 0, NULL, NULL);
            m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
            ::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)pByte, dwSize, m_pstrXML, nWide, NULL, NULL);
            m_pstrXML[nWide] = _T('\0');

            pByte -= 2;
        }
    }
    else
    {
        m_pstrXML = static_cast<LPTSTR>(malloc((dwSize + 1)*sizeof(TCHAR)));
        ::CopyMemory( m_pstrXML, pByte, dwSize * sizeof(TCHAR) );
        m_pstrXML[dwSize] = _T('\0');
    }
#endif // _UNICODE

    bool bRes = _Parse();
    if( !bRes ) Release();
    return bRes;
}

bool Markup::LoadFromFile(std::wstring pstrFilename, int encoding)
{
    Release();

	std::wstring sFile = PaintManager::GetResourcePath();
    if( PaintManager::GetResourceZip().empty() ) {
        sFile += pstrFilename;
        HANDLE hFile = ::CreateFile(sFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if( hFile == INVALID_HANDLE_VALUE ) return _Failed(_T("Error opening file"));
        DWORD dwSize = ::GetFileSize(hFile, NULL);
        if( dwSize == 0 ) return _Failed(_T("File is empty"));
        if ( dwSize > 4096*1024 ) return _Failed(_T("File too large"));

        DWORD dwRead = 0;
        BYTE* pByte = new BYTE[ dwSize ];
        ::ReadFile( hFile, pByte, dwSize, &dwRead, NULL );
        ::CloseHandle( hFile );
        if( dwRead != dwSize ) {
            delete[] pByte;
			pByte = NULL;
            Release();
            return _Failed(_T("Could not read file"));
        }

        bool ret = LoadFromMem(pByte, dwSize, encoding);
        delete[] pByte;
		pByte = NULL;

        return ret;
    }
    else {
		sFile += PaintManager::GetResourceZip();
		HZIP hz = NULL;
        if( PaintManager::IsCachedResourceZip() ) hz = (HZIP)PaintManager::GetResourceZipHandle();
        else {
			std::wstring sFilePwd = PaintManager::GetResourceZipPwd();
#ifdef UNICODE
			char* pwd = w2a((wchar_t*)sFilePwd.c_str());
			hz = OpenZip(sFile.c_str(), pwd);
			if(pwd) delete[] pwd;
#else
            hz = OpenZip(sFile.GetData(), sFilePwd.GetData());
#endif
		}
        if( hz == NULL ) return _Failed(_T("Error opening zip file"));
        ZIPENTRY ze; 
        int i; 
        if( FindZipItem(hz, pstrFilename.c_str(), true, &i, &ze) != 0 ) return _Failed(_T("Could not find ziped file"));
        DWORD dwSize = ze.unc_size;
        if( dwSize == 0 ) return _Failed(_T("File is empty"));
        if ( dwSize > 4096*1024 ) return _Failed(_T("File too large"));
        BYTE* pByte = new BYTE[ dwSize ];
        int res = UnzipItem(hz, i, pByte, dwSize);
        if( res != 0x00000000 && res != 0x00000600) {
            delete[] pByte;
            if( !PaintManager::IsCachedResourceZip() ) CloseZip(hz);
            return _Failed(_T("Could not unzip file"));
        }
        if( !PaintManager::IsCachedResourceZip() ) CloseZip(hz);
        bool ret = LoadFromMem(pByte, dwSize, encoding);
        delete[] pByte;
		pByte = NULL;
        return ret;
    }
}

void Markup::Release()
{
    if( m_pstrXML != NULL ) free(m_pstrXML);
    if( m_pElements != NULL ) free(m_pElements);
    m_pstrXML = NULL;
    m_pElements = NULL;
    m_nElements;
}

std::wstring Markup::GetLastErrorMessage() const
{
	return m_szErrorMsg;
}

std::wstring Markup::GetLastErrorLocation() const
{
	return m_szErrorXML;
}

MarkupNode Markup::GetRoot()
{
    if( m_nElements == 0 ) return MarkupNode();
    return MarkupNode(this, 1);
}

bool Markup::_Parse()
{
    _ReserveElement(); // Reserve index 0 for errors
    ::ZeroMemory(m_szErrorMsg, sizeof(m_szErrorMsg));
    ::ZeroMemory(m_szErrorXML, sizeof(m_szErrorXML));
    LPTSTR pstrXML = m_pstrXML;
    return _Parse(pstrXML, 0);
}

bool Markup::_Parse(LPTSTR& pstrText, ULONG iParent)
{
	LPTSTR pstrWatch =  m_pstrXML;
    _SkipWhitespace(pstrText);
    ULONG iPrevious = 0;
    for( ; ; ) 
    {
        if( *pstrText == _T('\0') && iParent <= 1 ) return true;
        _SkipWhitespace(pstrText);
        if( *pstrText != _T('<') ) return _Failed(_T("Expected start tag"), pstrText);
        if( pstrText[1] == _T('/') ) return true;
        *pstrText++ = _T('\0');
        _SkipWhitespace(pstrText);
        // Skip comment or processing directive
        if( *pstrText == _T('!') || *pstrText == _T('?') ) {
            TCHAR ch = *pstrText;
            if( *pstrText == _T('!') ) ch = _T('-');
            while( *pstrText != _T('\0') && !(*pstrText == ch && *(pstrText + 1) == _T('>')) ) pstrText = ::CharNext(pstrText);
            if( *pstrText != _T('\0') ) pstrText += 2;
            _SkipWhitespace(pstrText);
            continue;
        }
        _SkipWhitespace(pstrText);
        // Fill out element structure
        XMLELEMENT* pEl = _ReserveElement();
        ULONG iPos = pEl - m_pElements;
        pEl->iStart = pstrText - m_pstrXML;
        pEl->iParent = iParent;
        pEl->iNext = pEl->iChild = 0;
        if( iPrevious != 0 ) m_pElements[iPrevious].iNext = iPos;
        else if( iParent > 0 ) m_pElements[iParent].iChild = iPos;
        iPrevious = iPos;
        // Parse name
       LPCTSTR pstrName = pstrText;
        _SkipIdentifier(pstrText);
        LPTSTR pstrNameEnd = pstrText;
        if( *pstrText == _T('\0') ) return _Failed(_T("Error parsing element name"), pstrText);
        // Parse attributes
        if( !_ParseAttributes(pstrText) ) return false;
        _SkipWhitespace(pstrText);
        if( pstrText[0] == _T('/') && pstrText[1] == _T('>') )
        {
            pEl->iData = pstrText - m_pstrXML;
            *pstrText = _T('\0');
            pstrText += 2;
        }
        else
        {
            if( *pstrText != _T('>') ) return _Failed(_T("Expected start-tag closing"), pstrText);
            // Parse node data
            pEl->iData = ++pstrText - m_pstrXML;
            LPTSTR pstrDest = pstrText;
            if( !_ParseData(pstrText, pstrDest, _T('<')) ) return false;
            // Determine type of next element
            if( *pstrText == _T('\0') && iParent <= 1 ) return true;
            if( *pstrText != _T('<') ) return _Failed(_T("Expected end-tag start"), pstrText);
            if( pstrText[0] == _T('<') && pstrText[1] != _T('/') ) 
            {
                if( !_Parse(pstrText, iPos) ) return false;
            }
            if( pstrText[0] == _T('<') && pstrText[1] == _T('/') ) 
            {
                *pstrDest = _T('\0');
                *pstrText = _T('\0');
                pstrText += 2;
                _SkipWhitespace(pstrText);
                SIZE_T cchName = pstrNameEnd - pstrName;
                if( _tcsncmp(pstrText, pstrName, cchName) != 0 ) return _Failed(_T("Unmatched closing tag"), pstrText);
                pstrText += cchName;
                _SkipWhitespace(pstrText);
                if( *pstrText++ != _T('>') ) return _Failed(_T("Unmatched closing tag"), pstrText);
            }
        }
        *pstrNameEnd = _T('\0');
        _SkipWhitespace(pstrText);
    }
}

Markup::XMLELEMENT* Markup::_ReserveElement()
{
    if( m_nElements == 0 ) m_nReservedElements = 0;
    if( m_nElements >= m_nReservedElements ) {
        m_nReservedElements += (m_nReservedElements / 2) + 500;
        m_pElements = static_cast<XMLELEMENT*>(realloc(m_pElements, m_nReservedElements * sizeof(XMLELEMENT)));
    }
    return &m_pElements[m_nElements++];
}

void Markup::_SkipWhitespace(LPCTSTR& pstr) const
{
	while (*pstr > _T('\0') && *pstr <= _T(' ')) pstr = ::CharNext(pstr);
}

void Markup::_SkipWhitespace(LPTSTR& pstr) const
{
	while (*pstr > _T('\0') && *pstr <= _T(' ')) pstr = ::CharNext(pstr);
}

void Markup::_SkipIdentifier(LPCTSTR& pstr) const
{
	// 属性只能用英文，所以这样处理没有问题
	while (*pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || _istalnum(*pstr))) pstr = ::CharNext(pstr);
}

void Markup::_SkipIdentifier(LPTSTR& pstr) const
{
	// 属性只能用英文，所以这样处理没有问题
	while (*pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || _istalnum(*pstr))) pstr = ::CharNext(pstr);
}

bool Markup::_ParseAttributes(LPTSTR& pstrText)
{   
	// 无属性
	LPTSTR pstrIdentifier = pstrText;
	if( *pstrIdentifier == _T('/') && *++pstrIdentifier == _T('>') ) return true;
    if( *pstrText == _T('>') ) return true;
    *pstrText++ = _T('\0');
    _SkipWhitespace(pstrText);
    while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('/') ) {
        _SkipIdentifier(pstrText);
        LPTSTR pstrIdentifierEnd = pstrText;
        _SkipWhitespace(pstrText);
        if( *pstrText != _T('=') ) return _Failed(_T("Error while parsing attributes"), pstrText);
        *pstrText++ = _T(' ');
        *pstrIdentifierEnd = _T('\0');
        _SkipWhitespace(pstrText);
        if( *pstrText++ != _T('\"') ) return _Failed(_T("Expected attribute value"), pstrText);
        LPTSTR pstrDest = pstrText;
        if( !_ParseData(pstrText, pstrDest, _T('\"')) ) return false;
        if( *pstrText == _T('\0') ) return _Failed(_T("Error while parsing attribute string"), pstrText);
        *pstrDest = _T('\0');
        if( pstrText != pstrDest ) *pstrText = _T(' ');
        pstrText++;
        _SkipWhitespace(pstrText);
    }
    return true;
}

bool Markup::_ParseData(LPTSTR& pstrText, LPTSTR& pstrDest, char cEnd)
{
    while( *pstrText != _T('\0') && *pstrText != cEnd ) {
		if( *pstrText == _T('&') ) {
			while( *pstrText == _T('&') ) {
				_ParseMetaChar(++pstrText, pstrDest);
			}
			if (*pstrText == cEnd)
				break;
		}

        if( *pstrText == _T(' ') ) {
            *pstrDest++ = *pstrText++;
            if( !m_bPreserveWhitespace ) _SkipWhitespace(pstrText);
        }
        else {
            LPTSTR pstrTemp = ::CharNext(pstrText);
            while( pstrText < pstrTemp) {
                *pstrDest++ = *pstrText++;
            }
        }
    }
    // Make sure that MapAttributes() works correctly when it parses
    // over a value that has been transformed.
    LPTSTR pstrFill = pstrDest + 1;
    while( pstrFill < pstrText ) *pstrFill++ = _T(' ');
    return true;
}

void Markup::_ParseMetaChar(LPTSTR& pstrText, LPTSTR& pstrDest)
{
    if( pstrText[0] == _T('a') && pstrText[1] == _T('m') && pstrText[2] == _T('p') && pstrText[3] == _T(';') ) {
        *pstrDest++ = _T('&');
        pstrText += 4;
    }
    else if( pstrText[0] == _T('l') && pstrText[1] == _T('t') && pstrText[2] == _T(';') ) {
        *pstrDest++ = _T('<');
        pstrText += 3;
    }
    else if( pstrText[0] == _T('g') && pstrText[1] == _T('t') && pstrText[2] == _T(';') ) {
        *pstrDest++ = _T('>');
        pstrText += 3;
    }
    else if( pstrText[0] == _T('q') && pstrText[1] == _T('u') && pstrText[2] == _T('o') && pstrText[3] == _T('t') && pstrText[4] == _T(';') ) {
        *pstrDest++ = _T('\"');
        pstrText += 5;
    }
    else if( pstrText[0] == _T('a') && pstrText[1] == _T('p') && pstrText[2] == _T('o') && pstrText[3] == _T('s') && pstrText[4] == _T(';') ) {
        *pstrDest++ = _T('\'');
        pstrText += 5;
    }
    else {
        *pstrDest++ = _T('&');
    }
}

bool Markup::_Failed(LPCTSTR pstrError, LPCTSTR pstrLocation)
{
    // Register last error
    _tcsncpy(m_szErrorMsg, pstrError, (sizeof(m_szErrorMsg) / sizeof(m_szErrorMsg[0])) - 1);
    _tcsncpy(m_szErrorXML, pstrLocation, lengthof(m_szErrorXML) - 1);
    return false; // Always return 'false'
}

} // namespace DuiLib
