#ifndef GB_CPU_H_
#define GB_CPU_H_

#include <cstdint>
#include <string>
#include <vector>

class gb_cpu {
public:
    gb_cpu();
    ~gb_cpu();

    void dump_registers();
    uint16_t get_pc();
    uint64_t step();

private:
    struct registers_t {
        union { struct { uint8_t f; uint8_t a; }; uint16_t af; };
        union { struct { uint8_t c; uint8_t b; }; uint16_t bc; };
        union { struct { uint8_t e; uint8_t d; }; uint16_t de; };
        union { struct { uint8_t l; uint8_t h; }; uint16_t hl; };
        uint16_t sp;
        uint16_t pc;
    };

    enum flags_t {
        FLAGS_C = 0x10,
        FLAGS_H = 0x20,
        FLAGS_N = 0x40,
        FLAGS_Z = 0x80
    };

    struct instruction_t;

    typedef uint16_t (gb_cpu::*operand_get_func_t)();
    typedef void (gb_cpu::*operand_set_func_t)(uint16_t, uint16_t);
    typedef void (gb_cpu::*op_print_func_t)(std::string, uint16_t, uint16_t, uint16_t);
    typedef uint64_t (gb_cpu::*op_exec_func_t)(instruction_t&);

    struct instruction_t {
        std::string        disassembly;
        op_print_func_t    op_print;
        operand_get_func_t get_operand1;
        operand_get_func_t get_operand2;
        operand_set_func_t set_operand;
        op_exec_func_t     op_exec;
        uint64_t           cycles_hi;
        uint64_t           cycles_lo;
    };

    registers_t                m_registers;
    std::vector<instruction_t> m_instructions;
    std::vector<instruction_t> m_cb_instructions;

    // Op execution routines
    uint64_t _op_exec_cb(instruction_t& instruction);
    uint64_t _op_exec_nop(instruction_t& instruction);
    uint64_t _op_exec_stop(instruction_t& instruction);
    uint64_t _op_exec_halt(instruction_t& instruction);
    uint64_t _op_exec_ld(instruction_t& instruction);
    uint64_t _op_exec_ldhl(instruction_t& instruction);
    uint64_t _op_exec_jr(instruction_t& instruction);
    uint64_t _op_exec_jp(instruction_t& instruction);
    uint64_t _op_exec_call(instruction_t& instruction);
    uint64_t _op_exec_ret(instruction_t& instruction);
    uint64_t _op_exec_reti(instruction_t& instruction);
    uint64_t _op_exec_rst(instruction_t& instruction);
    uint64_t _op_exec_add8(instruction_t& instruction);
    uint64_t _op_exec_add16(instruction_t& instruction);
    uint64_t _op_exec_addsp(instruction_t& instruction);
    uint64_t _op_exec_add1(instruction_t& instruction);
    uint64_t _op_exec_adc(instruction_t& instruction);
    uint64_t _op_exec_sub(instruction_t& instruction);
    uint64_t _op_exec_sbc(instruction_t& instruction);
    uint64_t _op_exec_inc(instruction_t& instruction);
    uint64_t _op_exec_incf(instruction_t& instruction);
    uint64_t _op_exec_dec(instruction_t& instruction);
    uint64_t _op_exec_decf(instruction_t& instruction);
    uint64_t _op_exec_da(instruction_t& instruction);
    uint64_t _op_exec_rlc(instruction_t& instruction);
    uint64_t _op_exec_rlca(instruction_t& instruction);
    uint64_t _op_exec_rl(instruction_t& instruction);
    uint64_t _op_exec_rla(instruction_t& instruction);
    uint64_t _op_exec_rrc(instruction_t& instruction);
    uint64_t _op_exec_rrca(instruction_t& instruction);
    uint64_t _op_exec_rr(instruction_t& instruction);
    uint64_t _op_exec_rra(instruction_t& instruction);
    uint64_t _op_exec_sla(instruction_t& instruction);
    uint64_t _op_exec_sra(instruction_t& instruction);
    uint64_t _op_exec_srl(instruction_t& instruction);
    uint64_t _op_exec_swap(instruction_t& instruction);
    uint64_t _op_exec_cpl(instruction_t& instruction);
    uint64_t _op_exec_scf(instruction_t& instruction);
    uint64_t _op_exec_ccf(instruction_t& instruction);
    uint64_t _op_exec_and(instruction_t& instruction);
    uint64_t _op_exec_xor(instruction_t& instruction);
    uint64_t _op_exec_or(instruction_t& instruction);
    uint64_t _op_exec_bit(instruction_t& instruction);
    uint64_t _op_exec_set(instruction_t& instruction);
    uint64_t _op_exec_res(instruction_t& instruction);
    uint64_t _op_exec_di(instruction_t& instruction);
    uint64_t _op_exec_ei(instruction_t& instruction);

