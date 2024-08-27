#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

static struct WindowsPlatform {
    HWND hwnd;
    bool window_should_close;
} windows_platform;

LRESULT CALLBACK windows_window_callback(
    HWND hwnd, UINT msg,
    WPARAM wparam, LPARAM lparam
) {
    LRESULT result = 0;
    switch (msg) {
    case WM_CLOSE: {
        windows_platform.window_should_close = true;
    } break;
    default: {
        result = DefWindowProcA(hwnd, msg, wparam, lparam);
    } break;
    }

    return result;
}

bool windows_create_window(int width, int height, char *title) {
    HINSTANCE hInstance = GetModuleHandleA(NULL);

    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassa
    WNDCLASSA wc = {
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = windows_window_callback,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hInstance,
        .hIcon = LoadIconA(hInstance, IDI_APPLICATION),
        .hCursor = LoadCursorA(NULL, IDC_ARROW),
        .hbrBackground = NULL,
        .lpszMenuName = NULL,
        .lpszClassName = title // Probably shouldn't be the title in-practice.
    };

    if (!RegisterClassA(&wc)) {
        return false;
    }

    DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    HWND hwnd = CreateWindowExA(
        0, title, // Class name
        title,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return false;
    }

    windows_platform.hwnd = hwnd;
    ShowWindow(hwnd, SW_SHOW);

    return true;
}

void windows_update_window() {
    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

int windows_window_should_close() {
    return windows_platform.window_should_close;
}

int main(void) {
    bool success = windows_create_window(800, 600, "Hello, Window!");
    assert(success && "Failed to create window.");

    while (!windows_window_should_close()) {
        windows_update_window();
    }

    return 0;
}
