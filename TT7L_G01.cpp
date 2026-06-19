//group
//Group Members;
//1.Chong You Wei 242UC2431Z
//2.Tee Li Qing 261UC2606W
//3.Tiang Li Yuan 252UC241LG
//4.Chia Yee Shuen 252UC24306


#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
//strictly no STL

using namespace std;

//custom dynamic array to replace vector
template <typename T>
class CustomVector{
    private:
    T* arr;
    int capacity;
    int current_size;

    //resize the array when it gets full
    void resize(){
        capacity *= 2;
        T* temp = new T[capacity];
        for (int i= 0; i < current_size; i++){temp[i] = arr[i];}
        delete[] arr;
        arr = temp;
    }
    public:
    CustomVector(){
        capacity = 10;
        current_size = 0;
        arr = new T[capacity];
    } ~CustomVector(){delete[] arr;}

    void push_back(T data) {
        if (current_size == capacity) {resize();}
        arr[current_size] = data;
        current_size++;
    }
    T& get(int index){return arr[index];}
    int size(){return current_size;}
};

//custom stack for VM (max size 8 bytes)
class CustomStack{
    private:
    signed char elements[8];
    int top_index;

    public:
    CustomStack(){top_index = -1;}

    void push(signed char value){
        if (top_index >= 7){cout << "Error: Stack Overflow" << endl;}
        top_index++;
        elements[top_index] = value;
    }
    
    signed char pop(){
        if (top_index < 0){cout << "Error: Stack Underflow" << endl; exit(1);}
        signed char popped_value = elements[top_index];
        top_index--;
        return popped_value;
    }

    //get current stack index
    int getSI(){return top_index+1;}
};

class Memory {
    private:
    signed char data[64];

    public:
    Memory() {
        for (int i = 0; i < 64 ; i++){data[i] = 0;}
    }

    signed char read(int address){
        if (address < 0 || address > 63){
            cout << "Error: Memory read out of bounds at address " << address << endl;
            return 0;
        } return data[address];
    }
    
    void write(int address, signed char value){
        if (address < 0 || address > 63){
            cout << "Error: Memory write out of bounds at address " << address << endl;
            return;
        } data[address] = value;
    }
};

class Register {
    protected: 
    signed char value;

    public:
    Register(){value = 0;}
    virtual void setValue(signed char val){value = val;}
    virtual signed char getValue(){return value;}
};

class GeneralRegister : public Register{
    public:
    //inherits everything from Register
    //can add specific decimal to binary logic here if needed
};

class FlagRegister{
    private:
    bool OF;
    bool UF;
    bool CF;
    bool ZF;

    public:
    FlagRegister(){
        OF = false;
        UF = false;
        CF = false;
        ZF = false;
    }

    void setOF(bool val){OF = val;}
    void setUF(bool val){UF = val;}
    void setCF(bool val){CF = val;}
    void setZF(bool val){ZF = val;}

    bool getOF(){return OF;}
    bool getUF(){return UF;}
    bool getCF(){return CF;}
    bool getZF(){return ZF;}
};

class CPU;

class Instruction {
    public:
    virtual void execute(CPU* cpu) =0;

    virtual ~Instruction() {}
};

class CPU {
    private:
    GeneralRegister registers[8];
    FlagRegister flags;
    Memory memory;
    CustomStack stack;
    int PC;

    public:
    CPU(){PC=0;}
    GeneralRegister& getRegister(int index){return registers[index];}
    FlagRegister& getFlags(){return flags;}
    Memory& getMemory(){return memory;}
    CustomStack& getStack(){return stack;}
    int getPC(){return PC;}
    void incrementPC(){PC++;}
};

// arithmetic instructions
// helper for updating flags and storing result
class ArithmeticHelper {
    private:
    // keep the result within signed 8-bit range
    static signed char normalizeToByte(int result){
        // convert result into a value between 0 and 255
        int value = result % 256;
        if (value < 0){value += 256;}
        // convert back to signed value if more than 127
        if (value > 127){value -= 256;}
        return (signed char)value;
    }

