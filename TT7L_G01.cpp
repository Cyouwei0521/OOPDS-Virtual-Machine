/* ==========================================================================
 * Group ID: TT7L_G01
 * Members:  Chong You Wei 242UC2431Z
 *           Tee Li Qing 261UC2606W
 *           Tiang Li Yuan 252UC241LG
 *           Chia Yee Shuen 252UC24306
 * ==========================================================================
 * Strictly no STL containers (vector, stack, queue, etc.)
 * ========================================================================== */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

/* ==========================================================================
 * SECTION 1: CUSTOM DATA STRUCTURES (no STL allowed)
 * Author: Chong You Wei 242UC2431Z
 * ========================================================================== */

// Custom dynamic array, replaces std::vector.
// Used for: instruction-line storage, and the final list of decoded
// Instruction* objects executed by the CPU (polymorphic storage).
template <typename T>
class CustomVector {
    private:
    T* arr;
    int capacity;
    int current_size;

    // resize the array when it gets full
    void resize() {
        capacity *= 2;
        T* temp = new T[capacity];
        for (int i = 0; i < current_size; i++) { temp[i] = arr[i]; }
        delete[] arr;
        arr = temp;
    }

    public:
    CustomVector() {
        capacity = 10;
        current_size = 0;
        arr = new T[capacity];
    }
    ~CustomVector() { delete[] arr; }

    void push_back(T data) {
        if (current_size == capacity) { resize(); }
        arr[current_size] = data;
        current_size++;
    }
    T& get(int index) { return arr[index]; }
    int size() { return current_size; }
};

// Custom singly-linked-list queue, replaces std::queue.
// Used by the Runner to buffer raw lines read from the .asm file before
// they are validated and copied into a CustomVector<string> for indexed
// (random) access during parsing.
template <typename T>
class CustomQueue {
    private:
    struct Node {
        T data;
        Node* next;
        Node(T value) : data(value), next(nullptr) {}
    };
    Node* head;
    Node* tail;
    int count;

    public:
    CustomQueue() { head = nullptr; tail = nullptr; count = 0; }
    ~CustomQueue() { while (!isEmpty()) { dequeue(); } }

    void enqueue(T value) {
        Node* node = new Node(value);
        if (head == nullptr) { head = node; tail = node; }
        else { tail->next = node; tail = node; }
        count++;
    }

    T dequeue() {
        T value = head->data;
        Node* temp = head;
        head = head->next;
        if (head == nullptr) { tail = nullptr; }
        delete temp;
        count--;
        return value;
    }

    bool isEmpty() { return head == nullptr; }
    int size() { return count; }
};

// Custom stack for the VM (max size 8 bytes), replaces std::stack.
// Author: Chong You Wei 242UC2431Z
class CustomStack {
    private:
    signed char elements[8];
    int top_index;

    public:
    CustomStack() { top_index = -1; }

    void push(signed char value) {
        if (top_index >= 7) {
            cout << "Error: Stack Overflow" << endl;
            return; // FIX: must not fall through to an out-of-bounds write
        }
        top_index++;
        elements[top_index] = value;
    }

    signed char pop() {
        if (top_index < 0) {
            cout << "Error: Stack Underflow" << endl;
            exit(1); // pop on empty stack crashes the program, as required
        }
        signed char popped_value = elements[top_index];
        top_index--;
        return popped_value;
    }

    // get current stack index
    int getSI() { return top_index + 1; }
};

/* ==========================================================================
 * SECTION 2: MEMORY
 * Author: Chong You Wei 242UC2431Z
 * ========================================================================== */
class Memory {
    private:
    signed char data[64];

    public:
    Memory() {
        for (int i = 0; i < 64; i++) { data[i] = 0; }
    }

    signed char read(int address) {
        if (address < 0 || address > 63) {
            cout << "Error: Memory read out of bounds at address " << address << endl;
            return 0;
        }
        return data[address];
    }

    void write(int address, signed char value) {
        if (address < 0 || address > 63) {
            cout << "Error: Memory write out of bounds at address " << address << endl;
            return;
        }
        data[address] = value;
    }
};

/* ==========================================================================
 * SECTION 3: REGISTERS AND FLAGS
 * Author: Tee Li Qing 261UC2606W
 * ========================================================================== */
class Register {
    private:
    signed char value;

    public:
    Register() { value = 0; }
    virtual void setValue(signed char val) { value = val; }
    virtual signed char getValue() { return value; }
    virtual ~Register() {}
};

