#include <windows.h>
#include <commdlg.h>
#include <tchar.h>

// Глобальные переменные
HBITMAP hBitmap = NULL;  // Исходное изображение
HBITMAP hMask = NULL;    // Маска изображения
TCHAR filePath[MAX_PATH] = _T("");
HWND hButton;

// Прототипы функций
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL SelectFile(HWND hWnd);
HBITMAP CreateBitmapMask(HBITMAP hBitmap, COLORREF transparentColor);

BOOL SelectFile(HWND hWnd) {
    OPENFILENAME ofn = { sizeof(OPENFILENAME), hWnd, NULL, _T("Bitmap Files\0*.bmp\0All Files\0*.*\0"), NULL, 0, 0, filePath, MAX_PATH };
    return GetOpenFileName(&ofn);
}

HBITMAP CreateBitmapMask(HBITMAP hBitmap, COLORREF transparentColor) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    HDC hdcMem = CreateCompatibleDC(NULL);
    HDC hdcMask = CreateCompatibleDC(NULL);

    HBITMAP hMaskBitmap = CreateBitmap(bmp.bmWidth, bmp.bmHeight, 1, 1, NULL);
    SelectObject(hdcMask, hMaskBitmap);
    SelectObject(hdcMem, hBitmap);

    SetBkColor(hdcMem, transparentColor);

    // Создаем маску: черный для прозрачных областей, белый для остальных
    BitBlt(hdcMask, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY);

    DeleteDC(hdcMem);
    DeleteDC(hdcMask);

    return hMaskBitmap;
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
    if (hMask) DeleteObject(hMask);

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
            if (hMask) DeleteObject(hMask);

            hBitmap = (HBITMAP)LoadImage(NULL, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (!hBitmap) {
                MessageBox(hWnd, _T("Не удалось загрузить изображение!"), _T("Ошибка"), MB_OK);
            }
            else {
                // Определяем прозрачный цвет
                HDC hdcTemp = CreateCompatibleDC(NULL);
                SelectObject(hdcTemp, hBitmap);
                COLORREF transparentColor = GetPixel(hdcTemp, 0, 0);
                DeleteDC(hdcTemp);

                // Создаем маску
                hMask = CreateBitmapMask(hBitmap, transparentColor);
            }

            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        if (hBitmap && hMask) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HDC hdcMask = CreateCompatibleDC(hdc);

            SelectObject(hdcMem, hBitmap);
            SelectObject(hdcMask, hMask);

            BITMAP bitmap;
            GetObject(hBitmap, sizeof(BITMAP), &bitmap);

            int newWidth = clientRect.right / 2;
            int newHeight = clientRect.bottom / 2;
            int x = 0, y = clientRect.bottom - newHeight;

            StretchBlt(hdc, x, y, newWidth, newHeight, hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCAND);
            StretchBlt(hdc, x, y, newWidth, newHeight, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCPAINT);

            DeleteDC(hdcMem);
            DeleteDC(hdcMask);
        }
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_SIZE:
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
