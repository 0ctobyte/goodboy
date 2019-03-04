#include <iostream>

#include "gb_cpu.h"
#include "gb_memory.h"

#define FLAGS_IS_SET(flags)      ((_m_registers.f & (flags)) != 0)
#define FLAGS_IS_CLEAR(flags)    ((_m_registers.f & (flags)) == 0)
#define FLAGS_SET(flags)         { _m_registers.f |= (flags); }
#define FLAGS_CLEAR(flags)       { _m_registers.f &= ~(flags); }
#define FLAGS_TOGGLE(flags)      { _m_registers.f ^= (flags); }

#define FLAGS_SET_IF_Z(x)        { if ((x) == 0) FLAGS_SET(FLAGS_Z); }
#define FLAGS_SET_IF_H(x,y)      { if ((((x) & 0x0f) + ((y) & 0x0f)) & 0xf0) FLAGS_SET(FLAGS_H); }
#define FLAGS_SET_IF_C(x)        { if ((x) & 0xff00) FLAGS_SET(FLAGS_C); }

#define FLAGS_SET_IF_H_16(x,y)   { if ((((x) & 0xfff) + ((y) & 0xfff)) & 0xf000) FLAGS_SET(FLAGS_H); }
#define FLAGS_SET_IF_C_16(x)     { if ((x) & 0xffff0000) FLAGS_SET(FLAGS_C); }

