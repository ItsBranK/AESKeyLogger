#pragma once
#include "../../pch.h"

namespace AESLogger
{
	bool IsStringBase64(const std::string& str);
    std::string Base64(uint8_t* bytes, size_t byteLength);
    std::string Base64(std::string str);
    void Base64Inline(std::string& str);
    std::string Base64Decode(const std::string& str);
    void Base64DecodeInline(std::string& str);
}