#pragma once
#include "../../pch.h"

namespace AESLogger
{
	bool IsStringDecimal(std::string str);
	bool IsStringHexadecimal(std::string str);
	std::string ToHex(void* address, const std::string notation = "0x");
	std::string ToHex(uint64_t decimal, size_t width, const std::string notation = "0x");

	uintptr_t FindPattern(HMODULE hModule, const uint8_t* bytePattern, const std::string& maskStr, size_t offset = 0);
	uintptr_t FindPattern(const uint8_t* bytePattern, const std::string& maskStr, size_t offset = 0);
	uintptr_t FindPatternStr(const std::string& strPattern, const std::string& maskStr, size_t offset = 0);
}