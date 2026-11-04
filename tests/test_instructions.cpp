// Instruction tests. Owner: Karl
// Covers: clamping, auto-declare, ADD/SUBTRACT, PRINT, SLEEP, nested FOR,
// Process::executeNext, and the random program generator.
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "instructions/AddInstruction.h"
#include "instructions/DeclareInstruction.h"
#include "instructions/ForInstruction.h"
#include "instructions/Instruction.h"
#include "instructions/InstructionFactory.h"
#include "instructions/Operand.h"
#include "instructions/PrintInstruction.h"
#include "instructions/SleepInstruction.h"
#include "instructions/SubtractInstruction.h"
#include "process/Process.h"
#include "process/SymbolTable.h"

// ---------- fake instruction ----------

class NoOpInstruction : public Instruction {
public:
    void execute(Process& process) override { (void)process; }
    std::string toString() const override { return "noop"; }
};

// ---------- tiny test helpers ----------

static int failures = 0;

static void checkEqual(const std::string& name, long long actual, long long expected) {
    if (actual == expected) {
        std::cout << "  PASS: " << name << std::endl;
    } else {
        std::cout << "  FAIL: " << name << " (expected " << expected
                  << ", got " << actual << ")" << std::endl;
        failures++;
    }
}

static void checkText(const std::string& name, const std::string& actual, const std::string& expected) {
    if (actual == expected) {
        std::cout << "  PASS: " << name << std::endl;
    } else {
        std::cout << "  FAIL: " << name << " (expected '" << expected
                  << "', got '" << actual << "')" << std::endl;
        failures++;
    }
}

static void checkTrue(const std::string& name, bool condition) {
    if (condition) {
        std::cout << "  PASS: " << name << std::endl;
    } else {
        std::cout << "  FAIL: " << name << std::endl;
        failures++;
    }
}

// A process with no program. We call instructions on it directly.
static std::shared_ptr<Process> makeEmptyProcess() {
    std::vector<std::shared_ptr<Instruction>> program;
    return std::make_shared<Process>(1, "t", program);
}

// ---------- tests ----------

static void testClamp() {
    std::cout << "CLAMP: SymbolTable::clamp" << std::endl;
    checkEqual("-5 becomes 0", SymbolTable::clamp(-5), 0);
    checkEqual("70000 becomes 65535", SymbolTable::clamp(70000), 65535);
    checkEqual("123 stays 123", SymbolTable::clamp(123), 123);
}

static void testDeclare() {
    std::cout << "DECLARE: x = 10" << std::endl;
    std::shared_ptr<Process> p = makeEmptyProcess();
    DeclareInstruction declare("x", 10);
    declare.execute(*p);
    checkEqual("x is 10", p->symbols().get("x"), 10);
}

static void testAutoDeclare() {
    std::cout << "AUTO-DECLARE: ADD(a, q, 5) where q was never declared" << std::endl;
    std::shared_ptr<Process> p = makeEmptyProcess();
    checkTrue("q does not exist yet", !p->symbols().exists("q"));

    AddInstruction add("a", Operand::variable("q"), Operand::literal(5));
    add.execute(*p);

    checkEqual("a is 5", p->symbols().get("a"), 5);
    checkTrue("q was auto-declared", p->symbols().exists("q"));
    checkEqual("q is 0", p->symbols().get("q"), 0);
}

static void testAdd() {
    std::cout << "ADD: normal and clamped" << std::endl;
    std::shared_ptr<Process> p = makeEmptyProcess();

    AddInstruction small("sum", Operand::literal(3), Operand::literal(4));
    small.execute(*p);
    checkEqual("3 + 4 = 7", p->symbols().get("sum"), 7);

    DeclareInstruction declare("x", 65535);
    declare.execute(*p);
    AddInstruction big("x", Operand::variable("x"), Operand::literal(10));
    big.execute(*p);
    checkEqual("65535 + 10 stays 65535", p->symbols().get("x"), 65535);
}

static void testSubtract() {
    std::cout << "SUBTRACT: normal and clamped" << std::endl;
    std::shared_ptr<Process> p = makeEmptyProcess();

    SubtractInstruction normal("d", Operand::literal(10), Operand::literal(3));
    normal.execute(*p);
    checkEqual("10 - 3 = 7", p->symbols().get("d"), 7);

    SubtractInstruction negative("e", Operand::literal(3), Operand::literal(10));
    negative.execute(*p);
    checkEqual("3 - 10 stays 0", p->symbols().get("e"), 0);
}

