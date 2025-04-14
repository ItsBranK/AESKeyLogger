#include "FindPattern.hpp"

namespace AESLogger
{
    std::vector<std::string> Split(const std::string& str, char character)
    {
        std::vector<std::string> splitStrings;
        std::string currentWord;

        for (char c : str)
        {
            if (c == character)
            {
                if (!currentWord.empty())
                {
                    splitStrings.push_back(currentWord);
                }

                currentWord.clear();
            }
            else
            {
                currentWord += c;
            }
        }

        if (!currentWord.empty())
        {
            splitStrings.push_back(currentWord);
        }

        return splitStrings;
    }

    uint64_t ToDecimal(std::string hexStr)
    {
        uint64_t decimal = 0;
        std::stringstream stream;
        stream << std::right << std::uppercase << std::hex << hexStr;
        stream >> decimal;
        return decimal;
    }

    bool IsStringDecimal(std::string str)
    {
        if (str.empty())
        {
            return false;
        }

        std::transform(str.begin(), str.end(), str.begin(), tolower);

        bool first = true;
        bool negative = false;
        bool found = false;

        for (char c : str)
        {
            if (first)
            {
                first = false;
                negative = (c == '-');
            }

            if (std::isdigit(c))
            {
                found = true;
            }
            else if (!negative)
            {
                return false;
            }
        }

        return found;
    }

    bool IsStringHexadecimal(std::string str)
    {
        if (str.empty())
        {
            return false;
        }

        std::transform(str.begin(), str.end(), str.begin(), tolower);

        bool first = true;
        bool negative = false;
        bool found = false;

        for (char c : str)
        {
            if (first)
            {
                first = false;
                negative = (c == '-');
            }

            if (std::isxdigit(c))
            {
                found = true;
            }
            else if (!negative)
            {
                return false;
            }
        }

        return found;
    }

    std::string ToHex(void* address, const std::string notation)
    {
        return ToHex(reinterpret_cast<uint64_t>(address), sizeof(uint64_t), notation);
    }

    std::string ToHex(uint64_t decimal, size_t width, const std::string notation)
    {
        std::ostringstream stream;
        stream << notation << std::setfill('0') << std::setw(width) << std::right << std::uppercase << std::hex << decimal;
        return stream.str();
    }

    uintptr_t FindPattern(HMODULE hModule, const uint8_t* bytePattern, const std::string& mask, size_t offset)
    {
        if (hModule && bytePattern && !mask.empty())
        {
            MODULEINFO miInfos;
            ZeroMemory(&miInfos, sizeof(MODULEINFO));
            GetModuleInformation(GetCurrentProcess(), hModule, &miInfos, sizeof(MODULEINFO));

            uintptr_t start = (reinterpret_cast<uintptr_t>(hModule) + offset);
            uintptr_t end = ((start + miInfos.SizeOfImage) - offset);

            size_t currentPos = 0;
            size_t maskLength = (mask.length() - 1);

            for (uintptr_t retAddress = start; retAddress < end; retAddress++)
            {
                if ((*reinterpret_cast<uint8_t*>(retAddress) == bytePattern[currentPos]) || (mask[currentPos] == '?'))
                {
                    if (currentPos == maskLength)
                    {
                        return (retAddress - maskLength);
                    }

                    currentPos++;
                }
                else
                {
                    retAddress -= currentPos;
                    currentPos = 0;
                }
            }
        }

        return 0;
    }

    uintptr_t FindPattern(const uint8_t* bytePattern, const std::string& maskStr, size_t offset)
    {
        return FindPattern(GetModuleHandleW(nullptr), bytePattern, maskStr, offset);
    }

    uintptr_t FindPatternStr(const std::string& strPattern, const std::string& maskStr, size_t offset)
    {
        if (!strPattern.empty() && !maskStr.empty())
        {
            std::array<uint8_t, 2048> bytePattern{};
            std::vector<std::string> strBytes = Split(strPattern, ' ');

            if (strBytes.size() < strPattern.size())
            {
                size_t pos = 0;

                for (const std::string& byte : strBytes)
                {
                    uint32_t dec = (IsStringHexadecimal(byte) ? ToDecimal(byte) : 0);
                    bytePattern[pos] = (dec <= UINT8_MAX ? dec : 0);
                    pos++;
                }

                return FindPattern(bytePattern.data(), maskStr, offset);
            }
        }

        return 0;
    }
}