    public:
    // update all flags based on the result
    static void updateFlags(CPU* cpu, int result){
        FlagRegister& flags = cpu->getFlags();
        // check overflow, underflow, carry and zero
        flags.setOF(result > 127);
        flags.setUF(result < -128);
        flags.setCF(result > 127 || result < -128);
        flags.setZF(result == 0);
    }
    // update flag and store the final result into register
    static void storeArithmeticResult(CPU* cpu, int regIndex, int result){
        updateFlags(cpu, result);
        cpu->getRegister(regIndex).setValue(normalizeToByte(result));
    }
};

// base class for arithmetic instructions with two registers
class ArithmeticInstruction : public Instruction {
    private:
    int destRegIndex; // destination register
    int srcRegIndex;  // source register

    protected:
    // get register indexes for child classes
    int getDestRegIndex(){return destRegIndex;}
    int getSrcRegIndex(){return srcRegIndex;}

    public:
    // constructor for instructions with two register operands
    ArithmeticInstruction(int dest, int src){
        destRegIndex = dest;
        srcRegIndex = src;
    }
};

// ADD Instruction Class
class AddInstruction : public ArithmeticInstruction {
    public:
    // constructor, example ADD R1, R2
    AddInstruction(int dest, int src) : ArithmeticInstruction(dest, src) {}

    void execute(CPU* cpu) override {
        // get destination and source register indexes
        int dest = getDestRegIndex();
        int src = getSrcRegIndex();

        // add source register value to destination register value
        int result = (int)cpu->getRegister(dest).getValue()
                   + (int)cpu->getRegister(src).getValue();

        // update flags, store result and move to next instruction
        ArithmeticHelper::storeArithmeticResult(cpu, dest, result);
        cpu->incrementPC();
    }
};

// SUB Instruction Class
class SubInstruction : public ArithmeticInstruction {
    public:
    // constructor, example SUB R1, R2
    SubInstruction(int dest, int src) : ArithmeticInstruction(dest, src) {}

    void execute(CPU* cpu) override {
        // get the destination and source register indexes
        int dest = getDestRegIndex();
        int src = getSrcRegIndex();

        // subtract source register value from destination register value
        int result = (int)cpu->getRegister(dest).getValue()
                   - (int)cpu->getRegister(src).getValue();

        // update flags, store result and move to next instruction
        ArithmeticHelper::storeArithmeticResult(cpu, dest, result);
        cpu->incrementPC();
    }
};

// MUL Instruction Class
class MulInstruction : public ArithmeticInstruction {
    public:
    // constructor, example MUL R1, R2
    MulInstruction(int dest, int src) : ArithmeticInstruction(dest, src) {}

    void execute(CPU* cpu) override {
        // get the destination and source register indexes
        int dest = getDestRegIndex();
        int src = getSrcRegIndex();

        // multiply destination register value by source register value
        int result = (int)cpu->getRegister(dest).getValue()
                   * (int)cpu->getRegister(src).getValue();

        // update flags, store result and move to next instruction
        ArithmeticHelper::storeArithmeticResult(cpu, dest, result);
        cpu->incrementPC();
    }
};

// DIV Instruction Class
class DivInstruction : public ArithmeticInstruction {
    public:
    // constructor, example DIV R1, R2
    DivInstruction(int dest, int src) : ArithmeticInstruction(dest, src) {}

    void execute(CPU* cpu) override {
        // get the destination and source register indexes
        int dest = getDestRegIndex();
        int src = getSrcRegIndex();

        // get divisor from the source register
        int divisor = (int)cpu->getRegister(src).getValue();

        // display error and stop if divisor is zero
        if (divisor == 0){
            cout << "Error: Division by zero" << endl;
            exit(1);
        }

        // divide destination register value by source register value
        int result = (int)cpu->getRegister(dest).getValue() / divisor;

        // update flags, store result and move to next instruction
        ArithmeticHelper::storeArithmeticResult(cpu, dest, result);
        cpu->incrementPC();
    }
};

