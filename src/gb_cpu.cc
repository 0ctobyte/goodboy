#include "gb_logger.h"
#include "gb_memory_map.h"
#include "gb_cpu.h"
#include "gb_cpu_instructions.h"
#include "gb_cpu_cb_instructions.h"

#define FLAGS_IS_SET(flags)         ((m_registers.f & (flags)) != 0)
#define FLAGS_IS_CLEAR(flags)       ((m_registers.f & (flags)) == 0)
#define FLAGS_SET(flags)            { m_registers.f |= (flags); }
#define FLAGS_CLEAR(flags)          { m_registers.f &= ~(flags); }
#define FLAGS_TOGGLE(flags)         { m_registers.f ^= (flags); }

#define FLAGS_SET_IF_Z(x)           { if (static_cast<uint8_t>((x)) == 0) FLAGS_SET(FLAGS_Z); }
#define FLAGS_SET_IF_H(x,y)         { if ((((x) & 0x0f) + ((y) & 0x0f)) & 0xf0) FLAGS_SET(FLAGS_H); }
#define FLAGS_SET_IF_C(x)           { if ((x) & 0xff00) FLAGS_SET(FLAGS_C); }

#define FLAGS_SET_IF_H_16(x,y)      { if ((((x) & 0xfff) + ((y) & 0xfff)) & 0xf000) FLAGS_SET(FLAGS_H); }
#define FLAGS_SET_IF_C_16(x)        { if ((x) & 0xffff0000) FLAGS_SET(FLAGS_C); }

#define FLAGS_SET_IF_H_BORROW(x,y)  { if (((x) & 0x0f) > ((y) & 0x0f)) FLAGS_SET(FLAGS_H); }
#define FLAGS_SET_IF_C_BORROW(x,y)  { if ((x) > (y)) FLAGS_SET(FLAGS_C); }

gb_cpu::gb_cpu(gb_memory_map& memory_map)
    : m_instructions(INSTRUCTIONS_INIT), m_cb_instructions(CB_INSTRUCTIONS_INIT), m_memory_map(memory_map), m_eidi_flag(EIDI_NONE), m_interrupt_enable(true), m_halted(false)
{
    m_registers.af = 0x01b0;
    m_registers.bc = 0x0013;
    m_registers.de = 0x00d8;
    m_registers.hl = 0x014d;
    m_registers.sp = 0xfffe;
    m_registers.pc = 0x0000;
}

gb_cpu::~gb_cpu() {
}

void gb_cpu::dump_registers() const {
    char buf[256];
    snprintf(buf, 256, "AF: 0x%02x.%02x BC: 0x%02x.%02x DE: 0x%02x.%02x HL: 0x%02x.%02x SP: 0x%04x PC: 0x%04x FLAGS: %c%c%c%c",
        m_registers.a, m_registers.f, m_registers.b, m_registers.c, m_registers.d, m_registers.e, m_registers.h, m_registers.l, m_registers.sp, m_registers.pc,
        FLAGS_IS_SET(FLAGS_Z) ? 'Z' : '-', FLAGS_IS_SET(FLAGS_N) ? 'N' : '-', FLAGS_IS_SET(FLAGS_H) ? 'H' : '-', FLAGS_IS_SET(FLAGS_C) ? 'C' : '-');

    GB_LOGGER(GB_LOG_TRACE) << buf << std::endl;
}

uint16_t gb_cpu::get_pc() const {
    return m_registers.pc;
}

bool gb_cpu::handle_interrupt(uint16_t jump_address) {
    // Always break out of halted mode if an interrupt occurs
    m_halted = false;

    if (!m_interrupt_enable) return false;

    // Disable interrupts
    m_interrupt_enable = false;

    // Push current PC onto the stack
    _operand_set_mem_sp_16(0, m_registers.pc);

    // Jump to interrupt address
    m_registers.pc = jump_address;

    return true;
}

int gb_cpu::step() {
    // Check if in halted mode, do nothing and return 4 CPU clock cycles (i.e. 1 system clock cycle)
    if (m_halted) return 4;

    uint8_t opcode = m_memory_map.read_byte(m_registers.pc);

    const instruction_t& instruction = m_instructions[opcode];

    if (instruction.op_exec == nullptr) {
        std::string unknown_str ("UNKNOWN");
        _op_print_type0(unknown_str, m_registers.pc, 0, 0);
        m_registers.pc++;
        return 0;
    }

    if (m_eidi_flag == EIDI_IENABLE)  m_interrupt_enable = true;
    if (m_eidi_flag == EIDI_IDISABLE) m_interrupt_enable = false;
    m_eidi_flag = EIDI_NONE;

    return instruction.op_exec(instruction);
}

