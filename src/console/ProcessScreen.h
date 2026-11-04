#pragma once

class Process;

// The per-process screen entered via "screen -s" / "screen -r".
// Accepts: process-smi, exit.
class ProcessScreen {
public:
    explicit ProcessScreen(Process& process) : process_(process) {}
    void run();  // blocks until the user types "exit" (or input closes)

private:
    void printSmi() const;
    Process& process_;
};
