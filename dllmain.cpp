#include "pch.h"
#include "Libraries/Base64/Base64.hpp"
#include "Libraries/MinHook/MinHook.hpp"
#include "Libraries/FindPattern/FindPattern.hpp"
#include "Libraries/FunctionHook/FunctionHook.hpp"

static FILE* CONSOLE_FILE = nullptr;

#define OPEN_CONSOLE_WINDOW // Forces a console window to be opened, so you can actually see whats being written to std::cout.
//#define LOG_DECRYPTED_DATA // Uncomment this if you wish to view the decrypted data, this is experimental as the data doesn't stick around for long.

namespace AESLogger
{
    // Yeah the pattern is long, get over it!

    const static std::string DECRYPT_PATTERN = "48 89 5C 24 00 55 56 57 48 81 EC 00 00 00 00 48 8B 05 00 00 00 00 48 33 C4 48 89 84 24 00 00 00 00 49 8B D8 8B FA 48 8B F1 33 D2 41 B8 00 00 00 00 48 8D 4C 24 00 E8 00 00 00 00 41 B8 00 00 00 00 48 8D 4C 24 00 48 8B D3 E8 00 00 00 00 33 DB 8B E8 85 FF 74 00 66 66 0F 1F 84 00 00 00 00 00 44 8B C3 48 8D 4C 24 00 4C 03 C6 8B D5 4D 8B C8 E8 00 00 00 00 83 C3 00 3B DF 72 00 48 8B 8C 24 00 00 00 00 48 33 CC E8 00 00 00 00 48 8B 9C 24 00 00 00 00 48 81 C4 00 00 00 00 5F 5E 5D C3 CC 48 89 5C 24";
    const static std::string DECRYPT_MASK = "xxxx?xxxxxx????xxx????xxxxxxx????xxxxxxxxxxxx????xxxx?x????xx????xxxx?xxxx????xxxxxxx?xxxxxx????xxxxxxx?xxxxxxxxx????xx?xxx?xxxx????xxxx????xxxx????xxx????xxxxxxxxx";

    typedef void(*DecryptDataFunc)(uint8_t*, uint32_t, uint8_t*);
    static FunctionHook DECRYPT_HOOK;

    static void DecryptDataHook(uint8_t* data, uint32_t dataLength, uint8_t* aesKey) // The "data" pointer coming is fully encrypted, and the "aesKey" is used after we call the function hook.
    {
        std::cout << "[AESKeyLogger] Data Address: " << ToHex(data) << std::endl;
        std::cout << "[AESKeyLogger] Data Length: " << dataLength << std::endl;
        std::cout << "[AESKeyLogger] AES Key: " << Base64(aesKey, 32) << std::endl;

        DECRYPT_HOOK.Call<DecryptDataFunc>()(data, dataLength, aesKey);  // Call the original function, which decrypts the "data" parameter.

#ifdef LOG_DECRYPTED_DATA
        // This only works sometimes, after the data is used its instantly deleted along with the key. So sometimes this can just print out random values in memory.
        // The correct solution to this would be to copy the encrypted data first, then manually decrypt it yourself with the given key.

        dataLength = (dataLength > 512 ? 512 : dataLength); // If you want to limit how much is viewed at a time in the console.
        std::string dataPreview;

        for (uint32_t i = 0; i < dataLength; i++)
        {
            if (std::isprint(data[i]))
            {
                dataPreview += static_cast<char>(data[i]);
            }
            else
            {
                dataPreview += ".";
            }
        }

        std::cout << "[AESKeyLogger] Decrypted Data: " << dataPreview << std::endl;
#endif
    }

    static void HookDecryptFunction()
    {
        uintptr_t functionAddress = FindPatternStr(DECRYPT_PATTERN, DECRYPT_MASK);

        if (functionAddress)
        {
            std::cout << "[AESKeyLogger] Found decrypt function at: " << ToHex(functionAddress, sizeof(functionAddress)) << std::endl;

            if (DECRYPT_HOOK.Attach(reinterpret_cast<uintptr_t*>(functionAddress), DecryptDataHook))
            {
                std::cout << "[AESKeyLogger] Decrypt function hooked!" << std::endl;
                DECRYPT_HOOK.Enable();
            }
            else
            {
                std::cout << "[AESKeyLogger] Failed to hook decrypt function!" << std::endl;
            }
        }
        else
        {
            std::cout << "[AESKeyLogger] Failed to find decrypt function, find pattern failed!" << std::endl;
        }
    }

    static void UnhookDecryptFunction()
    {
        if (DECRYPT_HOOK.IsAttached())
        {
            std::cout << "[AESKeyLogger] Unhooking decrypt function!" << std::endl;
            DECRYPT_HOOK.Detatch();
        }
    }

    static void SetupConsoleWindow()
    {
#ifdef OPEN_CONSOLE_WINDOW
        if (!CONSOLE_FILE)
        {
            AllocConsole();
            freopen_s(&CONSOLE_FILE, "CONOUT$", "w", stdout);
            ShowWindow(GetConsoleWindow(), SW_SHOW);
        }
#endif
    }

    static void CloseConsoleWindow()
    {
#ifdef OPEN_CONSOLE_WINDOW
        if (CONSOLE_FILE)
        {
            FreeConsole();
            CONSOLE_FILE = nullptr;
        }
#endif
    }

    static void OnInit()
    {
        AESLogger::SetupConsoleWindow();
        MinHook::MH_STATUS initStatus = MinHook::MH_Initialize();

        if (initStatus == MinHook::MH_STATUS::MH_OK)
        {
            std::cout << "[AESKeyLogger] MinHook initialized!" << std::endl;
            AESLogger::HookDecryptFunction();
        }
        else
        {
            std::cout << "[AESKeyLogger] Failed to initialize MinHook!" << std::endl;
        }
    }

    static void OnExit()
    {
        AESLogger::UnhookDecryptFunction();
        AESLogger::CloseConsoleWindow();
        MinHook::MH_Uninitialize();
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        AESLogger::OnInit();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        AESLogger::OnExit();
        break;
    default:
        break;
    }

    return TRUE;
}
