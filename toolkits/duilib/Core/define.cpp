#include "StdAfx.h"
#include "Define.h"

namespace ui 
{

TDrawInfo::TDrawInfo()
{
	Clear();
}

void TDrawInfo::Parse(std::wstring StrImage, std::wstring StrModify, PaintManager *paintManager)
{
	// 1、aaa.jpg
	// 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
	// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'
	sDrawString = StrImage;
	sDrawModify = StrModify;
	sImageName = StrImage;

	auto pStrImage = StrImage.c_str();
	auto pStrModify = StrModify.c_str();

	std::wstring sItem;
	std::wstring sValue;
	LPTSTR pstr = NULL;
	for (int i = 0; i < 2; ++i) {
		if (i == 1) pStrImage = pStrModify;
		if (!pStrImage) continue;
		while (*pStrImage != _T('\0')) {
			sItem.clear();
			sValue.clear();
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while (pStrImage < pstrTemp) {
					sItem += *pStrImage++;
				}
			}
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			if (*pStrImage++ != _T('=')) break;
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			if (*pStrImage++ != _T('\'')) break;
			while (*pStrImage != _T('\0') && *pStrImage != _T('\'')) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while (pStrImage < pstrTemp) {
					sValue += *pStrImage++;
				}
			}
			if (*pStrImage++ != _T('\'')) break;
			if (!sValue.empty()) {
				if (sItem == _T("file") || sItem == _T("res")) {
					sImageName = sValue;
				}
				else if (sItem == _T("restype")) {
					sResType = sValue;
				}
				else if (sItem == _T("dest")) {
					rcDest.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
					rcDest.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					rcDest.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					rcDest.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
					paintManager->GetDPIObj()->Scale(&rcDest);
				}
				else if (sItem == _T("source")) {
					rcSource.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
					rcSource.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					rcSource.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					rcSource.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
					paintManager->GetDPIObj()->Scale(&rcSource);
				}
				else if (sItem == _T("corner")) {
					rcCorner.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
					rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
					paintManager->GetDPIObj()->Scale(&rcCorner);
				}
				else if (sItem == _T("mask")) {
					if (sValue[0] == _T('#')) dwMask = _tcstoul(sValue.c_str() + 1, &pstr, 16);
					else dwMask = _tcstoul(sValue.c_str(), &pstr, 16);
				}
				else if (sItem == _T("fade")) {
					uFade = (BYTE)_tcstoul(sValue.c_str(), &pstr, 10);
				}
				else if (sItem == _T("hole")) {
					bHole = (_tcsicmp(sValue.c_str(), _T("true")) == 0);
				}
				else if (sItem == _T("xtiled")) {
					bTiledX = (_tcsicmp(sValue.c_str(), _T("true")) == 0);
				}
				else if (sItem == _T("ytiled")) {
					bTiledY = (_tcsicmp(sValue.c_str(), _T("true")) == 0);
				}
				else if (sItem == _T("hsl")) {
					bHSL = (_tcsicmp(sValue.c_str(), _T("true")) == 0);
				}
			}
			if (*pStrImage++ != _T(' ')) break;
		}
	}

	// 调整DPI资源
	if (paintManager->GetDPIObj()->GetScale() != 100) {
		std::wstring sScale;
		sScale = nbase::StringPrintf(L"@%d.", paintManager->GetDPIObj()->GetScale());
		nbase::StringReplaceAll(L".", sScale, sImageName);
	}
}
void TDrawInfo::Parse(std::wstring strImage, std::wstring strModify)
{
	sDrawString = strImage;
	sDrawModify = strModify;
	sImageName = strImage;

	auto pStrImage = strImage.c_str();
	auto pStrModify = strModify.c_str();

	std::wstring sItem;
	std::wstring sValue;
	LPTSTR pstr = NULL;
	for (int i = 0; i < 2; ++i) {
		if (i == 1) pStrImage = pStrModify;
		if (!pStrImage) continue;
		while (*pStrImage != _T('\0')) {
			sItem.clear();
			sValue.clear();
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while (pStrImage < pstrTemp) {
					sItem += *pStrImage++;
				}
			}
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			if (*pStrImage++ != _T('=')) break;
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			if (*pStrImage++ != _T('\'')) break;
			while (*pStrImage != _T('\0') && *pStrImage != _T('\'')) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while (pStrImage < pstrTemp) {
					sValue += *pStrImage++;
				}
			}
			if (*pStrImage++ != _T('\'')) break;
			if (!sValue.empty()) {
				if (sItem == _T("file") || sItem == _T("res")) {
					sImageName = sValue;
				}
				else if (sItem == _T("restype")) {
					sResType = sValue;
				}
				else if (sItem == _T("dest")) {
					rcDest.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
					rcDest.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					rcDest.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					rcDest.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if (sItem == _T("source")) {
					rcSource.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
					rcSource.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					rcSource.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					rcSource.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if (sItem == _T("corner")) {
					rcCorner.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
					rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if (sItem == _T("mask")) {
					if (sValue[0] == _T('#')) dwMask = _tcstoul(sValue.c_str() + 1, &pstr, 16);
					else dwMask = _tcstoul(sValue.c_str(), &pstr, 16);
				}
				else if (sItem == _T("fade")) {
					uFade = (BYTE)_tcstoul(sValue.c_str(), &pstr, 10);
				}
				else if (sItem == _T("hole")) {
					bHole = (_tcsicmp(sValue.c_str(), _T("true")) == 0);
				}
				else if (sItem == _T("xtiled")) {
					bTiledX = (_tcsicmp(sValue.c_str(), _T("true")) == 0);
				}
				else if (sItem == _T("ytiled")) {
					bTiledY = (_tcsicmp(sValue.c_str(), _T("true")) == 0);
				}
				else if (sItem == _T("hsl")) {
					bHSL = (_tcsicmp(sValue.c_str(), _T("true")) == 0);
				}
			}
			if (*pStrImage++ != _T(' ')) break;
		}
	}
}

void TDrawInfo::Clear()
{
	sDrawString.clear();
	sDrawModify.clear();
	sImageName.clear();

	memset(&rcDest, 0, sizeof(RECT));
	memset(&rcSource, 0, sizeof(RECT));
	memset(&rcCorner, 0, sizeof(RECT));
	dwMask = 0;
	uFade = 255;
	bHole = false;
	bTiledX = false;
	bTiledY = false;
	bHSL = false;
}

bool TDrawInfo::IsValidDest()
{
	if ((rcDest.bottom - rcDest.top > 0) && (rcDest.right - rcDest.left > 0)) {
		return true;
	}
	return false;
}

} // namespace ui