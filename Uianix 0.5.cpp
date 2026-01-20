// Uianix 0.5.cpp : Defines the entry point for the application.
//

#include "Uianix 0.5.h"
#define Entrance HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd
#define ModifedWinMain WinMain

DWORD WINAPI shader1(LPVOID lpParam)
{
    HDC hdc = GetDC(NULL);
    HDC mem = CreateCompatibleDC(hdc);

    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    SelectObject(mem, bmp);

    DWORD start = GetTickCount();

    while (1)
    {
        // copy desktop
        BitBlt(mem, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

        float t = (GetTickCount() - start) * 0.002f;
        float s = sinf(t) * 0.15f;

        POINT p[3];
        p[0].x = LONG(w * s);
        p[0].y = LONG(h * s);
        p[1].x = w - p[0].x;
        p[1].y = LONG(h * s * 0.5f);
        p[2].x = LONG(w * s * 0.5f);
        p[2].y = h - p[0].y;

        // skew / cube-like distortion
        PlgBlt(hdc, p, mem, 0, 0, w, h, NULL, 0, 0);

        // zoom distortion
        StretchBlt(
            hdc,
            (int)(w * s),
            (int)(h * s),
            (int)(w * (1.0f - s * 2)),
            (int)(h * (1.0f - s * 2)),
            mem,
            0, 0, w, h,
            SRCCOPY
        );

        Sleep(1); // keeps system responsive
    }

    // unreachable unless process is killed
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(NULL, hdc);
    return 0;
}
DWORD WINAPI shader2(LPVOID lpParam)
{
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = sw;
    bmi.bmiHeader.biHeight = -sh; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* pixels = 0;

    HBITMAP hbm = CreateDIBSection(
        hdcScreen,
        &bmi,
        DIB_RGB_COLORS,
        (void**)&pixels,
        NULL,
        0
    );

    SelectObject(hdcMem, hbm);

    int t = 0;

    while (1)
    {
        for (int y = 0; y < sh; y++)
        {
            for (int x = 0; x < sw; x++)
            {
                int i = y * sw + x;

                pixels[i].rgbRed = (BYTE)((x ^ t) + (y));
                pixels[i].rgbGreen = (BYTE)((y ^ t) + (x));
                pixels[i].rgbBlue = (BYTE)(t * 5);
                pixels[i].rgbReserved = 0;
            }
        }

        BitBlt(
            hdcScreen,
            0, 0, sw, sh,
            hdcMem,
            0, 0,
            SRCCOPY
        );

        t++;
        Sleep(1);
    }

    // unreachable cleanup
    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    return 0;
}
DWORD WINAPI shader3(LPVOID lpParam)
{
    HDC hdc = GetDC(NULL);
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = sw;
    bmi.bmiHeader.biHeight = -sh;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* buffer = 0;
    HDC memdc = CreateCompatibleDC(hdc);
    HBITMAP dib = CreateDIBSection(
        hdc,
        &bmi,
        DIB_RGB_COLORS,
        (void**)&buffer,
        0,
        0
    );

    SelectObject(memdc, dib);

    double t = 0.0;

    while (1)
    {
        for (int y = 0; y < sh; y++)
        {
            for (int x = 0; x < sw; x++)
            {
                int i = x + y * sw;

                double sx = sin((x + t) * 0.01);
                double cy = cos((y - t) * 0.01);
                double tz = tan((x ^ y) * 0.0005);

                int r = (int)(sx * 127 + 128);
                int g = (int)(cy * 127 + 128);
                int b = (int)(tz * 127 + 128);

                buffer[i].rgbRed += (r ^ (x + y)) & 0xFF;
                buffer[i].rgbGreen += (g ^ (x * y)) & 0xFF;
                buffer[i].rgbBlue += (b ^ (x - y)) & 0xFF;
            }
        }

        StretchBlt(
            hdc,
            0, 0, sw, sh,
            memdc,
            (int)(sin(t * 0.01) * 50),
            (int)(cos(t * 0.01) * 50),
            sw - 100,
            sh - 100,
            SRCCOPY
        );

        t += 1.0;
        Sleep(1);
    }

    // ---- CLEANUP (unreachable unless loop breaks) ----
    DeleteObject(dib);
    DeleteDC(memdc);
    ReleaseDC(NULL, hdc);

    return 0;
}
DWORD WINAPI shader4(LPVOID lpParam)
{
    HDC desk = GetDC(NULL);
    HDC mem = CreateCompatibleDC(desk);

    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP bmp = CreateCompatibleBitmap(desk, w, h);
    SelectObject(mem, bmp);

    double t = 0.0;

    while (1)
    {
        // Grab desktop
        BitBlt(mem, 0, 0, w, h, desk, 0, 0, SRCCOPY);

        // asd frox sine offsets
        int sx = (int)(sin(t * 2.0) * 30);
        int sy = (int)(cos(t * 1.7) * 30);

        // bitbit XOR smear
        BitBlt(
            desk,
            sx, sy,
            w, h,
            mem,
            0, 0,
            SRCCOPY
        );

        // sine mama rotation
        double s = sin(t) * 0.25;
        double c = cos(t) * 0.25;

        POINT p[3];
        p[0].x = (LONG)(w * (0.5 - c));
        p[0].y = (LONG)(h * (0.5 - s));

        p[1].x = (LONG)(w * (0.5 + c));
        p[1].y = (LONG)(h * (0.5 - s));

        p[2].x = (LONG)(w * (0.5 - s));
        p[2].y = (LONG)(h * (0.5 + c));

        PlgBlt(
            desk,
            p,
            mem,
            0, 0,
            w, h,
            NULL,
            0, 0
        );

        // extra bitbit invert flicker
        if (((int)(t * 100)) % 40 == 0)
        {
            BitBlt(
                desk,
                0, 0,
                w, h,
                desk,
                0, 0,
                NOTSRCCOPY
            );
        }

        t += 0.01;
        Sleep(1);
    }

    // cleanup (reachable only if loop exits)
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(NULL, desk);

    return 0;
}
DWORD WINAPI shader5(LPVOID lpParam)
{
    HDC hdc = GetDC(NULL);
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    SelectObject(memDC, bmp);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* pixels = new RGBQUAD[w * h];

    double t = 0.0;

    while (1)
    {
        BitBlt(memDC, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

        GetDIBits(memDC, bmp, 0, h, pixels, &bmi, DIB_RGB_COLORS);

        for (int i = 0; i < w * h; i++)
        {
            int v =
                (int)(
                    sin(t + i * 0.00001) * 128 +
                    cos(t * 0.5) * 64 +
                    tan(t * 0.25) * 16
                    );

            pixels[i].rgbRed = (pixels[i].rgbRed + v) ^ 0x55;
            pixels[i].rgbGreen = (pixels[i].rgbGreen + v) ^ 0xAA;
            pixels[i].rgbBlue = (pixels[i].rgbBlue + v) ^ 0xFF;
        }

        SetDIBits(memDC, bmp, 0, h, pixels, &bmi, DIB_RGB_COLORS);

        StretchBlt(
            hdc,
            0, 0, w, h,
            memDC,
            5, 5, w - 10, h - 10,
            SRCCOPY
        );

        t += 0.01;
        Sleep(1);
    }

    delete[] pixels;
    DeleteObject(bmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, hdc);

    return 0;
}
DWORD WINAPI shader6(LPVOID lpParam)
{
    // Get desktop DC
    HDC desktopDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(desktopDC);

    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    // Create bitmap for PlgBlt
    HBITMAP bmp = CreateCompatibleBitmap(desktopDC, w, h);
    SelectObject(memDC, bmp);

    double t = 0.0;

    while (1)
    {
        // Copy screen into memory DC
        BitBlt(memDC, 0, 0, w, h, desktopDC, 0, 0, SRCCOPY);

        // Sin / Cos / Tan shake
        int shakeX = (int)(sin(t) * 10);
        int shakeY = (int)(cos(t) * 10);

        // Screwball rotation using PlgBlt
        POINT pts[3];
        double a = sin(t) * 0.05;

        pts[0].x = (LONG)(0 + cos(a) * w);
        pts[0].y = (LONG)(0 + sin(a) * w);

        pts[1].x = (LONG)(w + sin(a) * h);
        pts[1].y = (LONG)(0 + cos(a) * h);

        pts[2].x = (LONG)(tan(a) * 50);
        pts[2].y = (LONG)(h);

        // Apply effect to desktop
        PlgBlt(
            desktopDC,
            pts,
            memDC,
            0,
            0,
            w,
            h,
            NULL,
            0,
            0
        );

        // Extra sine BitBlt shake
        BitBlt(
            desktopDC,
            shakeX,
            shakeY,
            w,
            h,
            memDC,
            0,
            0,
            SRCCOPY
        );

        t += 0.03;
        Sleep(1);
    }

    // Cleanup (never reached unless loop breaks)
    DeleteObject(bmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, desktopDC);

    return 0;
}
VOID WINAPI soundman()
{
    HWAVEOUT hWaveOut = nullptr;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 16000, 16000, 1, 8, 0 };

    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

    BYTE buffer[16000 * 30] = {};

    for (DWORD t = 0; t < sizeof(buffer); ++t)
    {
        buffer[t] = static_cast<BYTE>(
            (t * 5321 + t * 13 ^ t * (t >> 5) & t) | (t * 5121 + t * 11 ^ t * (t >> 7) & t)
            );
    }

    WAVEHDR header = {};
    header.lpData = reinterpret_cast<LPSTR>(buffer);
    header.dwBufferLength = sizeof(buffer);

    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));

    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