// INC Instruction Class
class IncInstruction : public Instruction {
    private:
    int destRegIndex; // register to increase

    public:
    // constructor, example INC R1
    IncInstruction(int dest){destRegIndex = dest;}

    void execute(CPU* cpu) override {
        // add 1 to the destination register value
        int result = (int)cpu->getRegister(destRegIndex).getValue() + 1;

        // update flags, store result and move to next instruction
        ArithmeticHelper::storeArithmeticResult(cpu, destRegIndex, result);
        cpu->incrementPC();
    }
};

// DEC Instruction Class
class DecInstruction : public Instruction {
    private:
    int destRegIndex; // register to decrease

    public:
    // constructor, example DEC R1
    DecInstruction(int dest){destRegIndex = dest;}

    void execute(CPU* cpu) override {
        // subtract 1 from the destination register value
        int result = (int)cpu->getRegister(destRegIndex).getValue() - 1;

        // update flags, store result and move to next instruction
        ArithmeticHelper::storeArithmeticResult(cpu, destRegIndex, result);
        cpu->incrementPC();
    }
};

// RESET Instruction Class
class ResetInstruction : public Instruction {
    private:
    string targetFlag; // flag selected by the user

    public:
    // constructor, example RESET OF
    ResetInstruction(string flagName){targetFlag = flagName;}

    void execute(CPU* cpu) override {
        // find and reset the selected flag
        if (targetFlag == "OF"){cpu->getFlags().setOF(false);}
        else if (targetFlag == "UF"){cpu->getFlags().setUF(false);}
        else if (targetFlag == "CF"){cpu->getFlags().setCF(false);}
        else if (targetFlag == "ZF"){cpu->getFlags().setZF(false);}
        else {
            // display error if the flag name is invalid
            cout << "Error: Invalid flag name " << targetFlag << endl;
            exit(1);
        }

        // move to next instruction
        cpu->incrementPC();
    }
};

enum class MovMode{IMMEDIATE, REGISTER, INDIRECT};

class MovInstruction : public Instruction {
    private:
    int destRegIndex;
    MovMode mode;
    signed char immValue; //used when mode == IMMEDIATE
    int srcRegIndex; //used when mode == REGISTER or INDIRECT

    public:
    //constructor for MOV Rd, <immediate>
    MovInstruction(int dest, signed char val) 
    : destRegIndex(dest), mode(MovMode::IMMEDIATE), immValue(val), srcRegIndex(-1) {}

    //constructor for MOV Rd, Rs or MOV Rd, [Rs]
    MovInstruction(int dest, int src, bool indirect) 
        : destRegIndex(dest), mode(indirect ? MovMode::INDIRECT : MovMode::REGISTER), srcRegIndex(src), immValue(0) {}

    void execute(CPU* cpu) override {
        switch (mode) {
            case MovMode::IMMEDIATE:
                //1. Get the destination register index from the first operand (already stored in destRegIndex)
                //2. Directly set the value of the destination register to the immediate value
                cpu->getRegister(destRegIndex).setValue(immValue);
                break;
            case MovMode::REGISTER:
                cpu->getRegister(destRegIndex).setValue(cpu->getRegister(srcRegIndex).getValue());
                break;
            case MovMode::INDIRECT:{
                int memAddress = cpu->getRegister(srcRegIndex).getValue();
                cpu->getRegister(destRegIndex).setValue(cpu->getMemory().read(memAddress));
                break;
            }
        }
        cpu->incrementPC();
    }
};

class LoadInstruction : public Instruction {
    private:
    int destRegIndex;
    bool useRegisterAddress; //true if using register for address, false if using immediate address
    int addressOrRegIndex; //either the immediate address or the register index for the address

    public:
    LoadInstruction(int dest, int addressOrReg, bool useRegisterAddress) 
        : destRegIndex(dest), useRegisterAddress(useRegisterAddress),
            addressOrRegIndex(addressOrReg) {}