// Inheritance hierarchy #1: GeneralRegister IS-A Register.
class GeneralRegister : public Register {
    public:
    // inherits everything from Register; kept separate so register-specific
    // behaviour can be added later without touching the base class
};

class FlagRegister {
    private:
    bool OF;
    bool UF;
    bool CF;
    bool ZF;

    public:
    FlagRegister() {
        OF = false;
        UF = false;
        CF = false;
        ZF = false;
    }

    void setOF(bool val) { OF = val; }
    void setUF(bool val) { UF = val; }
    void setCF(bool val) { CF = val; }
    void setZF(bool val) { ZF = val; }

    bool getOF() { return OF; }
    bool getUF() { return UF; }
    bool getCF() { return CF; }
    bool getZF() { return ZF; }
};

class CPU;

/* ==========================================================================
 * SECTION 4: INSTRUCTION ABSTRACT BASE CLASS
 * Author: Chong You Wei 242UC2431Z
 * ========================================================================== */
class Instruction {
    public:
    virtual void execute(CPU* cpu) = 0;
    virtual ~Instruction() {}
};

/* ==========================================================================
 * SECTION 5: CPU
 * Composition: CPU owns Memory and CustomStack directly (they cannot
 * outlive the CPU and are created/destroyed with it).
 * Aggregation: CPU also exposes GeneralRegister[] and FlagRegister, which
 * conceptually exist independently and are simply accessed through CPU.
 * Author: Chong You Wei 242UC2431Z
 * ========================================================================== */
class CPU {
    private:
    GeneralRegister registers[8];
    FlagRegister flags;
    Memory memory;
    CustomStack stack;
    int PC;

    public:
    CPU() { PC = 0; }
    GeneralRegister& getRegister(int index) {
        if (index < 0 || index > 7) {
            cout << "Error: Invalid register index " << index << endl;
            exit(1);
        }
        return registers[index];
    }
    FlagRegister& getFlags() { return flags; }
    Memory& getMemory() { return memory; }
    CustomStack& getStack() { return stack; }
    int getPC() { return PC; }
    void incrementPC() { PC++; }
};

/* ==========================================================================
 * SECTION 6: ARITHMETIC INSTRUCTIONS (ADD, SUB, MUL, DIV, INC, DEC)
 * Inheritance hierarchy #2: ArithmeticInstruction IS-A Instruction, and
 * AddInstruction/SubInstruction/MulInstruction/DivInstruction IS-A
 * ArithmeticInstruction. Polymorphism #1: the Runner calls execute()
 * through an Instruction* pointer without knowing the concrete subtype.
 * Author: Tee Li Qing 261UC2606W
 * ========================================================================== */
class ArithmeticHelper {
    private:
    static signed char normalizeToByte(int result) {
        int value = result % 256;
        if (value < 0) { value += 256; }
        if (value > 127) { value -= 256; }
        return (signed char)value;
    }

    public:
    static void updateFlags(CPU* cpu, int result) {
        FlagRegister& flags = cpu->getFlags();
        flags.setOF(result > 127);
        flags.setUF(result < -128);
        flags.setCF(result > 127 || result < -128);
        flags.setZF(result == 0);
    }

    static void storeArithmeticResult(CPU* cpu, int regIndex, int result) {
        updateFlags(cpu, result);
        cpu->getRegister(regIndex).setValue(normalizeToByte(result));
    }
};

// NOTE: section 3.5 of the spec says both operands of ADD/SUB/MUL/DIV are
// registers, but the worked example in section 6 uses "ADD R1, 6" and
// "MUL R3, 4" with an immediate second operand. To stay compatible with
// the spec's own sample program, the second operand may be EITHER a
// register OR an immediate value.
class ArithmeticInstruction : public Instruction {
    private:
    int destRegIndex;
    int srcRegIndex;      // -1 when the second operand is an immediate
    signed char immValue;  // only meaningful when srcRegIndex == -1
    bool isImmediate;

    protected:
    int getDestRegIndex() { return destRegIndex; }

    // resolves the second operand, whether it is a register or immediate
    int getSourceValue(CPU* cpu) {
        if (isImmediate) { return (int)immValue; }
        return (int)cpu->getRegister(srcRegIndex).getValue();
    }

    public:
    // ADD/SUB/MUL/DIV Rd, Rs  (register form)
    ArithmeticInstruction(int dest, int src)
        : destRegIndex(dest), srcRegIndex(src), immValue(0), isImmediate(false) {}

