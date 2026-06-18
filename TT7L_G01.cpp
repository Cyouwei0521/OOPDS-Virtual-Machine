//group
//Group Members;
//1.Chong You Wei 242UC2431Z
//2.
//3.


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

int main() {
    cout << "--- Booting Virtual Machine ---" << endl;

    CPU myVM;

    myVM.getRegister(1).setValue(5);
    myVM.incrementPC();

    cout << "Value in R1: " << (int)myVM.getRegister(1).getValue() << endl;
    cout << "Current PC: " << myVM.getPC() << endl;

    return 0;
}