#pragma once
#include "console/CommandHandler.h"

// Main menu loop: banner, "root:\>" prompt, read line, dispatch.
class Console {
public:
    void run();

private:
    CommandHandler handler_;
};
