//Исходный код

#include <windows.h>
#include <commdlg.h>
#include <tchar.h>

HBITMAP hBitmap = NULL;
TCHAR filePath[MAX_PATH] = _T("");
HWND hButton;

// Прототипы функций
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL SelectFile(HWND hWnd) {
    OPENFILENAME ofn = { sizeof(OPENFILENAME), hWnd, NULL, _T("Bitmap Files\0*.bmp\0All Files\0*.*\0"), NULL, 0, 0, filePath, MAX_PATH };
    return GetOpenFileName(&ofn);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1), NULL, _T("ResizableBitmapWindow") };
    if (!RegisterClass(&wc)) return MessageBox(NULL, _T("Ошибка регистрации класса!"), _T("Ошибка"), MB_OK), 1;

    HWND hWnd = CreateWindow(wc.lpszClassName, _T("Практическая работа N2"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
    if (!hWnd) return MessageBox(NULL, _T("Ошибка создания окна!"), _T("Ошибка"), MB_OK), 1;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (hBitmap) DeleteObject(hBitmap);
    return (int)msg.wParam;                                                             
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        hButton = CreateWindow(_T("BUTTON"), _T("Выбрать файл"), WS_VISIBLE | WS_CHILD, 10, 10, 120, 30, hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);
        break;

    case WM_COMMAND:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
        if (LOWORD(wParam) == 1 && SelectFile(hWnd)) {
            if (hBitmap) DeleteObject(hBitmap);
            hBitmap = (HBITMAP)LoadImage(NULL, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (!hBitmap) MessageBox(hWnd, _T("Не удалось загрузить изображение!"), _T("Ошибка"), MB_OK);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT clientRect; GetClientRect(hWnd, &clientRect);

        if (hBitmap) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            SelectObject(hdcMem, hBitmap);

            BITMAP bitmap; GetObject(hBitmap, sizeof(BITMAP), &bitmap);

            int newWidth = clientRect.right / 2;
            int newHeight = clientRect.bottom / 2;
            int x = 0, y = clientRect.bottom - newHeight;

            RECT buttonRect;
            GetWindowRect(hButton, &buttonRect);
            ScreenToClient(hWnd, (LPPOINT)&buttonRect.left);
            ScreenToClient(hWnd, (LPPOINT)&buttonRect.right);

            if (y < buttonRect.bottom) y = buttonRect.bottom + 10;

            StretchBlt(hdc, x, y, newWidth, newHeight, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
            DeleteDC(hdcMem);
        }
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_SIZE: {
        int width = LOWORD(lParam), height = HIWORD(lParam);
        SetWindowPos(hButton, NULL, 10, 10, width / 4, height / 10, SWP_NOZORDER);
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