    // ADD/SUB/MUL/DIV Rd, <immediate>  (immediate form)
    ArithmeticInstruction(int dest, signed char imm)
        : destRegIndex(dest), srcRegIndex(-1), immValue(imm), isImmediate(true) {}
};

class AddInstruction : public ArithmeticInstruction {
    public:
    AddInstruction(int dest, int src) : ArithmeticInstruction(dest, src) {}
    AddInstruction(int dest, signed char imm) : ArithmeticInstruction(dest, imm) {}

    void execute(CPU* cpu) override {
        int dest = getDestRegIndex();
        int result = (int)cpu->getRegister(dest).getValue() + getSourceValue(cpu);
        ArithmeticHelper::storeArithmeticResult(cpu, dest, result);
        cpu->incrementPC();
    }
};

class SubInstruction : public ArithmeticInstruction {
    public:
    SubInstruction(int dest, int src) : ArithmeticInstruction(dest, src) {}
    SubInstruction(int dest, signed char imm) : ArithmeticInstruction(dest, imm) {}

    void execute(CPU* cpu) override {
        int dest = getDestRegIndex();
        int result = (int)cpu->getRegister(dest).getValue() - getSourceValue(cpu);
        ArithmeticHelper::storeArithmeticResult(cpu, dest, result);
        cpu->incrementPC();
    }
};

class MulInstruction : public ArithmeticInstruction {
    public:
    MulInstruction(int dest, int src) : ArithmeticInstruction(dest, src) {}
    MulInstruction(int dest, signed char imm) : ArithmeticInstruction(dest, imm) {}

    void execute(CPU* cpu) override {
        int dest = getDestRegIndex();
        int result = (int)cpu->getRegister(dest).getValue() * getSourceValue(cpu);
        ArithmeticHelper::storeArithmeticResult(cpu, dest, result);
        cpu->incrementPC();
    }
};

class DivInstruction : public ArithmeticInstruction {
    public:
    DivInstruction(int dest, int src) : ArithmeticInstruction(dest, src) {}
    DivInstruction(int dest, signed char imm) : ArithmeticInstruction(dest, imm) {}

    void execute(CPU* cpu) override {
        int dest = getDestRegIndex();
        int divisor = getSourceValue(cpu);
        if (divisor == 0) {
            cout << "Error: Division by zero" << endl;
            exit(1);
        }
        int result = (int)cpu->getRegister(dest).getValue() / divisor;
        ArithmeticHelper::storeArithmeticResult(cpu, dest, result);
        cpu->incrementPC();
    }
};

class IncInstruction : public Instruction {
    private:
    int destRegIndex;

    public:
    IncInstruction(int dest) { destRegIndex = dest; }

    void execute(CPU* cpu) override {
        int result = (int)cpu->getRegister(destRegIndex).getValue() + 1;
        ArithmeticHelper::storeArithmeticResult(cpu, destRegIndex, result);
        cpu->incrementPC();
    }
};

class DecInstruction : public Instruction {
    private:
    int destRegIndex;

    public:
    DecInstruction(int dest) { destRegIndex = dest; }

    void execute(CPU* cpu) override {
        int result = (int)cpu->getRegister(destRegIndex).getValue() - 1;
        ArithmeticHelper::storeArithmeticResult(cpu, destRegIndex, result);
        cpu->incrementPC();
    }
};

/* ==========================================================================
 * SECTION 7: FLAG RESET INSTRUCTION
 * Author: Tee Li Qing 261UC2606W
 * ========================================================================== */
class ResetInstruction : public Instruction {
    private:
    string targetFlag;

    public:
    ResetInstruction(string flagName) { targetFlag = flagName; }

    void execute(CPU* cpu) override {
        if (targetFlag == "OF") { cpu->getFlags().setOF(false); }
        else if (targetFlag == "UF") { cpu->getFlags().setUF(false); }
        else if (targetFlag == "CF") { cpu->getFlags().setCF(false); }
        else if (targetFlag == "ZF") { cpu->getFlags().setZF(false); }
        else {
            cout << "Error: Invalid flag name " << targetFlag << endl;
            exit(1);
        }
        cpu->incrementPC();
    }
};

/* ==========================================================================
 * SECTION 8: MOV / LOAD / STORE (data movement)
 * Author: Tiang Li Yuan 252UC241LG
 * ========================================================================== */
enum class MovMode { IMMEDIATE, REGISTER, INDIRECT };

class MovInstruction : public Instruction {
    private:
    int destRegIndex;
    MovMode mode;
    signed char immValue;
    int srcRegIndex;

