#ifndef BASE_URL_ENCODE_H__
#define BASE_URL_ENCODE_H__

#include <string>
#include "base/base_export.h"

namespace nbase
{
BASE_EXPORT std::string UrlEncode(const std::string &src);
BASE_EXPORT std::string UrlDecode(const std::string &src);
} // namespace nbase

#endif // BASE_URL_ENCODE_H__