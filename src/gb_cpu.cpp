#include <iostream>

#include "gb_cpu.h"
#include "gb_memory_map.h"

#define FLAGS_IS_SET(flags)      ((m_registers.f & (flags)) != 0)
#define FLAGS_IS_CLEAR(flags)    ((m_registers.f & (flags)) == 0)
#define FLAGS_SET(flags)         { m_registers.f |= (flags); }
#define FLAGS_CLEAR(flags)       { m_registers.f &= ~(flags); }
#define FLAGS_TOGGLE(flags)      { m_registers.f ^= (flags); }

#define FLAGS_SET_IF_Z(x)        { if ((x) == 0) FLAGS_SET(FLAGS_Z); }
#define FLAGS_SET_IF_H(x,y)      { if ((((x) & 0x0f) + ((y) & 0x0f)) & 0xf0) FLAGS_SET(FLAGS_H); }
#define FLAGS_SET_IF_C(x)        { if ((x) & 0xff00) FLAGS_SET(FLAGS_C); }

#define FLAGS_SET_IF_H_16(x,y)   { if ((((x) & 0xfff) + ((y) & 0xfff)) & 0xf000) FLAGS_SET(FLAGS_H); }
#define FLAGS_SET_IF_C_16(x)     { if ((x) & 0xffff0000) FLAGS_SET(FLAGS_C); }