    public:
    MovInstruction(int dest, signed char val)
        : destRegIndex(dest), mode(MovMode::IMMEDIATE), immValue(val), srcRegIndex(-1) {}

    MovInstruction(int dest, int src, bool indirect)
        : destRegIndex(dest), mode(indirect ? MovMode::INDIRECT : MovMode::REGISTER),
          immValue(0), srcRegIndex(src) {}

    void execute(CPU* cpu) override {
        int result;
        switch (mode) {
            case MovMode::IMMEDIATE:
                result = (int)immValue;
                break;
            case MovMode::REGISTER:
                result = (int)cpu->getRegister(srcRegIndex).getValue();
                break;
            default: { // INDIRECT
                int memAddress = cpu->getRegister(srcRegIndex).getValue();
                result = (int)cpu->getMemory().read(memAddress);
                break;
            }
        }
        // MOV writes a destination register, so flags update per spec 3.10.
        // Values here are always already in [-128,127] since they come from
        // an existing register/memory byte or a validated immediate, so this
        // only ever sets ZF true/false -- OF/UF/CF stay false.
        ArithmeticHelper::storeArithmeticResult(cpu, destRegIndex, result);
        cpu->incrementPC();
    }
};

class LoadInstruction : public Instruction {
    private:
    int destRegIndex;
    bool useRegisterAddress;
    int addressOrRegIndex;

    public:
    LoadInstruction(int dest, int addressOrReg, bool useRegisterAddress)
        : destRegIndex(dest), useRegisterAddress(useRegisterAddress), addressOrRegIndex(addressOrReg) {}

    void execute(CPU* cpu) override {
        int address = useRegisterAddress
            ? (int)cpu->getRegister(addressOrRegIndex).getValue()
            : addressOrRegIndex;
        signed char value = cpu->getMemory().read(address);
        ArithmeticHelper::storeArithmeticResult(cpu, destRegIndex, (int)value);
        cpu->incrementPC();
    }
};

class StoreInstruction : public Instruction {
    private:
    int addressOrRegIndex;
    int srcRegIndex;
    bool useRegisterAddress;

    public:
    StoreInstruction(int destAddrOrReg, int src, bool useRegAddr)
        : addressOrRegIndex(destAddrOrReg), srcRegIndex(src), useRegisterAddress(useRegAddr) {}

    void execute(CPU* cpu) override {
        int address = useRegisterAddress
            ? (int)cpu->getRegister(addressOrRegIndex).getValue()
            : addressOrRegIndex;
        signed char value = cpu->getRegister(srcRegIndex).getValue();
        cpu->getMemory().write(address, value);
        cpu->incrementPC();
    }
};

/* ==========================================================================
 * SECTION 9: STACK INSTRUCTIONS (PUSH / POP)
 * Author: Tiang Li Yuan 252UC241LG
 * ========================================================================== */
class PushInstruction : public Instruction {
    private:
    int srcRegIndex;

    public:
    PushInstruction(int srcRegIndex) : srcRegIndex(srcRegIndex) {}

    void execute(CPU* cpu) override {
        signed char value = cpu->getRegister(srcRegIndex).getValue();
        cpu->getStack().push(value);
        cpu->incrementPC();
    }
};

class PopInstruction : public Instruction {
    private:
    int destRegIndex;

    public:
    PopInstruction(int destRegIndex) : destRegIndex(destRegIndex) {}

    void execute(CPU* cpu) override {
        // CustomStack::pop() already exit(1)s on an empty stack.
        signed char value = cpu->getStack().pop();
        ArithmeticHelper::storeArithmeticResult(cpu, destRegIndex, (int)value);
        cpu->incrementPC();
    }
};

/* ==========================================================================
 * SECTION 10: I/O INSTRUCTIONS (INPUT / DISPLAY)
 * Author: Tiang Li Yuan 252UC241LG
 * ========================================================================== */
class InputInstruction : public Instruction {
    private:
    int destRegIndex;

    public:
    InputInstruction(int destRegIndex) : destRegIndex(destRegIndex) {}

    void execute(CPU* cpu) override {
        cout << "?";
        int rawInput;
        cin >> rawInput;

        if (cin.fail()) {
            cout << "Error: Invalid input, expected a number" << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            rawInput = 0;
        }

        FlagRegister& f = cpu->getFlags();
        f.setOF(rawInput > 127);
        f.setUF(rawInput < -128);
        f.setZF(rawInput == 0);

        signed char stored;
        if (rawInput > 127) stored = 127;
        else if (rawInput < -128) stored = -128;
        else stored = (signed char)rawInput;

        cpu->getRegister(destRegIndex).setValue(stored);
        cpu->incrementPC();
    }
};

