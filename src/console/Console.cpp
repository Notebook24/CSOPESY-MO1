#include "console/Console.h"

#include <iostream>
#include <string>

#include "console/Banner.h"
#include "console/Terminal.h"

void Console::run() {
    Terminal::clear();
    Banner::print();

    std::string line;
    while (!handler_.shouldExit()) {
        std::cout << "root:\\> " << std::flush;
        if (!std::getline(std::cin, line)) {  // EOF (Ctrl+D / Ctrl+Z): behave like "exit"
            handler_.execute("exit");
            break;
        }
        handler_.execute(line);
    }
}
