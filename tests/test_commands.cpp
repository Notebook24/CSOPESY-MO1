// Command tests. Owner: Karl
// Feeds command lines to CommandHandler and checks what it prints.
// Run from the build folder: it reads config.txt from the current folder.
// "screen -s <name>" is NOT tested here because it opens the process screen
// and waits for keyboard input. Test that one by hand.
#include <iostream>
#include <sstream>
#include <string>
#include "console/CommandHandler.h"

static int failures = 0;

static void checkTrue(const std::string& name, bool condition) {
    if (condition) {
        std::cout << "  PASS: " << name << std::endl;
    } else {
        std::cout << "  FAIL: " << name << std::endl;
        failures++;
    }
}

// Runs one command and returns everything it printed.
static std::string run(CommandHandler& handler, const std::string& line) {
    std::ostringstream captured;
    std::streambuf* oldBuffer = std::cout.rdbuf(captured.rdbuf());
    handler.execute(line);
    std::cout.rdbuf(oldBuffer);
    return captured.str();
}

static bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}

// ---------- tests ----------

static void testBeforeInitialize() {
    std::cout << "BEFORE INITIALIZE: only 'initialize' and 'exit' work" << std::endl;
    CommandHandler handler;

    checkTrue("screen -ls is blocked", contains(run(handler, "screen -ls"), "initialize"));
    checkTrue("scheduler-start is blocked", contains(run(handler, "scheduler-start"), "initialize"));
    checkTrue("scheduler-stop is blocked", contains(run(handler, "scheduler-stop"), "initialize"));
    checkTrue("report-util is blocked", contains(run(handler, "report-util"), "initialize"));
    checkTrue("unknown word is blocked too", contains(run(handler, "hello"), "initialize"));
    checkTrue("empty line prints nothing", run(handler, "").empty());
    checkTrue("spaces only prints nothing", run(handler, "   ").empty());
    checkTrue("not asked to exit yet", !handler.shouldExit());
}

static void testExitBeforeInitialize() {
    std::cout << "EXIT: works even before initialize" << std::endl;
    CommandHandler handler;
    run(handler, "exit");
    checkTrue("shouldExit is true", handler.shouldExit());
}

static void testInitialize() {
    std::cout << "INITIALIZE: works once, then says already initialized" << std::endl;
    CommandHandler handler;

    checkTrue("initialize extra is a usage error", contains(run(handler, "initialize now"), "Usage"));
    checkTrue("initialize succeeds", contains(run(handler, "initialize"), "Initialized"));
    checkTrue("second initialize is refused", contains(run(handler, "initialize"), "Already initialized"));
    run(handler, "exit");
}

static void testCaseInsensitive() {
    std::cout << "CASE: command words are not case sensitive" << std::endl;
    CommandHandler handler;
    checkTrue("INITIALIZE works", contains(run(handler, "INITIALIZE"), "Initialized"));
    checkTrue("Screen -ls works", contains(run(handler, "Screen -ls"), "CPU utilization"));
    run(handler, "exit");
}

static void testBadInputs() {
    std::cout << "BAD INPUTS: after initialize" << std::endl;
    CommandHandler handler;
    run(handler, "initialize");

    checkTrue("unknown command", contains(run(handler, "foo"), "Unknown command"));
    checkTrue("screen alone shows usage", contains(run(handler, "screen"), "Usage"));
    checkTrue("screen -x shows usage", contains(run(handler, "screen -x"), "Usage"));
    checkTrue("screen -s without a name shows usage", contains(run(handler, "screen -s"), "Usage"));
    checkTrue("screen -r without a name shows usage", contains(run(handler, "screen -r"), "Usage"));
    checkTrue("screen -ls extra shows usage", contains(run(handler, "screen -ls extra"), "Usage"));
    checkTrue("screen -r on a missing process", contains(run(handler, "screen -r nope"), "Process nope not found."));
    checkTrue("scheduler-start extra shows usage", contains(run(handler, "scheduler-start extra"), "Usage"));
    checkTrue("report-util extra shows usage", contains(run(handler, "report-util extra"), "Usage"));
    run(handler, "exit");
}

static void testSchedulerStartStop() {
    std::cout << "SCHEDULER-START / STOP" << std::endl;
    CommandHandler handler;
    run(handler, "initialize");

    checkTrue("stop before start is refused", contains(run(handler, "scheduler-stop"), "not generating"));
    checkTrue("start works", contains(run(handler, "scheduler-start"), "Scheduler started"));
    checkTrue("second start is refused", contains(run(handler, "scheduler-start"), "already"));
    checkTrue("stop works", contains(run(handler, "scheduler-stop"), "Scheduler stopped"));
    checkTrue("second stop is refused", contains(run(handler, "scheduler-stop"), "not generating"));
    run(handler, "exit");
}

static void testReports() {
    std::cout << "REPORTS: screen -ls and report-util" << std::endl;
    CommandHandler handler;
    run(handler, "initialize");

    std::string list = run(handler, "screen -ls");
    checkTrue("screen -ls shows CPU utilization", contains(list, "CPU utilization"));
    checkTrue("screen -ls shows cores used", contains(list, "Cores used"));
    checkTrue("screen -ls shows cores available", contains(list, "Cores available"));
    checkTrue("screen -ls shows Running processes", contains(list, "Running processes"));
    checkTrue("screen -ls shows Finished processes", contains(list, "Finished processes"));
    checkTrue("report-util saves the file", contains(run(handler, "report-util"), "Report generated"));
    run(handler, "exit");
}

static void testReinitialize() {
    std::cout << "EXIT then reuse: handler can shut down cleanly after work" << std::endl;
    CommandHandler handler;
    run(handler, "initialize");
    run(handler, "scheduler-start");
    run(handler, "exit");
    checkTrue("shouldExit is true", handler.shouldExit());
}

int main() {
    testBeforeInitialize();
    testExitBeforeInitialize();
    testInitialize();
    testCaseInsensitive();
    testBadInputs();
    testSchedulerStartStop();
    testReports();
    testReinitialize();

    if (failures == 0) {
        std::cout << "ALL TESTS PASSED" << std::endl;
        return 0;
    }
    std::cout << failures << " CHECK(S) FAILED" << std::endl;
    return 1;
}