class DisplayInstruction : public Instruction {
    private:
    int srcRegIndex;

    public:
    DisplayInstruction(int srcRegIndex) : srcRegIndex(srcRegIndex) {}

    void execute(CPU* cpu) override {
        cout << (int)cpu->getRegister(srcRegIndex).getValue() << endl;
        cpu->incrementPC();
    }
};

/* ==========================================================================
 * SECTION 11: ROTATE / SHIFT INSTRUCTIONS (ROL, ROR, SHL, SHR)
 * Inheritance hierarchy #3 + Polymorphism #2: ShiftInstruction IS-A
 * Instruction, and RolInstruction/RorInstruction/ShlInstruction/
 * ShrInstruction IS-A ShiftInstruction, each overriding execute().
 *
 * Bit convention used below: bits[0] = MSB (bit 7), bits[7] = LSB (bit 0).
 * This matches the worked examples in the assignment specification.
 * Author: Chia Yee Shuen 252UC24306
 * ========================================================================== */
class ShiftInstruction : public Instruction {
    private:
    int destRegIndex;
    int count;

    protected:
    int getDestRegIndex() { return destRegIndex; }
    int getCount() { return count; }

    static void toBits(signed char value, int bits[8]) {
        unsigned char uVal = (unsigned char)value;
        for (int i = 7; i >= 0; i--) {
            bits[i] = uVal & 1;
            uVal = uVal >> 1;
        }
    }

    static signed char fromBits(int bits[8]) {
        unsigned char uVal = 0;
        for (int i = 0; i < 8; i++) {
            uVal = (unsigned char)((uVal << 1) | bits[i]);
        }
        return (signed char)uVal;
    }

    // shared flag update for shift/rotate: only ZF is well-defined by spec
    static void updateZeroFlag(CPU* cpu, signed char result) {
        cpu->getFlags().setZF(result == 0);
    }

    public:
    ShiftInstruction(int dest, int cnt) : destRegIndex(dest), count(cnt) {}
};

class RolInstruction : public ShiftInstruction {
    public:
    RolInstruction(int dest, int cnt) : ShiftInstruction(dest, cnt) {}

    void execute(CPU* cpu) override {
        int bits[8];
        toBits(cpu->getRegister(getDestRegIndex()).getValue(), bits);
        for (int step = 0; step < getCount(); step++) {
            int wrapped = bits[0];
            for (int i = 0; i < 7; i++) { bits[i] = bits[i + 1]; }
            bits[7] = wrapped;
        }
        signed char result = fromBits(bits);
        cpu->getRegister(getDestRegIndex()).setValue(result);
        updateZeroFlag(cpu, result);
        cpu->incrementPC();
    }
};

class RorInstruction : public ShiftInstruction {
    public:
    RorInstruction(int dest, int cnt) : ShiftInstruction(dest, cnt) {}

    void execute(CPU* cpu) override {
        int bits[8];
        toBits(cpu->getRegister(getDestRegIndex()).getValue(), bits);
        for (int step = 0; step < getCount(); step++) {
            int wrapped = bits[7];
            for (int i = 7; i > 0; i--) { bits[i] = bits[i - 1]; }
            bits[0] = wrapped;
        }
        signed char result = fromBits(bits);
        cpu->getRegister(getDestRegIndex()).setValue(result);
        updateZeroFlag(cpu, result);
        cpu->incrementPC();
    }
};

class ShlInstruction : public ShiftInstruction {
    public:
    ShlInstruction(int dest, int cnt) : ShiftInstruction(dest, cnt) {}

    void execute(CPU* cpu) override {
        int bits[8];
        toBits(cpu->getRegister(getDestRegIndex()).getValue(), bits);
        for (int step = 0; step < getCount(); step++) {
            for (int i = 0; i < 7; i++) { bits[i] = bits[i + 1]; }
            bits[7] = 0;
        }
        signed char result = fromBits(bits);
        cpu->getRegister(getDestRegIndex()).setValue(result);
        updateZeroFlag(cpu, result);
        cpu->incrementPC();
    }
};

class ShrInstruction : public ShiftInstruction {
    public:
    ShrInstruction(int dest, int cnt) : ShiftInstruction(dest, cnt) {}