    void execute(CPU* cpu) override {
        int address = useRegisterAddress
            ? (int)cpu->getRegister(addressOrRegIndex).getValue()
            : addressOrRegIndex;

        signed char value = cpu->getMemory().read(address);
        cpu->getRegister(destRegIndex).setValue(value);
        cpu->incrementPC();
    }
};

class StoreInstruction : public Instruction {
    private:
    int addressOrRegIndex;
    int srcRegIndex;        
    bool useRegisterAddress; //true if using register indirect address, false if using direct address

    public:
    //STORE <Destination_Memory>, <Source_Register>: STORE 10, R1 or STORE [R0], R1
    StoreInstruction(int destAddrOrReg, int src, bool useRegAddr)
        : addressOrRegIndex(destAddrOrReg), srcRegIndex(src), useRegisterAddress(useRegAddr) {}

    void execute(CPU* cpu) override {
        // 1. Calculate the destination memory address from the first operand
        int address = useRegisterAddress 
            ? (int)cpu->getRegister(addressOrRegIndex).getValue()
            : addressOrRegIndex;

        // 2. Fetch the data value from the source register (the second operand)
        signed char value = cpu->getRegister(srcRegIndex).getValue();

        // 3. Write the value into the virtual machine's memory
        cpu->getMemory().write(address, value);
        cpu->incrementPC();
    }
};

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
        // CustomStack::pop() already exit(1)s on empty stack,
        // satisfying the "crash on pop from empty stack" requirement.
        signed char value = cpu->getStack().pop();
        cpu->getRegister(destRegIndex).setValue(value);
        cpu->incrementPC();
    }
};

class InputInstruction : public Instruction {
    private:
    int destRegIndex;

    public:
    InputInstruction(int destRegIndex) : destRegIndex(destRegIndex) {}