DWORD WINAPI sound666(LPVOID lpParam) {
    soundman();
    Sleep(999999999999);
    return 0;
}
void Payload(LPTHREAD_START_ROUTINE proc, DWORD durationMs)
{
    HANDLE hThread = CreateThread(
        nullptr,
        0,
        proc,
        nullptr,
        0,
        nullptr
    );

    if (hThread)
    {
        Sleep(durationMs);
        TerminateThread(hThread, 0);   // intentionally kept
        CloseHandle(hThread);
    }

    Sleep(100);
}
int WINAPI WinMain(Entrance)
{
    if (MessageBoxW(
        NULL,
        L"",
        L"",
        MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2
    ) == IDNO)
    {
        ExitProcess(0);
    }

    if (MessageBoxW(
        NULL,
        L"Did you run Uianix 0.5?",
        L"?",
        MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2
    ) == IDYES)
    {
        HANDLE audios = CreateThread(0, 0, sound666, 0, 0, 0);
        Payload(shader1, 30000);
        Payload(shader2, 30000);
        Payload(shader3, 30000);
        Payload(shader4, 30000);
        Payload(shader5, 30000);
        Payload(shader6, 30000);
        ExitProcess(0);
        Sleep(INFINITE);
    }
    else
    {
        ExitProcess(0);
    }

    return 0;
}