    void execute(CPU* cpu) override {
        int bits[8];
        toBits(cpu->getRegister(getDestRegIndex()).getValue(), bits);
        for (int step = 0; step < getCount(); step++) {
            for (int i = 7; i > 0; i--) { bits[i] = bits[i - 1]; }
            bits[0] = 0;
        }
        signed char result = fromBits(bits);
        cpu->getRegister(getDestRegIndex()).setValue(result);
        updateZeroFlag(cpu, result);
        cpu->incrementPC();
    }
};

/* ==========================================================================
 * SECTION 12: RUNNER (the interpreter / loader / output formatter)
 * Coordinates everything: reads the .asm file, decodes each line into an
 * Instruction object, executes the program via the CPU, then dumps the
 * final state to the screen and to a file.
 * Author: Chia Yee Shuen 252UC24306 & Tiang Li Yuan 252UC241LG
 * ========================================================================== */
class Runner {
    private:
    CPU cpu;
    CustomVector<Instruction*> instructions;

    // ---- small string helpers (no STL algorithms used) ----

    static string trim(const string& s) {
        int start = 0;
        int end = (int)s.length() - 1;
        while (start <= end && isspace((unsigned char)s[start])) { start++; }
        while (end >= start && isspace((unsigned char)s[end])) { end--; }
        if (start > end) { return ""; }
        return s.substr(start, end - start + 1);
    }

    static string toUpper(const string& s) {
        string result = s;
        for (int i = 0; i < (int)result.length(); i++) {
            result[i] = (char)toupper((unsigned char)result[i]);
        }
        return result;
    }

    static bool isOpcode(const string& token) {
        string t = toUpper(token);
        const char* opcodes[] = {
            "MOV", "ADD", "SUB", "MUL", "DIV", "INC", "DEC", "RESET",
            "LOAD", "STORE", "PUSH", "POP", "INPUT", "DISPLAY",
            "ROL", "ROR", "SHL", "SHR"
        };
        for (int i = 0; i < 18; i++) {
            if (t == opcodes[i]) { return true; }
        }
        return false;
    }

    // splits a line on spaces, tabs and commas; brackets like "[R2]" stay
    // attached to the token since they never contain whitespace
    static void tokenize(const string& line, CustomVector<string>& tokensOut) {
        string current = "";
        for (int i = 0; i < (int)line.length(); i++) {
            char ch = line[i];
            if (ch == ' ' || ch == '\t' || ch == ',') {
                if (!current.empty()) { tokensOut.push_back(current); current = ""; }
            } else {
                current += ch;
            }
        }
        if (!current.empty()) { tokensOut.push_back(current); }
    }

    static bool isBracketed(const string& s) {
        return s.length() >= 2 && s[0] == '[' && s[s.length() - 1] == ']';
    }

    static string stripBrackets(const string& s) {
        return s.substr(1, s.length() - 2);
    }

    static int parseRegisterIndex(const string& s) {
        if (s.length() == 2 && (s[0] == 'R' || s[0] == 'r') && s[1] >= '0' && s[1] <= '7') {
            return s[1] - '0';
        }
        return -1;
    }

    static int parseImmediate(const string& s) {
        int result = 0;
        int sign = 1;
        int i = 0;
        if (!s.empty() && s[0] == '-') { sign = -1; i = 1; }
        else if (!s.empty() && s[0] == '+') { i = 1; }
        for (; i < (int)s.length(); i++) {
            if (s[i] >= '0' && s[i] <= '9') { result = result * 10 + (s[i] - '0'); }
        }
        return result * sign;
    }

    // counts how many tokens look like opcodes; >1 means more than one
    // instruction was written on the same line, which is not allowed
    static bool hasMultipleInstructions(CustomVector<string>& tokens) {
        int opcodeCount = 0;
        for (int i = 0; i < tokens.size(); i++) {
            if (isOpcode(tokens.get(i))) { opcodeCount++; }
        }
        return opcodeCount > 1;
    }

    // ---- decoding: turns one line's tokens into an Instruction object ----

    Instruction* decodeMov(CustomVector<string>& tk) {
        int dest = parseRegisterIndex(tk.get(1));
        string op2 = tk.get(2);
        if (isBracketed(op2)) {
            int src = parseRegisterIndex(stripBrackets(op2));
            return new MovInstruction(dest, src, true);
        }
        if (parseRegisterIndex(op2) != -1) {
            return new MovInstruction(dest, parseRegisterIndex(op2), false);
        }
        return new MovInstruction(dest, (signed char)parseImmediate(op2));
    }

