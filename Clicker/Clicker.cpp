// Practice_01.12.2021].cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Clicker.h"
#include <Windowsx.h>
#include <CommCtrl.h>
/*          Clicker               */

#define MAX_LOADSTRING              100
#define CMD_BUTTON_START            1001
#define TIMER_CLOCK                 2001


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND start;
HWND totalDigit;
HWND total;
HWND maxcpsDigit;
HWND maxcps;
HWND curcpsDigit;
HWND curcps;
HWND timer;
HWND progress;
BOOL isStart = FALSE;
long ds;
long cur_clicks = 0;
long cps_clicks = 0;
ULONGLONG t, prev_t;
long maxcps_clicks = 0;
WCHAR str[MAX_LOADSTRING];

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void CPS();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLICKER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLICKER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_CLICKER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(255, 160, 122));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLICKER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        const WCHAR fontName[] = L"Courier";
        const long nFontSize = 20;

        HFONT s_hFont;
        HDC hdc = GetDC(hWnd);
        LOGFONT logFont = { 0 };
        logFont.lfHeight = -MulDiv(nFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        logFont.lfWeight = FW_BOLD;
        wcscpy_s(logFont.lfFaceName, fontName);
        s_hFont = CreateFontIndirect(&logFont);
        t = 0;
        prev_t = GetTickCount64();
        ds = 0;
        start = CreateWindowW(L"Button", L"Start", WS_CHILD | WS_VISIBLE | WS_BORDER, 600, 350, 100, 100, hWnd, (HMENU)CMD_BUTTON_START, hInst, NULL);
        CreateWindowW(L"Static", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 170, 50, 400, 400, hWnd, 0, hInst, NULL);
        total = CreateWindowW(L"Static", L"Total : ", WS_CHILD | WS_VISIBLE, 575, 270, 120, 30, hWnd, 0, hInst, NULL);
        maxcps = CreateWindowW(L"Static", L"MAX CPS : ", WS_CHILD | WS_VISIBLE, 575, 230, 160, 27, hWnd, 0, hInst, NULL);
        curcps = CreateWindowW(L"Static", L"CPS : ", WS_CHILD | WS_VISIBLE | SS_CENTER, 70, 50, 70, 27, hWnd, 0, hInst, NULL);
        timer = CreateWindowW(L"Static", L"00:00.0", WS_CHILD | WS_VISIBLE | SS_CENTER, 620, 50, 120, 30, hWnd, 0, hInst, NULL);
        progress = CreateWindowW(PROGRESS_CLASS, L"", WS_CHILD | WS_VISIBLE | PBS_SMOOTH | PBS_VERTICAL, 30, 150, 50, 300, hWnd, 0, hInst, NULL);
        SendMessageW(progress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        SendMessageW(progress, PBM_SETSTEP, 1, 0);
        SendMessageW(progress, PBM_DELTAPOS, 50, 0);
        SendMessageW(progress, PBM_SETBARCOLOR, 0, RGB(200, 200, 200));
        
        totalDigit = CreateWindowW(L"Static", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 690, 270, 100, 30, hWnd, 0, hInst, NULL);
        maxcpsDigit = CreateWindowW(L"Static", L"0.0", WS_CHILD | WS_VISIBLE | SS_CENTER, 730, 230, 50, 27, hWnd, 0, hInst, NULL);
        curcpsDigit = CreateWindowW(L"Static", L"0.0", WS_CHILD | WS_VISIBLE | SS_CENTER, 20, 50, 50, 27, hWnd, 0, hInst, NULL);
        SendMessage(timer, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(start, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(total, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(totalDigit, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(maxcps, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(maxcpsDigit, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(curcps, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        SendMessage(curcpsDigit, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

        break;
    }
    case WM_TIMER: {
        if (wParam == TIMER_CLOCK)
        {
            SendMessageW(progress, PBM_DELTAPOS, -1, 0);
            int pos = SendMessageW(progress, PBM_GETPOS, 0, 0);
            SendMessageW(progress, PBM_SETBARCOLOR, 0, RGB(250-pos, 150+pos, 200));
            ds+=1;
            
            
            _snwprintf_s(str, MAX_LOADSTRING, L"%02d:%02d.%d",
                ds / 600,
                (ds % 600) / 10,
                ds % 10);
            SendMessageW(timer, WM_SETTEXT, 0, (LPARAM)str);
        }
        break;
    }
    case WM_LBUTTONDOWN: {
        
        if (isStart == TRUE)
        {
            SendMessageW(progress, PBM_DELTAPOS, 2, 0);
            cur_clicks += 1;
            t = GetTickCount64();
            _snwprintf_s(str, MAX_LOADSTRING, L"%.1f", 1000.0f / (t - prev_t));
            if ((1000.0f / (t - prev_t)) > maxcps_clicks)
            {
                maxcps_clicks = 1000.0f / (t - prev_t);
                SendMessageW(maxcpsDigit, WM_SETTEXT, 100, (LPARAM)str);
            }
            prev_t = t;
            SendMessageW(curcpsDigit, WM_SETTEXT, 100, (LPARAM)str);
            _itow_s(cur_clicks, str, 10);
            SendMessageW(totalDigit, WM_SETTEXT, 100, (LPARAM)str);
        }
        
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case CMD_BUTTON_START: {
            isStart = TRUE;
            SendMessageW(start, WM_KILLFOCUS, 0, 0);
            SetTimer(hWnd, TIMER_CLOCK, 100, NULL);
            break;
        }
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void CPS() {
    
}