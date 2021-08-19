#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#include <PGE/iostream>
#include <PGE/sys/ioctl.h>
#include <PGE/unistd.h>
#endif

#include <PGE/Math/Vector.h>

using namespace PGE;

namespace ConsoleHelper {

Vector2i getSize() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    SMALL_RECT& w = info.srWindow;
    return Vector2i(w.Right - w.Left + 1, w.Bottom - w.Top + 1);
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return Vector2i(w.ws_row, w.ws_col);
#endif
}

void setCursorVisibility(bool visible) {
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(handle, &info);
    info.bVisible = visible;
    SetConsoleCursorInfo(handle, &info);
#else
    std::cout << (visible ? "\e[?25h" : "\e[?25l") << std::endl;
#endif
}

}
