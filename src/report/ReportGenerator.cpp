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
    const int total = scheduler.getTotalCores();
    const int busy  = scheduler.getBusyCores();
    const int avail = total - busy;
    const int util  = total > 0 ? (busy * 100) / total : 0;

    // all() is already in creation order
    std::vector<std::shared_ptr<Process>> all = table.all();
    std::vector<std::shared_ptr<Process>> running;
    std::vector<std::shared_ptr<Process>> finished;
    size_t nameWidth = 0;

    for (size_t i = 0; i < all.size(); i++) {
        const std::shared_ptr<Process>& p = all[i];
        if (p->name().size() > nameWidth) nameWidth = p->name().size();
        if (p->isFinished()) {
            finished.push_back(p);
        } else if (p->state() == ProcessState::RUNNING) {
            running.push_back(p);
        }
    }

    std::ostringstream out;
    out << "CPU utilization: " << util << "%\n"
        << "Cores used: " << busy << "\n"
        << "Cores available: " << avail << "\n\n"
        << kRule << "\n"
        << "Running processes:\n";
    for (size_t i = 0; i < running.size(); i++) {
        const std::shared_ptr<Process>& p = running[i];
        out << std::left << std::setw(static_cast<int>(nameWidth)) << p->name() << "  "
            << stamp(p->createdAt()) << "  Core: " << p->coreId() << "  "
            << p->currentLine() << " / " << p->totalLines() << "\n";
    }
    out << "\nFinished processes:\n";
    for (size_t i = 0; i < finished.size(); i++) {
        const std::shared_ptr<Process>& p = finished[i];
        out << std::left << std::setw(static_cast<int>(nameWidth)) << p->name() << "  "
            << stamp(p->createdAt()) << "  Finished  "
            << p->totalLines() << " / " << p->totalLines() << "\n";
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
