#ifndef GB_CPU_H_
#define GB_CPU_H_

#include <cstdint>
#include <string>
#include <vector>

#include "gb_memory_map.h"

class gb_cpu {
public:
    gb_cpu(gb_memory_map& memory_map);
    ~gb_cpu();

    void dump_registers() const;
    uint16_t get_pc() const;
    int step();
    bool handle_interrupt(uint16_t jump_address);

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
    typedef void (gb_cpu::*op_print_func_t)(const std::string&, uint16_t, uint16_t, uint16_t) const;
    typedef int (gb_cpu::*op_exec_func_t)(const instruction_t&);

    struct instruction_t {
        std::string        disassembly;
        op_print_func_t    op_print;
        operand_get_func_t get_operand1;
        operand_get_func_t get_operand2;
        operand_set_func_t set_operand;
        op_exec_func_t     op_exec;
        int                cycles_hi;
        int                cycles_lo;
    };

    enum eidiflag_t {
        EIDI_NONE,
        EIDI_IENABLE,
        EIDI_IDISABLE
    };

    registers_t                m_registers;
    std::vector<instruction_t> m_instructions;
    std::vector<instruction_t> m_cb_instructions;
    gb_memory_map&             m_memory_map;
    eidiflag_t                 m_eidi_flag;
    bool                       m_interrupt_enable;
    bool                       m_halted;

    // Op execution routines
    int _op_exec_cb(const instruction_t& instruction);
    int _op_exec_nop(const instruction_t& instruction);
    int _op_exec_stop(const instruction_t& instruction);
    int _op_exec_halt(const instruction_t& instruction);
    int _op_exec_ld(const instruction_t& instruction);
    int _op_exec_ldhl(const instruction_t& instruction);
    int _op_exec_jr(const instruction_t& instruction);
    int _op_exec_jp(const instruction_t& instruction);
    int _op_exec_call(const instruction_t& instruction);
    int _op_exec_ret(const instruction_t& instruction);
    int _op_exec_reti(const instruction_t& instruction);
    int _op_exec_rst(const instruction_t& instruction);
    int _op_exec_add8(const instruction_t& instruction);
    int _op_exec_add16(const instruction_t& instruction);
    int _op_exec_addsp(const instruction_t& instruction);
    int _op_exec_add1(const instruction_t& instruction);
    int _op_exec_adc(const instruction_t& instruction);
    int _op_exec_sub(const instruction_t& instruction);
    int _op_exec_sbc(const instruction_t& instruction);
    int _op_exec_inc(const instruction_t& instruction);
    int _op_exec_incf(const instruction_t& instruction);
    int _op_exec_dec(const instruction_t& instruction);
    int _op_exec_decf(const instruction_t& instruction);
    int _op_exec_da(const instruction_t& instruction);
    int _op_exec_rlc(const instruction_t& instruction);
    int _op_exec_rlca(const instruction_t& instruction);
    int _op_exec_rl(const instruction_t& instruction);
    int _op_exec_rla(const instruction_t& instruction);
    int _op_exec_rrc(const instruction_t& instruction);
    int _op_exec_rrca(const instruction_t& instruction);
    int _op_exec_rr(const instruction_t& instruction);
    int _op_exec_rra(const instruction_t& instruction);
    int _op_exec_sla(const instruction_t& instruction);
    int _op_exec_sra(const instruction_t& instruction);
    int _op_exec_srl(const instruction_t& instruction);
    int _op_exec_swap(const instruction_t& instruction);
    int _op_exec_cpl(const instruction_t& instruction);
    int _op_exec_scf(const instruction_t& instruction);
    int _op_exec_ccf(const instruction_t& instruction);
    int _op_exec_and(const instruction_t& instruction);
    int _op_exec_xor(const instruction_t& instruction);
    int _op_exec_or(const instruction_t& instruction);
    int _op_exec_bit(const instruction_t& instruction);
    int _op_exec_set(const instruction_t& instruction);
    int _op_exec_res(const instruction_t& instruction);
    int _op_exec_di(const instruction_t& instruction);
    int _op_exec_ei(const instruction_t& instruction);

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
    void _operand_set_mem_16(uint16_t addr, uint16_t val);
    void _operand_set_mem_hl_8(uint16_t addr, uint16_t val);
    void _operand_set_mem_sp_16(uint16_t addr, uint16_t val);

    // Op print routines
    void _op_print_type0(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const;
    void _op_print_type1(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const;
    void _op_print_type2(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const;
    void _op_print_type3(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const;
    void _op_print_type4(const std::string& disassembly, uint16_t pc, uint16_t operand1, uint16_t operand2) const;
};

#endif // GB_CPU_H_
