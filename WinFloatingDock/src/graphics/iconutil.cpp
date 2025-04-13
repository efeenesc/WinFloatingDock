#include "iconutil.h";

// Helper function to convert HICON to HBITMAP
HBITMAP IconToBitmap(HICON hIcon) {
    ICONINFO iconInfo = { 0 };
    if (!GetIconInfo(hIcon, &iconInfo)) {
        return NULL;
    }

    // Clean up the bitmaps that GetIconInfo created
    if (iconInfo.hbmMask)
        DeleteObject(iconInfo.hbmMask);

    // Return the color bitmap
    return iconInfo.hbmColor;
}

bool SaveIconToFile(HICON hIcon, const std::wstring& fileName) {
    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    std::wstring ext = PathFindExtensionW(fileName.c_str());

    return SaveIconToIcoFile(hIcon, fileName);
}

bool SaveIconToIcoFile(HICON hIcon, const std::wstring& filePath) {
    // Get icon info
    ICONINFO iconInfo;
    if (!GetIconInfo(hIcon, &iconInfo)) {
        return false;
    }

    // Get bitmap info
    BITMAP bm;
    GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);

    // Clean up the bitmaps GetIconInfo created
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);

    // Create file
    HANDLE hFile = CreateFileW(
        filePath.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    // Write ICO header
    WORD idReserved = 0;
    WORD idType = 1;
    WORD idCount = 1;
    DWORD bytesWritten;

    // bytesWritten is a "bit" of a tennis ball bouncing around here getting the number of bits written
    WriteFile(hFile, &idReserved, sizeof(WORD), &bytesWritten, NULL);
    WriteFile(hFile, &idType, sizeof(WORD), &bytesWritten, NULL);
    WriteFile(hFile, &idCount, sizeof(WORD), &bytesWritten, NULL);

    // Write ICO directory entry
    BYTE bWidth = (BYTE)bm.bmWidth;
    BYTE bHeight = (BYTE)bm.bmHeight;
    BYTE bColorCount = 0;
    BYTE bReserved = 0;
    WORD wPlanes = 1;
    WORD wBitCount = 32;
    DWORD dwBytesInRes = 40 + bm.bmWidth * bm.bmHeight * 4; // Bitmap size
    DWORD dwImageOffset = 22; // Size of header + directory

    WriteFile(hFile, &bWidth, sizeof(BYTE), &bytesWritten, NULL);
    WriteFile(hFile, &bHeight, sizeof(BYTE), &bytesWritten, NULL);
    WriteFile(hFile, &bColorCount, sizeof(BYTE), &bytesWritten, NULL);
    WriteFile(hFile, &bReserved, sizeof(BYTE), &bytesWritten, NULL);
    WriteFile(hFile, &wPlanes, sizeof(WORD), &bytesWritten, NULL);
    WriteFile(hFile, &wBitCount, sizeof(WORD), &bytesWritten, NULL);
    WriteFile(hFile, &dwBytesInRes, sizeof(DWORD), &bytesWritten, NULL);
    WriteFile(hFile, &dwImageOffset, sizeof(DWORD), &bytesWritten, NULL);

    // Create a compatible DC
    HDC hdc = CreateCompatibleDC(NULL);
    HBITMAP hBitmap = IconToBitmap(hIcon);
    HGDIOBJ oldObj = SelectObject(hdc, hBitmap);

    // Create bitmap info header
    BITMAPINFOHEADER bi;
    ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = bm.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    // Write bitmap info header
    WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &bytesWritten, NULL);

    // Get bitmap bits
    int bitsSize = bm.bmWidth * bm.bmHeight * 4;
    LPBYTE bits = new BYTE[bitsSize];
    GetDIBits(hdc, hBitmap, 0, bm.bmHeight, bits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Write bitmap bits
    WriteFile(hFile, bits, bitsSize, &bytesWritten, NULL);

    // Clean up
    delete[] bits;
    SelectObject(hdc, oldObj);
    DeleteObject(hBitmap);
    DeleteDC(hdc);
    CloseHandle(hFile);

    return true;
}
