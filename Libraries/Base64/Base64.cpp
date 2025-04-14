#include "Base64.hpp"

namespace AESLogger
{
    static const std::string BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    bool IsStringBase64(const std::string& str)
    {
        if (str.empty())
        {
            return false;
        }

        for (char c : str)
        {
            if ((!std::isalpha(c) && !std::isdigit(c)) && ((c != '+') && (c != '=') && (c != '/') && (c != '.') && (c != '_') && (c != '-')))
            {
                return false;
            }
        }

        return true;
    }

    std::string Base64(uint8_t* bytes, size_t byteLength)
    {
        if (bytes)
        {
            size_t i = 0;
            size_t j = 0;

            std::string encodedStr;
            uint8_t char_array_3[3];
            uint8_t char_array_4[4];

            for (size_t k = 0; k < byteLength; k++)
            {
                char_array_3[i++] = bytes[k];

                if (i == 3)
                {
                    char_array_4[0] = ((char_array_3[0] & 0xfc) >> 2);
                    char_array_4[1] = (((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4));
                    char_array_4[2] = (((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6));
                    char_array_4[3] = (char_array_3[2] & 0x3f);

                    for (i = 0; (i < 4); i++)
                    {
                        encodedStr += BASE64_CHARS[char_array_4[i]];
                    }

                    i = 0;
                }
            }

            if (i)
            {
                for (j = i; j < 3; j++)
                {
                    char_array_3[j] = '\0';
                }

                char_array_4[0] = ((char_array_3[0] & 0xfc) >> 2);
                char_array_4[1] = (((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4));
                char_array_4[2] = (((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6));
                char_array_4[3] = (char_array_3[2] & 0x3f);

                for (j = 0; j < (i + 1); j++)
                {
                    encodedStr += BASE64_CHARS[char_array_4[j]];
                }

                while (i++ < 3)
                {
                    encodedStr += '=';
                }
            }

            return encodedStr;
        }

        return "";
    }

    std::string Base64(std::string str)
    {
        if (!str.empty())
        {
            return Base64(reinterpret_cast<uint8_t*>(str.data()), str.length());
        }

        return str;
    }

    void Base64Inline(std::string& str)
    {
        str = Base64(str);
    }

    std::string Base64Decode(const std::string& str)
    {
        if (IsStringBase64(str))
        {
            size_t in_len = str.length();
            size_t i = 0;
            size_t j = 0;
            size_t in_ = 0;

            uint8_t char_array_4[4];
            uint8_t char_array_3[3];
            std::string decodedStr;

            while (in_len-- && (str[in_] != '='))
            {
                char_array_4[i++] = str[in_];
                in_++;

                if (i == 4)
                {
                    for (i = 0; i < 4; i++)
                    {
                        char_array_4[i] = BASE64_CHARS.find(char_array_4[i]);
                    }

                    char_array_3[0] = ((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
                    char_array_3[1] = (((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
                    char_array_3[2] = (((char_array_4[2] & 0x3) << 6) + char_array_4[3]);

                    for (i = 0; i < 3; i++)
                    {
                        decodedStr += char_array_3[i];
                    }

                    i = 0;
                }
            }

            if (i)
            {
                for (j = i; j < 4; j++)
                {
                    char_array_4[j] = 0;
                }

                for (j = 0; j < 4; j++)
                {
                    char_array_4[j] = BASE64_CHARS.find(char_array_4[j]);
                }

                char_array_3[0] = ((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
                char_array_3[1] = (((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
                char_array_3[2] = (((char_array_4[2] & 0x3) << 6) + char_array_4[3]);

                for (j = 0; j < (i - 1); j++)
                {
                    decodedStr += char_array_3[j];
                }
            }

            return decodedStr;
        }

        return "";
    }

    void Base64DecodeInline(std::string& str)
    {
        str = Base64Decode(str);
    }
}