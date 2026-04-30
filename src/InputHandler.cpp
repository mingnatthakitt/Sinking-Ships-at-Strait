#include "InputHandler.h"
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <poll.h>
using namespace std;

struct termios InputHandler::orig_termios;

void InputHandler::enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void InputHandler::disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

Key InputHandler::getKey() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return Key::UNKNOWN;

    if (c == '\033') {
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return Key::UNKNOWN;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return Key::UNKNOWN;

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return Key::UP;
                case 'B': return Key::DOWN;
                case 'C': return Key::RIGHT;
                case 'D': return Key::LEFT;
            }
        }
    } else if (c == '\n' || c == '\r') {
        tcflush(STDIN_FILENO, TCIFLUSH);
        return Key::ENTER;
    } else if (c == ' ') {
        tcflush(STDIN_FILENO, TCIFLUSH);
        return Key::SPACE;
    } else if (c == 'w' || c == 'W') {
        return Key::WEAPON_TOGGLE;
    } else if (c == 'r' || c == 'R') {
        return Key::MODE_TOGGLE;
    } else if (c == 'q' || c == 'Q') {
        return Key::QUIT;
    } else if (c == 's' || c == 'S') {
        return Key::SAVE_GAME;
    }
    return Key::UNKNOWN;
}

Key InputHandler::getKeyNonBlocking() {
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    if (poll(&pfd, 1, 0) > 0) {
        return getKey();
    }
    return Key::UNKNOWN;
}