static void testPrint() {
    std::cout << "PRINT: plain message and message with a variable" << std::endl;
    std::shared_ptr<Process> p = makeEmptyProcess();

    PrintInstruction plain("Hello world from p01!");
    plain.execute(*p);
    checkEqual("one log entry", (long long)p->logs().size(), 1);
    checkText("log message", p->logs()[0].message, "Hello world from p01!");

    DeclareInstruction declare("x", 5);
    declare.execute(*p);
    PrintInstruction withVar("Value from: ", "x");
    withVar.execute(*p);
    checkEqual("two log entries", (long long)p->logs().size(), 2);
    checkText("log message with variable", p->logs()[1].message, "Value from: 5");
}

static void testSleep() {
    std::cout << "SLEEP: SLEEP(3) puts the process in SLEEPING" << std::endl;
    std::shared_ptr<Process> p = makeEmptyProcess();
    SleepInstruction sleep(3);
    sleep.execute(*p);
    checkTrue("process is sleeping", p->isSleeping());
}

static void testExecuteNext() {
    std::cout << "EXECUTE-NEXT: process with 3 instructions" << std::endl;
    std::vector<std::shared_ptr<Instruction>> program;
    program.push_back(std::make_shared<NoOpInstruction>());
    program.push_back(std::make_shared<NoOpInstruction>());
    program.push_back(std::make_shared<NoOpInstruction>());
    Process p(1, "t", program);

    p.executeNext();
    p.executeNext();
    checkEqual("current line is 2", (long long)p.currentLine(), 2);
    checkTrue("not finished after 2", !p.isFinished());

    p.executeNext();
    checkEqual("current line is 3", (long long)p.currentLine(), 3);
    checkTrue("finished after 3", p.isFinished());
}

static void testForExpand() {
    std::cout << "FOR: expand() and nesting" << std::endl;

    // FOR([noop, noop], 3) -> 6
    std::vector<std::shared_ptr<Instruction>> body;
    body.push_back(std::make_shared<NoOpInstruction>());
    body.push_back(std::make_shared<NoOpInstruction>());
    ForInstruction simple(body, 3);
    checkEqual("2 instructions x 3 repeats = 6", (long long)simple.expand().size(), 6);

    // FOR([FOR([FOR([noop], 2)], 2)], 2) -> 8 (three levels)
    std::vector<std::shared_ptr<Instruction>> level1Body;
    level1Body.push_back(std::make_shared<NoOpInstruction>());
    std::shared_ptr<ForInstruction> level1 = std::make_shared<ForInstruction>(level1Body, 2);

    std::vector<std::shared_ptr<Instruction>> level2Body;
    level2Body.push_back(level1);
    std::shared_ptr<ForInstruction> level2 = std::make_shared<ForInstruction>(level2Body, 2);

    std::vector<std::shared_ptr<Instruction>> level3Body;
    level3Body.push_back(level2);
    ForInstruction level3(level3Body, 2);

    checkEqual("3 nested levels, 2 x 2 x 2 = 8", (long long)level3.expand().size(), 8);

    // zero and negative repeats give an empty loop
    ForInstruction zero(body, 0);
    checkEqual("0 repeats = 0 instructions", (long long)zero.expand().size(), 0);
    ForInstruction negative(body, -4);
    checkEqual("negative repeats = 0 instructions", (long long)negative.expand().size(), 0);
}

static void testFactory() {
    std::cout << "FACTORY: 200 random programs, min 10, max 20" << std::endl;
    bool allInRange = true;
    bool noFor = true;
    bool printsOk = true;
    std::string expectedPrint = "PRINT(\"Hello world from p01!\")";

    for (int i = 0; i < 200; i++) {
        std::vector<std::shared_ptr<Instruction>> program =
            InstructionFactory::generateProgram("p01", 10, 20);

        if (program.size() < 10 || program.size() > 20) {
            allInRange = false;
        }
        for (size_t j = 0; j < program.size(); j++) {
            if (std::dynamic_pointer_cast<ForInstruction>(program[j])) {
                noFor = false;
            }
            std::string text = program[j]->toString();
            if (text.rfind("PRINT(", 0) == 0 && text != expectedPrint) {
                printsOk = false;
            }
        }
    }
    checkTrue("length always between 10 and 20", allInRange);
    checkTrue("program is flat (no FOR left)", noFor);
    checkTrue("PRINT text is always 'Hello world from p01!'", printsOk);

    std::vector<std::shared_ptr<Instruction>> exact = InstructionFactory::generateProgram("p01", 5, 5);
    checkEqual("min = max = 5 gives exactly 5", (long long)exact.size(), 5);
}

int main() {
    testClamp();
    testDeclare();
    testAutoDeclare();
    testAdd();
    testSubtract();
    testPrint();
    testSleep();
    testExecuteNext();
    testForExpand();
    testFactory();

    if (failures == 0) {
        std::cout << "ALL TESTS PASSED" << std::endl;
        return 0;
    }
    std::cout << failures << " CHECK(S) FAILED" << std::endl;
    return 1;
}