    Instruction* decodeLoad(CustomVector<string>& tk) {
        int dest = parseRegisterIndex(tk.get(1));
        string op2 = tk.get(2);
        string inner = isBracketed(op2) ? stripBrackets(op2) : op2;
        if (parseRegisterIndex(inner) != -1) {
            return new LoadInstruction(dest, parseRegisterIndex(inner), true);
        }
        return new LoadInstruction(dest, parseImmediate(inner), false);
    }

    // The assignment spec is inconsistent about STORE's operand order:
    // section 3.9's own example is "STORE R1, 43" (register first, address
    // second), but the worked sample program uses "STORE 20, R3" (address
    // first, register second). Rather than pick one and risk a crash on the
    // examiner's test programs, we detect which operand is the register by
    // TYPE rather than by POSITION, so both orders work.
    Instruction* decodeStore(CustomVector<string>& tk) {
        string op1 = tk.get(1);
        string op2 = tk.get(2);

        // Register-indirect form: STORE [Rd], Rs  or  STORE Rs, [Rd]
        if (isBracketed(op1)) {
            return new StoreInstruction(parseRegisterIndex(stripBrackets(op1)), parseRegisterIndex(op2), true);
        }
        if (isBracketed(op2)) {
            return new StoreInstruction(parseRegisterIndex(stripBrackets(op2)), parseRegisterIndex(op1), true);
        }

        // Direct-address form: one operand is a plain register, the other a
        // plain numeric address.
        int reg1 = parseRegisterIndex(op1);
        int reg2 = parseRegisterIndex(op2);
        if (reg1 != -1 && reg2 == -1) { return new StoreInstruction(parseImmediate(op2), reg1, false); }
        if (reg2 != -1 && reg1 == -1) { return new StoreInstruction(parseImmediate(op1), reg2, false); }

        cout << "Error: Invalid STORE operands" << endl;
        exit(1);
    }

    // second operand may be a register (e.g. "ADD R2, R0") or an
    // immediate (e.g. "ADD R1, 6", as used in the spec's own example)
    Instruction* decodeArithmetic(const string& cmd, CustomVector<string>& tk) {
        int dest = parseRegisterIndex(tk.get(1));
        string op2 = tk.get(2);
        int src = parseRegisterIndex(op2);
        bool isRegister = (src != -1);

        if (cmd == "ADD") {
            return isRegister ? new AddInstruction(dest, src) : new AddInstruction(dest, (signed char)parseImmediate(op2));
        }
        if (cmd == "SUB") {
            return isRegister ? new SubInstruction(dest, src) : new SubInstruction(dest, (signed char)parseImmediate(op2));
        }
        if (cmd == "MUL") {
            return isRegister ? new MulInstruction(dest, src) : new MulInstruction(dest, (signed char)parseImmediate(op2));
        }
        return isRegister ? new DivInstruction(dest, src) : new DivInstruction(dest, (signed char)parseImmediate(op2));
    }

    Instruction* decodeShift(const string& cmd, CustomVector<string>& tk) {
        int dest = parseRegisterIndex(tk.get(1));
        int count = parseImmediate(tk.get(2));
        if (cmd == "ROL") { return new RolInstruction(dest, count); }
        if (cmd == "ROR") { return new RorInstruction(dest, count); }
        if (cmd == "SHL") { return new ShlInstruction(dest, count); }
        return new ShrInstruction(dest, count);
    }

    Instruction* decodeLine(CustomVector<string>& tk) {
        string cmd = toUpper(tk.get(0));
        if (cmd == "MOV") { return decodeMov(tk); }
        if (cmd == "ADD" || cmd == "SUB" || cmd == "MUL" || cmd == "DIV") { return decodeArithmetic(cmd, tk); }
        if (cmd == "INC") { return new IncInstruction(parseRegisterIndex(tk.get(1))); }
        if (cmd == "DEC") { return new DecInstruction(parseRegisterIndex(tk.get(1))); }
        if (cmd == "RESET") { return new ResetInstruction(toUpper(tk.get(1))); }
        if (cmd == "LOAD") { return decodeLoad(tk); }
        if (cmd == "STORE") { return decodeStore(tk); }
        if (cmd == "PUSH") { return new PushInstruction(parseRegisterIndex(tk.get(1))); }
        if (cmd == "POP") { return new PopInstruction(parseRegisterIndex(tk.get(1))); }
        if (cmd == "INPUT") { return new InputInstruction(parseRegisterIndex(tk.get(1))); }
        if (cmd == "DISPLAY") { return new DisplayInstruction(parseRegisterIndex(tk.get(1))); }
        if (cmd == "ROL" || cmd == "ROR" || cmd == "SHL" || cmd == "SHR") { return decodeShift(cmd, tk); }

        cout << "Error: Unknown instruction " << cmd << endl;
        exit(1);
    }

