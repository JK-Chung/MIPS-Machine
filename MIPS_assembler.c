#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define uint unsigned int

#define OPCODE_LENGTH 6
#define RS_LENGTH 5
#define RT_LENGTH 5
#define RD_LENGTH 6
#define SHAMT_LENGTH 5
#define FUNCT_LENGTH 6

#define IMMD_LENGTH 16
#define ADDRESS_LENGTH 26

uint32_t createRMachineInstruction(uint opcode, uint rs, uint rt, uint rd, uint shamt, uint funct);
uint32_t createIMachineInstruction(uint opcode, uint rs, uint rt, int16_t immd);
uint32_t createJMachineInstruction(uint opcode, uint32_t address);


int main() {

}

uint32_t createRMachineInstruction(uint opcode, uint rs, uint rt, uint rd, uint shamt, uint funct) {
    uint32_t instruction = funct;
    int bitShift = FUNCT_LENGTH;

    instruction |= (shamt << bitShift);
    bitShift += SHAMT_LENGTH;

    instruction |= (rd << bitShift);
    bitShift += RD_LENGTH;

    instruction |= (rt << bitShift);
    bitShift += RT_LENGTH;

    instruction |= (rs << bitShift);
    bitShift += RS_LENGTH;

    instruction |= (opcode << bitShift);
    return instruction;
}

uint32_t createIMachineInstruction(uint opcode, uint rs, uint rt, int16_t immd) {
    uint32_t instruction = immd;
    int bitShift = IMMD_LENGTH;

    instruction |= (rt << bitShift);
    bitShift += RT_LENGTH;

    instruction |= (rs << bitShift);
    bitShift += RS_LENGTH;

    instruction |= (opcode << bitShift);
    return instruction;
}

uint32_t createJMachineInstruction(uint opcode, uint32_t address) {
    uint32_t instruction = address;
    instruction |= (opcode << ADDRESS_LENGTH);
    return instruction;
}