    // Operand getters
    uint16_t _operand_get_register_a();
    uint16_t _operand_get_register_f();
    uint16_t _operand_get_register_af();
    uint16_t _operand_get_register_b();
    uint16_t _operand_get_register_c();
    uint16_t _operand_get_register_bc();
    uint16_t _operand_get_register_d();
    uint16_t _operand_get_register_e();
    uint16_t _operand_get_register_de();
    uint16_t _operand_get_register_h();
    uint16_t _operand_get_register_l();
    uint16_t _operand_get_register_hl();
    uint16_t _operand_get_register_sp();
    uint16_t _operand_get_register_pc();
    uint16_t _operand_get_register_hl_plus();
    uint16_t _operand_get_register_hl_minus();
    uint16_t _operand_get_mem_8();
    uint16_t _operand_get_mem_8_plus_io_base();
    uint16_t _operand_get_mem_8_plus_io_base_mem();
    uint16_t _operand_get_register_c_plus_io_base();
    uint16_t _operand_get_register_c_plus_io_base_mem();
    uint16_t _operand_get_mem_16();
    uint16_t _operand_get_mem_16_mem();
    uint16_t _operand_get_mem_bc();
    uint16_t _operand_get_mem_de();
    uint16_t _operand_get_mem_hl();
    uint16_t _operand_get_mem_sp_8();
    uint16_t _operand_get_mem_sp_16();
    uint16_t _operand_get_flags_is_nz();
    uint16_t _operand_get_flags_is_z();
    uint16_t _operand_get_flags_is_nc();
    uint16_t _operand_get_flags_is_c();
    uint16_t _operand_get_rst_00();
    uint16_t _operand_get_rst_08();
    uint16_t _operand_get_rst_10();
    uint16_t _operand_get_rst_18();
    uint16_t _operand_get_rst_20();
    uint16_t _operand_get_rst_28();
    uint16_t _operand_get_rst_30();
    uint16_t _operand_get_rst_38();
    uint16_t _operand_get_imm_0();
    uint16_t _operand_get_imm_1();
    uint16_t _operand_get_imm_2();
    uint16_t _operand_get_imm_3();
    uint16_t _operand_get_imm_4();
    uint16_t _operand_get_imm_5();
    uint16_t _operand_get_imm_6();
    uint16_t _operand_get_imm_7();

    // Operand setters
    void _operand_set_register_a(uint16_t addr, uint16_t val);
    void _operand_set_register_f(uint16_t addr, uint16_t val);
    void _operand_set_register_af(uint16_t addr, uint16_t val);
    void _operand_set_register_b(uint16_t addr, uint16_t val);
    void _operand_set_register_c(uint16_t addr, uint16_t val);
    void _operand_set_register_bc(uint16_t addr, uint16_t val);
    void _operand_set_register_d(uint16_t addr, uint16_t val);
    void _operand_set_register_e(uint16_t addr, uint16_t val);
    void _operand_set_register_de(uint16_t addr, uint16_t val);
    void _operand_set_register_h(uint16_t addr, uint16_t val);
    void _operand_set_register_l(uint16_t addr, uint16_t val);
    void _operand_set_register_hl(uint16_t addr, uint16_t val);
    void _operand_set_register_sp(uint16_t addr, uint16_t val);
    void _operand_set_register_pc(uint16_t addr, uint16_t val);
    void _operand_set_mem_8(uint16_t addr, uint16_t val);
    void _operand_set_mem_hl_8(uint16_t addr, uint16_t val);
    void _operand_set_mem_sp_16(uint16_t addr, uint16_t val);

    // Op print routines
    void _op_print_type0(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2);
    void _op_print_type1(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2);
    void _op_print_type2(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2);
    void _op_print_type3(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2);
    void _op_print_type4(std::string disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2);
};

#endif // GB_CPU_H_
