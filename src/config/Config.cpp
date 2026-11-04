#include "config.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <set>
#include <sstream>

// Helper Functions
namespace {
    const uint64_t MAX_U32_RANGE = 4294967296ULL;

    std::string trim(const std::string& s) {
        size_t a = 0, b = s.size();
        while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) 
            a++;
        while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) 
            b--;
        
        return s.substr(a, b - a);
    }

// removes surrounding double quotes
std::string stripQuotes(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        return s.substr(1, s.size() - 2);
    return s;
}

// converts all letters in the string into lowercase
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
            [](unsigned char c) 
                { return std::tolower(c);});
    return s;
}

// checks wheter a value contains a valid non-negative whole number, then converts it into a number the program can use
bool parseUnsigned(const std::string& text, uint64_t& out) {
    if(text.empty() || text.size() > 12)
        return false;

    for (char c : text) 
        if (!std::isdigit(static_cast<unsigned char>(c)))
            return false;

    out = std::stoull(text);
    
    return true;
}

// checks wheter a number is valid AND falls within the allowed range
bool parseInRange(const std::string& key, const std::string& value, uint64_t lo, uint64_t hi, uint64_t& out, std::string& error) {
    if (!parseUnsigned(value, out)) {
        error = key + " must be a whole number (got '" + value + "')";
        return false;
    }
    if (out < lo || out > hi) {
        error = key + " must be in range [" +std::to_string(lo) + "," + std::to_string(hi) + "]";
        return false;
    }
    return true;
}
}

bool loadConfig(const std::string& path, Config& out, std::string& error) {
    // try to open the config file using the path provided
    std::ifstream file(path);

    // check if the file was successfully opened
    if(!file.is_open()) {
        error = "Could not open config files: " + path;
        return false;
    }

    Config cfg;
    std::set<std::string> seen;
    std::string line;
    int lineNo = 0;

    // read the config file one line at a time
    while (std::getline(file, line)) {
        lineNo++;
        line = trim(line);
        
        if(line.empty()) 
            continue;

        // separate the line into indivudal words
        std::istringstream iss(line);
        std:: string key, value, extra;
        iss >> key >> value;

        if(key.empty() || value.empty() || (iss >> extra)) {
            error = "Line " + std::to_string(lineNo) + ": expected '<parameter> <value>'";
            return false;
        }

        key = toLower(key);
        value = stripQuotes(value);

        uint64_t n = 0;

        std::string err;

        bool ok = true; // assume the parameter is valid unless a check fails

        if (key == "num-cpu") {
            ok = parseInRange(key, value, 1, 128, n, err);
            if (ok)
                cfg.numCpu = static_cast<uint32_t>(n);
        } else if(key == "scheduler") {
            std::string v = toLower(value);
            if (v == "fcfs") 
                cfg.scheduler = SchedulerType::FCFS;
            else if (v == "rr") 
                cfg.scheduler = SchedulerType::RR;
            else {
                ok = false;
                err = "scheduler must be fcfs or rr"; 
            }
        } else if (key == "quantum-cycles") {
            ok = parseInRange(key, value, 1, MAX_U32_RANGE, n, err);
            if (ok)
                cfg.quantumCycles = n;
        } else if (key == "batch-process-freq") {
            ok = parseInRange(key, value, 1, MAX_U32_RANGE, n, err);
            if (ok)
                cfg.batchProcessFreq = n;
        } else if (key == "min-ins") {
            ok = parseInRange(key, value, 1, MAX_U32_RANGE, n, err);
            if (ok) 
                cfg.minIns = n; 
        } else if (key == "max-ins") {
            ok = parseInRange(key, value, 1, MAX_U32_RANGE, n, err);
            if (ok) 
                cfg.maxIns = n; 
        } else if (key == "delay-per-exec") {
            ok = parseInRange(key, value, 0, MAX_U32_RANGE, n, err);
            if (ok) 
                cfg.delayPerExec = n; 
        } else {
            ok = false;
            err = "unknown parameter '" + key + "'";
        }

        if (!ok) {
            error = "Line " + std::to_string(lineNo) + ": " + err;
            return false;
        }

        seen.insert(key);
    } 

    static const char* required[] = {"num-cpu", "scheduler", "quantum-cycles", "batch-process-freq", "min-ins", "max-ins", "delay-per-exec"};

    for (const char* r : required) {
        if (!seen.count(r)) {
            error = std::string("Missing parameter: ") + r;
            return false;
        }
    }

    if (cfg.minIns > cfg.maxIns) {
        error = "min-ins must not be greater than max-ins";
        return false;
    }
    
    out = cfg;
    return true;
}

std::string schedulerToString(SchedulerType s) {
    return s == SchedulerType::FCFS ? "fcfs" : "rr";
}