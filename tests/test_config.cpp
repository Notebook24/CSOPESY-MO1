// Config tests. Owner: Karl
// Writes small config files to disk, then calls loadConfig() on them.
// The temp file is created in the folder you run the test from.
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include "config/Config.h"

static int failures = 0;
static const std::string TEMP_FILE = "test_config_tmp.txt";

static void checkTrue(const std::string& name, bool condition) {
    if (condition) {
        std::cout << "  PASS: " << name << std::endl;
    } else {
        std::cout << "  FAIL: " << name << std::endl;
        failures++;
    }
}

// Builds a full config text. Tests change one value at a time.
static std::string makeConfig(const std::string& numCpu,
                              const std::string& scheduler,
                              const std::string& quantum,
                              const std::string& freq,
                              const std::string& minIns,
                              const std::string& maxIns,
                              const std::string& delay) {
    std::string text = "";
    text += "num-cpu " + numCpu + "\n";
    text += "scheduler " + scheduler + "\n";
    text += "quantum-cycles " + quantum + "\n";
    text += "batch-process-freq " + freq + "\n";
    text += "min-ins " + minIns + "\n";
    text += "max-ins " + maxIns + "\n";
    text += "delays-per-exec " + delay + "\n";
    return text;
}

static std::string validConfig() {
    return makeConfig("4", "rr", "5", "1", "10", "20", "0");
}

// Writes the text to the temp file and loads it.
static bool tryLoad(const std::string& text, Config& cfg, std::string& error) {
    std::ofstream file(TEMP_FILE);
    file << text;
    file.close();
    return loadConfig(TEMP_FILE, cfg, error);
}

static void expectOk(const std::string& name, const std::string& text) {
    Config cfg;
    std::string error;
    bool ok = tryLoad(text, cfg, error);
    if (ok) {
        std::cout << "  PASS: " << name << std::endl;
    } else {
        std::cout << "  FAIL: " << name << " (error: " << error << ")" << std::endl;
        failures++;
    }
}

static void expectFail(const std::string& name, const std::string& text) {
    Config cfg;
    std::string error;
    bool ok = tryLoad(text, cfg, error);
    if (!ok) {
        std::cout << "  PASS: " << name << " (error: " << error << ")" << std::endl;
    } else {
        std::cout << "  FAIL: " << name << " (should have been rejected)" << std::endl;
        failures++;
    }
}

// ---------- tests ----------

static void testValues() {
    std::cout << "VALUES: a valid config is read correctly" << std::endl;
    Config cfg;
    std::string error;
    bool ok = tryLoad(makeConfig("8", "fcfs", "7", "3", "100", "200", "2"), cfg, error);
    checkTrue("loads", ok);
    checkTrue("num-cpu = 8", cfg.numCpu == 8);
    checkTrue("scheduler = fcfs", cfg.scheduler == SchedulerType::FCFS);
    checkTrue("quantum-cycles = 7", cfg.quantumCycles == 7);
    checkTrue("batch-process-freq = 3", cfg.batchProcessFreq == 3);
    checkTrue("min-ins = 100", cfg.minIns == 100);
    checkTrue("max-ins = 200", cfg.maxIns == 200);
    checkTrue("delays-per-exec = 2", cfg.delayPerExec == 2);
}

static void testFormat() {
    std::cout << "FORMAT: case, quotes, blank lines" << std::endl;
    expectOk("scheduler FCFS in capitals", makeConfig("4", "FCFS", "5", "1", "10", "20", "0"));
    expectOk("scheduler in quotes", makeConfig("4", "\"rr\"", "5", "1", "10", "20", "0"));
    expectOk("blank lines are skipped", "\n\n" + validConfig() + "\n\n");
}

static void testNumCpu() {
    std::cout << "NUM-CPU: range [1, 128]" << std::endl;
    expectOk("1 is allowed", makeConfig("1", "rr", "5", "1", "10", "20", "0"));
    expectOk("128 is allowed", makeConfig("128", "rr", "5", "1", "10", "20", "0"));
    expectFail("0 is rejected", makeConfig("0", "rr", "5", "1", "10", "20", "0"));
    expectFail("129 is rejected", makeConfig("129", "rr", "5", "1", "10", "20", "0"));
    expectFail("-1 is rejected", makeConfig("-1", "rr", "5", "1", "10", "20", "0"));
    expectFail("abc is rejected", makeConfig("abc", "rr", "5", "1", "10", "20", "0"));
}

static void testOtherValues() {
    std::cout << "OTHER VALUES: scheduler, quantum, freq, delay, min/max" << std::endl;
    expectFail("scheduler sjf is rejected", makeConfig("4", "sjf", "5", "1", "10", "20", "0"));
    expectFail("quantum-cycles 0 is rejected", makeConfig("4", "rr", "0", "1", "10", "20", "0"));
    expectFail("batch-process-freq 0 is rejected", makeConfig("4", "rr", "5", "0", "10", "20", "0"));
    expectFail("min-ins 0 is rejected", makeConfig("4", "rr", "5", "1", "0", "20", "0"));
    expectOk("delays-per-exec 0 is allowed", makeConfig("4", "rr", "5", "1", "10", "20", "0"));
    expectFail("min-ins greater than max-ins is rejected", makeConfig("4", "rr", "5", "1", "30", "20", "0"));
    expectOk("min-ins equal to max-ins is allowed", makeConfig("4", "rr", "5", "1", "20", "20", "0"));
}

static void testBadStructure() {
    std::cout << "STRUCTURE: missing, unknown, malformed lines" << std::endl;

    std::string missing = "";
    missing += "num-cpu 4\n";
    missing += "scheduler rr\n";
    missing += "quantum-cycles 5\n";
    missing += "batch-process-freq 1\n";
    missing += "min-ins 10\n";
    missing += "max-ins 20\n";
    expectFail("missing delays-per-exec is rejected", missing);

    expectFail("unknown parameter is rejected", validConfig() + "foo 1\n");
    expectFail("line with only a key is rejected", validConfig() + "num-cpu\n");
    expectFail("line with 3 words is rejected", validConfig() + "num-cpu 4 5\n");

    Config cfg;
    std::string error;
    bool ok = loadConfig("this_file_does_not_exist.txt", cfg, error);
    checkTrue("missing file is rejected", !ok);
}

int main() {
    testValues();
    testFormat();
    testNumCpu();
    testOtherValues();
    testBadStructure();

    std::remove(TEMP_FILE.c_str());

    if (failures == 0) {
        std::cout << "ALL TESTS PASSED" << std::endl;
        return 0;
    }
    std::cout << failures << " CHECK(S) FAILED" << std::endl;
    return 1;
}
