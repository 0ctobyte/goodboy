/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#ifndef GB_CPU_INSTRUCTIONS_H_
#define GB_CPU_INSTRUCTIONS_H_

#define B0_(x) std::bind(&gb_cpu::x, this)
#define B1_(x) std::bind(&gb_cpu::x, this, std::placeholders::_1)
#define B2_(x) std::bind(&gb_cpu::x, this, std::placeholders::_1, std::placeholders::_2)
#define B4_(x) std::bind(&gb_cpu::x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)

#define INSTRUCTIONS_INIT \
{{\
/* 0x00 */ {"NOP", B4_(_op_print_type0), nullptr, nullptr, nullptr, B1_(_op_exec_nop), 4, 4},\
/* 0x01 */ {"LD BC, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), nullptr, B2_(_operand_set_register_bc), B1_(_op_exec_ld), 12, 12},\
/* 0x02 */ {"LD (BC), A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_bc), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x03 */ {"INC BC", B4_(_op_print_type0), B0_(_operand_get_register_bc), nullptr, B2_(_operand_set_register_bc), B1_(_op_exec_inc), 8, 8},\
/* 0x04 */ {"INC B", B4_(_op_print_type0), B0_(_operand_get_register_b), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_incf), 4, 4},\
/* 0x05 */ {"DEC B", B4_(_op_print_type0), B0_(_operand_get_register_b), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_decf), 4, 4},\
/* 0x06 */ {"LD B, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_ld), 8, 8},\
/* 0x07 */ {"RLCA", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_rlca), 4, 4},\
/* 0x08 */ {"LD (0x%04x), SP", B4_(_op_print_type3), B0_(_operand_get_register_sp), B0_(_operand_get_mem_16), B2_(_operand_set_mem_16), B1_(_op_exec_ld), 20, 20},\
/* 0x09 */ {"ADD HL, BC", B4_(_op_print_type0), B0_(_operand_get_register_bc), B0_(_operand_get_register_hl), B2_(_operand_set_register_hl), B1_(_op_exec_add16), 8, 8},\
/* 0x0A */ {"LD A, (BC)", B4_(_op_print_type0), B0_(_operand_get_mem_bc), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 8, 8},\
/* 0x0B */ {"DEC BC", B4_(_op_print_type0), B0_(_operand_get_register_bc), nullptr, B2_(_operand_set_register_bc), B1_(_op_exec_dec), 8, 8},\
/* 0x0C */ {"INC C", B4_(_op_print_type0), B0_(_operand_get_register_c), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_incf), 4, 4},\
/* 0x0D */ {"DEC C", B4_(_op_print_type0), B0_(_operand_get_register_c), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_decf), 4, 4},\
/* 0x0E */ {"LD C, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_ld), 8, 8},\
/* 0x0F */ {"RRCA", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_rrca), 4, 4},\
/* 0x10 */ {"STOP 0", B4_(_op_print_type0), B0_(_operand_get_mem_8), nullptr, nullptr, B1_(_op_exec_stop), 4, 4},\
/* 0x11 */ {"LD DE, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), nullptr, B2_(_operand_set_register_de), B1_(_op_exec_ld), 12, 12},\
/* 0x12 */ {"LD (DE), A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_de), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x13 */ {"INC DE", B4_(_op_print_type0), B0_(_operand_get_register_de), nullptr, B2_(_operand_set_register_de), B1_(_op_exec_inc), 8, 8},\
/* 0x14 */ {"INC D", B4_(_op_print_type0), B0_(_operand_get_register_d), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_incf), 4, 4},\
/* 0x15 */ {"DEC D", B4_(_op_print_type0), B0_(_operand_get_register_d), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_decf), 4, 4},\
/* 0x16 */ {"LD D, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_ld), 8, 8},\
/* 0x17 */ {"RLA", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_rla), 4, 4},\
/* 0x18 */ {"JR 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), nullptr, B2_(_operand_set_register_pc), B1_(_op_exec_jr), 12, 12},\
/* 0x19 */ {"ADD HL, DE", B4_(_op_print_type0), B0_(_operand_get_register_de), B0_(_operand_get_register_hl), B2_(_operand_set_register_hl), B1_(_op_exec_add16), 8, 8},\
/* 0x1A */ {"LD A, (DE)", B4_(_op_print_type0), B0_(_operand_get_mem_de), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 8, 8},\
/* 0x1B */ {"DEC DE", B4_(_op_print_type0), B0_(_operand_get_register_de), nullptr, B2_(_operand_set_register_de), B1_(_op_exec_dec), 8, 8},\
/* 0x1C */ {"INC E", B4_(_op_print_type0), B0_(_operand_get_register_e), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_incf), 4, 4},\
/* 0x1D */ {"DEC E", B4_(_op_print_type0), B0_(_operand_get_register_e), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_decf), 4, 4},\
/* 0x1E */ {"LD E, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_ld), 8, 8},\
/* 0x1F */ {"RRA", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_rra), 4, 4},\
/* 0x20 */ {"JR NZ, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_8), B0_(_operand_get_flags_is_nz), B2_(_operand_set_register_pc), B1_(_op_exec_jr), 12, 8},\
/* 0x21 */ {"LD HL, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), nullptr, B2_(_operand_set_register_hl), B1_(_op_exec_ld), 12, 12},\
/* 0x22 */ {"LD (HL+), A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_hl_plus), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x23 */ {"INC HL", B4_(_op_print_type0), B0_(_operand_get_register_hl), nullptr, B2_(_operand_set_register_hl), B1_(_op_exec_inc), 8, 8},\
/* 0x24 */ {"INC H", B4_(_op_print_type0), B0_(_operand_get_register_h), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_incf), 4, 4},\
/* 0x25 */ {"DEC H", B4_(_op_print_type0), B0_(_operand_get_register_h), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_decf), 4, 4},\
/* 0x26 */ {"LD H, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_ld), 8, 8},\
/* 0x27 */ {"DAA", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_da), 4, 4},\
/* 0x28 */ {"JR Z, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_flags_is_z), B2_(_operand_set_register_pc), B1_(_op_exec_jr), 12, 8},\
/* 0x29 */ {"ADD HL, HL", B4_(_op_print_type0), B0_(_operand_get_register_hl), B0_(_operand_get_register_hl), B2_(_operand_set_register_hl), B1_(_op_exec_add16), 8, 8},\
/* 0x2A */ {"LD A, (HL+)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_hl_plus), B2_(_operand_set_register_a), B1_(_op_exec_ld), 8, 8},\
/* 0x2B */ {"DEC HL", B4_(_op_print_type0), B0_(_operand_get_register_hl), nullptr, B2_(_operand_set_register_hl), B1_(_op_exec_dec), 8, 8},\
/* 0x2C */ {"INC L", B4_(_op_print_type0), B0_(_operand_get_register_l), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_incf), 4, 4},\
/* 0x2D */ {"DEC L", B4_(_op_print_type0), B0_(_operand_get_register_l), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_decf), 4, 4},\
/* 0x2E */ {"LD L, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_ld), 8, 8},\
/* 0x2F */ {"CPL", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_cpl), 4, 4},\
/* 0x30 */ {"JR NC, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_flags_is_nc), B2_(_operand_set_register_pc), B1_(_op_exec_jr), 12, 8},\
/* 0x31 */ {"LD SP, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), nullptr, B2_(_operand_set_register_sp), B1_(_op_exec_ld), 12, 12},\
/* 0x32 */ {"LD (HL-), A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_hl_minus), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x33 */ {"INC SP", B4_(_op_print_type0), B0_(_operand_get_register_sp), nullptr, B2_(_operand_set_register_sp), B1_(_op_exec_inc), 8, 8},\
/* 0x34 */ {"INC (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_incf), 12, 12},\
/* 0x35 */ {"DEC (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_decf), 12, 12},\
/* 0x36 */ {"LD (HL), 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 12, 12},\
/* 0x37 */ {"SCF", B4_(_op_print_type0), nullptr, nullptr, nullptr, B1_(_op_exec_scf), 4, 4},\
/* 0x38 */ {"JR C, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_flags_is_c), B2_(_operand_set_register_pc), B1_(_op_exec_jr), 12, 8},\
/* 0x39 */ {"ADD HL, SP", B4_(_op_print_type0), B0_(_operand_get_register_sp), B0_(_operand_get_register_hl), B2_(_operand_set_register_hl), B1_(_op_exec_add16), 8, 8},\
/* 0x3A */ {"LD A, (HL-)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_hl_minus), B2_(_operand_set_register_a), B1_(_op_exec_ld), 8, 8},\
/* 0x3B */ {"DEC SP", B4_(_op_print_type0), B0_(_operand_get_register_sp), nullptr, B2_(_operand_set_register_sp), B1_(_op_exec_dec), 8, 8},\
/* 0x3C */ {"INC A", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_incf), 4, 4},\
/* 0x3D */ {"DEC A", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_decf), 4, 4},\
/* 0x3E */ {"LD A, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 8, 8},\
/* 0x3F */ {"CCF", B4_(_op_print_type0), nullptr, nullptr, nullptr, B1_(_op_exec_ccf), 4, 4},\
/* 0x40 */ {"LD B, B", B4_(_op_print_type0), B0_(_operand_get_register_b), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_ld), 4, 4},\
/* 0x41 */ {"LD B, C", B4_(_op_print_type0), B0_(_operand_get_register_c), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_ld), 4, 4},\
/* 0x42 */ {"LD B, D", B4_(_op_print_type0), B0_(_operand_get_register_d), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_ld), 4, 4},\
/* 0x43 */ {"LD B, E", B4_(_op_print_type0), B0_(_operand_get_register_e), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_ld), 4, 4},\
/* 0x44 */ {"LD B, H", B4_(_op_print_type0), B0_(_operand_get_register_h), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_ld), 4, 4},\
/* 0x45 */ {"LD B, L", B4_(_op_print_type0), B0_(_operand_get_register_l), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_ld), 4, 4},\
/* 0x46 */ {"LD B, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_ld), 8, 8},\
/* 0x47 */ {"LD B, A", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_b), B1_(_op_exec_ld), 4, 4},\
/* 0x48 */ {"LD C, B", B4_(_op_print_type0), B0_(_operand_get_register_b), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_ld), 4, 4},\
/* 0x49 */ {"LD C, C", B4_(_op_print_type0), B0_(_operand_get_register_c), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_ld), 4, 4},\
/* 0x4A */ {"LD C, D", B4_(_op_print_type0), B0_(_operand_get_register_d), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_ld), 4, 4},\
/* 0x4B */ {"LD C, E", B4_(_op_print_type0), B0_(_operand_get_register_e), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_ld), 4, 4},\
/* 0x4C */ {"LD C, H", B4_(_op_print_type0), B0_(_operand_get_register_h), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_ld), 4, 4},\
/* 0x4D */ {"LD C, L", B4_(_op_print_type0), B0_(_operand_get_register_l), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_ld), 4, 4},\
/* 0x4E */ {"LD C, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_ld), 8, 8},\
/* 0x4F */ {"LD C, A", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_c), B1_(_op_exec_ld), 4, 4},\
/* 0x50 */ {"LD D, B", B4_(_op_print_type0), B0_(_operand_get_register_b), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_ld), 4, 4},\
/* 0x51 */ {"LD D, C", B4_(_op_print_type0), B0_(_operand_get_register_c), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_ld), 4, 4},\
/* 0x52 */ {"LD D, D", B4_(_op_print_type0), B0_(_operand_get_register_d), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_ld), 4, 4},\
/* 0x53 */ {"LD D, E", B4_(_op_print_type0), B0_(_operand_get_register_e), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_ld), 4, 4},\
/* 0x54 */ {"LD D, H", B4_(_op_print_type0), B0_(_operand_get_register_h), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_ld), 4, 4},\
/* 0x55 */ {"LD D, L", B4_(_op_print_type0), B0_(_operand_get_register_l), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_ld), 4, 4},\
/* 0x56 */ {"LD D, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_ld), 8, 8},\
/* 0x57 */ {"LD D, A", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_d), B1_(_op_exec_ld), 4, 4},\
/* 0x58 */ {"LD E, B", B4_(_op_print_type0), B0_(_operand_get_register_b), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_ld), 4, 4},\
/* 0x59 */ {"LD E, C", B4_(_op_print_type0), B0_(_operand_get_register_c), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_ld), 4, 4},\
/* 0x5A */ {"LD E, D", B4_(_op_print_type0), B0_(_operand_get_register_d), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_ld), 4, 4},\
/* 0x5B */ {"LD E, E", B4_(_op_print_type0), B0_(_operand_get_register_e), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_ld), 4, 4},\
/* 0x5C */ {"LD E, H", B4_(_op_print_type0), B0_(_operand_get_register_h), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_ld), 4, 4},\
/* 0x5D */ {"LD E, L", B4_(_op_print_type0), B0_(_operand_get_register_l), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_ld), 4, 4},\
/* 0x5E */ {"LD E, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_ld), 8, 8},\
/* 0x5F */ {"LD E, A", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_e), B1_(_op_exec_ld), 4, 4},\
/* 0x60 */ {"LD H, B", B4_(_op_print_type0), B0_(_operand_get_register_b), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_ld), 4, 4},\
/* 0x61 */ {"LD H, C", B4_(_op_print_type0), B0_(_operand_get_register_c), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_ld), 4, 4},\
/* 0x62 */ {"LD H, D", B4_(_op_print_type0), B0_(_operand_get_register_d), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_ld), 4, 4},\
/* 0x63 */ {"LD H, E", B4_(_op_print_type0), B0_(_operand_get_register_e), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_ld), 4, 4},\
/* 0x64 */ {"LD H, H", B4_(_op_print_type0), B0_(_operand_get_register_h), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_ld), 4, 4},\
/* 0x65 */ {"LD H, L", B4_(_op_print_type0), B0_(_operand_get_register_l), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_ld), 4, 4},\
/* 0x66 */ {"LD H, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_ld), 8, 8},\
/* 0x67 */ {"LD H, A", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_h), B1_(_op_exec_ld), 4, 4},\
/* 0x68 */ {"LD L, B", B4_(_op_print_type0), B0_(_operand_get_register_b), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_ld), 4, 4},\
/* 0x69 */ {"LD L, C", B4_(_op_print_type0), B0_(_operand_get_register_c), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_ld), 4, 4},\
/* 0x6A */ {"LD L, D", B4_(_op_print_type0), B0_(_operand_get_register_d), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_ld), 4, 4},\
/* 0x6B */ {"LD L, E", B4_(_op_print_type0), B0_(_operand_get_register_e), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_ld), 4, 4},\
/* 0x6C */ {"LD L, H", B4_(_op_print_type0), B0_(_operand_get_register_h), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_ld), 4, 4},\
/* 0x6D */ {"LD L, L", B4_(_op_print_type0), B0_(_operand_get_register_l), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_ld), 4, 4},\
/* 0x6E */ {"LD L, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_ld), 8, 8},\
/* 0x6F */ {"LD L, A", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_l), B1_(_op_exec_ld), 4, 4},\
/* 0x70 */ {"LD (HL), B", B4_(_op_print_type0), B0_(_operand_get_register_b), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x71 */ {"LD (HL), C", B4_(_op_print_type0), B0_(_operand_get_register_c), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x72 */ {"LD (HL), D", B4_(_op_print_type0), B0_(_operand_get_register_d), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x73 */ {"LD (HL), E", B4_(_op_print_type0), B0_(_operand_get_register_e), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x74 */ {"LD (HL), H", B4_(_op_print_type0), B0_(_operand_get_register_h), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x75 */ {"LD (HL), L", B4_(_op_print_type0), B0_(_operand_get_register_l), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x76 */ {"HALT", B4_(_op_print_type0), nullptr, nullptr, nullptr, B1_(_op_exec_halt), 4, 4},\
/* 0x77 */ {"LD (HL), A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_hl), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0x78 */ {"LD A, B", B4_(_op_print_type0), B0_(_operand_get_register_b), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 4, 4},\
/* 0x79 */ {"LD A, C", B4_(_op_print_type0), B0_(_operand_get_register_c), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 4, 4},\
/* 0x7A */ {"LD A, D", B4_(_op_print_type0), B0_(_operand_get_register_d), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 4, 4},\
/* 0x7B */ {"LD A, E", B4_(_op_print_type0), B0_(_operand_get_register_e), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 4, 4},\
/* 0x7C */ {"LD A, H", B4_(_op_print_type0), B0_(_operand_get_register_h), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 4, 4},\
/* 0x7D */ {"LD A, L", B4_(_op_print_type0), B0_(_operand_get_register_l), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 4, 4},\
/* 0x7E */ {"LD A, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 8, 8},\
/* 0x7F */ {"LD A, A", B4_(_op_print_type0), B0_(_operand_get_register_a), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 4, 4},\
/* 0x80 */ {"ADD A, B", B4_(_op_print_type0), B0_(_operand_get_register_b), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_add8), 4, 4},\
/* 0x81 */ {"ADD A, C", B4_(_op_print_type0), B0_(_operand_get_register_c), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_add8), 4, 4},\
/* 0x82 */ {"ADD A, D", B4_(_op_print_type0), B0_(_operand_get_register_d), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_add8), 4, 4},\
/* 0x83 */ {"ADD A, E", B4_(_op_print_type0), B0_(_operand_get_register_e), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_add8), 4, 4},\
/* 0x84 */ {"ADD A, H", B4_(_op_print_type0), B0_(_operand_get_register_h), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_add8), 4, 4},\
/* 0x85 */ {"ADD A, L", B4_(_op_print_type0), B0_(_operand_get_register_l), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_add8), 4, 4},\
/* 0x86 */ {"ADD A, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_add8), 8, 8},\
/* 0x87 */ {"ADD A, A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_add8), 4, 4},\
/* 0x88 */ {"ADC A, B", B4_(_op_print_type0), B0_(_operand_get_register_b), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_adc), 4, 4},\
/* 0x89 */ {"ADC A, C", B4_(_op_print_type0), B0_(_operand_get_register_c), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_adc), 4, 4},\
/* 0x8A */ {"ADC A, D", B4_(_op_print_type0), B0_(_operand_get_register_d), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_adc), 4, 4},\
/* 0x8B */ {"ADC A, E", B4_(_op_print_type0), B0_(_operand_get_register_e), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_adc), 4, 4},\
/* 0x8C */ {"ADC A, H", B4_(_op_print_type0), B0_(_operand_get_register_h), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_adc), 4, 4},\
/* 0x8D */ {"ADC A, L", B4_(_op_print_type0), B0_(_operand_get_register_l), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_adc), 4, 4},\
/* 0x8E */ {"ADC A, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_adc), 8, 8},\
/* 0x8F */ {"ADC A, A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_adc), 4, 4},\
/* 0x90 */ {"SUB B", B4_(_op_print_type0), B0_(_operand_get_register_b), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sub), 4, 4},\
/* 0x91 */ {"SUB C", B4_(_op_print_type0), B0_(_operand_get_register_c), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sub), 4, 4},\
/* 0x92 */ {"SUB D", B4_(_op_print_type0), B0_(_operand_get_register_d), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sub), 4, 4},\
/* 0x93 */ {"SUB E", B4_(_op_print_type0), B0_(_operand_get_register_e), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sub), 4, 4},\
/* 0x94 */ {"SUB H", B4_(_op_print_type0), B0_(_operand_get_register_h), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sub), 4, 4},\
/* 0x95 */ {"SUB L", B4_(_op_print_type0), B0_(_operand_get_register_l), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sub), 4, 4},\
/* 0x96 */ {"SUB (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sub), 8, 8},\
/* 0x97 */ {"SUB A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sub), 4, 4},\
/* 0x98 */ {"SBC A, B", B4_(_op_print_type0), B0_(_operand_get_register_b), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sbc), 4, 4},\
/* 0x99 */ {"SBC A, C", B4_(_op_print_type0), B0_(_operand_get_register_c), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sbc), 4, 4},\
/* 0x9A */ {"SBC A, D", B4_(_op_print_type0), B0_(_operand_get_register_d), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sbc), 4, 4},\
/* 0x9B */ {"SBC A, E", B4_(_op_print_type0), B0_(_operand_get_register_e), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sbc), 4, 4},\
/* 0x9C */ {"SBC A, H", B4_(_op_print_type0), B0_(_operand_get_register_h), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sbc), 4, 4},\
/* 0x9D */ {"SBC A, L", B4_(_op_print_type0), B0_(_operand_get_register_l), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sbc), 4, 4},\
/* 0x9E */ {"SBC A, (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sbc), 8, 8},\
/* 0x9F */ {"SBC A, A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sbc), 4, 4},\
/* 0xA0 */ {"AND B", B4_(_op_print_type0), B0_(_operand_get_register_b), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_and), 4, 4},\
/* 0xA1 */ {"AND C", B4_(_op_print_type0), B0_(_operand_get_register_c), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_and), 4, 4},\
/* 0xA2 */ {"AND D", B4_(_op_print_type0), B0_(_operand_get_register_d), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_and), 4, 4},\
/* 0xA3 */ {"AND E", B4_(_op_print_type0), B0_(_operand_get_register_e), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_and), 4, 4},\
/* 0xA4 */ {"AND H", B4_(_op_print_type0), B0_(_operand_get_register_h), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_and), 4, 4},\
/* 0xA5 */ {"AND L", B4_(_op_print_type0), B0_(_operand_get_register_l), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_and), 4, 4},\
/* 0xA6 */ {"AND (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_and), 8, 8},\
/* 0xA7 */ {"AND A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_and), 4, 4},\
/* 0xA8 */ {"XOR B", B4_(_op_print_type0), B0_(_operand_get_register_b), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_xor), 4, 4},\
/* 0xA9 */ {"XOR C", B4_(_op_print_type0), B0_(_operand_get_register_c), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_xor), 4, 4},\
/* 0xAA */ {"XOR D", B4_(_op_print_type0), B0_(_operand_get_register_d), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_xor), 4, 4},\
/* 0xAB */ {"XOR E", B4_(_op_print_type0), B0_(_operand_get_register_e), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_xor), 4, 4},\
/* 0xAC */ {"XOR H", B4_(_op_print_type0), B0_(_operand_get_register_h), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_xor), 4, 4},\
/* 0xAD */ {"XOR L", B4_(_op_print_type0), B0_(_operand_get_register_l), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_xor), 4, 4},\
/* 0xAE */ {"XOR (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_xor), 8, 8},\
/* 0xAF */ {"XOR A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_xor), 4, 4},\
/* 0xB0 */ {"OR B", B4_(_op_print_type0), B0_(_operand_get_register_b), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_or), 4, 4},\
/* 0xB1 */ {"OR C", B4_(_op_print_type0), B0_(_operand_get_register_c), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_or), 4, 4},\
/* 0xB2 */ {"OR D", B4_(_op_print_type0), B0_(_operand_get_register_d), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_or), 4, 4},\
/* 0xB3 */ {"OR E", B4_(_op_print_type0), B0_(_operand_get_register_e), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_or), 4, 4},\
/* 0xB4 */ {"OR H", B4_(_op_print_type0), B0_(_operand_get_register_h), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_or), 4, 4},\
/* 0xB5 */ {"OR L", B4_(_op_print_type0), B0_(_operand_get_register_l), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_or), 4, 4},\
/* 0xB6 */ {"OR (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_or), 8, 8},\
/* 0xB7 */ {"OR A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_or), 4, 4},\
/* 0xB8 */ {"CP B", B4_(_op_print_type0), B0_(_operand_get_register_b), B0_(_operand_get_register_a), nullptr, B1_(_op_exec_sub), 4, 4},\
/* 0xB9 */ {"CP C", B4_(_op_print_type0), B0_(_operand_get_register_c), B0_(_operand_get_register_a), nullptr, B1_(_op_exec_sub), 4, 4},\
/* 0xBA */ {"CP D", B4_(_op_print_type0), B0_(_operand_get_register_d), B0_(_operand_get_register_a), nullptr, B1_(_op_exec_sub), 4, 4},\
/* 0xBB */ {"CP E", B4_(_op_print_type0), B0_(_operand_get_register_e), B0_(_operand_get_register_a), nullptr, B1_(_op_exec_sub), 4, 4},\
/* 0xBC */ {"CP H", B4_(_op_print_type0), B0_(_operand_get_register_h), B0_(_operand_get_register_a), nullptr, B1_(_op_exec_sub), 4, 4},\
/* 0xBD */ {"CP L", B4_(_op_print_type0), B0_(_operand_get_register_l), B0_(_operand_get_register_a), nullptr, B1_(_op_exec_sub), 4, 4},\
/* 0xBE */ {"CP (HL)", B4_(_op_print_type0), B0_(_operand_get_mem_hl), B0_(_operand_get_register_a), nullptr, B1_(_op_exec_sub), 8, 8},\
/* 0xBF */ {"CP A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_a), nullptr, B1_(_op_exec_sub), 4, 4},\
/* 0xC0 */ {"RET NZ", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), B0_(_operand_get_flags_is_nz), B2_(_operand_set_register_pc), B1_(_op_exec_ret), 20, 8},\
/* 0xC1 */ {"POP BC", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), nullptr, B2_(_operand_set_register_bc), B1_(_op_exec_ld), 12, 12},\
/* 0xC2 */ {"JP NZ, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), B0_(_operand_get_flags_is_nz), B2_(_operand_set_register_pc), B1_(_op_exec_jp), 16, 12},\
/* 0xC3 */ {"JP 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), nullptr, B2_(_operand_set_register_pc), B1_(_op_exec_jp), 16, 16},\
/* 0xC4 */ {"CALL NZ, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), B0_(_operand_get_flags_is_nz), B2_(_operand_set_mem_sp_16), B1_(_op_exec_call), 24, 12},\
/* 0xC5 */ {"PUSH BC", B4_(_op_print_type0), B0_(_operand_get_register_bc), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_ld), 16, 16},\
/* 0xC6 */ {"ADD A, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_add8), 8, 8},\
/* 0xC7 */ {"RST 00H", B4_(_op_print_type0), B0_(_operand_get_rst_00), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_rst), 16, 16},\
/* 0xC8 */ {"RET Z", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), B0_(_operand_get_flags_is_z), B2_(_operand_set_register_pc), B1_(_op_exec_ret), 20, 8},\
/* 0xC9 */ {"RET", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), nullptr, B2_(_operand_set_register_pc), B1_(_op_exec_ret), 16, 16},\
/* 0xCA */ {"JP Z, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), B0_(_operand_get_flags_is_z), B2_(_operand_set_register_pc), B1_(_op_exec_jp), 16, 12},\
/* 0xCB */ {"CB", B4_(_op_print_type0), B0_(_operand_get_mem_8), nullptr, nullptr, B1_(_op_exec_cb), 4, 4},\
/* 0xCC */ {"CALL Z, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), B0_(_operand_get_flags_is_z), B2_(_operand_set_mem_sp_16), B1_(_op_exec_call), 24, 12},\
/* 0xCD */ {"CALL 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_call), 24, 24},\
/* 0xCE */ {"ADC A, 0x%02x", B4_(_op_print_type1), B0_(_operand_get_mem_8), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_adc), 8, 8},\
/* 0xCF */ {"RST 08H", B4_(_op_print_type0), B0_(_operand_get_rst_08), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_rst), 16, 16},\
/* 0xD0 */ {"RET NC", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), B0_(_operand_get_flags_is_nc), B2_(_operand_set_register_pc), B1_(_op_exec_ret), 20, 8},\
/* 0xD1 */ {"POP DE", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), nullptr, B2_(_operand_set_register_de), B1_(_op_exec_ld), 12, 12},\
/* 0xD2 */ {"JP NC, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), B0_(_operand_get_flags_is_nc), B2_(_operand_set_register_pc), B1_(_op_exec_jp), 16, 12},\
/* 0xD3 */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xD4 */ {"CALL NC, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), B0_(_operand_get_flags_is_nc), B2_(_operand_set_mem_sp_16), B1_(_op_exec_call), 24, 12},\
/* 0xD5 */ {"PUSH DE", B4_(_op_print_type0), B0_(_operand_get_register_de), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_ld), 16, 16},\
/* 0xD6 */ {"SUB 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sub), 8, 8},\
/* 0xD7 */ {"RST 10H", B4_(_op_print_type0), B0_(_operand_get_rst_10), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_rst), 16, 16},\
/* 0xD8 */ {"RET C", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), B0_(_operand_get_flags_is_c), B2_(_operand_set_register_pc), B1_(_op_exec_ret), 20, 8},\
/* 0xD9 */ {"RETI", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), nullptr, B2_(_operand_set_register_pc), B1_(_op_exec_reti), 16, 16},\
/* 0xDA */ {"JP C, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), B0_(_operand_get_flags_is_c), B2_(_operand_set_register_pc), B1_(_op_exec_jp), 16, 12},\
/* 0xDB */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xDC */ {"CALL C, 0x%04x", B4_(_op_print_type1), B0_(_operand_get_mem_16), B0_(_operand_get_flags_is_c), B2_(_operand_set_mem_sp_16), B1_(_op_exec_call), 24, 12},\
/* 0xDD */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xDE */ {"SBC A, 0x%02x", B4_(_op_print_type1), B0_(_operand_get_mem_8), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_sbc), 8, 8},\
/* 0xDF */ {"RST 18H", B4_(_op_print_type0), B0_(_operand_get_rst_18), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_rst), 16, 16},\
/* 0xE0 */ {"LD (0xff00+0x%02x), A", B4_(_op_print_type4), B0_(_operand_get_register_a), B0_(_operand_get_mem_8_plus_io_base), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 12, 12},\
/* 0xE1 */ {"POP HL", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), nullptr, B2_(_operand_set_register_hl), B1_(_op_exec_ld), 12, 12},\
/* 0xE2 */ {"LD (0xff00+C), A", B4_(_op_print_type0), B0_(_operand_get_register_a), B0_(_operand_get_register_c_plus_io_base), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 8, 8},\
/* 0xE3 */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xE4 */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xE5 */ {"PUSH HL", B4_(_op_print_type0), B0_(_operand_get_register_hl), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_ld), 16, 16},\
/* 0xE6 */ {"AND 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_and), 8, 8},\
/* 0xE7 */ {"RST 20H", B4_(_op_print_type0), B0_(_operand_get_rst_20), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_rst), 16, 16},\
/* 0xE8 */ {"ADD SP, 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_register_sp), B2_(_operand_set_register_sp), B1_(_op_exec_addsp), 16, 16},\
/* 0xE9 */ {"JP HL", B4_(_op_print_type0), B0_(_operand_get_register_hl), nullptr, B2_(_operand_set_register_pc), B1_(_op_exec_jp), 4, 4},\
/* 0xEA */ {"LD (0x%04x), A", B4_(_op_print_type3), B0_(_operand_get_register_a), B0_(_operand_get_mem_16), B2_(_operand_set_mem_8), B1_(_op_exec_ld), 16, 16},\
/* 0xEB */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xEC */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xED */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xEE */ {"XOR 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_xor), 8, 8},\
/* 0xEF */ {"RST 28H", B4_(_op_print_type0), B0_(_operand_get_rst_28), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_rst), 16, 16},\
/* 0xF0 */ {"LD A, (0xff00+0x%02x)", B4_(_op_print_type2), B0_(_operand_get_mem_8_plus_io_base), nullptr, B2_(_operand_set_register_a_mem), B1_(_op_exec_ld), 12, 12},\
/* 0xF1 */ {"POP AF", B4_(_op_print_type0), B0_(_operand_get_mem_sp_16), nullptr, B2_(_operand_set_register_af), B1_(_op_exec_ld), 12, 12},\
/* 0xF2 */ {"LD A, (0xff00+C)", B4_(_op_print_type0), B0_(_operand_get_register_c_plus_io_base_mem), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 8, 8},\
/* 0xF3 */ {"DI", B4_(_op_print_type0), nullptr, nullptr, nullptr, B1_(_op_exec_di), 4, 4},\
/* 0xF4 */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xF5 */ {"PUSH AF", B4_(_op_print_type0), B0_(_operand_get_register_af), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_ld), 16, 16},\
/* 0xF6 */ {"OR 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_register_a), B2_(_operand_set_register_a), B1_(_op_exec_or), 8, 8},\
/* 0xF7 */ {"RST 30H", B4_(_op_print_type0), B0_(_operand_get_rst_30), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_rst), 16, 16},\
/* 0xF8 */ {"LD HL, SP+0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_register_sp), B2_(_operand_set_register_hl), B1_(_op_exec_addsp), 12, 12},\
/* 0xF9 */ {"LD SP, HL", B4_(_op_print_type0), B0_(_operand_get_register_hl), nullptr, B2_(_operand_set_register_sp), B1_(_op_exec_ld), 8, 8},\
/* 0xFA */ {"LD A, (0x%04x)", B4_(_op_print_type1), B0_(_operand_get_mem_16_mem), nullptr, B2_(_operand_set_register_a), B1_(_op_exec_ld), 16, 16},\
/* 0xFB */ {"EI", B4_(_op_print_type0), nullptr, nullptr, nullptr, B1_(_op_exec_ei), 4, 4},\
/* 0xFC */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xFD */ {"UNKOWN", B4_(_op_print_type0), nullptr, nullptr, nullptr, nullptr, 0, 0},\
/* 0xFE */ {"CP 0x%02x", B4_(_op_print_type2), B0_(_operand_get_mem_8), B0_(_operand_get_register_a), nullptr, B1_(_op_exec_sub), 8, 8},\
/* 0xFF */ {"RST 38H", B4_(_op_print_type0), B0_(_operand_get_rst_38), nullptr, B2_(_operand_set_mem_sp_16), B1_(_op_exec_rst), 16, 16},\
}}

#endif // GB_CPU_INSTRUCTIONS_H_