    void execute(CPU* cpu) override {
        cout << "?";
        int rawInput;
        cin >> rawInput;

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

// Utility class for binary conversion and manual bitwise manipulation (No STL)
class BitwiseUtils {
public:
    // 1. Converts a signed decimal char to an 8-bit binary array (Two's Complement)
    static void decToBinary(signed char num, int bits[8]) {
        unsigned char uNum = (unsigned char)num; 
        for (int i = 7; i >= 0; i--) {
            bits[i] = uNum % 2;
            uNum /= 2;
        }
    }

    // 2. Converts an 8-bit binary array back to a signed decimal char
    static signed char binaryToDec(int bits[8]) {
        unsigned char uNum = 0;
        for (int i = 0; i < 8; i++) {
            uNum = (uNum << 1) | bits[i];
        }
        return (signed char)uNum;
    }
};

// SHL Instruction Class (Manual Shift Left implementation)
class ShlInstruction : public Instruction {
private:
    int regIndex;      // The register to shift 
    int shiftAmount;   // How many positions to shift left

public:
    // Constructor: e.g., SHL R1, 2
    ShlInstruction(int reg, int amount) : regIndex(reg), shiftAmount(amount) {}

    void execute(CPU* cpu) override {
        // Get current value from the register
        signed char val = cpu->getRegister(regIndex).getValue();
        int bits[8];

        // Convert decimal to binary array manually
        BitwiseUtils::decToBinary(val, bits);

        // Perform manual Shift Left (SHL)
        for (int step = 0; step < shiftAmount; step++) {
            // Shift elements to the left
            for (int i = 0; i < 7; i++) {
                bits[i] = bits[i + 1];
            }
            // Pad the rightmost bit with 0
            bits[7] = 0;
        }

        // Convert the shifted binary array back to decimal
        signed char result = BitwiseUtils::binaryToDec(bits);

        // Update flags based on the result
        FlagRegister& f = cpu->getFlags();
        f.setZF(result == 0); 

        // Store the result back to the register and move to next instruction
        cpu->getRegister(regIndex).setValue(result);
        cpu->incrementPC();
    }
};

// SHR Instruction Class (Manual Shift Right implementation)
class ShrInstruction : public Instruction {
private:
    int regIndex;      // The register to shift (e.g., R1)
    int shiftAmount;   // How many positions to shift right

public:
    // Constructor
    ShrInstruction(int reg, int amount) : regIndex(reg), shiftAmount(amount) {}

    void execute(CPU* cpu) override {
        // Get current value from the register
        signed char val = cpu->getRegister(regIndex).getValue();
        int bits[8];

        // Convert decimal to binary array manually
        BitwiseUtils::decToBinary(val, bits);

        // Perform manual Shift Right (SHR)
        for (int step = 0; step < shiftAmount; step++) {
            // Shift elements to the right (starting from the end)
            for (int i = 7; i > 0; i--) {
                bits[i] = bits[i - 1];
            }
            // Pad the leftmost bit with 0
            bits[0] = 0;
        }

        // Convert the shifted binary array back to decimal
        signed char result = BitwiseUtils::binaryToDec(bits);

        // Update flags based on the result
        FlagRegister& f = cpu->getFlags();
        f.setZF(result == 0); 

        // Store the result back to the register and move to next instruction
        cpu->getRegister(regIndex).setValue(result);
        cpu->incrementPC();
    }
};

// ROL Instruction Class (Manual Rotate Left implementation)
class RolInstruction : public Instruction {
private:
    int regIndex;      // The register to rotate 
    int rotateAmount;  // How many positions to rotate left

public:
    // Constructor
    RolInstruction(int reg, int amount) : regIndex(reg), rotateAmount(amount) {}

    void execute(CPU* cpu) override {
        // Get current value from the register
        signed char val = cpu->getRegister(regIndex).getValue();
        int bits[8];

        // Convert decimal to binary array manually
        BitwiseUtils::decToBinary(val, bits);

        // Perform manual Rotate Left (ROL)
        for (int step = 0; step < rotateAmount; step++) {
            // Save the leftmost bit before it falls off
            int leftmostBit = bits[0];
            
            // Shift elements to the left
            for (int i = 0; i < 7; i++) {
                bits[i] = bits[i + 1];
            }
            
            // Place the saved leftmost bit into the rightmost position
            bits[7] = leftmostBit;
        }

        // Convert the rotated binary array back to decimal
        signed char result = BitwiseUtils::binaryToDec(bits);

        // Update flags based on the result
        FlagRegister& f = cpu->getFlags();
        f.setZF(result == 0); 

        // Store the result back to the register and move to next instruction
        cpu->getRegister(regIndex).setValue(result);
        cpu->incrementPC();
    }
};

// ROR Instruction Class 
class RorInstruction : public Instruction {
private:
    int regIndex;      // The register to rotate 
    int rotateAmount;  // How many positions to rotate right

public:
    // Constructor
    RorInstruction(int reg, int amount) : regIndex(reg), rotateAmount(amount) {}

    void execute(CPU* cpu) override {
        // Get current value from the register
        signed char val = cpu->getRegister(regIndex).getValue();
        int bits[8];

        // Convert decimal to binary array manually
        BitwiseUtils::decToBinary(val, bits);

        // Perform manual Rotate Right (ROR)
        for (int step = 0; step < rotateAmount; step++) {
            // Save the rightmost bit before it falls off
            int rightmostBit = bits[7];
            
            // Shift elements to the right
            for (int i = 7; i > 0; i--) {
                bits[i] = bits[i - 1];
            }
            
            // Place the saved rightmost bit into the leftmost position
            bits[0] = rightmostBit;
        }

        // Convert the rotated binary array back to decimal
        signed char result = BitwiseUtils::binaryToDec(bits);

        // Update flags based on the result
        FlagRegister& f = cpu->getFlags();
        f.setZF(result == 0); 

        // Store the result back to the register and move to next instruction
        cpu->getRegister(regIndex).setValue(result);
        cpu->incrementPC();
    }
};

int main() {
    cout << "--- Booting Virtual Machine ---" << endl;

    CPU myVM;

    myVM.getRegister(1).setValue(5);
    myVM.incrementPC();

    cout << "Value in R1: " << (int)myVM.getRegister(1).getValue() << endl;
    cout << "Current PC: " << myVM.getPC() << endl;

    
    return 0;
}