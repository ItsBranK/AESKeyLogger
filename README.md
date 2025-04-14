# AESKeyLogger
Logs the raw AES keys used to decrypt files for Rocket League. This will not show unreleased AES keys, only ones Psyonix has released manually. So this method cannot be used to leak unreleased content if that's what your intentions are.

## Additional Info

There are two macros in the `dllmain.cpp` file that give you two options, `OPEN_CONSOLE_WINDOW` automatically opens a console window to actually view whats being printed; and the second one is `LOG_DECRYPTED_DATA` which is commented out by default.