#define INSTRUCTIONS_LIST_INIT \
{\
/* 0x00 */ {"NOP", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_nop, 4, 4},\
/* 0x01 */ {"LD BC, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_bc, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0x02 */ {"LD (BC), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_bc, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x03 */ {"INC BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_bc, nullptr, &gb_cpu::_operand_set_register_bc, &gb_cpu::_op_exec_inc, 8, 8},\
/* 0x04 */ {"INC B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_incf, 4, 4},\
/* 0x05 */ {"DEC B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_decf, 4, 4},\
/* 0x06 */ {"LD B, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x07 */ {"RLCA", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rlca, 4, 4},\
/* 0x08 */ {"LD (0x%04x), SP", &gb_cpu::_op_print_type3, &gb_cpu::_operand_get_register_sp, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 20, 20},\
/* 0x09 */ {"ADD HL, BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_bc, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_add16, 8, 8},\
/* 0x0A */ {"LD A, (BC)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_bc, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x0B */ {"DEC BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_bc, nullptr, &gb_cpu::_operand_set_register_bc, &gb_cpu::_op_exec_dec, 8, 8},\
/* 0x0C */ {"INC C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_incf, 4, 4},\
/* 0x0D */ {"DEC C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_decf, 4, 4},\
/* 0x0E */ {"LD C, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x0F */ {"RRCA", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rrca, 4, 4},\
/* 0x10 */ {"STOP 0", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_8, nullptr, nullptr, &gb_cpu::_op_exec_stop, 4, 4},\
/* 0x11 */ {"LD DE, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_de, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0x12 */ {"LD (DE), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_de, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x13 */ {"INC DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_de, nullptr, &gb_cpu::_operand_set_register_de, &gb_cpu::_op_exec_inc, 8, 8},\
/* 0x14 */ {"INC D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_incf, 4, 4},\
/* 0x15 */ {"DEC D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_decf, 4, 4},\
/* 0x16 */ {"LD D, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x17 */ {"RLA", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rla, 4, 4},\
/* 0x18 */ {"JR 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 12},\
/* 0x19 */ {"ADD HL, DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_de, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_add16, 8, 8},\
/* 0x1A */ {"LD A, (DE)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_de, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x1B */ {"DEC DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_de, nullptr, &gb_cpu::_operand_set_register_de, &gb_cpu::_op_exec_dec, 8, 8},\
/* 0x1C */ {"INC E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_incf, 4, 4},\
/* 0x1D */ {"DEC E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_decf, 4, 4},\
/* 0x1E */ {"LD E, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x1F */ {"RRA", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rra, 4, 4},\
/* 0x20 */ {"JR NZ, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_flags_is_nz, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 8},\
/* 0x21 */ {"LD HL, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0x22 */ {"LD (HL+), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_hl_plus, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x23 */ {"INC HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, nullptr, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_inc, 8, 8},\
/* 0x24 */ {"INC H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_incf, 4, 4},\
/* 0x25 */ {"DEC H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_decf, 4, 4},\
/* 0x26 */ {"LD H, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x27 */ {"DA A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_da, 4, 4},\
/* 0x28 */ {"JR Z, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_flags_is_z, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 8},\
/* 0x29 */ {"ADD HL, HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_add16, 8, 8},\
/* 0x2A */ {"LD A, (HL+)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl_plus, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x2B */ {"DEC HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, nullptr, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_dec, 8, 8},\
/* 0x2C */ {"INC L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_incf, 4, 4},\
/* 0x2D */ {"DEC L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_decf, 4, 4},\
/* 0x2E */ {"LD L, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x2F */ {"CPL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_cpl, 4, 4},\
/* 0x30 */ {"JR NC, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_flags_is_nc, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 8},\
/* 0x31 */ {"LD SP, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0x32 */ {"LD (HL-), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_hl_minus, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x33 */ {"INC SP", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_sp, nullptr, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_inc, 8, 8},\
/* 0x34 */ {"INC (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_incf, 12, 12},\
/* 0x35 */ {"DEC (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_decf, 12, 12},\
/* 0x36 */ {"LD (HL), 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0x37 */ {"SCF", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_scf, 4, 4},\
/* 0x38 */ {"JR C, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_flags_is_c, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jr, 12, 8},\
/* 0x39 */ {"ADD HL, SP", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_sp, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_add16, 8, 8},\
/* 0x3A */ {"LD A, (HL-)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl_minus, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x3B */ {"DEC SP", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_sp, nullptr, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_dec, 8, 8},\
/* 0x3C */ {"INC A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_incf, 4, 4},\
/* 0x3D */ {"DEC A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_decf, 4, 4},\
/* 0x3E */ {"LD A, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x3F */ {"CCF", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_ccf, 4, 4},\
/* 0x40 */ {"LD B, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x41 */ {"LD B, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x42 */ {"LD B, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x43 */ {"LD B, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x44 */ {"LD B, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x45 */ {"LD B, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x46 */ {"LD B, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x47 */ {"LD B, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x48 */ {"LD C, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x49 */ {"LD C, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x4A */ {"LD C, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x4B */ {"LD C, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x4C */ {"LD C, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x4D */ {"LD C, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x4E */ {"LD C, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x4F */ {"LD C, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x50 */ {"LD D, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x51 */ {"LD D, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x52 */ {"LD D, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x53 */ {"LD D, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x54 */ {"LD D, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x55 */ {"LD D, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x56 */ {"LD D, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x57 */ {"LD D, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x58 */ {"LD E, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x59 */ {"LD E, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x5A */ {"LD E, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x5B */ {"LD E, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x5C */ {"LD E, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x5D */ {"LD E, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x5E */ {"LD E, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x5F */ {"LD E, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x60 */ {"LD H, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x61 */ {"LD H, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x62 */ {"LD H, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x63 */ {"LD H, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x64 */ {"LD H, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x65 */ {"LD H, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x66 */ {"LD H, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x67 */ {"LD H, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x68 */ {"LD L, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x69 */ {"LD L, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x6A */ {"LD L, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x6B */ {"LD L, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x6C */ {"LD L, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x6D */ {"LD L, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x6E */ {"LD L, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x6F */ {"LD L, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x70 */ {"LD (HL), B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x71 */ {"LD (HL), C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x72 */ {"LD (HL), D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x73 */ {"LD (HL), E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x74 */ {"LD (HL), H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x75 */ {"LD (HL), L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x76 */ {"HALT", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_halt, 4, 4},\
/* 0x77 */ {"LD (HL), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x78 */ {"LD A, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x79 */ {"LD A, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x7A */ {"LD A, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x7B */ {"LD A, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x7C */ {"LD A, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x7D */ {"LD A, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x7E */ {"LD A, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0x7F */ {"LD A, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 4, 4},\
/* 0x80 */ {"ADD A, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4},\
/* 0x81 */ {"ADD A, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4},\
/* 0x82 */ {"ADD A, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4},\
/* 0x83 */ {"ADD A, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4},\
/* 0x84 */ {"ADD A, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4},\
/* 0x85 */ {"ADD A, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4},\
/* 0x86 */ {"ADD A, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 8, 8},\
/* 0x87 */ {"ADD A, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 4, 4},\
/* 0x88 */ {"ADC A, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4},\
/* 0x89 */ {"ADC A, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4},\
/* 0x8A */ {"ADC A, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4},\
/* 0x8B */ {"ADC A, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4},\
/* 0x8C */ {"ADC A, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4},\
/* 0x8D */ {"ADC A, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4},\
/* 0x8E */ {"ADC A, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 8, 8},\
/* 0x8F */ {"ADC A, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 4, 4},\
/* 0x90 */ {"SUB B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0x91 */ {"SUB C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0x92 */ {"SUB D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0x93 */ {"SUB E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0x94 */ {"SUB H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0x95 */ {"SUB L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0x96 */ {"SUB (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 8, 8},\
/* 0x97 */ {"SUB A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0x98 */ {"SBC A, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4},\
/* 0x99 */ {"SBC A, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4},\
/* 0x9A */ {"SBC A, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4},\
/* 0x9B */ {"SBC A, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4},\
/* 0x9C */ {"SBC A, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4},\
/* 0x9D */ {"SBC A, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4},\
/* 0x9E */ {"SBC A, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 8, 8},\
/* 0x9F */ {"SBC A, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 4, 4},\
/* 0xA0 */ {"AND B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4},\
/* 0xA1 */ {"AND C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4},\
/* 0xA2 */ {"AND D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4},\
/* 0xA3 */ {"AND E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4},\
/* 0xA4 */ {"AND H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4},\
/* 0xA5 */ {"AND L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4},\
/* 0xA6 */ {"AND (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 8, 8},\
/* 0xA7 */ {"AND A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 4, 4},\
/* 0xA8 */ {"XOR B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4},\
/* 0xA9 */ {"XOR C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4},\
/* 0xAA */ {"XOR D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4},\
/* 0xAB */ {"XOR E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4},\
/* 0xAC */ {"XOR H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4},\
/* 0xAD */ {"XOR L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4},\
/* 0xAE */ {"XOR (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 8, 8},\
/* 0xAF */ {"XOR A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 4, 4},\
/* 0xB0 */ {"OR B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4},\
/* 0xB1 */ {"OR C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4},\
/* 0xB2 */ {"OR D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4},\
/* 0xB3 */ {"OR E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4},\
/* 0xB4 */ {"OR H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4},\
/* 0xB5 */ {"OR L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4},\
/* 0xB6 */ {"OR (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 8, 8},\
/* 0xB7 */ {"OR A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 4, 4},\
/* 0xB8 */ {"CP B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0xB9 */ {"CP C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0xBA */ {"CP D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0xBB */ {"CP E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0xBC */ {"CP H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0xBD */ {"CP L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0xBE */ {"CP (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 8, 8},\
/* 0xBF */ {"CP A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 4, 4},\
/* 0xC0 */ {"RET NZ", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, &gb_cpu::_operand_get_flags_is_nz, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 20, 8},\
/* 0xC1 */ {"POP BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_bc, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0xC2 */ {"JP NZ, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_nz, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 12},\
/* 0xC3 */ {"JP 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 16},\
/* 0xC4 */ {"CALL NZ, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_nz, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 12},\
/* 0xC5 */ {"PUSH BC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_bc, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_ld, 16, 16},\
/* 0xC6 */ {"ADD A, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_add8, 8, 8},\
/* 0xC7 */ {"RST 00H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_00, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16},\
/* 0xC8 */ {"RET Z", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, &gb_cpu::_operand_get_flags_is_z, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 20, 8},\
/* 0xC9 */ {"RET", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 16, 16},\
/* 0xCA */ {"JP Z, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_z, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 12},\
/* 0xCB */ {"CB", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_8, nullptr, nullptr, &gb_cpu::_op_exec_cb, 4, 4},\
/* 0xCC */ {"CALL Z, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_z, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 12},\
/* 0xCD */ {"CALL 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 24},\
/* 0xCE */ {"ADC A, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_adc, 8, 8},\
/* 0xCF */ {"RST 08H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_08, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16},\
/* 0xD0 */ {"RET NC", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, &gb_cpu::_operand_get_flags_is_nc, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 20, 8},\
/* 0xD1 */ {"POP DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_de, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0xD2 */ {"JP NC, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_nc, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 12},\
/* 0xD3 */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xD4 */ {"CALL NC, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_nc, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 12},\
/* 0xD5 */ {"PUSH DE", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_de, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_ld, 16, 16},\
/* 0xD6 */ {"SUB 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sub, 8, 8},\
/* 0xD7 */ {"RST 10H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_10, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16},\
/* 0xD8 */ {"RET C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, &gb_cpu::_operand_get_flags_is_c, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_ret, 20, 8},\
/* 0xD9 */ {"RETI", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_reti, 16, 16},\
/* 0xDA */ {"JP C, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_c, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 16, 12},\
/* 0xDB */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xDC */ {"CALL C, 0x%04x", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_flags_is_c, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_call, 24, 12},\
/* 0xDD */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xDE */ {"SBC A, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sbc, 8, 8},\
/* 0xDF */ {"RST 18H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_18, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16},\
/* 0xE0 */ {"LD (0xff00+0x%02x), A", &gb_cpu::_op_print_type4, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_mem_8_plus_io_base, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0xE1 */ {"POP HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0xE2 */ {"LD (0xff00+C), A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_register_c_plus_io_base, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0xE3 */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xE4 */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xE5 */ {"PUSH HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_ld, 16, 16},\
/* 0xE6 */ {"AND 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_and, 8, 8},\
/* 0xE7 */ {"RST 20H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_20, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16},\
/* 0xE8 */ {"ADD SP, 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_sp, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_addsp, 16, 16},\
/* 0xE9 */ {"JP (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, nullptr, &gb_cpu::_operand_set_register_pc, &gb_cpu::_op_exec_jp, 4, 4},\
/* 0xEA */ {"LD (0x%04x), A", &gb_cpu::_op_print_type3, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_mem_16, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_ld, 16, 16},\
/* 0xEB */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xEC */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xED */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xEE */ {"XOR 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_xor, 8, 8},\
/* 0xEF */ {"RST 28H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_28, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16},\
/* 0xF0 */ {"LD A, (0x%02x)", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8_plus_io_base_mem, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0xF1 */ {"POP AF", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_sp_16, nullptr, &gb_cpu::_operand_set_register_af, &gb_cpu::_op_exec_ld, 12, 12},\
/* 0xF2 */ {"LD A, (0xff00+C)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c_plus_io_base_mem, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0xF3 */ {"DI", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_di, 4, 4},\
/* 0xF4 */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xF5 */ {"PUSH AF", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_af, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_ld, 16, 16},\
/* 0xF6 */ {"OR 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_or, 8, 8},\
/* 0xF7 */ {"RST 30H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_30, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16},\
/* 0xF8 */ {"LD HL, SP+0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_sp, &gb_cpu::_operand_set_register_hl, &gb_cpu::_op_exec_ldhl, 12, 12},\
/* 0xF9 */ {"LD SP, HL", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_hl, nullptr, &gb_cpu::_operand_set_register_sp, &gb_cpu::_op_exec_ld, 8, 8},\
/* 0xFA */ {"LD A, (0x%04x)", &gb_cpu::_op_print_type1, &gb_cpu::_operand_get_mem_16_mem, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_ld, 16, 16},\
/* 0xFB */ {"EI", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, &gb_cpu::_op_exec_ei, 4, 4},\
/* 0xFC */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xFD */ {"UNKOWN", &gb_cpu::_op_print_type0, nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xFE */ {"CP 0x%02x", &gb_cpu::_op_print_type2, &gb_cpu::_operand_get_mem_8, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_op_exec_sub, 8, 8},\
/* 0xFF */ {"RST 38H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_rst_38, nullptr, &gb_cpu::_operand_set_mem_sp_16, &gb_cpu::_op_exec_rst, 16, 16},\
}

#define CB_INSTRUCTIONS_LIST_INIT \
{\
/* 0x00 */ {"RLC B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_rlc, 8, 8},\
/* 0x01 */ {"RLC C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_rlc, 8, 8},\
/* 0x02 */ {"RLC D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_rlc, 8, 8},\
/* 0x03 */ {"RLC E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_rlc, 8, 8},\
/* 0x04 */ {"RLC H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_rlc, 8, 8},\
/* 0x05 */ {"RLC L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_rlc, 8, 8},\
/* 0x06 */ {"RLC (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_rlc, 16, 16},\
/* 0x07 */ {"RLC A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rlc, 8, 8},\
/* 0x08 */ {"RRC B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_rrc, 8, 8},\
/* 0x09 */ {"RRC C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_rrc, 8, 8},\
/* 0x0A */ {"RRC D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_rrc, 8, 8},\
/* 0x0B */ {"RRC E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_rrc, 8, 8},\
/* 0x0C */ {"RRC H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_rrc, 8, 8},\
/* 0x0D */ {"RRC L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_rrc, 8, 8},\
/* 0x0E */ {"RRC (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_rrc, 16, 16},\
/* 0x0F */ {"RRC A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rrc, 8, 8},\
/* 0x10 */ {"RL B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_rl, 8, 8},\
/* 0x11 */ {"RL C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_rl, 8, 8},\
/* 0x12 */ {"RL D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_rl, 8, 8},\
/* 0x13 */ {"RL E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_rl, 8, 8},\
/* 0x14 */ {"RL H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_rl, 8, 8},\
/* 0x15 */ {"RL L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_rl, 8, 8},\
/* 0x16 */ {"RL (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_rl, 16, 16},\
/* 0x17 */ {"RL A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rl, 8, 8},\
/* 0x18 */ {"RR B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_rr, 8, 8},\
/* 0x19 */ {"RR C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_rr, 8, 8},\
/* 0x1A */ {"RR D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_rr, 8, 8},\
/* 0x1B */ {"RR E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_rr, 8, 8},\
/* 0x1C */ {"RR H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_rr, 8, 8},\
/* 0x1D */ {"RR L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_rr, 8, 8},\
/* 0x1E */ {"RR (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_rr, 16, 16},\
/* 0x1F */ {"RR A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_rr, 8, 8},\
/* 0x20 */ {"SLA B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_sla, 8, 8},\
/* 0x21 */ {"SLA C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_sla, 8, 8},\
/* 0x22 */ {"SLA D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_sla, 8, 8},\
/* 0x23 */ {"SLA E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_sla, 8, 8},\
/* 0x24 */ {"SLA H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_sla, 8, 8},\
/* 0x25 */ {"SLA L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_sla, 8, 8},\
/* 0x26 */ {"SLA (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_sla, 16, 16},\
/* 0x27 */ {"SLA A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sla, 8, 8},\
/* 0x28 */ {"SRA B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_sra, 8, 8},\
/* 0x29 */ {"SRA C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_sra, 8, 8},\
/* 0x2A */ {"SRA D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_sra, 8, 8},\
/* 0x2B */ {"SRA E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_sra, 8, 8},\
/* 0x2C */ {"SRA H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_sra, 8, 8},\
/* 0x2D */ {"SRA L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_sra, 8, 8},\
/* 0x2E */ {"SRA (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_sra, 16, 16},\
/* 0x2F */ {"SRA A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_sra, 8, 8},\
/* 0x30 */ {"SWAP B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_swap, 8, 8},\
/* 0x31 */ {"SWAP C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_swap, 8, 8},\
/* 0x32 */ {"SWAP D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_swap, 8, 8},\
/* 0x33 */ {"SWAP E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_swap, 8, 8},\
/* 0x34 */ {"SWAP H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_swap, 8, 8},\
/* 0x35 */ {"SWAP L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_swap, 8, 8},\
/* 0x36 */ {"SWAP (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_swap, 16, 16},\
/* 0x37 */ {"SWAP A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_swap, 8, 8},\
/* 0x38 */ {"SRL B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, nullptr, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_srl, 8, 8},\
/* 0x39 */ {"SRL C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, nullptr, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_srl, 8, 8},\
/* 0x3A */ {"SRL D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, nullptr, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_srl, 8, 8},\
/* 0x3B */ {"SRL E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, nullptr, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_srl, 8, 8},\
/* 0x3C */ {"SRL H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, nullptr, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_srl, 8, 8},\
/* 0x3D */ {"SRL L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, nullptr, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_srl, 8, 8},\
/* 0x3E */ {"SRL (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_register_hl, &gb_cpu::_operand_set_mem_8, &gb_cpu::_op_exec_srl, 16, 16},\
/* 0x3F */ {"SRL A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, nullptr, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_srl, 8, 8},\
/* 0x40 */ {"BIT 0, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x41 */ {"BIT 0, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x42 */ {"BIT 0, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x43 */ {"BIT 0, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x44 */ {"BIT 0, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x45 */ {"BIT 0, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x46 */ {"BIT 0, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_bit, 16, 16},\
/* 0x47 */ {"BIT 0, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x48 */ {"BIT 1, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x49 */ {"BIT 1, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x4A */ {"BIT 1, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x4B */ {"BIT 1, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x4C */ {"BIT 1, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x4D */ {"BIT 1, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x4E */ {"BIT 1, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_bit, 16, 16},\
/* 0x4F */ {"BIT 1, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x50 */ {"BIT 2, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x51 */ {"BIT 2, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x52 */ {"BIT 2, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x53 */ {"BIT 2, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x54 */ {"BIT 2, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x55 */ {"BIT 2, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x56 */ {"BIT 2, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_bit, 16, 16},\
/* 0x57 */ {"BIT 2, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x58 */ {"BIT 3, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x59 */ {"BIT 3, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x5A */ {"BIT 3, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x5B */ {"BIT 3, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x5C */ {"BIT 3, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x5D */ {"BIT 3, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x5E */ {"BIT 3, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_bit, 16, 16},\
/* 0x5F */ {"BIT 3, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x60 */ {"BIT 4, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x61 */ {"BIT 4, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x62 */ {"BIT 4, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x63 */ {"BIT 4, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x64 */ {"BIT 4, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x65 */ {"BIT 4, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x66 */ {"BIT 4, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_bit, 16, 16},\
/* 0x67 */ {"BIT 4, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x68 */ {"BIT 5, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x69 */ {"BIT 5, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x6A */ {"BIT 5, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x6B */ {"BIT 5, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x6C */ {"BIT 5, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x6D */ {"BIT 5, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x6E */ {"BIT 5, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_bit, 16, 16},\
/* 0x6F */ {"BIT 5, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x70 */ {"BIT 6, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x71 */ {"BIT 6, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x72 */ {"BIT 6, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x73 */ {"BIT 6, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x74 */ {"BIT 6, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x75 */ {"BIT 6, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x76 */ {"BIT 6, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_bit, 16, 16},\
/* 0x77 */ {"BIT 6, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x78 */ {"BIT 7, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x79 */ {"BIT 7, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x7A */ {"BIT 7, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x7B */ {"BIT 7, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x7C */ {"BIT 7, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x7D */ {"BIT 7, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x7E */ {"BIT 7, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_bit, 16, 16},\
/* 0x7F */ {"BIT 7, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_bit, 8, 8},\
/* 0x80 */ {"RES 0, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x81 */ {"RES 0, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x82 */ {"RES 0, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x83 */ {"RES 0, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x84 */ {"RES 0, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x85 */ {"RES 0, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x86 */ {"RES 0, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0x87 */ {"RES 0, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x88 */ {"RES 1, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x89 */ {"RES 1, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8A */ {"RES 1, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8B */ {"RES 1, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8C */ {"RES 1, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8D */ {"RES 1, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8E */ {"RES 1, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0x8F */ {"RES 1, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x90 */ {"RES 2, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x91 */ {"RES 2, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x92 */ {"RES 2, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x93 */ {"RES 2, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x94 */ {"RES 2, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x95 */ {"RES 2, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x96 */ {"RES 2, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_res, 16, 16},\
/* 0x97 */ {"RES 2, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x98 */ {"RES 3, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x99 */ {"RES 3, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9A */ {"RES 3, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9B */ {"RES 3, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9C */ {"RES 3, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9D */ {"RES 3, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9E */ {"RES 3, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0x9F */ {"RES 3, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA0 */ {"RES 4, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA1 */ {"RES 4, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA2 */ {"RES 4, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA3 */ {"RES 4, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA4 */ {"RES 4, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA5 */ {"RES 4, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA6 */ {"RES 4, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0xA7 */ {"RES 4, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA8 */ {"RES 5, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA9 */ {"RES 5, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAA */ {"RES 5, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAB */ {"RES 5, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAC */ {"RES 5, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAD */ {"RES 5, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAE */ {"RES 5, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0xAF */ {"RES 5, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB0 */ {"RES 6, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB1 */ {"RES 6, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB2 */ {"RES 6, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB3 */ {"RES 6, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB4 */ {"RES 6, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB5 */ {"RES 6, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB6 */ {"RES 6, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0xB7 */ {"RES 6, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB8 */ {"RES 7, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB9 */ {"RES 7, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBA */ {"RES 7, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBB */ {"RES 7, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBC */ {"RES 7, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBD */ {"RES 7, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBE */ {"RES 7, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0xBF */ {"RES 7, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xC0 */ {"SET 0, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC1 */ {"SET 0, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC2 */ {"SET 0, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC3 */ {"SET 0, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC4 */ {"SET 0, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC5 */ {"SET 0, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC6 */ {"SET 0, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xC7 */ {"SET 0, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_0, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC8 */ {"SET 1, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC9 */ {"SET 1, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCA */ {"SET 1, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCB */ {"SET 1, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCC */ {"SET 1, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCD */ {"SET 1, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCE */ {"SET 1, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xCF */ {"SET 1, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_1, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD0 */ {"SET 2, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD1 */ {"SET 2, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD2 */ {"SET 2, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD3 */ {"SET 2, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD4 */ {"SET 2, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD5 */ {"SET 2, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD6 */ {"SET 2, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_2, nullptr, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xD7 */ {"SET 2, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD8 */ {"SET 3, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD9 */ {"SET 3, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDA */ {"SET 3, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDB */ {"SET 3, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDC */ {"SET 3, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDD */ {"SET 3, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDE */ {"SET 3, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xDF */ {"SET 3, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_3, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE0 */ {"SET 4, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE1 */ {"SET 4, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE2 */ {"SET 4, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE3 */ {"SET 4, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE4 */ {"SET 4, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE5 */ {"SET 4, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE6 */ {"SET 4, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xE7 */ {"SET 4, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_4, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE8 */ {"SET 5, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE9 */ {"SET 5, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xEA */ {"SET 5, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xEB */ {"SET 5, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xEC */ {"SET 5, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xED */ {"SET 5, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xEE */ {"SET 5, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xEF */ {"SET 5, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_5, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF0 */ {"SET 6, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF1 */ {"SET 6, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF2 */ {"SET 6, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF3 */ {"SET 6, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF4 */ {"SET 6, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF5 */ {"SET 6, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF6 */ {"SET 6, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xF7 */ {"SET 6, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_6, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF8 */ {"SET 7, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF9 */ {"SET 7, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFA */ {"SET 7, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFB */ {"SET 7, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFC */ {"SET 7, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFD */ {"SET 7, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFE */ {"SET 7, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xFF */ {"SET 7, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_7, nullptr, &gb_cpu::_op_exec_set, 8, 8},\
}

gb_cpu::gb_cpu() {
    m_instructions = std::vector<instruction_t>(INSTRUCTIONS_LIST_INIT);
    m_cb_instructions = std::vector<instruction_t>(CB_INSTRUCTIONS_LIST_INIT);
}

gb_cpu::~gb_cpu() {
}

void gb_cpu::dump_registers() {
    char buf[256];
    snprintf(buf, 256, "AF: 0x%02x.%02x BC: 0x%02x.%02x DE: 0x%02x.%02x HL: 0x%02x.%02x SP: 0x%04x PC: 0x%04x FLAGS: %c%c%c%c",
        m_registers.a, m_registers.f, m_registers.b, m_registers.c, m_registers.d, m_registers.e, m_registers.h, m_registers.l, m_registers.sp, m_registers.pc,
        FLAGS_IS_SET(FLAGS_Z) ? 'Z' : '-', FLAGS_IS_SET(FLAGS_N) ? 'N' : '-', FLAGS_IS_SET(FLAGS_H) ? 'H' : '-', FLAGS_IS_SET(FLAGS_C) ? 'C' : '-');

    std::cout << buf << std::endl;
}

uint16_t gb_cpu::get_pc() {
    return m_registers.pc;
}

uint64_t gb_cpu::step() {
    uint8_t opcode = mem[m_registers.pc];

    instruction_t& instruction = m_instructions[opcode];

    if (instruction.op_exec == nullptr) {
        _op_print_type0("UNKNOWN", m_registers.pc, 0, 0);
        m_registers.pc++;
        return 0;
    }

    return (this->*(instruction.op_exec))(instruction);
}

uint64_t gb_cpu::_op_exec_cb(instruction_t& instruction) {
    uint8_t cb_opcode = mem[m_registers.pc+1];

    instruction_t& cb_instruction = m_cb_instructions[cb_opcode];

    if (cb_instruction.op_exec == nullptr) {
        _op_print_type0("UNKNOWN CB", m_registers.pc, 0, 0);
        m_registers.pc += 2;
        return 0;
    }

    uint64_t cycles = (this->*(cb_instruction.op_exec))(cb_instruction);

    m_registers.pc++;

    return cycles;
}

uint64_t gb_cpu::_op_exec_nop(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    (this->*(instruction.op_print))(instruction.disassembly, pc, 0, 0);

    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_stop(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    // TODO: Implement stop

    (this->*(instruction.op_print))(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_halt(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    // TODO: Implement halt

    (this->*(instruction.op_print))(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_ld(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t data = (this->*(instruction.get_operand1))();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();

    (this->*(instruction.set_operand))(addr, data);

    (this->*(instruction.op_print))(instruction.disassembly, pc, data, addr);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_ldhl(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    int32_t offset = (this->*(instruction.get_operand1))();
    int32_t sp = (this->*(instruction.get_operand2))();
    uint32_t effective_addr = static_cast<uint32_t>(sp + offset);

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_H_16(sp, offset);
    FLAGS_SET_IF_C_16(effective_addr);

    (this->*(instruction.set_operand))(0, static_cast<uint16_t>(effective_addr));

    (this->*(instruction.op_print))(instruction.disassembly, pc, static_cast<uint16_t>(offset), static_cast<uint16_t>(sp));
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_add8(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = (this->*(instruction.get_operand1))();
    uint16_t a2 = (this->*(instruction.get_operand2))();
    uint16_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET_IF_H(a1, a2);
    FLAGS_SET_IF_C(sum);

    (this->*(instruction.set_operand))(0, sum);

    (this->*(instruction.op_print))(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_add16(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint32_t a1 = (this->*(instruction.get_operand1))();
    uint32_t a2 = (this->*(instruction.get_operand2))();
    uint32_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_H_16(a1, a2);
    FLAGS_SET_IF_C_16(sum);

    (this->*(instruction.set_operand))(0, static_cast<uint16_t>(sum));

    (this->*(instruction.op_print))(instruction.disassembly, pc, static_cast<uint16_t>(a1), static_cast<uint16_t>(a2));
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_addsp(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    int32_t a1 = (this->*(instruction.get_operand1))();
    int32_t a2 = (this->*(instruction.get_operand2))();
    uint32_t sum = static_cast<uint32_t>(a1 + a2);

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_H_16(a1, a2);
    FLAGS_SET_IF_C_16(sum);

    (this->*(instruction.set_operand))(0, static_cast<uint16_t>(sum));

    (this->*(instruction.op_print))(instruction.disassembly, pc, static_cast<uint16_t>(a1), static_cast<uint16_t>(a2));
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_adc(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = (this->*(instruction.get_operand1))() + (FLAGS_IS_SET(FLAGS_C) ? 1 : 0);
    uint16_t a2 = (this->*(instruction.get_operand2))();
    uint16_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET_IF_H(a1, a2);
    FLAGS_SET_IF_C(sum);

    (this->*(instruction.set_operand))(0, sum);

    (this->*(instruction.op_print))(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_sub(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    int16_t a1 = static_cast<int16_t>((this->*(instruction.get_operand1))());
    int16_t a2 = static_cast<int16_t>((this->*(instruction.get_operand2))());
    int16_t sum = a2 - a1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET(FLAGS_N);
    FLAGS_SET_IF_H(a2, -a1);
    FLAGS_SET_IF_C(sum);

    if (instruction.set_operand != nullptr) {
        (this->*(instruction.set_operand))(0, static_cast<uint16_t>(sum));
    }

    (this->*(instruction.op_print))(instruction.disassembly, pc, static_cast<uint16_t>(a1), static_cast<uint16_t>(a2));
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_sbc(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    int16_t a1 = static_cast<int16_t>((this->*(instruction.get_operand1))() + (FLAGS_IS_SET(FLAGS_C) ? 1 : 0));
    int16_t a2 = static_cast<int16_t>((this->*(instruction.get_operand2))());
    int16_t sum = a2 - a1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET(FLAGS_N);
    FLAGS_SET_IF_H(a2, -a1);
    FLAGS_SET_IF_C(sum);

    (this->*(instruction.set_operand))(0, static_cast<uint16_t>(sum));

    (this->*(instruction.op_print))(instruction.disassembly, pc, static_cast<uint16_t>(a1), static_cast<uint16_t>(a2));
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_jr(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint64_t cycles = instruction.cycles_lo;
    int16_t offset = static_cast<int8_t>((this->*(instruction.get_operand1))());
    uint16_t jump_pc = static_cast<uint16_t>(static_cast<int16_t>(m_registers.pc) + offset);
    uint16_t do_jump = instruction.get_operand2 == nullptr ? 1 : (this->*(instruction.get_operand2))();

    if (do_jump != 0) {
        (this->*(instruction.set_operand))(0, jump_pc);
        cycles = instruction.cycles_hi;
    }

    (this->*(instruction.op_print))(instruction.disassembly, pc, jump_pc, 0);
    return cycles;
}

uint64_t gb_cpu::_op_exec_jp(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint64_t cycles = instruction.cycles_lo;
    uint16_t jump_pc = (this->*(instruction.get_operand1))();
    uint16_t do_jump = instruction.get_operand2 == nullptr ? 1 : (this->*(instruction.get_operand2))();

    if (do_jump != 0) {
        (this->*(instruction.set_operand))(0, jump_pc);
        cycles = instruction.cycles_hi;
    }

    (this->*(instruction.op_print))(instruction.disassembly, pc, jump_pc, 0);
    return cycles;
}

uint64_t gb_cpu::_op_exec_call(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint64_t cycles = instruction.cycles_lo;
    uint16_t jump_pc = (this->*(instruction.get_operand1))();
    uint16_t do_jump = instruction.get_operand2 == nullptr ? 1 : (this->*(instruction.get_operand2))();

    if (do_jump != 0) {
        (this->*(instruction.set_operand))(0, m_registers.pc);
        m_registers.pc = jump_pc;
        cycles = instruction.cycles_hi;
    }

    (this->*(instruction.op_print))(instruction.disassembly, pc, jump_pc, 0);
    return cycles;
}

uint64_t gb_cpu::_op_exec_ret(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint64_t cycles = instruction.cycles_lo;
    uint16_t do_jump = instruction.get_operand2 == nullptr ? 1 : (this->*(instruction.get_operand2))();

    if (do_jump != 0) {
        uint16_t jump_pc = (this->*(instruction.get_operand1))();
        (this->*(instruction.set_operand))(0, jump_pc);
        cycles = instruction.cycles_hi;
    }

    (this->*(instruction.op_print))(instruction.disassembly, pc, 0, 0);
    return cycles;
}

uint64_t gb_cpu::_op_exec_reti(instruction_t& instruction) {
    // TODO: Enable interrupts
    return gb_cpu::_op_exec_ret(instruction);
}

uint64_t gb_cpu::_op_exec_rst(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t jump_pc = (this->*(instruction.get_operand1))();

    (this->*(instruction.set_operand))(0, pc);
    m_registers.pc = jump_pc;

    (this->*(instruction.op_print))(instruction.disassembly, pc, jump_pc, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_da(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t operand = (this->*(instruction.get_operand1))();
    (this->*(instruction.set_operand))(0, operand);

    (this->*(instruction.op_print))(instruction.disassembly, pc, operand, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_rlc(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = static_cast<uint16_t>((this->*(instruction.get_operand1))() << 1);
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(val);

    (this->*(instruction.set_operand))(addr, val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_rlca(instruction_t& instruction) {
    uint64_t cycles = _op_exec_rlc(instruction);
    FLAGS_CLEAR(FLAGS_Z);
    return cycles;
}

uint64_t gb_cpu::_op_exec_rl(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = static_cast<uint16_t>((this->*(instruction.get_operand1))() << 1);
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();
    int carry = FLAGS_IS_SET(FLAGS_C) ? 1 : 0;
    val |= carry;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(val);

    (this->*(instruction.set_operand))(addr, val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_rla(instruction_t& instruction) {
    uint64_t cycles = _op_exec_rl(instruction);
    FLAGS_CLEAR(FLAGS_Z);
    return cycles;
}

uint64_t gb_cpu::_op_exec_rrc(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();
    int bit1 = val & 0x1;
    val = val >> 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(bit1 << 8);

    (this->*(instruction.set_operand))(addr, val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_rrca(instruction_t& instruction) {
    uint64_t cycles = _op_exec_rrc(instruction);
    FLAGS_CLEAR(FLAGS_Z);
    return cycles;
}

uint64_t gb_cpu::_op_exec_rr(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();
    int bit1 = val & 0x1;
    int carry = FLAGS_IS_SET(FLAGS_C) ? 1 : 0;
    val = static_cast<uint16_t>((val >> 1) | (carry << 0x7));

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(bit1 << 8);

    (this->*(instruction.set_operand))(addr, val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_rra(instruction_t& instruction) {
    uint64_t cycles = _op_exec_rr(instruction);
    FLAGS_CLEAR(FLAGS_Z);
    return cycles;
}

uint64_t gb_cpu::_op_exec_sla(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = static_cast<uint16_t>((this->*(instruction.get_operand1))() << 1);
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(val);

    (this->*(instruction.set_operand))(addr, val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_sra(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();
    int bit0 = val & 0x1;
    int bit7 = val & 0x80;
    val = static_cast<uint16_t>((val >> 1) | bit7);

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(bit0 << 0x8);

    (this->*(instruction.set_operand))(addr, val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_srl(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();
    int bit0 = val & 0x1;
    val = val >> 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(bit0 << 0x8);

    (this->*(instruction.set_operand))(addr, val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_swap(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();

    val = static_cast<uint16_t>((val << 4) | (val >> 4));

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);

    (this->*(instruction.set_operand))(addr, val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_cpl(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();

    FLAGS_SET(FLAGS_N | FLAGS_H);

    (this->*(instruction.set_operand))(0, ~val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_inc(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();

    (this->*(instruction.set_operand))(0, ++val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_dec(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();

    (this->*(instruction.set_operand))(0, --val);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_incf(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();
    uint16_t vali = val + 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H);
    FLAGS_SET_IF_Z(vali);
    FLAGS_SET_IF_H(val, 1);

    (this->*(instruction.set_operand))(addr, vali);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_decf(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = (this->*(instruction.get_operand1))();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : (this->*(instruction.get_operand2))();
    uint16_t vald = val - 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H);
    FLAGS_SET_IF_Z(vald);
    FLAGS_SET_IF_H(val, -1);

    (this->*(instruction.set_operand))(addr, vald);

    (this->*(instruction.op_print))(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_scf(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    FLAGS_CLEAR(FLAGS_N | FLAGS_H);
    FLAGS_SET(FLAGS_C);

    (this->*(instruction.op_print))(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_ccf(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    FLAGS_CLEAR(FLAGS_N | FLAGS_H);
    FLAGS_TOGGLE(FLAGS_C);

    (this->*(instruction.op_print))(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_and(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = (this->*(instruction.get_operand1))();
    uint16_t a2 = (this->*(instruction.get_operand2))();
    uint16_t result = a1 & a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(result);
    FLAGS_SET(FLAGS_H);

    (this->*(instruction.set_operand))(0, result);

    (this->*(instruction.op_print))(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_xor(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = (this->*(instruction.get_operand1))();
    uint16_t a2 = (this->*(instruction.get_operand2))();
    uint16_t result = a1 ^ a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(result);

    (this->*(instruction.set_operand))(0, result);

    (this->*(instruction.op_print))(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_or(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = (this->*(instruction.get_operand1))();
    uint16_t a2 = (this->*(instruction.get_operand2))();
    uint16_t result = a1 | a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(result);

    (this->*(instruction.set_operand))(0, result);

    (this->*(instruction.op_print))(instruction.disassembly, pc, a1, a1);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_bit(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = (this->*(instruction.get_operand1))();
    uint16_t bit = (this->*(instruction.get_operand2))();
    uint16_t result = a1 & (1 << bit);

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H);
    FLAGS_SET_IF_Z(result);
    FLAGS_SET(FLAGS_H);

    (this->*(instruction.op_print))(instruction.disassembly, pc, a1, bit);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_set(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = (this->*(instruction.get_operand1))();
    uint16_t bit = (this->*(instruction.get_operand2))();
    uint16_t result = static_cast<uint16_t>(a1 | (1 << bit));

    (this->*(instruction.set_operand))(0, result);

    (this->*(instruction.op_print))(instruction.disassembly, pc, a1, bit);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_res(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = (this->*(instruction.get_operand1))();
    uint16_t bit = (this->*(instruction.get_operand2))();
    uint16_t result = a1 & ~(1 << bit);

    (this->*(instruction.set_operand))(0, result);

    (this->*(instruction.op_print))(instruction.disassembly, pc, a1, bit);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_di(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    // TODO: Disable interrupts

    (this->*(instruction.op_print))(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

uint64_t gb_cpu::_op_exec_ei(instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    // TODO: Enable interrupts

    (this->*(instruction.op_print))(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

uint16_t gb_cpu::_operand_get_register_a() {
    return m_registers.f;
}

uint16_t gb_cpu::_operand_get_register_f() {
    return m_registers.f;
}

uint16_t gb_cpu::_operand_get_register_af() {
    return m_registers.af;
}

uint16_t gb_cpu::_operand_get_register_b() {
    return m_registers.b;
}

uint16_t gb_cpu::_operand_get_register_c() {
    return m_registers.c;
}

uint16_t gb_cpu::_operand_get_register_bc() {
    return m_registers.bc;
}

uint16_t gb_cpu::_operand_get_register_d() {
    return m_registers.d;
}

uint16_t gb_cpu::_operand_get_register_e() {
    return m_registers.e;
}

uint16_t gb_cpu::_operand_get_register_de() {
    return m_registers.de;
}

uint16_t gb_cpu::_operand_get_register_h() {
    return m_registers.h;
}

uint16_t gb_cpu::_operand_get_register_l() {
    return m_registers.l;
}

uint16_t gb_cpu::_operand_get_register_hl() {
    return m_registers.hl;
}

uint16_t gb_cpu::_operand_get_register_sp() {
    return m_registers.sp;
}

uint16_t gb_cpu::_operand_get_register_pc() {
    return m_registers.pc;
}

uint16_t gb_cpu::_operand_get_register_hl_plus() {
    uint16_t hl = m_registers.hl;
    m_registers.hl++;
    return hl;
}

uint16_t gb_cpu::_operand_get_register_hl_minus() {
    uint16_t hl = m_registers.hl;
    m_registers.hl--;
    return hl;
}

uint16_t gb_cpu::_operand_get_mem_8() {
    return mem[m_registers.pc++];
}

uint16_t gb_cpu::_operand_get_mem_8_plus_io_base() {
    uint16_t offset = _operand_get_mem_8();
    return (offset + GB_MEMORY_MAP_IO_BASE);
}

uint16_t gb_cpu::_operand_get_mem_8_plus_io_base_mem() {
    uint16_t addr = _operand_get_mem_8_plus_io_base();
    return mem[addr];
}

uint16_t gb_cpu::_operand_get_register_c_plus_io_base() {
    uint16_t offset = _operand_get_register_c();
    return (offset + GB_MEMORY_MAP_IO_BASE);
}

uint16_t gb_cpu::_operand_get_register_c_plus_io_base_mem() {
    uint16_t addr = _operand_get_register_c_plus_io_base();
    return mem[addr];
}

uint16_t gb_cpu::_operand_get_mem_16() {
    uint16_t byte_lo = mem[m_registers.pc++];
    uint16_t byte_hi = mem[m_registers.pc++];
    return static_cast<uint16_t>((byte_hi << 8) | byte_lo);
}

uint16_t gb_cpu::_operand_get_mem_16_mem() {
    uint16_t addr = _operand_get_mem_16();
    return mem[addr];
}

uint16_t gb_cpu::_operand_get_mem_bc() {
    return mem[m_registers.bc];
}

uint16_t gb_cpu::_operand_get_mem_de() {
    return mem[m_registers.de];
}

uint16_t gb_cpu::_operand_get_mem_hl() {
    return mem[m_registers.hl];
}

uint16_t gb_cpu::_operand_get_mem_sp_8() {
    return mem[m_registers.sp++];
}

uint16_t gb_cpu::_operand_get_mem_sp_16() {
    uint16_t byte_lo = mem[m_registers.sp++];
    uint16_t byte_hi = mem[m_registers.sp++];
    return static_cast<uint16_t>((byte_hi << 8) | byte_lo);
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

uint16_t gb_cpu::_operand_get_imm_0() {
    return 0x0;
}

uint16_t gb_cpu::_operand_get_imm_1() {
    return 0x1;
}

uint16_t gb_cpu::_operand_get_imm_2() {
    return 0x2;
}

uint16_t gb_cpu::_operand_get_imm_3() {
    return 0x3;
}

uint16_t gb_cpu::_operand_get_imm_4() {
    return 0x4;
}

uint16_t gb_cpu::_operand_get_imm_5() {
    return 0x5;
}

uint16_t gb_cpu::_operand_get_imm_6() {
    return 0x6;
}

uint16_t gb_cpu::_operand_get_imm_7() {
    return 0x7;
}

void gb_cpu::_operand_set_register_a(uint16_t addr, uint16_t val) {
    m_registers.a = static_cast<uint8_t>(val);
}

void gb_cpu::_operand_set_register_f(uint16_t addr, uint16_t val) {
    m_registers.f = static_cast<uint8_t>(val);
}

void gb_cpu::_operand_set_register_af(uint16_t addr, uint16_t val) {
    m_registers.af = val;
}

void gb_cpu::_operand_set_register_b(uint16_t addr, uint16_t val) {
    m_registers.b = static_cast<uint8_t>(val);
}

void gb_cpu::_operand_set_register_c(uint16_t addr, uint16_t val) {
    m_registers.c = static_cast<uint8_t>(val);
}

void gb_cpu::_operand_set_register_bc(uint16_t addr, uint16_t val) {
    m_registers.bc = val;
}

void gb_cpu::_operand_set_register_d(uint16_t addr, uint16_t val) {
    m_registers.d = static_cast<uint8_t>(val);
}

void gb_cpu::_operand_set_register_e(uint16_t addr, uint16_t val) {
    m_registers.e = static_cast<uint8_t>(val);
}

void gb_cpu::_operand_set_register_de(uint16_t addr, uint16_t val) {
    m_registers.de = val;
}

void gb_cpu::_operand_set_register_h(uint16_t addr, uint16_t val) {
    m_registers.h = static_cast<uint8_t>(val);
}

void gb_cpu::_operand_set_register_l(uint16_t addr, uint16_t val) {
    m_registers.l = static_cast<uint8_t>(val);
}

void gb_cpu::_operand_set_register_hl(uint16_t addr, uint16_t val) {
    m_registers.hl = val;
}

void gb_cpu::_operand_set_register_sp(uint16_t addr, uint16_t val) {
    m_registers.sp = val;
}

void gb_cpu::_operand_set_register_pc(uint16_t addr, uint16_t val) {
    m_registers.pc = val;
}

void gb_cpu::_operand_set_mem_8(uint16_t addr, uint16_t val) {
    mem[addr] = static_cast<uint8_t>(val);
}

void gb_cpu::_operand_set_mem_hl_8(uint16_t addr, uint16_t val) {
    _operand_set_mem_8(_operand_get_register_hl(), val);
}

void gb_cpu::_operand_set_mem_sp_16(uint16_t addr, uint16_t val) {
    mem[--m_registers.sp] = static_cast<uint8_t>(val >> 8);
    mem[--m_registers.sp] = static_cast<uint8_t>(val);
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
    _op_print_type1(disassembly, pc, static_cast<uint8_t>(operand1), static_cast<uint8_t>(operand2));
}

void gb_cpu::_op_print_type3(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) {
    char buf[256], out[256];
    snprintf(buf, 256, disassembly.c_str(), operand2);
    snprintf(out, 256, "%04x: %s", pc, buf);
    std::string fmt = out;
    std::cout << fmt << std::endl;
}

void gb_cpu::_op_print_type4(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) {
    _op_print_type3(disassembly, pc, static_cast<uint8_t>(operand1), static_cast<uint8_t>(operand2));
}
