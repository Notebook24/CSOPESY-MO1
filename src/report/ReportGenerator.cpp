#include "report/ReportGenerator.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "process/Process.h"
#include "process/ProcessState.h"
#include "process/ProcessTable.h"
#include "scheduler/Scheduler.h"

namespace {

const char* const kRule = "--------------------------------------";

// Wrap in parentheses unless the timestamp already has them.
std::string stamp(const std::string& ts) {
    if (!ts.empty() && ts.front() == '(') return ts;
    return "(" + ts + ")";
}

}  // namespace

std::string ReportGenerator::buildReport(const ProcessTable& table, const Scheduler& scheduler) {
    const int total = scheduler.getNumCores();
    const int busy  = std::min(scheduler.getBusyCores(), total);
    const int avail = total - busy;
    const int util  = total > 0 ? (busy * 100) / total : 0;

    std::vector<Process*> all = table.getAll();
    std::sort(all.begin(), all.end(),
              [](const Process* a, const Process* b) { return a->getId() < b->getId(); });

    std::vector<const Process*> running, finished;
    size_t nameWidth = 0;
    for (const Process* p : all) {
        nameWidth = std::max(nameWidth, p->getName().size());
        if (p->isFinished())                         finished.push_back(p);
        else if (p->getState() == ProcessState::RUNNING) running.push_back(p);
        // READY / SLEEPING processes are waiting, not on a core: not listed.
    }

    std::ostringstream out;
    out << "CPU utilization: " << util << "%\n"
        << "Cores used: " << busy << "\n"
        << "Cores available: " << avail << "\n\n"
        << kRule << "\n"
        << "Running processes:\n";
    for (const Process* p : running) {
        out << std::left << std::setw(static_cast<int>(nameWidth)) << p->getName() << "  "
            << stamp(p->getStartTime()) << "  Core: " << p->getCoreId() << "  "
            << p->getInstructionPointer() << " / " << p->getTotalInstructions() << "\n";
    }
    out << "\nFinished processes:\n";
    for (const Process* p : finished) {
        out << std::left << std::setw(static_cast<int>(nameWidth)) << p->getName() << "  "
            << stamp(p->getStartTime()) << "  Finished  "
            << p->getTotalInstructions() << " / " << p->getTotalInstructions() << "\n";
    }
    out << kRule << "\n";
    return out.str();
}

bool ReportGenerator::saveToFile(const std::string& path, const std::string& text) {
    std::ofstream file(path, std::ios::out | std::ios::trunc);
    if (!file) return false;
    file << text;
    return static_cast<bool>(file);
}
