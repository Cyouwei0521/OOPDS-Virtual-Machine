//group
//Group Members;
//1.Chong You Wei 242UC2431Z
//2.
//3.Tiang Li Yuan 252UC241LG
//4.Chia Yee Shuen 252UC24306


#include <iostream>
#include <fstream>
#include <string>
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



int main() {
    cout << "--- Booting Virtual Machine ---" << endl;

    CPU myVM;

    myVM.getRegister(1).setValue(5);
    myVM.incrementPC();

    cout << "Value in R1: " << (int)myVM.getRegister(1).getValue() << endl;
    cout << "Current PC: " << myVM.getPC() << endl;

    
    return 0;
}