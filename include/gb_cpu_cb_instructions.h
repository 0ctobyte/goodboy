#ifndef GB_CPU_CB_INSTRUCTIONS_H_
#define GB_CPU_CB_INSTRUCTIONS_H_

#define CB_INSTRUCTIONS_LIST_INIT \
{{\
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
/* 0x80 */ {"RES 0, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x81 */ {"RES 0, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x82 */ {"RES 0, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x83 */ {"RES 0, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x84 */ {"RES 0, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x85 */ {"RES 0, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x86 */ {"RES 0, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0x87 */ {"RES 0, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x88 */ {"RES 1, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x89 */ {"RES 1, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8A */ {"RES 1, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8B */ {"RES 1, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8C */ {"RES 1, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8D */ {"RES 1, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x8E */ {"RES 1, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0x8F */ {"RES 1, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x90 */ {"RES 2, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x91 */ {"RES 2, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x92 */ {"RES 2, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x93 */ {"RES 2, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x94 */ {"RES 2, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x95 */ {"RES 2, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x96 */ {"RES 2, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0x97 */ {"RES 2, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x98 */ {"RES 3, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x99 */ {"RES 3, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9A */ {"RES 3, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9B */ {"RES 3, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9C */ {"RES 3, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9D */ {"RES 3, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_res, 8, 8},\
/* 0x9E */ {"RES 3, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0x9F */ {"RES 3, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA0 */ {"RES 4, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA1 */ {"RES 4, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA2 */ {"RES 4, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA3 */ {"RES 4, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA4 */ {"RES 4, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA5 */ {"RES 4, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA6 */ {"RES 4, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0xA7 */ {"RES 4, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA8 */ {"RES 5, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xA9 */ {"RES 5, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAA */ {"RES 5, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAB */ {"RES 5, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAC */ {"RES 5, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAD */ {"RES 5, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xAE */ {"RES 5, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0xAF */ {"RES 5, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB0 */ {"RES 6, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB1 */ {"RES 6, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB2 */ {"RES 6, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB3 */ {"RES 6, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB4 */ {"RES 6, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB5 */ {"RES 6, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB6 */ {"RES 6, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0xB7 */ {"RES 6, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB8 */ {"RES 7, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xB9 */ {"RES 7, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBA */ {"RES 7, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBB */ {"RES 7, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBC */ {"RES 7, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBD */ {"RES 7, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xBE */ {"RES 7, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_res, 16, 16},\
/* 0xBF */ {"RES 7, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_res, 8, 8},\
/* 0xC0 */ {"SET 0, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC1 */ {"SET 0, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC2 */ {"SET 0, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC3 */ {"SET 0, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC4 */ {"SET 0, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC5 */ {"SET 0, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC6 */ {"SET 0, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xC7 */ {"SET 0, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_0, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC8 */ {"SET 1, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xC9 */ {"SET 1, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCA */ {"SET 1, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCB */ {"SET 1, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCC */ {"SET 1, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCD */ {"SET 1, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xCE */ {"SET 1, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xCF */ {"SET 1, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_1, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD0 */ {"SET 2, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD1 */ {"SET 2, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD2 */ {"SET 2, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD3 */ {"SET 2, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD4 */ {"SET 2, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD5 */ {"SET 2, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD6 */ {"SET 2, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xD7 */ {"SET 2, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_2, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD8 */ {"SET 3, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xD9 */ {"SET 3, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDA */ {"SET 3, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDB */ {"SET 3, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDC */ {"SET 3, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDD */ {"SET 3, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xDE */ {"SET 3, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xDF */ {"SET 3, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_3, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE0 */ {"SET 4, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE1 */ {"SET 4, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE2 */ {"SET 4, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE3 */ {"SET 4, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE4 */ {"SET 4, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE5 */ {"SET 4, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE6 */ {"SET 4, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xE7 */ {"SET 4, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_4, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE8 */ {"SET 5, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xE9 */ {"SET 5, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xEA */ {"SET 5, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xEB */ {"SET 5, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xEC */ {"SET 5, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xED */ {"SET 5, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xEE */ {"SET 5, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xEF */ {"SET 5, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_5, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF0 */ {"SET 6, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF1 */ {"SET 6, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF2 */ {"SET 6, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF3 */ {"SET 6, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF4 */ {"SET 6, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF5 */ {"SET 6, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF6 */ {"SET 6, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xF7 */ {"SET 6, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_6, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF8 */ {"SET 7, B", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_b, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_b, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xF9 */ {"SET 7, C", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_c, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_c, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFA */ {"SET 7, D", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_d, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_d, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFB */ {"SET 7, E", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_e, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_e, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFC */ {"SET 7, H", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_h, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_h, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFD */ {"SET 7, L", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_l, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_l, &gb_cpu::_op_exec_set, 8, 8},\
/* 0xFE */ {"SET 7, (HL)", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_mem_hl, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_mem_hl_8, &gb_cpu::_op_exec_set, 16, 16},\
/* 0xFF */ {"SET 7, A", &gb_cpu::_op_print_type0, &gb_cpu::_operand_get_register_a, &gb_cpu::_operand_get_imm_7, &gb_cpu::_operand_set_register_a, &gb_cpu::_op_exec_set, 8, 8},\
}}

#endif // GB_CPU_CB_INSTRUCTIONS_H_