    // formats a value into a fixed 4-character wide field, e.g. 5 -> "0005",
    // -5 -> "-005". Chosen so every field is exactly 4 characters wide.
    static string formatNumber(int value) {
        bool negative = value < 0;
        int absVal = negative ? -value : value;
        string digits = "";
        if (absVal == 0) { digits = "0"; }
        while (absVal > 0) { digits = (char)('0' + absVal % 10) + digits; absVal /= 10; }
        int padWidth = negative ? 3 : 4;
        while ((int)digits.length() < padWidth) { digits = "0" + digits; }
        return negative ? ("-" + digits) : digits;
    }

    public:
    ~Runner() {
        for (int i = 0; i < instructions.size(); i++) { delete instructions.get(i); }
    }

    // Reads the .asm file using a CustomQueue as a buffer, validates each
    // line (skip blanks, reject lines with multiple instructions), and
    // decodes valid lines straight into the CustomVector<Instruction*>.
    void loadFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open file " << filename << endl;
            exit(1);
        }

        CustomQueue<string> rawLines;
        string rawLine;
        while (getline(file, rawLine)) { rawLines.enqueue(rawLine); }
        file.close();

        while (!rawLines.isEmpty()) {
            string line = trim(rawLines.dequeue());

            int commentPos = -1;
            for(int i = 0; i < (int)line.length(); i++) {
                if(line[i] == ';') { commentPos = i; break; }
            }
            if(commentPos != -1) {
                line = line.substr(0, commentPos);
                line = trim(line); // re-trim just in case
            }

            if (line.empty()) { continue; }

            CustomVector<string> tokens;
            tokenize(line, tokens);
            if (tokens.size() == 0) { continue; }

            if (hasMultipleInstructions(tokens)) {
                cout << "Error: More than one instruction found on a single line" << endl;
                exit(1);
            }

            instructions.push_back(decodeLine(tokens));
        }
    }

    // Fetch-execute cycle: the Runner does not know which concrete
    // Instruction subtype it holds; execute() is resolved at runtime.
    void run() {
        while (cpu.getPC() < instructions.size()) {
            instructions.get(cpu.getPC())->execute(&cpu);
        }
    }

    // Writes the exact "#Begin# ... #End#" dump format to a stream
    // (used for both screen output and file output).
    void dumpTo(ostream& out) {
        out << "#Begin#" << endl;

        out << "#Registers";
        for (int i = 0; i < 8; i++) {
            out << "#" << formatNumber((int)cpu.getRegister(i).getValue());
        }
        out << "#" << endl;

        // Flag order and labels match the official sample output exactly:
        // #Flags#OF#0#UF#0#CF#0#ZF#0#
        out << "#Flags#OF#" << cpu.getFlags().getOF() << "#UF#" << cpu.getFlags().getUF()
            << "#CF#" << cpu.getFlags().getCF() << "#ZF#" << cpu.getFlags().getZF() << "#" << endl;

        out << "#PC#" << formatNumber(cpu.getPC()) << "#" << endl;

        out << "#Memory#" << endl;
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                int address = row * 8 + col;
                out << "#" << formatNumber((int)cpu.getMemory().read(address));
            }
            out << "#" << endl;
        }

        out << "#End#" << endl;
    }

    void dumpResults(const string& outputFilename) {
        dumpTo(cout);
        ofstream outFile(outputFilename);
        if (outFile.is_open()) {
            dumpTo(outFile);
            outFile.close();
        } else {
            cout << "Error: Could not write to output file " << outputFilename << endl;
        }
    }
};

/* ==========================================================================
 * SECTION 13: MAIN (command-line entry point)
 * Author: Chong You Wei 242UC2431Z
 * Usage:  ./program <input.asm> [output.txt]
 * If no filename is given on the command line, the program asks for one,
 * so it still works if accidentally double-clicked instead of run from a
 * terminal.
 * ========================================================================== */
int main(int argc, char* argv[]) {
    string inputFile;
    string outputFile = "output.txt";

    if (argc >= 2) {
        inputFile = argv[1];
        if (argc >= 3) { outputFile = argv[2]; }
    } else {
        cout << "Enter the .asm file to run: ";
        cin >> inputFile;
    }

    Runner runner;
    runner.loadFile(inputFile);
    runner.run();
    runner.dumpResults(outputFile);

    return 0;
}