#define INSTRUCTIONS_LIST_INIT \
{ \
/* 0x00 */ {"NOP", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_nop, 4, 4}, \
/* 0x01 */ {"LD BC, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_bc, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0x02 */ {"LD (BC), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_bc, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x03 */ {"INC BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_bc, nullptr, &gb_cpu::_operand_set_register_bc, &gb_cpu::_op_exec_inc, 8, 8}, \
/* 0x04 */ {"INC B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_incf, 4, 4}, \
/* 0x05 */ {"DEC B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_decf, 4, 4}, \
/* 0x06 */ {"LD B, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x07 */ {"RLC A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rlc, 4, 4}, \
/* 0x08 */ {"LD (0x%04x), SP", &gb_cpu::_op_print_type3, &gb_cpu::_operand_get_register_sp, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 20, 20}, \
/* 0x09 */ {"ADD HL, BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_bc, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_add16, 8, 8}, \
/* 0x0A */ {"LD A, (BC)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_bc, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x0B */ {"DEC BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_bc, nullptr, &gb_cpu::_operand_set_register_bc, &gb_cpu::_op_exec_dec, 8, 8}, \
/* 0x0C */ {"INC C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_incf, 4, 4}, \
/* 0x0D */ {"DEC C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_decf, 4, 4}, \
/* 0x0E */ {"LD C, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x0F */ {"RRC A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rrc, 4, 4}, \
/* 0x10 */ {"STOP 0", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_8, nullptr, nullptr, &gb_cpu::_op_exec_stop, 4, 4}, \
/* 0x11 */ {"LD DE, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_de, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0x12 */ {"LD (DE), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_de, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x13 */ {"INC DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_de, nullptr, &gb_cpu::_operand_set_register_de, &gb_cpu::_op_exec_inc, 8, 8}, \
/* 0x14 */ {"INC D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_incf, 4, 4}, \
/* 0x15 */ {"DEC D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_decf, 4, 4}, \
/* 0x16 */ {"LD D, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x17 */ {"RL A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rl, 4, 4}, \
/* 0x18 */ {"JR 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 12}, \
/* 0x19 */ {"ADD HL, DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_de, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_add16, 8, 8}, \
/* 0x1A */ {"LD A, (DE)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_de, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x1B */ {"DEC DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_de, nullptr, &gb_cpu::_operand_set_register_de, &gb_cpu::_op_exec_dec, 8, 8}, \
/* 0x1C */ {"INC E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_incf, 4, 4}, \
/* 0x1D */ {"DEC E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_decf, 4, 4}, \
/* 0x1E */ {"LD E, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x1F */ {"RR A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rr, 4, 4}, \
/* 0x20 */ {"JR NZ, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_flags_is_nz, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 8}, \
/* 0x21 */ {"LD HL, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0x22 */ {"LD (HL+), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_hl_plus, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x23 */ {"INC HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, nullptr, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_inc, 8, 8}, \
/* 0x24 */ {"INC H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_incf, 4, 4}, \
/* 0x25 */ {"DEC H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_decf, 4, 4}, \
/* 0x26 */ {"LD H, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x27 */ {"DA A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_da, 4, 4}, \
/* 0x28 */ {"JR Z, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_flags_is_z, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 8}, \
/* 0x29 */ {"ADD HL, HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_add16, 8, 8}, \
/* 0x2A */ {"LD A, (HL+)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl_plus, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x2B */ {"DEC HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, nullptr, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_dec, 8, 8}, \
/* 0x2C */ {"INC L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_incf, 4, 4}, \
/* 0x2D */ {"DEC L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_decf, 4, 4}, \
/* 0x2E */ {"LD L, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x2F */ {"CPL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_cpl, 4, 4}, \
/* 0x30 */ {"JR NC, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_flags_is_nc, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 8}, \
/* 0x31 */ {"LD SP, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0x32 */ {"LD (HL-), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_hl_minus, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x33 */ {"INC SP", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_sp, nullptr, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_inc, 8, 8}, \
/* 0x34 */ {"INC (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_incf, 12, 12}, \
/* 0x35 */ {"DEC (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_decf, 12, 12}, \
/* 0x36 */ {"LD (HL), 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0x37 */ {"SCF", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_scf, 4, 4}, \
/* 0x38 */ {"JR C, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_flags_is_c, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 8}, \
/* 0x39 */ {"ADD HL, SP", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_sp, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_add16, 8, 8}, \
/* 0x3A */ {"LD A, (HL-)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl_minus, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x3B */ {"DEC SP", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_sp, nullptr, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_dec, 8, 8}, \
/* 0x3C */ {"INC A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_incf, 4, 4}, \
/* 0x3D */ {"DEC A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_decf, 4, 4}, \
/* 0x3E */ {"LD A, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x3F */ {"CCF", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_ccf, 4, 4}, \
/* 0x40 */ {"LD B, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x41 */ {"LD B, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x42 */ {"LD B, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x43 */ {"LD B, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x44 */ {"LD B, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x45 */ {"LD B, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x46 */ {"LD B, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x47 */ {"LD B, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x48 */ {"LD C, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x49 */ {"LD C, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x4A */ {"LD C, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x4B */ {"LD C, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x4C */ {"LD C, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x4D */ {"LD C, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x4E */ {"LD C, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x4F */ {"LD C, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x50 */ {"LD D, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x51 */ {"LD D, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x52 */ {"LD D, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x53 */ {"LD D, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x54 */ {"LD D, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x55 */ {"LD D, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x56 */ {"LD D, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x57 */ {"LD D, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x58 */ {"LD E, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x59 */ {"LD E, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x5A */ {"LD E, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x5B */ {"LD E, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x5C */ {"LD E, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x5D */ {"LD E, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x5E */ {"LD E, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x5F */ {"LD E, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x60 */ {"LD H, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x61 */ {"LD H, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x62 */ {"LD H, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x63 */ {"LD H, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x64 */ {"LD H, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x65 */ {"LD H, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x66 */ {"LD H, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x67 */ {"LD H, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x68 */ {"LD L, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x69 */ {"LD L, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x6A */ {"LD L, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x6B */ {"LD L, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x6C */ {"LD L, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x6D */ {"LD L, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x6E */ {"LD L, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x6F */ {"LD L, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x70 */ {"LD (HL), B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x71 */ {"LD (HL), C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x72 */ {"LD (HL), D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x73 */ {"LD (HL), E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x74 */ {"LD (HL), H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x75 */ {"LD (HL), L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x76 */ {"HALT", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_halt, 4, 4}, \
/* 0x77 */ {"LD (HL), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x78 */ {"LD A, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x79 */ {"LD A, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x7A */ {"LD A, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x7B */ {"LD A, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x7C */ {"LD A, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x7D */ {"LD A, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x7E */ {"LD A, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0x7F */ {"LD A, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4}, \
/* 0x80 */ {"ADD A, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4}, \
/* 0x81 */ {"ADD A, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4}, \
/* 0x82 */ {"ADD A, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4}, \
/* 0x83 */ {"ADD A, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4}, \
/* 0x84 */ {"ADD A, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4}, \
/* 0x85 */ {"ADD A, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4}, \
/* 0x86 */ {"ADD A, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 8, 8}, \
/* 0x87 */ {"ADD A, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4}, \
/* 0x88 */ {"ADC A, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4}, \
/* 0x89 */ {"ADC A, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4}, \
/* 0x8A */ {"ADC A, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4}, \
/* 0x8B */ {"ADC A, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4}, \
/* 0x8C */ {"ADC A, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4}, \
/* 0x8D */ {"ADC A, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4}, \
/* 0x8E */ {"ADC A, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 8, 8}, \
/* 0x8F */ {"ADC A, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4}, \
/* 0x90 */ {"SUB B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0x91 */ {"SUB C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0x92 */ {"SUB D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0x93 */ {"SUB E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0x94 */ {"SUB H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0x95 */ {"SUB L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0x96 */ {"SUB (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 8, 8}, \
/* 0x97 */ {"SUB A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0x98 */ {"SBC A, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4}, \
/* 0x99 */ {"SBC A, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4}, \
/* 0x9A */ {"SBC A, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4}, \
/* 0x9B */ {"SBC A, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4}, \
/* 0x9C */ {"SBC A, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4}, \
/* 0x9D */ {"SBC A, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4}, \
/* 0x9E */ {"SBC A, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 8, 8}, \
/* 0x9F */ {"SBC A, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4}, \
/* 0xA0 */ {"AND B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4}, \
/* 0xA1 */ {"AND C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4}, \
/* 0xA2 */ {"AND D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4}, \
/* 0xA3 */ {"AND E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4}, \
/* 0xA4 */ {"AND H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4}, \
/* 0xA5 */ {"AND L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4}, \
/* 0xA6 */ {"AND (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 8, 8}, \
/* 0xA7 */ {"AND A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4}, \
/* 0xA8 */ {"XOR B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4}, \
/* 0xA9 */ {"XOR C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4}, \
/* 0xAA */ {"XOR D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4}, \
/* 0xAB */ {"XOR E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4}, \
/* 0xAC */ {"XOR H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4}, \
/* 0xAD */ {"XOR L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4}, \
/* 0xAE */ {"XOR (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 8, 8}, \
/* 0xAF */ {"XOR A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4}, \
/* 0xB0 */ {"OR B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4}, \
/* 0xB1 */ {"OR C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4}, \
/* 0xB2 */ {"OR D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4}, \
/* 0xB3 */ {"OR E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4}, \
/* 0xB4 */ {"OR H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4}, \
/* 0xB5 */ {"OR L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4}, \
/* 0xB6 */ {"OR (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 8, 8}, \
/* 0xB7 */ {"OR A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4}, \
/* 0xB8 */ {"CP B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0xB9 */ {"CP C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0xBA */ {"CP D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0xBB */ {"CP E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0xBC */ {"CP H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0xBD */ {"CP L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0xBE */ {"CP (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 8, 8}, \
/* 0xBF */ {"CP A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4}, \
/* 0xC0 */ {"RET NZ", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, &gb_cpu::_operand_get_flags_is_nz, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 20, 8}, \
/* 0xC1 */ {"POP BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_bc, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0xC2 */ {"JP NZ, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_nz, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 12}, \
/* 0xC3 */ {"JP 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 16}, \
/* 0xC4 */ {"CALL NZ, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_nz, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 12}, \
/* 0xC5 */ {"PUSH BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_bc, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_ld, 16, 16}, \
/* 0xC6 */ {"ADD A, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 8, 8}, \
/* 0xC7 */ {"RST 00H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_00, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16}, \
/* 0xC8 */ {"RET Z", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, &gb_cpu::_operand_get_flags_is_z, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 20, 8}, \
/* 0xC9 */ {"RET", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 16, 16}, \
/* 0xCA */ {"JP Z, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_z, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 12}, \
/* 0xCB */ {"CB", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 4, 4}, \
/* 0xCC */ {"CALL Z, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_z, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 12}, \
/* 0xCD */ {"CALL 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 24}, \
/* 0xCE */ {"ADC A, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 8, 8}, \
/* 0xCF */ {"RST 08H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_08, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16}, \
/* 0xD0 */ {"RET NC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, &gb_cpu::_operand_get_flags_is_nc, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 20, 8}, \
/* 0xD1 */ {"POP DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_de, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0xD2 */ {"JP NC, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_nc, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 12}, \
/* 0xD3 */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xD4 */ {"CALL NC, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_nc, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 12}, \
/* 0xD5 */ {"PUSH DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_de, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_ld, 16, 16}, \
/* 0xD6 */ {"SUB 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 8, 8}, \
/* 0xD7 */ {"RST 10H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_10, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16}, \
/* 0xD8 */ {"RET C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, &gb_cpu::_operand_get_flags_is_c, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 20, 8}, \
/* 0xD9 */ {"RETI", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_reti, 16, 16}, \
/* 0xDA */ {"JP C, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_c, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 12}, \
/* 0xDB */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xDC */ {"CALL C, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_c, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 12}, \
/* 0xDD */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xDE */ {"SBC A, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 8, 8}, \
/* 0xDF */ {"RST 18H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_18, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16}, \
/* 0xE0 */ {"LD (0xff00+0x%02x), A", &gb_cpu::_op_print_type4, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_mem_8_plus_io_base, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0xE1 */ {"POP HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0xE2 */ {"LD (0xff00+C), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_c_plus_io_base, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0xE3 */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xE4 */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xE5 */ {"PUSH HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_ld, 16, 16}, \
/* 0xE6 */ {"AND 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 8, 8}, \
/* 0xE7 */ {"RST 20H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_20, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16}, \
/* 0xE8 */ {"ADD SP, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_sp, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_addsp, 16, 16}, \
/* 0xE9 */ {"JP (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 4, 4}, \
/* 0xEA */ {"LD (0x%04x), A", &gb_cpu::_op_print_type3, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 16, 16}, \
/* 0xEB */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xEC */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xED */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xEE */ {"XOR 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 8, 8}, \
/* 0xEF */ {"RST 28H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_28, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16}, \
/* 0xF0 */ {"LD A, (0x%02x)", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8_plus_io_base_mem, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0xF1 */ {"POP AF", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_af, &gb_cpu::_op_exec_ld, 12, 12}, \
/* 0xF2 */ {"LD A, (0xff00+C)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c_plus_io_base_mem, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0xF3 */ {"DI", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_di, 4, 4}, \
/* 0xF4 */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xF5 */ {"PUSH AF", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_af, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_ld, 16, 16}, \
/* 0xF6 */ {"OR 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 8, 8}, \
/* 0xF7 */ {"RST 30H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_30, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16}, \
/* 0xF8 */ {"LD HL, SP+0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_sp, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_ldhl, 12, 12}, \
/* 0xF9 */ {"LD SP, HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, nullptr, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_ld, 8, 8}, \
/* 0xFA */ {"LD A, (0x%04x)", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16_mem, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 16, 16}, \
/* 0xFB */ {"EI", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_ei, 4, 4}, \
/* 0xFC */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xFD */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0}, \
/* 0xFE */ {"CP 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 8, 8}, \
/* 0xFF */ {"RST 38H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_38, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16}, \
}

gb_cpu::gb_cpu() {
    _m_instructions = std::vector<instruction_t>(INSTRUCTIONS_LIST_INIT);
}

gb_cpu::~gb_cpu() {
}

void gb_cpu::dump_registers() {
    char buf[256];
    snprintf(buf, 256, "AF: 0x%02x.%02x BC: 0x%02x.%02x DE: 0x%02x.%02x HL: 0x%02x.%02x SP: 0x%04x PC: 0x%04x FLAGS: %c%c%c%c",
        _m_registers.a, _m_registers.f, _m_registers.b, _m_registers.c, _m_registers.d, _m_registers.e, _m_registers.h, _m_registers.l, _m_registers.sp, _m_registers.pc,
        FLAGS_IS_SET(FLAGS_Z) ? 'Z' : '-', FLAGS_IS_SET(FLAGS_N) ? 'Z' : '-', FLAGS_IS_SET(FLAGS_H) ? 'Z' : '-', FLAGS_IS_SET(FLAGS_C) ? 'Z' : '-');

    std::cout << buf << std::endl;
}

uint16_t gb_cpu::get_pc() {
    return _m_registers.pc;
}

uint64_t gb_cpu::step() {
    uint8_t opcode = mem[_m_registers.pc];

    instruction_t *instruction = &_m_instructions[opcode];

    if (instruction->op_exec == nullptr) {
        _op_print_type0("UNKNOWN", _m_registers.pc, 0, 0);
        _m_registers.pc++;
        return 0;
    }

    int cycles = (this->*(instruction->op_exec))(instruction);

    return cycles;
}

uint64_t gb_cpu::_op_exec_nop(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    (this->*(instruction->op_print))(instruction->disassembly, pc, 0, 0);

    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_stop(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    // TODO: Implement stop

    (this->*(instruction->op_print))(instruction->disassembly, pc, 0, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_halt(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    // TODO: Implement halt

    (this->*(instruction->op_print))(instruction->disassembly, pc, 0, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_ld(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t data = (this->*(instruction->get_operand1))();
    uint16_t addr = instruction->get_operand2 == nullptr ? 0 : (this->*(instruction->get_operand2))();

    (this->*(instruction->set_operand))(addr, data);

    (this->*(instruction->op_print))(instruction->disassembly, pc, data, addr);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_ldhl(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    int32_t offset = (this->*(instruction->get_operand1))();
    int32_t sp = (this->*(instruction->get_operand2))();
    uint32_t effective_addr = sp + offset;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_H_16(sp, offset);
    FLAGS_SET_IF_C_16(effective_addr);

    (this->*(instruction->set_operand))(0, effective_addr);

    (this->*(instruction->op_print))(instruction->disassembly, pc, offset, sp);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_add8(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t a1 = (this->*(instruction->get_operand1))();
    uint16_t a2 = (this->*(instruction->get_operand2))();
    uint16_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET_IF_H(a1, a2);
    FLAGS_SET_IF_C(sum);

    (this->*(instruction->set_operand))(0, sum);

    (this->*(instruction->op_print))(instruction->disassembly, pc, a1, a2);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_add16(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint32_t a1 = (this->*(instruction->get_operand1))();
    uint32_t a2 = (this->*(instruction->get_operand2))();
    uint32_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_H_16(a1, a2);
    FLAGS_SET_IF_C_16(sum);

    (this->*(instruction->set_operand))(0, sum);

    (this->*(instruction->op_print))(instruction->disassembly, pc, a1, a2);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_addsp(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    int32_t a1 = (this->*(instruction->get_operand1))();
    int32_t a2 = (this->*(instruction->get_operand2))();
    uint32_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_H_16(a1, a2);
    FLAGS_SET_IF_C_16(sum);

    (this->*(instruction->set_operand))(0, sum);

    (this->*(instruction->op_print))(instruction->disassembly, pc, a1, a2);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_adc(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t a1 = (this->*(instruction->get_operand1))() + (FLAGS_IS_SET(FLAGS_C) ? 1 : 0);
    uint16_t a2 = (this->*(instruction->get_operand2))();
    uint16_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET_IF_H(a1, a2);
    FLAGS_SET_IF_C(sum);

    (this->*(instruction->set_operand))(0, sum);

    (this->*(instruction->op_print))(instruction->disassembly, pc, a1, a2);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_sub(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    int16_t a1 = (this->*(instruction->get_operand1))();
    int16_t a2 = (this->*(instruction->get_operand2))();
    int16_t sum = a2 - a1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET(FLAGS_N);
    FLAGS_SET_IF_H(a2, -a1);
    FLAGS_SET_IF_C(sum);

    if (instruction->set_operand != nullptr) {
        (this->*(instruction->set_operand))(0, sum);
    }

    (this->*(instruction->op_print))(instruction->disassembly, pc, a1, a2);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_sbc(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    int16_t a1 = (this->*(instruction->get_operand1))() + (FLAGS_IS_SET(FLAGS_C) ? 1 : 0);
    int16_t a2 = (this->*(instruction->get_operand2))();
    int16_t sum = a2 - a1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET(FLAGS_N);
    FLAGS_SET_IF_H(a2, -a1);
    FLAGS_SET_IF_C(sum);

    (this->*(instruction->set_operand))(0, sum);

    (this->*(instruction->op_print))(instruction->disassembly, pc, a1, a2);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_jr(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    int cycles = instruction->cycles_lo;
    int16_t offset = (int8_t)(this->*(instruction->get_operand1))();
    uint16_t jump_pc = (int16_t)_m_registers.pc + offset;
    uint16_t do_jump = instruction->get_operand2 == nullptr ? 1 : (this->*(instruction->get_operand2))();

    if (do_jump != 0) {
        (this->*(instruction->set_operand))(0, jump_pc);
        cycles = instruction->cycles_hi;
    }

    (this->*(instruction->op_print))(instruction->disassembly, pc, jump_pc, 0);
    return cycles;
}

uint64_t gb_cpu::_op_exec_jp(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    int cycles = instruction->cycles_lo;
    uint16_t jump_pc = (this->*(instruction->get_operand1))();
    uint16_t do_jump = instruction->get_operand2 == nullptr ? 1 : (this->*(instruction->get_operand2))();

    if (do_jump != 0) {
        (this->*(instruction->set_operand))(0, jump_pc);
        cycles = instruction->cycles_hi;
    }

    (this->*(instruction->op_print))(instruction->disassembly, pc, jump_pc, 0);
    return cycles;
}

uint64_t gb_cpu::_op_exec_call(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    int cycles = instruction->cycles_lo;
    uint16_t jump_pc = (this->*(instruction->get_operand1))();
    uint16_t do_jump = instruction->get_operand2 == nullptr ? 1 : (this->*(instruction->get_operand2))();

    if (do_jump != 0) {
        (this->*(instruction->set_operand))(0, _m_registers.pc);
        _m_registers.pc = jump_pc;
        cycles = instruction->cycles_hi;
    }

    (this->*(instruction->op_print))(instruction->disassembly, pc, jump_pc, 0);
    return cycles;
}

uint64_t gb_cpu::_op_exec_ret(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    int cycles = instruction->cycles_lo;
    uint16_t do_jump = instruction->get_operand2 == nullptr ? 1 : (this->*(instruction->get_operand2))();

    if (do_jump != 0) {
        uint16_t jump_pc = (this->*(instruction->get_operand1))();
        (this->*(instruction->set_operand))(0, jump_pc);
        cycles = instruction->cycles_hi;
    }

    (this->*(instruction->op_print))(instruction->disassembly, pc, 0, 0);
    return cycles;
}

uint64_t gb_cpu::_op_exec_reti(instruction_t *instruction) {
    // TODO: Enable interrupts
    return gb_cpu::_op_exec_ret(instruction);
}

uint64_t gb_cpu::_op_exec_rst(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t jump_pc = (this->*(instruction->get_operand1))();

    (this->*(instruction->set_operand))(0, pc);
    _m_registers.pc = jump_pc;

    (this->*(instruction->op_print))(instruction->disassembly, pc, jump_pc, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_da(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t operand = (this->*(instruction->get_operand1))();
    (this->*(instruction->set_operand))(0, operand);

    (this->*(instruction->op_print))(instruction->disassembly, pc, operand, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_rlc(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t val = (this->*(instruction->get_operand1))() << 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_C(val);

    (this->*(instruction->set_operand))(0, val);

    (this->*(instruction->op_print))(instruction->disassembly, pc, val, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_rrc(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t val = (this->*(instruction->get_operand1))();
    int bit1 = val & 0x1;
    val = val >> 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_C(bit1 << 8);

    (this->*(instruction->set_operand))(0, val);

    (this->*(instruction->op_print))(instruction->disassembly, pc, val, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_rl(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t val = (this->*(instruction->get_operand1))() << 1;
    int carry = FLAGS_IS_SET(FLAGS_C) ? 1 : 0;
    val |= carry;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_C(val);

    (this->*(instruction->set_operand))(0, val);

    (this->*(instruction->op_print))(instruction->disassembly, pc, val, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_rr(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t val = (this->*(instruction->get_operand1))();
    int bit1 = val & 0x1;
    int carry = FLAGS_IS_SET(FLAGS_C) ? 1 : 0;
    val = (val >> 1) | (carry << 0x7);

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_C(bit1 << 8);

    (this->*(instruction->set_operand))(0, val);

    (this->*(instruction->op_print))(instruction->disassembly, pc, val, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_cpl(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t val = (this->*(instruction->get_operand1))();

    FLAGS_SET(FLAGS_N | FLAGS_H);

    (this->*(instruction->set_operand))(0, ~val);

    (this->*(instruction->op_print))(instruction->disassembly, pc, val, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_inc(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t val = (this->*(instruction->get_operand1))();

    (this->*(instruction->set_operand))(0, ++val);

    (this->*(instruction->op_print))(instruction->disassembly, pc, val, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_dec(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t val = (this->*(instruction->get_operand1))();

    (this->*(instruction->set_operand))(0, --val);

    (this->*(instruction->op_print))(instruction->disassembly, pc, val, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_incf(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t val = (this->*(instruction->get_operand1))();
    uint16_t addr = instruction->get_operand2 == nullptr ? 0 : (this->*(instruction->get_operand2))();
    uint16_t vali = val + 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H);
    FLAGS_SET_IF_Z(vali);
    FLAGS_SET_IF_H(val, 1);

    (this->*(instruction->set_operand))(addr, vali);

    (this->*(instruction->op_print))(instruction->disassembly, pc, val, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_decf(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t val = (this->*(instruction->get_operand1))();
    uint16_t addr = instruction->get_operand2 == nullptr ? 0 : (this->*(instruction->get_operand2))();
    uint16_t vald = val - 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H);
    FLAGS_SET_IF_Z(vald);
    FLAGS_SET_IF_H(val, -1);

    (this->*(instruction->set_operand))(addr, vald);

    (this->*(instruction->op_print))(instruction->disassembly, pc, val, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_scf(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    FLAGS_CLEAR(FLAGS_N | FLAGS_H);
    FLAGS_SET(FLAGS_C);

    (this->*(instruction->op_print))(instruction->disassembly, pc, 0, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_ccf(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    FLAGS_CLEAR(FLAGS_N | FLAGS_H);
    FLAGS_TOGGLE(FLAGS_C);

    (this->*(instruction->op_print))(instruction->disassembly, pc, 0, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_and(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t a1 = (this->*(instruction->get_operand1))();
    uint16_t a2 = (this->*(instruction->get_operand2))();
    uint16_t result = a1 & a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(result);
    FLAGS_SET(FLAGS_H);

    (this->*(instruction->set_operand))(0, result);

    (this->*(instruction->op_print))(instruction->disassembly, pc, a1, a2);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_xor(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t a1 = (this->*(instruction->get_operand1))();
    uint16_t a2 = (this->*(instruction->get_operand2))();
    uint16_t result = a1 ^ a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(result);

    (this->*(instruction->set_operand))(0, result);

    (this->*(instruction->op_print))(instruction->disassembly, pc, a1, a2);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_or(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    uint16_t a1 = (this->*(instruction->get_operand1))();
    uint16_t a2 = (this->*(instruction->get_operand2))();
    uint16_t result = a1 | a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(result);

    (this->*(instruction->set_operand))(0, result);

    (this->*(instruction->op_print))(instruction->disassembly, pc, a1, a2);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_di(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    // TODO: Disable interrupts

    (this->*(instruction->op_print))(instruction->disassembly, pc, 0, 0);
    return instruction->cycles_hi;
}

uint64_t gb_cpu::_op_exec_ei(instruction_t *instruction) {
    uint16_t pc = _m_registers.pc++;

    // TODO: Enable interrupts

    (this->*(instruction->op_print))(instruction->disassembly, pc, 0, 0);
    return instruction->cycles_hi;
}

uint16_t gb_cpu::_operand_get_register_a() {
    return _m_registers.f;
}

uint16_t gb_cpu::_operand_get_register_f() {
    return _m_registers.f;
}

uint16_t gb_cpu::_operand_get_register_af() {
    return _m_registers.af;
}

uint16_t gb_cpu::_operand_get_register_b() {
    return _m_registers.b;
}

uint16_t gb_cpu::_operand_get_register_c() {
    return _m_registers.c;
}

uint16_t gb_cpu::_operand_get_register_bc() {
    return _m_registers.bc;
}

uint16_t gb_cpu::_operand_get_register_d() {
    return _m_registers.d;
}

uint16_t gb_cpu::_operand_get_register_e() {
    return _m_registers.e;
}

uint16_t gb_cpu::_operand_get_register_de() {
    return _m_registers.de;
}

uint16_t gb_cpu::_operand_get_register_h() {
    return _m_registers.h;
}

uint16_t gb_cpu::_operand_get_register_l() {
    return _m_registers.l;
}

uint16_t gb_cpu::_operand_get_register_hl() {
    return _m_registers.hl;
}

uint16_t gb_cpu::_operand_get_register_sp() {
    return _m_registers.sp;
}

uint16_t gb_cpu::_operand_get_register_pc() {
    return _m_registers.pc;
}

uint16_t gb_cpu::_operand_get_register_hl_plus() {
    uint16_t hl = _m_registers.hl;
    _m_registers.hl++;
    return hl;
}

uint16_t gb_cpu::_operand_get_register_hl_minus() {
    uint16_t hl = _m_registers.hl;
    _m_registers.hl--;
    return hl;
}

uint16_t gb_cpu::_operand_get_mem_8() {
    return mem[_m_registers.pc++];
}

uint16_t gb_cpu::_operand_get_mem_8_plus_io_base() {
    uint16_t offset = _operand_get_mem_8();
    return (offset + GB_MEM_IO_BASE);
}

uint16_t gb_cpu::_operand_get_mem_8_plus_io_base_mem() {
    uint16_t addr = _operand_get_mem_8_plus_io_base();
    return mem[addr];
}

uint16_t gb_cpu::_operand_get_register_c_plus_io_base() {
    uint16_t offset = _operand_get_register_c();
    return (offset + GB_MEM_IO_BASE);
}

uint16_t gb_cpu::_operand_get_register_c_plus_io_base_mem() {
    uint16_t addr = _operand_get_register_c_plus_io_base();
    return mem[addr];
}

uint16_t gb_cpu::_operand_get_mem_16() {
    uint16_t byte_lo = mem[_m_registers.pc++];
    uint16_t byte_hi = mem[_m_registers.pc++];
    return (byte_hi << 8) | byte_lo;
}

uint16_t gb_cpu::_operand_get_mem_16_mem() {
    uint16_t addr = _operand_get_mem_16();
    return mem[addr];
}

uint16_t gb_cpu::_operand_get_mem_bc() {
    return mem[_m_registers.bc];
}

uint16_t gb_cpu::_operand_get_mem_de() {
    return mem[_m_registers.de];
}

uint16_t gb_cpu::_operand_get_mem_hl() {
    return mem[_m_registers.hl];
}

uint16_t gb_cpu::_operand_get_mem_sp_8() {
    return mem[_m_registers.sp++];
}

uint16_t gb_cpu::_operand_get_mem_sp_16() {
    uint16_t byte_lo = mem[_m_registers.sp++];
    uint16_t byte_hi = mem[_m_registers.sp++];
    return (byte_hi << 8) | byte_lo;
}

uint16_t gb_cpu::_operand_get_flags_is_nz() {
    return FLAGS_IS_CLEAR(FLAGS_Z);
}

uint16_t gb_cpu::_operand_get_flags_is_z() {
    return FLAGS_IS_SET(FLAGS_Z);
}

uint16_t gb_cpu::_operand_get_flags_is_nc() {
    return FLAGS_IS_CLEAR(FLAGS_C);
}

uint16_t gb_cpu::_operand_get_flags_is_c() {
    return FLAGS_IS_SET(FLAGS_C);
}

uint16_t gb_cpu::_operand_get_rst_00() {
    return 0x0;
}

uint16_t gb_cpu::_operand_get_rst_08() {
    return 0x8;
}

uint16_t gb_cpu::_operand_get_rst_10() {
    return 0x10;
}

uint16_t gb_cpu::_operand_get_rst_18() {
    return 0x18;
}

uint16_t gb_cpu::_operand_get_rst_20() {
    return 0x20;
}

uint16_t gb_cpu::_operand_get_rst_28() {
    return 0x28;
}

uint16_t gb_cpu::_operand_get_rst_30() {
    return 0x30;
}

uint16_t gb_cpu::_operand_get_rst_38() {
    return 0x38;
}

void gb_cpu::_operand_set_register_a(uint16_t addr, uint16_t val) {
    _m_registers.a = (uint8_t)val;
}

void gb_cpu::_operand_set_register_f(uint16_t addr, uint16_t val) {
    _m_registers.f = (uint8_t)val;
}

void gb_cpu::_operand_set_register_af(uint16_t addr, uint16_t val) {
    _m_registers.af = val;
}

void gb_cpu::_operand_set_register_b(uint16_t addr, uint16_t val) {
    _m_registers.b = (uint8_t)val;
}

void gb_cpu::_operand_set_register_c(uint16_t addr, uint16_t val) {
    _m_registers.c = (uint8_t)val;
}

void gb_cpu::_operand_set_register_bc(uint16_t addr, uint16_t val) {
    _m_registers.bc = val;
}

void gb_cpu::_operand_set_register_d(uint16_t addr, uint16_t val) {
    _m_registers.d = (uint8_t)val;
}

void gb_cpu::_operand_set_register_e(uint16_t addr, uint16_t val) {
    _m_registers.e = (uint8_t)val;
}

void gb_cpu::_operand_set_register_de(uint16_t addr, uint16_t val) {
    _m_registers.de = val;
}

void gb_cpu::_operand_set_register_h(uint16_t addr, uint16_t val) {
    _m_registers.h = (uint8_t)val;
}

void gb_cpu::_operand_set_register_l(uint16_t addr, uint16_t val) {
    _m_registers.l = (uint8_t)val;
}

void gb_cpu::_operand_set_register_hl(uint16_t addr, uint16_t val) {
    _m_registers.hl = val;
}

void gb_cpu::_operand_set_register_sp(uint16_t addr, uint16_t val) {
    _m_registers.sp = val;
}

void gb_cpu::_operand_set_register_pc(uint16_t addr, uint16_t val) {
    _m_registers.pc = val;
}

void gb_cpu::_operand_set_mem_8(uint16_t addr, uint16_t val) {
    mem[addr] = (uint8_t)val;
}

void gb_cpu::_operand_set_mem_sp_16(uint16_t addr, uint16_t val) {
    mem[--_m_registers.sp] = (uint8_t)(val >> 8);
    mem[--_m_registers.sp] = (uint8_t)val;
}

void gb_cpu::_op_print_type0(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) {
    char out[256];
    snprintf(out, 256, "%04x: %s", pc, disassembly.c_str());
    std::string fmt = out;
    std::cout << fmt << std::endl;
}

void gb_cpu::_op_print_type1(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) {
    char buf[256], out[256];
    snprintf(buf, 256, disassembly.c_str(), operand1);
    snprintf(out, 256, "%04x: %s", pc, buf);
    std::string fmt = out;
    std::cout << fmt << std::endl;
}

void gb_cpu::_op_print_type2(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) {
    _op_print_type1(disassembly, pc, (uint8_t)operand1, (uint8_t)operand2);
}

void gb_cpu::_op_print_type3(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) {
    char buf[256], out[256];
    snprintf(buf, 256, disassembly.c_str(), operand2);
    snprintf(out, 256, "%04x: %s", pc, buf);
    std::string fmt = out;
    std::cout << fmt << std::endl;
}

void gb_cpu::_op_print_type4(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) {
    _op_print_type3(disassembly, pc, (uint8_t)operand1, (uint8_t)operand2);
}