int gb_cpu::_op_exec_cb(const instruction_t& instruction) {
    uint8_t cb_opcode = m_memory_map.read_byte(m_registers.pc+1);

    const instruction_t& cb_instruction = m_cb_instructions[cb_opcode];

    if (cb_instruction.op_exec == nullptr) {
        std::string unknown_str ("UNKNOWN CB");
        _op_print_type0(unknown_str, m_registers.pc, 0, 0);
        m_registers.pc += 2;
        return 0;
    }

    int cycles = cb_instruction.op_exec(cb_instruction);

    m_registers.pc++;

    return cycles;
}

int gb_cpu::_op_exec_nop(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    instruction.op_print(instruction.disassembly, pc, 0, 0);

    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_stop(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    // TODO: Implement stop

    instruction.op_print(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_halt(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    m_halted = true;

    instruction.op_print(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_ld(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t data = instruction.get_operand1();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();

    instruction.set_operand(addr, data);

    instruction.op_print(instruction.disassembly, pc, data, addr);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_add8(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = instruction.get_operand1();
    uint16_t a2 = instruction.get_operand2();
    uint16_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET_IF_H(a1, a2);
    FLAGS_SET_IF_C(sum);

    instruction.set_operand(0, sum);

    instruction.op_print(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_add16(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint32_t a1 = instruction.get_operand1();
    uint32_t a2 = instruction.get_operand2();
    uint32_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_H_16(a1, a2);
    FLAGS_SET_IF_C_16(sum);

    instruction.set_operand(0, static_cast<uint16_t>(sum));

    instruction.op_print(instruction.disassembly, pc, static_cast<uint16_t>(a1), static_cast<uint16_t>(a2));
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_addsp(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    int32_t a1 = static_cast<int32_t>(static_cast<int8_t>(instruction.get_operand1()));
    int32_t a2 = static_cast<int32_t>(instruction.get_operand2());
    int32_t sum = a1 + a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_H(a1, a2);
    FLAGS_SET_IF_C((a1 & 0xff) + (a2 & 0xff));

    instruction.set_operand(0, static_cast<uint16_t>(sum));

    instruction.op_print(instruction.disassembly, pc, static_cast<uint16_t>(a1), static_cast<uint16_t>(a2));
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_adc(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint8_t carry = FLAGS_IS_SET(FLAGS_C) ? 1 : 0;
    uint16_t a1 = instruction.get_operand1();
    uint16_t a2 = instruction.get_operand2();
    uint16_t sum = a1 + a2 + carry;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET_IF_C(sum);
    if (((a1 & 0x0f) + (a2 & 0x0f) + carry) & 0xf0) FLAGS_SET(FLAGS_H);

    instruction.set_operand(0, sum);

    instruction.op_print(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_sub(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = instruction.get_operand1();
    uint16_t a2 = instruction.get_operand2();
    uint16_t sum = a2 - a1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET(FLAGS_N);
    FLAGS_SET_IF_H_BORROW(a1, a2);
    FLAGS_SET_IF_C_BORROW(a1, a2);

    if (instruction.set_operand != nullptr) {
        instruction.set_operand(0, sum);
    }

    instruction.op_print(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_sbc(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t carry = FLAGS_IS_SET(FLAGS_C) ? 1 : 0;
    uint16_t a1 = instruction.get_operand1();
    uint16_t a2 = instruction.get_operand2();
    uint16_t sum = a2 - (a1 + carry);

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(sum);
    FLAGS_SET(FLAGS_N);
    if (((a1 & 0x0f) + carry) > (a2 & 0x0f)) FLAGS_SET(FLAGS_H);
    if ((a1 + carry) > a2) FLAGS_SET(FLAGS_C);

    instruction.set_operand(0, sum);

    instruction.op_print(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_jr(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    int cycles = instruction.cycles_lo;
    int16_t offset = static_cast<int8_t>(instruction.get_operand1());
    uint16_t jump_pc = static_cast<uint16_t>(static_cast<int16_t>(m_registers.pc) + offset);
    uint16_t do_jump = instruction.get_operand2 == nullptr ? 1 : instruction.get_operand2();

    if (do_jump != 0) {
        instruction.set_operand(0, jump_pc);
        cycles = instruction.cycles_hi;
    }

    instruction.op_print(instruction.disassembly, pc, jump_pc, 0);
    return cycles;
}

int gb_cpu::_op_exec_jp(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    int cycles = instruction.cycles_lo;
    uint16_t jump_pc = instruction.get_operand1();
    uint16_t do_jump = instruction.get_operand2 == nullptr ? 1 : instruction.get_operand2();

    if (do_jump != 0) {
        instruction.set_operand(0, jump_pc);
        cycles = instruction.cycles_hi;
    }

    instruction.op_print(instruction.disassembly, pc, jump_pc, 0);
    return cycles;
}

int gb_cpu::_op_exec_call(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    int cycles = instruction.cycles_lo;
    uint16_t jump_pc = instruction.get_operand1();
    uint16_t do_jump = instruction.get_operand2 == nullptr ? 1 : instruction.get_operand2();

    if (do_jump != 0) {
        instruction.set_operand(0, m_registers.pc);
        m_registers.pc = jump_pc;
        cycles = instruction.cycles_hi;
    }

    instruction.op_print(instruction.disassembly, pc, jump_pc, 0);
    return cycles;
}

int gb_cpu::_op_exec_ret(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    int cycles = instruction.cycles_lo;
    uint16_t do_jump = instruction.get_operand2 == nullptr ? 1 : instruction.get_operand2();

    if (do_jump != 0) {
        uint16_t jump_pc = instruction.get_operand1();
        instruction.set_operand(0, jump_pc);
        cycles = instruction.cycles_hi;
    }

    instruction.op_print(instruction.disassembly, pc, 0, 0);
    return cycles;
}

int gb_cpu::_op_exec_reti(const instruction_t& instruction) {
    m_interrupt_enable = true;
    return gb_cpu::_op_exec_ret(instruction);
}

int gb_cpu::_op_exec_rst(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t jump_pc = instruction.get_operand1();

    instruction.set_operand(0, m_registers.pc);
    m_registers.pc = jump_pc;

    instruction.op_print(instruction.disassembly, pc, jump_pc, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_da(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a = instruction.get_operand1();

    // Good explanation for this instruction here: https://ehaskins.com/2018-01-30%20Z80%20DAA/
    if (FLAGS_IS_SET(FLAGS_N)) {
        // Subtraction case; adjust top nibble if C flag is set
        // adjust bottom nibble if H flag is set. Adjustment is done by subtracting 0x6.
        if (FLAGS_IS_SET(FLAGS_H)) a = (a - 0x06) & 0xff;
        if (FLAGS_IS_SET(FLAGS_C)) a -= 0x60;
    } else {
        // Addition case; adjust top nibble if C flag is set or result is > 0x99
        // which is the largest # that can be represented by 8-bit BCD.
        // Adjust bottom nibble if H flag is set or if > 0x9. 0x9 is the largest each nibble
        // can represent.
        if (FLAGS_IS_SET(FLAGS_H) || (a & 0x0f) > 0x09) a += 0x06;
        if (FLAGS_IS_SET(FLAGS_C) || a > 0x9F) a += 0x60;
    }

    FLAGS_CLEAR(FLAGS_Z | FLAGS_H);
    FLAGS_SET_IF_Z(a);
    FLAGS_SET_IF_C(a);

    instruction.set_operand(0, a);

    instruction.op_print(instruction.disassembly, pc, a, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_rlc(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = static_cast<uint16_t>(instruction.get_operand1() << 1);
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();
    val |= ((val >> 8) & 0x1);

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(val);

    instruction.set_operand(addr, val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_rlca(const instruction_t& instruction) {
    int cycles = _op_exec_rlc(instruction);
    FLAGS_CLEAR(FLAGS_Z);
    return cycles;
}

int gb_cpu::_op_exec_rl(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = static_cast<uint16_t>(instruction.get_operand1() << 1);
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();
    int carry = FLAGS_IS_SET(FLAGS_C) ? 1 : 0;
    val |= carry;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(val);

    instruction.set_operand(addr, val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_rla(const instruction_t& instruction) {
    int cycles = _op_exec_rl(instruction);
    FLAGS_CLEAR(FLAGS_Z);
    return cycles;
}

int gb_cpu::_op_exec_rrc(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();
    int bit1 = val & 0x1;
    val = static_cast<uint16_t>((val >> 1) | (bit1 << 7));

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(bit1 << 8);

    instruction.set_operand(addr, val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_rrca(const instruction_t& instruction) {
    int cycles = _op_exec_rrc(instruction);
    FLAGS_CLEAR(FLAGS_Z);
    return cycles;
}

int gb_cpu::_op_exec_rr(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();
    int bit1 = val & 0x1;
    int carry = FLAGS_IS_SET(FLAGS_C) ? 1 : 0;
    val = static_cast<uint16_t>((val >> 1) | (carry << 0x7));

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(bit1 << 8);

    instruction.set_operand(addr, val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_rra(const instruction_t& instruction) {
    int cycles = _op_exec_rr(instruction);
    FLAGS_CLEAR(FLAGS_Z);
    return cycles;
}

int gb_cpu::_op_exec_sla(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = static_cast<uint16_t>(instruction.get_operand1() << 1);
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(val);

    instruction.set_operand(addr, val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_sra(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();
    int bit0 = val & 0x1;
    int bit7 = val & 0x80;
    val = static_cast<uint16_t>((val >> 1) | bit7);

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(bit0 << 0x8);

    instruction.set_operand(addr, val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_srl(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();
    int bit0 = val & 0x1;
    val = val >> 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);
    FLAGS_SET_IF_C(bit0 << 0x8);

    instruction.set_operand(addr, val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_swap(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();

    val = static_cast<uint16_t>((val << 4) | (val >> 4));

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(val);

    instruction.set_operand(addr, val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_cpl(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();

    FLAGS_SET(FLAGS_N | FLAGS_H);

    instruction.set_operand(0, ~val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_inc(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();

    instruction.set_operand(0, ++val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_dec(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();

    instruction.set_operand(0, --val);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_incf(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();
    uint16_t vali = val + 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H);
    FLAGS_SET_IF_Z(vali);
    FLAGS_SET_IF_H(val, 1);

    instruction.set_operand(addr, vali);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_decf(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t val = instruction.get_operand1();
    uint16_t addr = instruction.get_operand2 == nullptr ? 0 : instruction.get_operand2();
    uint16_t vald = val - 1;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H);
    FLAGS_SET_IF_Z(vald);
    FLAGS_SET(FLAGS_N);
    FLAGS_SET_IF_H_BORROW(1, val);

    instruction.set_operand(addr, vald);

    instruction.op_print(instruction.disassembly, pc, val, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_scf(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    FLAGS_CLEAR(FLAGS_N | FLAGS_H);
    FLAGS_SET(FLAGS_C);

    instruction.op_print(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_ccf(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    FLAGS_CLEAR(FLAGS_N | FLAGS_H);
    FLAGS_TOGGLE(FLAGS_C);

    instruction.op_print(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_and(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = instruction.get_operand1();
    uint16_t a2 = instruction.get_operand2();
    uint16_t result = a1 & a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(result);
    FLAGS_SET(FLAGS_H);

    instruction.set_operand(0, result);

    instruction.op_print(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_xor(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = instruction.get_operand1();
    uint16_t a2 = instruction.get_operand2();
    uint16_t result = a1 ^ a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(result);

    instruction.set_operand(0, result);

    instruction.op_print(instruction.disassembly, pc, a1, a2);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_or(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = instruction.get_operand1();
    uint16_t a2 = instruction.get_operand2();
    uint16_t result = a1 | a2;

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H | FLAGS_C);
    FLAGS_SET_IF_Z(result);

    instruction.set_operand(0, result);

    instruction.op_print(instruction.disassembly, pc, a1, a1);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_bit(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = instruction.get_operand1();
    uint16_t bit = instruction.get_operand2();
    uint16_t result = a1 & (1 << bit);

    FLAGS_CLEAR(FLAGS_Z | FLAGS_N | FLAGS_H);
    FLAGS_SET_IF_Z(result);
    FLAGS_SET(FLAGS_H);

    instruction.op_print(instruction.disassembly, pc, a1, bit);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_set(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = instruction.get_operand1();
    uint16_t bit = instruction.get_operand2();
    uint16_t result = static_cast<uint16_t>(a1 | (1 << bit));

    instruction.set_operand(0, result);

    instruction.op_print(instruction.disassembly, pc, a1, bit);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_res(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    uint16_t a1 = instruction.get_operand1();
    uint16_t bit = instruction.get_operand2();
    uint16_t result = a1 & ~(1 << bit);

    instruction.set_operand(0, result);

    instruction.op_print(instruction.disassembly, pc, a1, bit);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_di(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    m_eidi_flag = EIDI_IDISABLE;

    instruction.op_print(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

int gb_cpu::_op_exec_ei(const instruction_t& instruction) {
    uint16_t pc = m_registers.pc++;

    m_eidi_flag = EIDI_IENABLE;

    instruction.op_print(instruction.disassembly, pc, 0, 0);
    return instruction.cycles_hi;
}

uint16_t gb_cpu::_operand_get_register_a() {
    return m_registers.a;
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
    return m_memory_map.read_byte(m_registers.pc++);
}

uint16_t gb_cpu::_operand_get_mem_8_plus_io_base() {
    uint16_t offset = _operand_get_mem_8();
    return (offset + GB_MEMORY_MAP_IO_BASE);
}

uint16_t gb_cpu::_operand_get_mem_8_plus_io_base_mem() {
    uint16_t addr = _operand_get_mem_8_plus_io_base();
    return m_memory_map.read_byte(addr);
}

uint16_t gb_cpu::_operand_get_register_c_plus_io_base() {
    uint16_t offset = _operand_get_register_c();
    return (offset + GB_MEMORY_MAP_IO_BASE);
}

uint16_t gb_cpu::_operand_get_register_c_plus_io_base_mem() {
    uint16_t addr = _operand_get_register_c_plus_io_base();
    return m_memory_map.read_byte(addr);
}

uint16_t gb_cpu::_operand_get_mem_16() {
    uint16_t byte_lo = m_memory_map.read_byte(m_registers.pc++);
    uint16_t byte_hi = m_memory_map.read_byte(m_registers.pc++);
    return static_cast<uint16_t>((byte_hi << 8) | byte_lo);
}

uint16_t gb_cpu::_operand_get_mem_16_mem() {
    uint16_t addr = _operand_get_mem_16();
    return m_memory_map.read_byte(addr);
}

uint16_t gb_cpu::_operand_get_mem_bc() {
    return m_memory_map.read_byte(m_registers.bc);
}

uint16_t gb_cpu::_operand_get_mem_de() {
    return m_memory_map.read_byte(m_registers.de);
}

uint16_t gb_cpu::_operand_get_mem_hl() {
    return m_memory_map.read_byte(m_registers.hl);
}

uint16_t gb_cpu::_operand_get_mem_sp_8() {
    return m_memory_map.read_byte(m_registers.sp++);
}

uint16_t gb_cpu::_operand_get_mem_sp_16() {
    uint16_t byte_lo = m_memory_map.read_byte(m_registers.sp++);
    uint16_t byte_hi = m_memory_map.read_byte(m_registers.sp++);
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
    m_registers.f = (static_cast<uint8_t>(val) & 0xF0);
}

void gb_cpu::_operand_set_register_af(uint16_t addr, uint16_t val) {
    m_registers.af = (val & 0xFFF0);
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
    m_memory_map.write_byte(addr, static_cast<uint8_t>(val));
}

void gb_cpu::_operand_set_mem_16(uint16_t addr, uint16_t val) {
    _operand_set_mem_8(addr, static_cast<uint8_t>(val & 0xff));
    _operand_set_mem_8(addr+1, static_cast<uint8_t>(val >> 8));
}

void gb_cpu::_operand_set_mem_hl_8(uint16_t addr, uint16_t val) {
    _operand_set_mem_8(_operand_get_register_hl(), val);
}

void gb_cpu::_operand_set_mem_sp_16(uint16_t addr, uint16_t val) {
    m_memory_map.write_byte(--m_registers.sp, static_cast<uint8_t>(val >> 8));
    m_memory_map.write_byte(--m_registers.sp, static_cast<uint8_t>(val));
}

void gb_cpu::_op_print_type0(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const {
    char out[256];
    snprintf(out, 256, "%04x: %s", pc, disassembly.c_str());
    std::string fmt = out;
    GB_LOGGER(GB_LOG_TRACE) << fmt << std::endl;
}

void gb_cpu::_op_print_type1(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const {
    char buf[256], out[256];
    snprintf(buf, 256, disassembly.c_str(), operand1);
    snprintf(out, 256, "%04x: %s", pc, buf);
    std::string fmt = out;
    GB_LOGGER(GB_LOG_TRACE) << fmt << std::endl;
}

void gb_cpu::_op_print_type2(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const {
    _op_print_type1(disassembly, pc, static_cast<uint8_t>(operand1), static_cast<uint8_t>(operand2));
}

void gb_cpu::_op_print_type3(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const {
    char buf[256], out[256];
    snprintf(buf, 256, disassembly.c_str(), operand2);
    snprintf(out, 256, "%04x: %s", pc, buf);
    std::string fmt = out;
    GB_LOGGER(GB_LOG_TRACE) << fmt << std::endl;
}

void gb_cpu::_op_print_type4(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const {
    _op_print_type3(disassembly, pc, static_cast<uint8_t>(operand1), static_cast<uint8_t>(operand2));
}
