#include "iconutil.h"

namespace WinFloatingDock {
    namespace graphics {

        BOOL SaveIconToIcoFile(HICON hIcon, const std::wstring& fileName) {
            // Get icon info
            ICONINFO iconInfo = { 0 };
            if (!GetIconInfo(hIcon, &iconInfo)) {
                return FALSE;
            }

            // Get BITMAP info written into bm via GetObject
            BITMAP bm;
            GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);

            // Create file
            HANDLE hFile = CreateFileW(
                fileName.c_str(),
                GENERIC_WRITE,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
            if (hFile == INVALID_HANDLE_VALUE) {
                // Clean up bitmaps if file creation fails
                DeleteObject(iconInfo.hbmColor);
                DeleteObject(iconInfo.hbmMask);
                return FALSE;
            }

            // Write ICO header
            WORD idReserved = 0;
            WORD idType = 1;
            WORD idCount = 1;
            DWORD bytesWritten;

            // Write ICO directory entry
            BYTE bWidth = (BYTE)bm.bmWidth;
            BYTE bHeight = (BYTE)bm.bmHeight;  // ICO format uses single height in directory
            BYTE bColorCount = 0;
            BYTE bReserved = 0;
            WORD wPlanes = 1;
            WORD wBitCount = 32;
            DWORD dwBytesInRes = 40 + bm.bmWidth * bm.bmHeight * 4; // Bitmap size with color data
            DWORD dwImageOffset = 22; // Size of header + directory

            // bytesWritten is a bit of a tennis ball bouncing around here, getting the number of bits written
            WriteFile(hFile, &idReserved, sizeof(WORD), &bytesWritten, NULL);
            WriteFile(hFile, &idType, sizeof(WORD), &bytesWritten, NULL);
            WriteFile(hFile, &idCount, sizeof(WORD), &bytesWritten, NULL);
            WriteFile(hFile, &bWidth, sizeof(BYTE), &bytesWritten, NULL);
            WriteFile(hFile, &bHeight, sizeof(BYTE), &bytesWritten, NULL);
            WriteFile(hFile, &bColorCount, sizeof(BYTE), &bytesWritten, NULL);
            WriteFile(hFile, &bReserved, sizeof(BYTE), &bytesWritten, NULL);
            WriteFile(hFile, &wPlanes, sizeof(WORD), &bytesWritten, NULL);
            WriteFile(hFile, &wBitCount, sizeof(WORD), &bytesWritten, NULL);
            WriteFile(hFile, &dwBytesInRes, sizeof(DWORD), &bytesWritten, NULL);
            WriteFile(hFile, &dwImageOffset, sizeof(DWORD), &bytesWritten, NULL);

            // Create new device context, don't attempt to use window HDC; select BITMAP
            HDC hdc = CreateCompatibleDC(NULL);
            HGDIOBJ oldObj = SelectObject(hdc, iconInfo.hbmColor);

            BITMAPINFOHEADER bi = { 0 };
            bi.biSize = sizeof(BITMAPINFOHEADER);
            bi.biWidth = bm.bmWidth;
            bi.biHeight = bm.bmHeight;  // Positive height for bottom-up DIB, which is what we want
            bi.biPlanes = 1;
            bi.biBitCount = 32;
            bi.biCompression = BI_RGB;

            // Write bitmap info header now that the header's fully written
            WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &bytesWritten, NULL);

            // Get bitmap bits
            int bitsSize = bm.bmWidth * bm.bmHeight * 4;
            LPBYTE bits = new BYTE[bitsSize];
            GetDIBits(hdc, iconInfo.hbmColor, 0, bm.bmHeight, bits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

            // Write bitmap bits - done with writing to file now
            WriteFile(hFile, bits, bitsSize, &bytesWritten, NULL);

            // Free everything in proper order
            delete[] bits;
            SelectObject(hdc, oldObj);
            DeleteDC(hdc);

            // Free bitmaps created by GetIconInfo
            DeleteObject(iconInfo.hbmColor);
            DeleteObject(iconInfo.hbmMask);

            CloseHandle(hFile);
            return TRUE;
        }
    }
}