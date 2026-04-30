#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "Common.h"
#include <termios.h>
using namespace std;

class InputHandler {
public:
    static void enableRawMode();
    static void disableRawMode();
    static Key getKey();
    static Key getKeyNonBlocking();

private:
    static struct termios orig_termios;
};

#endif