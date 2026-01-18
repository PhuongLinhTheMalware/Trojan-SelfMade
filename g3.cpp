// g3.cpp : Defines the entry point for the application.
//

#include "g3.h"

DWORD WINAPI payloada(LPVOID lpParam)
{
    // Get desktop DC without GetDesktopWindow
    HDC hdc = GetDC(NULL);
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    SelectObject(memDC, bmp);

    double angle = 0.0;
    double spring = 0.0;

    while (true)
    {
        // Capture screen
        BitBlt(memDC, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

        // Spring oscillation
        spring += 0.05;
        angle += 0.01;

        double s = sin(spring) * 80.0;
        double c = cos(angle);

        int cx = w / 2;
        int cy = h / 2;

        POINT pts[3];

        // Rotating "eye" parallelogram
        pts[0].x = cx + (int)(-cx * c - s);
        pts[0].y = cy + (int)(-cy * c);

        pts[1].x = cx + (int)(cx * c);
        pts[1].y = cy + (int)(-cy * c + s);

        pts[2].x = cx + (int)(-cx * c + s);
        pts[2].y = cy + (int)(cy * c);

        // PlgBlt distortion
        PlgBlt(
            hdc,
            pts,
            memDC,
            0, 0,
            w, h,
            NULL,
            0, 0
        );

        Sleep(16); // ~60 FPS
    }

    // Cleanup (never reached)
    DeleteObject(bmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, hdc);

    return 0;
}
DWORD WINAPI payloadb(LPVOID lpParam)
{
    // Get screen DC WITHOUT GetDesktopWindow
    HDC hdc = GetDC(NULL);
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    // Memory DC for feedback
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, sw, sh);
    SelectObject(memDC, bmp);

    // Seed randomness
    srand((unsigned)time(NULL));

    float angle = 0.0f;

    while (true)
    {
        // Feedback copy (glitch cubie base)
        BitBlt(
            memDC,
            rand() % 20 - 10,
            rand() % 20 - 10,
            sw,
            sh,
            hdc,
            0,
            0,
            SRCCOPY
        );

        // Rotating PLGBLT "eye"
        POINT pts[3];

        int cx = sw / 2;
        int cy = sh / 2;
        int r = min(sw, sh) / 2;

        angle += 0.02f;

        pts[0].x = cx + (int)(cos(angle) * r);
        pts[0].y = cy + (int)(sin(angle) * r);

        pts[1].x = cx + (int)(cos(angle + 2.09f) * r);
        pts[1].y = cy + (int)(sin(angle + 2.09f) * r);

        pts[2].x = cx + (int)(cos(angle + 4.18f) * r);
        pts[2].y = cy + (int)(sin(angle + 4.18f) * r);

        PlgBlt(
            hdc,
            pts,
            memDC,
            0,
            0,
            sw,
            sh,
            NULL,
            0,
            0
        );

        // Color glitch overlay (NO black/white clears)
        HBRUSH brush = CreateSolidBrush(
            RGB(
                rand() % 256,
                rand() % 256,
                rand() % 256
            )
        );

        RECT rc = {
            rand() % sw,
            rand() % sh,
            rand() % sw,
            rand() % sh
        };

        FillRect(hdc, &rc, brush);
        DeleteObject(brush);

        Sleep(10);
    }

    // Cleanup (never reached, but correct)
    DeleteObject(bmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, hdc);

    return 0;
}
DWORD WINAPI payloadc(LPVOID lpParam)
{

    HDC hdc = GetDC(NULL); // screen DC (not GetDesktopWindow)
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, sw, sh);
    SelectObject(memDC, bmp);

    double t = 0.0;
    srand((unsigned)time(0));

    while (true)
    {
        // --- BitBlt rain distortion ---
        for (int y = 0; y < sh; y += 8)
        {
            int dx = (int)(sin(t + y * 0.02) * 20);
            BitBlt(
                memDC,
                dx, y,
                sw, 8,
                hdc,
                0, y,
                SRCCOPY
            );
        }

        // --- Sinebow cubix color overlay ---
        for (int i = 0; i < 80; i++)
        {
            int x = rand() % sw;
            int y = rand() % sh;

            int r = (int)(127 + 127 * sin(t + i));
            int g = (int)(127 + 127 * sin(t + i + 2.094));
            int b = (int)(127 + 127 * sin(t + i + 4.188));

            HBRUSH br = CreateSolidBrush(RGB(r, g, b));
            RECT rc = { x, y, x + 20, y + 20 };
            FillRect(memDC, &rc, br);
            DeleteObject(br);
        }

        // --- PlgBlt rotating redux warp ---
        POINT plg[3];
        double a = t * 0.5;
        int cx = sw / 2;
        int cy = sh / 2;
        int w = sw;
        int h = sh;

        plg[0].x = cx + (int)(cos(a) * w / 2 - sin(a) * h / 2);
        plg[0].y = cy + (int)(sin(a) * w / 2 + cos(a) * h / 2);

        plg[1].x = cx + (int)(cos(a) * w / 2 + sin(a) * h / 2);
        plg[1].y = cy + (int)(sin(a) * w / 2 - cos(a) * h / 2);

        plg[2].x = cx + (int)(-cos(a) * w / 2 - sin(a) * h / 2);
        plg[2].y = cy + (int)(-sin(a) * w / 2 + cos(a) * h / 2);

        PlgBlt(
            hdc,
            plg,
            memDC,
            0, 0,
            sw, sh,
            NULL,
            0, 0
        );

        t += 0.03;
        Sleep(10);
    }

    // unreachable but correct cleanup
    DeleteObject(bmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, hdc);
    return 0;
}
DWORD WINAPI BeepGlitch(LPVOID lpParam)
{
    srand(GetTickCount());

    while (true)
    {
        int pattern = rand() % 4;

        switch (pattern)
        {
        case 0: // short error ticks
            for (int i = 0; i < 5; i++)
            {
                Beep(200 + rand() % 800, 30 + rand() % 40);
                Sleep(10 + rand() % 20);
            }
            break;

        case 1: // harsh low buzz
            Beep(100 + rand() % 200, 200);
            break;

        case 2: // glitchy rising tones
            for (int f = 400; f < 1200; f += 150)
            {
                Beep(f + rand() % 50, 40);
            }
            break;

        case 3: // broken stutter
            for (int i = 0; i < 10; i++)
            {
                Beep(rand() % 3000 + 200, 10);
                Sleep(rand() % 30);
            }
            break;
        }

        Sleep(100 + rand() % 300); // pause between glitches
    }
    return 0;
}
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow
)
{
    HANDLE thread1;
    DWORD threadId;

    thread1 = CreateThread(
        NULL,
        0,
        payloada,
        NULL,
        0,
        &threadId
    );

    HANDLE beeper;

    beeper = CreateThread(
        NULL,
        0,
        BeepGlitch,
        NULL,
        0,
        &threadId
    );


    Sleep(30000);

    TerminateThread(thread1, 0);
    CloseHandle(thread1);

    InvalidateRect(NULL, NULL, TRUE);

    Sleep(100);


    HANDLE thread2;

    thread2 = CreateThread(
        NULL,
        0,
        payloadb,
        NULL,
        0,
        &threadId
    );



    Sleep(30000);

    TerminateThread(thread2, 0);
    CloseHandle(thread2);

    InvalidateRect(NULL, NULL, TRUE);

    Sleep(100);



    HANDLE thread3;

    thread3 = CreateThread(
        NULL,
        0,
        payloadc,
        NULL,
        0,
        &threadId
    );



    Sleep(30000);

    TerminateThread(thread3, 0);
    CloseHandle(thread3);

    TerminateThread(beeper, 0);
    CloseHandle(beeper);

    InvalidateRect(NULL, NULL, TRUE);

    Sleep(100);


    ExitProcess(0);

    return 0;
}