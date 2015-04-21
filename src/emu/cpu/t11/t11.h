/*** T-11: Portable DEC T-11 emulator ******************************************/

#pragma once

#ifndef __T11_H__
#define __T11_H__


enum
{
	T11_R0=1, T11_R1, T11_R2, T11_R3, T11_R4, T11_R5, T11_SP, T11_PC, T11_PSW
};

#define T11_IRQ0        0      /* IRQ0 */
#define T11_IRQ1        1      /* IRQ1 */
#define T11_IRQ2        2      /* IRQ2 */
#define T11_IRQ3        3      /* IRQ3 */

#define T11_RESERVED    0x000   /* Reserved vector */
#define T11_TIMEOUT     0x004   /* Time-out/system error vector */
#define T11_ILLINST     0x008   /* Illegal and reserved instruction vector */
#define T11_BPT         0x00C   /* BPT instruction vector */
#define T11_IOT         0x010   /* IOT instruction vector */
#define T11_PWRFAIL     0x014   /* Power fail vector */
#define T11_EMT         0x018   /* EMT instruction vector */
#define T11_TRAP        0x01C   /* TRAP instruction vector */


#define MCFG_T11_INITIAL_MODE(_mode) \
	t11_device::set_initial_mode(*device, _mode);


class t11_device :  public cpu_device
{
public:
	// construction/destruction
	t11_device(const machine_config &mconfig, const char *_tag, device_t *_owner, UINT32 _clock);

	// static configuration helpers
	static void set_initial_mode(device_t &device, const UINT16 mode) { downcast<t11_device &>(device).c_initial_mode = mode; }

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_execute_interface overrides
	virtual UINT32 execute_min_cycles() const { return 12; }
	virtual UINT32 execute_max_cycles() const { return 110; }
	virtual UINT32 execute_input_lines() const { return 4; }
	virtual void execute_run();
	virtual void execute_set_input(int inputnum, int state);
	virtual UINT32 execute_default_irq_vector() const { return -1; };

	// device_memory_interface overrides
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const { return (spacenum == AS_PROGRAM) ? &m_program_config : NULL; }

	// device_state_interface overrides
	void state_string_export(const device_state_entry &entry, astring &str);

	// device_disasm_interface overrides
	virtual UINT32 disasm_min_opcode_bytes() const { return 2; }
	virtual UINT32 disasm_max_opcode_bytes() const { return 6; }
	virtual offs_t disasm_disassemble(char *buffer, offs_t pc, const UINT8 *oprom, const UINT8 *opram, UINT32 options);

private:
	address_space_config m_program_config;

	UINT16 c_initial_mode;

	PAIR                m_ppc;    /* previous program counter */
	PAIR                m_reg[8];
	PAIR                m_psw;
	UINT16              m_initial_pc;
	UINT8               m_wait_state;
	UINT8               m_irq_state;
	int                 m_icount;
	address_space *m_program;
	direct_read_data *m_direct;

	inline int ROPCODE();
	inline int RBYTE(int addr);
	inline void WBYTE(int addr, int data);
	inline int RWORD(int addr);
	inline void WWORD(int addr, int data);
	inline void PUSH(int val);
	inline int POP();
	void t11_check_irqs();

	typedef void ( t11_device::*opcode_func )(UINT16 op);
	static const opcode_func s_opcode_table[65536 >> 3];

	void op_0000(UINT16 op);
	void halt(UINT16 op);
	void illegal(UINT16 op);
	void jmp_rgd(UINT16 op);
	void jmp_in(UINT16 op);
	void jmp_ind(UINT16 op);
	void jmp_de(UINT16 op);
	void jmp_ded(UINT16 op);
	void jmp_ix(UINT16 op);
	void jmp_ixd(UINT16 op);
	void rts(UINT16 op);
	void ccc(UINT16 op);
	void scc(UINT16 op);
	void swab_rg(UINT16 op);
	void swab_rgd(UINT16 op);
	void swab_in(UINT16 op);
	void swab_ind(UINT16 op);
	void swab_de(UINT16 op);
	void swab_ded(UINT16 op);
	void swab_ix(UINT16 op);
	void swab_ixd(UINT16 op);
	void br(UINT16 op);
	void bne(UINT16 op);
	void beq(UINT16 op);
	void bge(UINT16 op);
	void blt(UINT16 op);
	void bgt(UINT16 op);
	void ble(UINT16 op);
	void jsr_rgd(UINT16 op);
	void jsr_in(UINT16 op);
	void jsr_ind(UINT16 op);
	void jsr_de(UINT16 op);
	void jsr_ded(UINT16 op);
	void jsr_ix(UINT16 op);
	void jsr_ixd(UINT16 op);
	void clr_rg(UINT16 op);
	void clr_rgd(UINT16 op);
	void clr_in(UINT16 op);
	void clr_ind(UINT16 op);
	void clr_de(UINT16 op);
	void clr_ded(UINT16 op);
	void clr_ix(UINT16 op);
	void clr_ixd(UINT16 op);
	void com_rg(UINT16 op);
	void com_rgd(UINT16 op);
	void com_in(UINT16 op);
	void com_ind(UINT16 op);
	void com_de(UINT16 op);
	void com_ded(UINT16 op);
	void com_ix(UINT16 op);
	void com_ixd(UINT16 op);
	void inc_rg(UINT16 op);
	void inc_rgd(UINT16 op);
	void inc_in(UINT16 op);
	void inc_ind(UINT16 op);
	void inc_de(UINT16 op);
	void inc_ded(UINT16 op);
	void inc_ix(UINT16 op);
	void inc_ixd(UINT16 op);
	void dec_rg(UINT16 op);
	void dec_rgd(UINT16 op);
	void dec_in(UINT16 op);
	void dec_ind(UINT16 op);
	void dec_de(UINT16 op);
	void dec_ded(UINT16 op);
	void dec_ix(UINT16 op);
	void dec_ixd(UINT16 op);
	void neg_rg(UINT16 op);
	void neg_rgd(UINT16 op);
	void neg_in(UINT16 op);
	void neg_ind(UINT16 op);
	void neg_de(UINT16 op);
	void neg_ded(UINT16 op);
	void neg_ix(UINT16 op);
	void neg_ixd(UINT16 op);
	void adc_rg(UINT16 op);
	void adc_rgd(UINT16 op);
	void adc_in(UINT16 op);
	void adc_ind(UINT16 op);
	void adc_de(UINT16 op);
	void adc_ded(UINT16 op);
	void adc_ix(UINT16 op);
	void adc_ixd(UINT16 op);
	void sbc_rg(UINT16 op);
	void sbc_rgd(UINT16 op);
	void sbc_in(UINT16 op);
	void sbc_ind(UINT16 op);
	void sbc_de(UINT16 op);
	void sbc_ded(UINT16 op);
	void sbc_ix(UINT16 op);
	void sbc_ixd(UINT16 op);
	void tst_rg(UINT16 op);
	void tst_rgd(UINT16 op);
	void tst_in(UINT16 op);
	void tst_ind(UINT16 op);
	void tst_de(UINT16 op);
	void tst_ded(UINT16 op);
	void tst_ix(UINT16 op);
	void tst_ixd(UINT16 op);
	void ror_rg(UINT16 op);
	void ror_rgd(UINT16 op);
	void ror_in(UINT16 op);
	void ror_ind(UINT16 op);
	void ror_de(UINT16 op);
	void ror_ded(UINT16 op);
	void ror_ix(UINT16 op);
	void ror_ixd(UINT16 op);
	void rol_rg(UINT16 op);
	void rol_rgd(UINT16 op);
	void rol_in(UINT16 op);
	void rol_ind(UINT16 op);
	void rol_de(UINT16 op);
	void rol_ded(UINT16 op);
	void rol_ix(UINT16 op);
	void rol_ixd(UINT16 op);
	void asr_rg(UINT16 op);
	void asr_rgd(UINT16 op);
	void asr_in(UINT16 op);
	void asr_ind(UINT16 op);
	void asr_de(UINT16 op);
	void asr_ded(UINT16 op);
	void asr_ix(UINT16 op);
	void asr_ixd(UINT16 op);
	void asl_rg(UINT16 op);
	void asl_rgd(UINT16 op);
	void asl_in(UINT16 op);
	void asl_ind(UINT16 op);
	void asl_de(UINT16 op);
	void asl_ded(UINT16 op);
	void asl_ix(UINT16 op);
	void asl_ixd(UINT16 op);
	void mark(UINT16 op);
	void sxt_rg(UINT16 op);
	void sxt_rgd(UINT16 op);
	void sxt_in(UINT16 op);
	void sxt_ind(UINT16 op);
	void sxt_de(UINT16 op);
	void sxt_ded(UINT16 op);
	void sxt_ix(UINT16 op);
	void sxt_ixd(UINT16 op);
	void mov_rg_rg(UINT16 op);
	void mov_rg_rgd(UINT16 op);
	void mov_rg_in(UINT16 op);
	void mov_rg_ind(UINT16 op);
	void mov_rg_de(UINT16 op);
	void mov_rg_ded(UINT16 op);
	void mov_rg_ix(UINT16 op);
	void mov_rg_ixd(UINT16 op);
	void mov_rgd_rg(UINT16 op);
	void mov_rgd_rgd(UINT16 op);
	void mov_rgd_in(UINT16 op);
	void mov_rgd_ind(UINT16 op);
	void mov_rgd_de(UINT16 op);
	void mov_rgd_ded(UINT16 op);
	void mov_rgd_ix(UINT16 op);
	void mov_rgd_ixd(UINT16 op);
	void mov_in_rg(UINT16 op);
	void mov_in_rgd(UINT16 op);
	void mov_in_in(UINT16 op);
	void mov_in_ind(UINT16 op);
	void mov_in_de(UINT16 op);
	void mov_in_ded(UINT16 op);
	void mov_in_ix(UINT16 op);
	void mov_in_ixd(UINT16 op);
	void mov_ind_rg(UINT16 op);
	void mov_ind_rgd(UINT16 op);
	void mov_ind_in(UINT16 op);
	void mov_ind_ind(UINT16 op);
	void mov_ind_de(UINT16 op);
	void mov_ind_ded(UINT16 op);
	void mov_ind_ix(UINT16 op);
	void mov_ind_ixd(UINT16 op);
	void mov_de_rg(UINT16 op);
	void mov_de_rgd(UINT16 op);
	void mov_de_in(UINT16 op);
	void mov_de_ind(UINT16 op);
	void mov_de_de(UINT16 op);
	void mov_de_ded(UINT16 op);
	void mov_de_ix(UINT16 op);
	void mov_de_ixd(UINT16 op);
	void mov_ded_rg(UINT16 op);
	void mov_ded_rgd(UINT16 op);
	void mov_ded_in(UINT16 op);
	void mov_ded_ind(UINT16 op);
	void mov_ded_de(UINT16 op);
	void mov_ded_ded(UINT16 op);
	void mov_ded_ix(UINT16 op);
	void mov_ded_ixd(UINT16 op);
	void mov_ix_rg(UINT16 op);
	void mov_ix_rgd(UINT16 op);
	void mov_ix_in(UINT16 op);
	void mov_ix_ind(UINT16 op);
	void mov_ix_de(UINT16 op);
	void mov_ix_ded(UINT16 op);
	void mov_ix_ix(UINT16 op);
	void mov_ix_ixd(UINT16 op);
	void mov_ixd_rg(UINT16 op);
	void mov_ixd_rgd(UINT16 op);
	void mov_ixd_in(UINT16 op);
	void mov_ixd_ind(UINT16 op);
	void mov_ixd_de(UINT16 op);
	void mov_ixd_ded(UINT16 op);
	void mov_ixd_ix(UINT16 op);
	void mov_ixd_ixd(UINT16 op);
	void cmp_rg_rg(UINT16 op);
	void cmp_rg_rgd(UINT16 op);
	void cmp_rg_in(UINT16 op);
	void cmp_rg_ind(UINT16 op);
	void cmp_rg_de(UINT16 op);
	void cmp_rg_ded(UINT16 op);
	void cmp_rg_ix(UINT16 op);
	void cmp_rg_ixd(UINT16 op);
	void cmp_rgd_rg(UINT16 op);
	void cmp_rgd_rgd(UINT16 op);
	void cmp_rgd_in(UINT16 op);
	void cmp_rgd_ind(UINT16 op);
	void cmp_rgd_de(UINT16 op);
	void cmp_rgd_ded(UINT16 op);
	void cmp_rgd_ix(UINT16 op);
	void cmp_rgd_ixd(UINT16 op);
	void cmp_in_rg(UINT16 op);
	void cmp_in_rgd(UINT16 op);
	void cmp_in_in(UINT16 op);
	void cmp_in_ind(UINT16 op);
	void cmp_in_de(UINT16 op);
	void cmp_in_ded(UINT16 op);
	void cmp_in_ix(UINT16 op);
	void cmp_in_ixd(UINT16 op);
	void cmp_ind_rg(UINT16 op);
	void cmp_ind_rgd(UINT16 op);
	void cmp_ind_in(UINT16 op);
	void cmp_ind_ind(UINT16 op);
	void cmp_ind_de(UINT16 op);
	void cmp_ind_ded(UINT16 op);
	void cmp_ind_ix(UINT16 op);
	void cmp_ind_ixd(UINT16 op);
	void cmp_de_rg(UINT16 op);
	void cmp_de_rgd(UINT16 op);
	void cmp_de_in(UINT16 op);
	void cmp_de_ind(UINT16 op);
	void cmp_de_de(UINT16 op);
	void cmp_de_ded(UINT16 op);
	void cmp_de_ix(UINT16 op);
	void cmp_de_ixd(UINT16 op);
	void cmp_ded_rg(UINT16 op);
	void cmp_ded_rgd(UINT16 op);
	void cmp_ded_in(UINT16 op);
	void cmp_ded_ind(UINT16 op);
	void cmp_ded_de(UINT16 op);
	void cmp_ded_ded(UINT16 op);
	void cmp_ded_ix(UINT16 op);
	void cmp_ded_ixd(UINT16 op);
	void cmp_ix_rg(UINT16 op);
	void cmp_ix_rgd(UINT16 op);
	void cmp_ix_in(UINT16 op);
	void cmp_ix_ind(UINT16 op);
	void cmp_ix_de(UINT16 op);
	void cmp_ix_ded(UINT16 op);
	void cmp_ix_ix(UINT16 op);
	void cmp_ix_ixd(UINT16 op);
	void cmp_ixd_rg(UINT16 op);
	void cmp_ixd_rgd(UINT16 op);
	void cmp_ixd_in(UINT16 op);
	void cmp_ixd_ind(UINT16 op);
	void cmp_ixd_de(UINT16 op);
	void cmp_ixd_ded(UINT16 op);
	void cmp_ixd_ix(UINT16 op);
	void cmp_ixd_ixd(UINT16 op);
	void bit_rg_rg(UINT16 op);
	void bit_rg_rgd(UINT16 op);
	void bit_rg_in(UINT16 op);
	void bit_rg_ind(UINT16 op);
	void bit_rg_de(UINT16 op);
	void bit_rg_ded(UINT16 op);
	void bit_rg_ix(UINT16 op);
	void bit_rg_ixd(UINT16 op);
	void bit_rgd_rg(UINT16 op);
	void bit_rgd_rgd(UINT16 op);
	void bit_rgd_in(UINT16 op);
	void bit_rgd_ind(UINT16 op);
	void bit_rgd_de(UINT16 op);
	void bit_rgd_ded(UINT16 op);
	void bit_rgd_ix(UINT16 op);
	void bit_rgd_ixd(UINT16 op);
	void bit_in_rg(UINT16 op);
	void bit_in_rgd(UINT16 op);
	void bit_in_in(UINT16 op);
	void bit_in_ind(UINT16 op);
	void bit_in_de(UINT16 op);
	void bit_in_ded(UINT16 op);
	void bit_in_ix(UINT16 op);
	void bit_in_ixd(UINT16 op);
	void bit_ind_rg(UINT16 op);
	void bit_ind_rgd(UINT16 op);
	void bit_ind_in(UINT16 op);
	void bit_ind_ind(UINT16 op);
	void bit_ind_de(UINT16 op);
	void bit_ind_ded(UINT16 op);
	void bit_ind_ix(UINT16 op);
	void bit_ind_ixd(UINT16 op);
	void bit_de_rg(UINT16 op);
	void bit_de_rgd(UINT16 op);
	void bit_de_in(UINT16 op);
	void bit_de_ind(UINT16 op);
	void bit_de_de(UINT16 op);
	void bit_de_ded(UINT16 op);
	void bit_de_ix(UINT16 op);
	void bit_de_ixd(UINT16 op);
	void bit_ded_rg(UINT16 op);
	void bit_ded_rgd(UINT16 op);
	void bit_ded_in(UINT16 op);
	void bit_ded_ind(UINT16 op);
	void bit_ded_de(UINT16 op);
	void bit_ded_ded(UINT16 op);
	void bit_ded_ix(UINT16 op);
	void bit_ded_ixd(UINT16 op);
	void bit_ix_rg(UINT16 op);
	void bit_ix_rgd(UINT16 op);
	void bit_ix_in(UINT16 op);
	void bit_ix_ind(UINT16 op);
	void bit_ix_de(UINT16 op);
	void bit_ix_ded(UINT16 op);
	void bit_ix_ix(UINT16 op);
	void bit_ix_ixd(UINT16 op);
	void bit_ixd_rg(UINT16 op);
	void bit_ixd_rgd(UINT16 op);
	void bit_ixd_in(UINT16 op);
	void bit_ixd_ind(UINT16 op);
	void bit_ixd_de(UINT16 op);
	void bit_ixd_ded(UINT16 op);
	void bit_ixd_ix(UINT16 op);
	void bit_ixd_ixd(UINT16 op);
	void bic_rg_rg(UINT16 op);
	void bic_rg_rgd(UINT16 op);
	void bic_rg_in(UINT16 op);
	void bic_rg_ind(UINT16 op);
	void bic_rg_de(UINT16 op);
	void bic_rg_ded(UINT16 op);
	void bic_rg_ix(UINT16 op);
	void bic_rg_ixd(UINT16 op);
	void bic_rgd_rg(UINT16 op);
	void bic_rgd_rgd(UINT16 op);
	void bic_rgd_in(UINT16 op);
	void bic_rgd_ind(UINT16 op);
	void bic_rgd_de(UINT16 op);
	void bic_rgd_ded(UINT16 op);
	void bic_rgd_ix(UINT16 op);
	void bic_rgd_ixd(UINT16 op);
	void bic_in_rg(UINT16 op);
	void bic_in_rgd(UINT16 op);
	void bic_in_in(UINT16 op);
	void bic_in_ind(UINT16 op);
	void bic_in_de(UINT16 op);
	void bic_in_ded(UINT16 op);
	void bic_in_ix(UINT16 op);
	void bic_in_ixd(UINT16 op);
	void bic_ind_rg(UINT16 op);
	void bic_ind_rgd(UINT16 op);
	void bic_ind_in(UINT16 op);
	void bic_ind_ind(UINT16 op);
	void bic_ind_de(UINT16 op);
	void bic_ind_ded(UINT16 op);
	void bic_ind_ix(UINT16 op);
	void bic_ind_ixd(UINT16 op);
	void bic_de_rg(UINT16 op);
	void bic_de_rgd(UINT16 op);
	void bic_de_in(UINT16 op);
	void bic_de_ind(UINT16 op);
	void bic_de_de(UINT16 op);
	void bic_de_ded(UINT16 op);
	void bic_de_ix(UINT16 op);
	void bic_de_ixd(UINT16 op);
	void bic_ded_rg(UINT16 op);
	void bic_ded_rgd(UINT16 op);
	void bic_ded_in(UINT16 op);
	void bic_ded_ind(UINT16 op);
	void bic_ded_de(UINT16 op);
	void bic_ded_ded(UINT16 op);
	void bic_ded_ix(UINT16 op);
	void bic_ded_ixd(UINT16 op);
	void bic_ix_rg(UINT16 op);
	void bic_ix_rgd(UINT16 op);
	void bic_ix_in(UINT16 op);
	void bic_ix_ind(UINT16 op);
	void bic_ix_de(UINT16 op);
	void bic_ix_ded(UINT16 op);
	void bic_ix_ix(UINT16 op);
	void bic_ix_ixd(UINT16 op);
	void bic_ixd_rg(UINT16 op);
	void bic_ixd_rgd(UINT16 op);
	void bic_ixd_in(UINT16 op);
	void bic_ixd_ind(UINT16 op);
	void bic_ixd_de(UINT16 op);
	void bic_ixd_ded(UINT16 op);
	void bic_ixd_ix(UINT16 op);
	void bic_ixd_ixd(UINT16 op);
	void bis_rg_rg(UINT16 op);
	void bis_rg_rgd(UINT16 op);
	void bis_rg_in(UINT16 op);
	void bis_rg_ind(UINT16 op);
	void bis_rg_de(UINT16 op);
	void bis_rg_ded(UINT16 op);
	void bis_rg_ix(UINT16 op);
	void bis_rg_ixd(UINT16 op);
	void bis_rgd_rg(UINT16 op);
	void bis_rgd_rgd(UINT16 op);
	void bis_rgd_in(UINT16 op);
	void bis_rgd_ind(UINT16 op);
	void bis_rgd_de(UINT16 op);
	void bis_rgd_ded(UINT16 op);
	void bis_rgd_ix(UINT16 op);
	void bis_rgd_ixd(UINT16 op);
	void bis_in_rg(UINT16 op);
	void bis_in_rgd(UINT16 op);
	void bis_in_in(UINT16 op);
	void bis_in_ind(UINT16 op);
	void bis_in_de(UINT16 op);
	void bis_in_ded(UINT16 op);
	void bis_in_ix(UINT16 op);
	void bis_in_ixd(UINT16 op);
	void bis_ind_rg(UINT16 op);
	void bis_ind_rgd(UINT16 op);
	void bis_ind_in(UINT16 op);
	void bis_ind_ind(UINT16 op);
	void bis_ind_de(UINT16 op);
	void bis_ind_ded(UINT16 op);
	void bis_ind_ix(UINT16 op);
	void bis_ind_ixd(UINT16 op);
	void bis_de_rg(UINT16 op);
	void bis_de_rgd(UINT16 op);
	void bis_de_in(UINT16 op);
	void bis_de_ind(UINT16 op);
	void bis_de_de(UINT16 op);
	void bis_de_ded(UINT16 op);
	void bis_de_ix(UINT16 op);
	void bis_de_ixd(UINT16 op);
	void bis_ded_rg(UINT16 op);
	void bis_ded_rgd(UINT16 op);
	void bis_ded_in(UINT16 op);
	void bis_ded_ind(UINT16 op);
	void bis_ded_de(UINT16 op);
	void bis_ded_ded(UINT16 op);
	void bis_ded_ix(UINT16 op);
	void bis_ded_ixd(UINT16 op);
	void bis_ix_rg(UINT16 op);
	void bis_ix_rgd(UINT16 op);
	void bis_ix_in(UINT16 op);
	void bis_ix_ind(UINT16 op);
	void bis_ix_de(UINT16 op);
	void bis_ix_ded(UINT16 op);
	void bis_ix_ix(UINT16 op);
	void bis_ix_ixd(UINT16 op);
	void bis_ixd_rg(UINT16 op);
	void bis_ixd_rgd(UINT16 op);
	void bis_ixd_in(UINT16 op);
	void bis_ixd_ind(UINT16 op);
	void bis_ixd_de(UINT16 op);
	void bis_ixd_ded(UINT16 op);
	void bis_ixd_ix(UINT16 op);
	void bis_ixd_ixd(UINT16 op);
	void add_rg_rg(UINT16 op);
	void add_rg_rgd(UINT16 op);
	void add_rg_in(UINT16 op);
	void add_rg_ind(UINT16 op);
	void add_rg_de(UINT16 op);
	void add_rg_ded(UINT16 op);
	void add_rg_ix(UINT16 op);
	void add_rg_ixd(UINT16 op);
	void add_rgd_rg(UINT16 op);
	void add_rgd_rgd(UINT16 op);
	void add_rgd_in(UINT16 op);
	void add_rgd_ind(UINT16 op);
	void add_rgd_de(UINT16 op);
	void add_rgd_ded(UINT16 op);
	void add_rgd_ix(UINT16 op);
	void add_rgd_ixd(UINT16 op);
	void add_in_rg(UINT16 op);
	void add_in_rgd(UINT16 op);
	void add_in_in(UINT16 op);
	void add_in_ind(UINT16 op);
	void add_in_de(UINT16 op);
	void add_in_ded(UINT16 op);
	void add_in_ix(UINT16 op);
	void add_in_ixd(UINT16 op);
	void add_ind_rg(UINT16 op);
	void add_ind_rgd(UINT16 op);
	void add_ind_in(UINT16 op);
	void add_ind_ind(UINT16 op);
	void add_ind_de(UINT16 op);
	void add_ind_ded(UINT16 op);
	void add_ind_ix(UINT16 op);
	void add_ind_ixd(UINT16 op);
	void add_de_rg(UINT16 op);
	void add_de_rgd(UINT16 op);
	void add_de_in(UINT16 op);
	void add_de_ind(UINT16 op);
	void add_de_de(UINT16 op);
	void add_de_ded(UINT16 op);
	void add_de_ix(UINT16 op);
	void add_de_ixd(UINT16 op);
	void add_ded_rg(UINT16 op);
	void add_ded_rgd(UINT16 op);
	void add_ded_in(UINT16 op);
	void add_ded_ind(UINT16 op);
	void add_ded_de(UINT16 op);
	void add_ded_ded(UINT16 op);
	void add_ded_ix(UINT16 op);
	void add_ded_ixd(UINT16 op);
	void add_ix_rg(UINT16 op);
	void add_ix_rgd(UINT16 op);
	void add_ix_in(UINT16 op);
	void add_ix_ind(UINT16 op);
	void add_ix_de(UINT16 op);
	void add_ix_ded(UINT16 op);
	void add_ix_ix(UINT16 op);
	void add_ix_ixd(UINT16 op);
	void add_ixd_rg(UINT16 op);
	void add_ixd_rgd(UINT16 op);
	void add_ixd_in(UINT16 op);
	void add_ixd_ind(UINT16 op);
	void add_ixd_de(UINT16 op);
	void add_ixd_ded(UINT16 op);
	void add_ixd_ix(UINT16 op);
	void add_ixd_ixd(UINT16 op);
	void xor_rg(UINT16 op);
	void xor_rgd(UINT16 op);
	void xor_in(UINT16 op);
	void xor_ind(UINT16 op);
	void xor_de(UINT16 op);
	void xor_ded(UINT16 op);
	void xor_ix(UINT16 op);
	void xor_ixd(UINT16 op);
	void sob(UINT16 op);
	void bpl(UINT16 op);
	void bmi(UINT16 op);
	void bhi(UINT16 op);
	void blos(UINT16 op);
	void bvc(UINT16 op);
	void bvs(UINT16 op);
	void bcc(UINT16 op);
	void bcs(UINT16 op);
	void emt(UINT16 op);
	void trap(UINT16 op);
	void clrb_rg(UINT16 op);
	void clrb_rgd(UINT16 op);
	void clrb_in(UINT16 op);
	void clrb_ind(UINT16 op);
	void clrb_de(UINT16 op);
	void clrb_ded(UINT16 op);
	void clrb_ix(UINT16 op);
	void clrb_ixd(UINT16 op);
	void comb_rg(UINT16 op);
	void comb_rgd(UINT16 op);
	void comb_in(UINT16 op);
	void comb_ind(UINT16 op);
	void comb_de(UINT16 op);
	void comb_ded(UINT16 op);
	void comb_ix(UINT16 op);
	void comb_ixd(UINT16 op);
	void incb_rg(UINT16 op);
	void incb_rgd(UINT16 op);
	void incb_in(UINT16 op);
	void incb_ind(UINT16 op);
	void incb_de(UINT16 op);
	void incb_ded(UINT16 op);
	void incb_ix(UINT16 op);
	void incb_ixd(UINT16 op);
	void decb_rg(UINT16 op);
	void decb_rgd(UINT16 op);
	void decb_in(UINT16 op);
	void decb_ind(UINT16 op);
	void decb_de(UINT16 op);
	void decb_ded(UINT16 op);
	void decb_ix(UINT16 op);
	void decb_ixd(UINT16 op);
	void negb_rg(UINT16 op);
	void negb_rgd(UINT16 op);
	void negb_in(UINT16 op);
	void negb_ind(UINT16 op);
	void negb_de(UINT16 op);
	void negb_ded(UINT16 op);
	void negb_ix(UINT16 op);
	void negb_ixd(UINT16 op);
	void adcb_rg(UINT16 op);
	void adcb_rgd(UINT16 op);
	void adcb_in(UINT16 op);
	void adcb_ind(UINT16 op);
	void adcb_de(UINT16 op);
	void adcb_ded(UINT16 op);
	void adcb_ix(UINT16 op);
	void adcb_ixd(UINT16 op);
	void sbcb_rg(UINT16 op);
	void sbcb_rgd(UINT16 op);
	void sbcb_in(UINT16 op);
	void sbcb_ind(UINT16 op);
	void sbcb_de(UINT16 op);
	void sbcb_ded(UINT16 op);
	void sbcb_ix(UINT16 op);
	void sbcb_ixd(UINT16 op);
	void tstb_rg(UINT16 op);
	void tstb_rgd(UINT16 op);
	void tstb_in(UINT16 op);
	void tstb_ind(UINT16 op);
	void tstb_de(UINT16 op);
	void tstb_ded(UINT16 op);
	void tstb_ix(UINT16 op);
	void tstb_ixd(UINT16 op);
	void rorb_rg(UINT16 op);
	void rorb_rgd(UINT16 op);
	void rorb_in(UINT16 op);
	void rorb_ind(UINT16 op);
	void rorb_de(UINT16 op);
	void rorb_ded(UINT16 op);
	void rorb_ix(UINT16 op);
	void rorb_ixd(UINT16 op);
	void rolb_rg(UINT16 op);
	void rolb_rgd(UINT16 op);
	void rolb_in(UINT16 op);
	void rolb_ind(UINT16 op);
	void rolb_de(UINT16 op);
	void rolb_ded(UINT16 op);
	void rolb_ix(UINT16 op);
	void rolb_ixd(UINT16 op);
	void asrb_rg(UINT16 op);
	void asrb_rgd(UINT16 op);
	void asrb_in(UINT16 op);
	void asrb_ind(UINT16 op);
	void asrb_de(UINT16 op);
	void asrb_ded(UINT16 op);
	void asrb_ix(UINT16 op);
	void asrb_ixd(UINT16 op);
	void aslb_rg(UINT16 op);
	void aslb_rgd(UINT16 op);
	void aslb_in(UINT16 op);
	void aslb_ind(UINT16 op);
	void aslb_de(UINT16 op);
	void aslb_ded(UINT16 op);
	void aslb_ix(UINT16 op);
	void aslb_ixd(UINT16 op);
	void mtps_rg(UINT16 op);
	void mtps_rgd(UINT16 op);
	void mtps_in(UINT16 op);
	void mtps_ind(UINT16 op);
	void mtps_de(UINT16 op);
	void mtps_ded(UINT16 op);
	void mtps_ix(UINT16 op);
	void mtps_ixd(UINT16 op);
	void mfps_rg(UINT16 op);
	void mfps_rgd(UINT16 op);
	void mfps_in(UINT16 op);
	void mfps_ind(UINT16 op);
	void mfps_de(UINT16 op);
	void mfps_ded(UINT16 op);
	void mfps_ix(UINT16 op);
	void mfps_ixd(UINT16 op);
	void movb_rg_rg(UINT16 op);
	void movb_rg_rgd(UINT16 op);
	void movb_rg_in(UINT16 op);
	void movb_rg_ind(UINT16 op);
	void movb_rg_de(UINT16 op);
	void movb_rg_ded(UINT16 op);
	void movb_rg_ix(UINT16 op);
	void movb_rg_ixd(UINT16 op);
	void movb_rgd_rg(UINT16 op);
	void movb_rgd_rgd(UINT16 op);
	void movb_rgd_in(UINT16 op);
	void movb_rgd_ind(UINT16 op);
	void movb_rgd_de(UINT16 op);
	void movb_rgd_ded(UINT16 op);
	void movb_rgd_ix(UINT16 op);
	void movb_rgd_ixd(UINT16 op);
	void movb_in_rg(UINT16 op);
	void movb_in_rgd(UINT16 op);
	void movb_in_in(UINT16 op);
	void movb_in_ind(UINT16 op);
	void movb_in_de(UINT16 op);
	void movb_in_ded(UINT16 op);
	void movb_in_ix(UINT16 op);
	void movb_in_ixd(UINT16 op);
	void movb_ind_rg(UINT16 op);
	void movb_ind_rgd(UINT16 op);
	void movb_ind_in(UINT16 op);
	void movb_ind_ind(UINT16 op);
	void movb_ind_de(UINT16 op);
	void movb_ind_ded(UINT16 op);
	void movb_ind_ix(UINT16 op);
	void movb_ind_ixd(UINT16 op);
	void movb_de_rg(UINT16 op);
	void movb_de_rgd(UINT16 op);
	void movb_de_in(UINT16 op);
	void movb_de_ind(UINT16 op);
	void movb_de_de(UINT16 op);
	void movb_de_ded(UINT16 op);
	void movb_de_ix(UINT16 op);
	void movb_de_ixd(UINT16 op);
	void movb_ded_rg(UINT16 op);
	void movb_ded_rgd(UINT16 op);
	void movb_ded_in(UINT16 op);
	void movb_ded_ind(UINT16 op);
	void movb_ded_de(UINT16 op);
	void movb_ded_ded(UINT16 op);
	void movb_ded_ix(UINT16 op);
	void movb_ded_ixd(UINT16 op);
	void movb_ix_rg(UINT16 op);
	void movb_ix_rgd(UINT16 op);
	void movb_ix_in(UINT16 op);
	void movb_ix_ind(UINT16 op);
	void movb_ix_de(UINT16 op);
	void movb_ix_ded(UINT16 op);
	void movb_ix_ix(UINT16 op);
	void movb_ix_ixd(UINT16 op);
	void movb_ixd_rg(UINT16 op);
	void movb_ixd_rgd(UINT16 op);
	void movb_ixd_in(UINT16 op);
	void movb_ixd_ind(UINT16 op);
	void movb_ixd_de(UINT16 op);
	void movb_ixd_ded(UINT16 op);
	void movb_ixd_ix(UINT16 op);
	void movb_ixd_ixd(UINT16 op);
	void cmpb_rg_rg(UINT16 op);
	void cmpb_rg_rgd(UINT16 op);
	void cmpb_rg_in(UINT16 op);
	void cmpb_rg_ind(UINT16 op);
	void cmpb_rg_de(UINT16 op);
	void cmpb_rg_ded(UINT16 op);
	void cmpb_rg_ix(UINT16 op);
	void cmpb_rg_ixd(UINT16 op);
	void cmpb_rgd_rg(UINT16 op);
	void cmpb_rgd_rgd(UINT16 op);
	void cmpb_rgd_in(UINT16 op);
	void cmpb_rgd_ind(UINT16 op);
	void cmpb_rgd_de(UINT16 op);
	void cmpb_rgd_ded(UINT16 op);
	void cmpb_rgd_ix(UINT16 op);
	void cmpb_rgd_ixd(UINT16 op);
	void cmpb_in_rg(UINT16 op);
	void cmpb_in_rgd(UINT16 op);
	void cmpb_in_in(UINT16 op);
	void cmpb_in_ind(UINT16 op);
	void cmpb_in_de(UINT16 op);
	void cmpb_in_ded(UINT16 op);
	void cmpb_in_ix(UINT16 op);
	void cmpb_in_ixd(UINT16 op);
	void cmpb_ind_rg(UINT16 op);
	void cmpb_ind_rgd(UINT16 op);
	void cmpb_ind_in(UINT16 op);
	void cmpb_ind_ind(UINT16 op);
	void cmpb_ind_de(UINT16 op);
	void cmpb_ind_ded(UINT16 op);
	void cmpb_ind_ix(UINT16 op);
	void cmpb_ind_ixd(UINT16 op);
	void cmpb_de_rg(UINT16 op);
	void cmpb_de_rgd(UINT16 op);
	void cmpb_de_in(UINT16 op);
	void cmpb_de_ind(UINT16 op);
	void cmpb_de_de(UINT16 op);
	void cmpb_de_ded(UINT16 op);
	void cmpb_de_ix(UINT16 op);
	void cmpb_de_ixd(UINT16 op);
	void cmpb_ded_rg(UINT16 op);
	void cmpb_ded_rgd(UINT16 op);
	void cmpb_ded_in(UINT16 op);
	void cmpb_ded_ind(UINT16 op);
	void cmpb_ded_de(UINT16 op);
	void cmpb_ded_ded(UINT16 op);
	void cmpb_ded_ix(UINT16 op);
	void cmpb_ded_ixd(UINT16 op);
	void cmpb_ix_rg(UINT16 op);
	void cmpb_ix_rgd(UINT16 op);
	void cmpb_ix_in(UINT16 op);
	void cmpb_ix_ind(UINT16 op);
	void cmpb_ix_de(UINT16 op);
	void cmpb_ix_ded(UINT16 op);
	void cmpb_ix_ix(UINT16 op);
	void cmpb_ix_ixd(UINT16 op);
	void cmpb_ixd_rg(UINT16 op);
	void cmpb_ixd_rgd(UINT16 op);
	void cmpb_ixd_in(UINT16 op);
	void cmpb_ixd_ind(UINT16 op);
	void cmpb_ixd_de(UINT16 op);
	void cmpb_ixd_ded(UINT16 op);
	void cmpb_ixd_ix(UINT16 op);
	void cmpb_ixd_ixd(UINT16 op);
	void bitb_rg_rg(UINT16 op);
	void bitb_rg_rgd(UINT16 op);
	void bitb_rg_in(UINT16 op);
	void bitb_rg_ind(UINT16 op);
	void bitb_rg_de(UINT16 op);
	void bitb_rg_ded(UINT16 op);
	void bitb_rg_ix(UINT16 op);
	void bitb_rg_ixd(UINT16 op);
	void bitb_rgd_rg(UINT16 op);
	void bitb_rgd_rgd(UINT16 op);
	void bitb_rgd_in(UINT16 op);
	void bitb_rgd_ind(UINT16 op);
	void bitb_rgd_de(UINT16 op);
	void bitb_rgd_ded(UINT16 op);
	void bitb_rgd_ix(UINT16 op);
	void bitb_rgd_ixd(UINT16 op);
	void bitb_in_rg(UINT16 op);
	void bitb_in_rgd(UINT16 op);
	void bitb_in_in(UINT16 op);
	void bitb_in_ind(UINT16 op);
	void bitb_in_de(UINT16 op);
	void bitb_in_ded(UINT16 op);
	void bitb_in_ix(UINT16 op);
	void bitb_in_ixd(UINT16 op);
	void bitb_ind_rg(UINT16 op);
	void bitb_ind_rgd(UINT16 op);
	void bitb_ind_in(UINT16 op);
	void bitb_ind_ind(UINT16 op);
	void bitb_ind_de(UINT16 op);
	void bitb_ind_ded(UINT16 op);
	void bitb_ind_ix(UINT16 op);
	void bitb_ind_ixd(UINT16 op);
	void bitb_de_rg(UINT16 op);
	void bitb_de_rgd(UINT16 op);
	void bitb_de_in(UINT16 op);
	void bitb_de_ind(UINT16 op);
	void bitb_de_de(UINT16 op);
	void bitb_de_ded(UINT16 op);
	void bitb_de_ix(UINT16 op);
	void bitb_de_ixd(UINT16 op);
	void bitb_ded_rg(UINT16 op);
	void bitb_ded_rgd(UINT16 op);
	void bitb_ded_in(UINT16 op);
	void bitb_ded_ind(UINT16 op);
	void bitb_ded_de(UINT16 op);
	void bitb_ded_ded(UINT16 op);
	void bitb_ded_ix(UINT16 op);
	void bitb_ded_ixd(UINT16 op);
	void bitb_ix_rg(UINT16 op);
	void bitb_ix_rgd(UINT16 op);
	void bitb_ix_in(UINT16 op);
	void bitb_ix_ind(UINT16 op);
	void bitb_ix_de(UINT16 op);
	void bitb_ix_ded(UINT16 op);
	void bitb_ix_ix(UINT16 op);
	void bitb_ix_ixd(UINT16 op);
	void bitb_ixd_rg(UINT16 op);
	void bitb_ixd_rgd(UINT16 op);
	void bitb_ixd_in(UINT16 op);
	void bitb_ixd_ind(UINT16 op);
	void bitb_ixd_de(UINT16 op);
	void bitb_ixd_ded(UINT16 op);
	void bitb_ixd_ix(UINT16 op);
	void bitb_ixd_ixd(UINT16 op);
	void bicb_rg_rg(UINT16 op);
	void bicb_rg_rgd(UINT16 op);
	void bicb_rg_in(UINT16 op);
	void bicb_rg_ind(UINT16 op);
	void bicb_rg_de(UINT16 op);
	void bicb_rg_ded(UINT16 op);
	void bicb_rg_ix(UINT16 op);
	void bicb_rg_ixd(UINT16 op);
	void bicb_rgd_rg(UINT16 op);
	void bicb_rgd_rgd(UINT16 op);
	void bicb_rgd_in(UINT16 op);
	void bicb_rgd_ind(UINT16 op);
	void bicb_rgd_de(UINT16 op);
	void bicb_rgd_ded(UINT16 op);
	void bicb_rgd_ix(UINT16 op);
	void bicb_rgd_ixd(UINT16 op);
	void bicb_in_rg(UINT16 op);
	void bicb_in_rgd(UINT16 op);
	void bicb_in_in(UINT16 op);
	void bicb_in_ind(UINT16 op);
	void bicb_in_de(UINT16 op);
	void bicb_in_ded(UINT16 op);
	void bicb_in_ix(UINT16 op);
	void bicb_in_ixd(UINT16 op);
	void bicb_ind_rg(UINT16 op);
	void bicb_ind_rgd(UINT16 op);
	void bicb_ind_in(UINT16 op);
	void bicb_ind_ind(UINT16 op);
	void bicb_ind_de(UINT16 op);
	void bicb_ind_ded(UINT16 op);
	void bicb_ind_ix(UINT16 op);
	void bicb_ind_ixd(UINT16 op);
	void bicb_de_rg(UINT16 op);
	void bicb_de_rgd(UINT16 op);
	void bicb_de_in(UINT16 op);
	void bicb_de_ind(UINT16 op);
	void bicb_de_de(UINT16 op);
	void bicb_de_ded(UINT16 op);
	void bicb_de_ix(UINT16 op);
	void bicb_de_ixd(UINT16 op);
	void bicb_ded_rg(UINT16 op);
	void bicb_ded_rgd(UINT16 op);
	void bicb_ded_in(UINT16 op);
	void bicb_ded_ind(UINT16 op);
	void bicb_ded_de(UINT16 op);
	void bicb_ded_ded(UINT16 op);
	void bicb_ded_ix(UINT16 op);
	void bicb_ded_ixd(UINT16 op);
	void bicb_ix_rg(UINT16 op);
	void bicb_ix_rgd(UINT16 op);
	void bicb_ix_in(UINT16 op);
	void bicb_ix_ind(UINT16 op);
	void bicb_ix_de(UINT16 op);
	void bicb_ix_ded(UINT16 op);
	void bicb_ix_ix(UINT16 op);
	void bicb_ix_ixd(UINT16 op);
	void bicb_ixd_rg(UINT16 op);
	void bicb_ixd_rgd(UINT16 op);
	void bicb_ixd_in(UINT16 op);
	void bicb_ixd_ind(UINT16 op);
	void bicb_ixd_de(UINT16 op);
	void bicb_ixd_ded(UINT16 op);
	void bicb_ixd_ix(UINT16 op);
	void bicb_ixd_ixd(UINT16 op);
	void bisb_rg_rg(UINT16 op);
	void bisb_rg_rgd(UINT16 op);
	void bisb_rg_in(UINT16 op);
	void bisb_rg_ind(UINT16 op);
	void bisb_rg_de(UINT16 op);
	void bisb_rg_ded(UINT16 op);
	void bisb_rg_ix(UINT16 op);
	void bisb_rg_ixd(UINT16 op);
	void bisb_rgd_rg(UINT16 op);
	void bisb_rgd_rgd(UINT16 op);
	void bisb_rgd_in(UINT16 op);
	void bisb_rgd_ind(UINT16 op);
	void bisb_rgd_de(UINT16 op);
	void bisb_rgd_ded(UINT16 op);
	void bisb_rgd_ix(UINT16 op);
	void bisb_rgd_ixd(UINT16 op);
	void bisb_in_rg(UINT16 op);
	void bisb_in_rgd(UINT16 op);
	void bisb_in_in(UINT16 op);
	void bisb_in_ind(UINT16 op);
	void bisb_in_de(UINT16 op);
	void bisb_in_ded(UINT16 op);
	void bisb_in_ix(UINT16 op);
	void bisb_in_ixd(UINT16 op);
	void bisb_ind_rg(UINT16 op);
	void bisb_ind_rgd(UINT16 op);
	void bisb_ind_in(UINT16 op);
	void bisb_ind_ind(UINT16 op);
	void bisb_ind_de(UINT16 op);
	void bisb_ind_ded(UINT16 op);
	void bisb_ind_ix(UINT16 op);
	void bisb_ind_ixd(UINT16 op);
	void bisb_de_rg(UINT16 op);
	void bisb_de_rgd(UINT16 op);
	void bisb_de_in(UINT16 op);
	void bisb_de_ind(UINT16 op);
	void bisb_de_de(UINT16 op);
	void bisb_de_ded(UINT16 op);
	void bisb_de_ix(UINT16 op);
	void bisb_de_ixd(UINT16 op);
	void bisb_ded_rg(UINT16 op);
	void bisb_ded_rgd(UINT16 op);
	void bisb_ded_in(UINT16 op);
	void bisb_ded_ind(UINT16 op);
	void bisb_ded_de(UINT16 op);
	void bisb_ded_ded(UINT16 op);
	void bisb_ded_ix(UINT16 op);
	void bisb_ded_ixd(UINT16 op);
	void bisb_ix_rg(UINT16 op);
	void bisb_ix_rgd(UINT16 op);
	void bisb_ix_in(UINT16 op);
	void bisb_ix_ind(UINT16 op);
	void bisb_ix_de(UINT16 op);
	void bisb_ix_ded(UINT16 op);
	void bisb_ix_ix(UINT16 op);
	void bisb_ix_ixd(UINT16 op);
	void bisb_ixd_rg(UINT16 op);
	void bisb_ixd_rgd(UINT16 op);
	void bisb_ixd_in(UINT16 op);
	void bisb_ixd_ind(UINT16 op);
	void bisb_ixd_de(UINT16 op);
	void bisb_ixd_ded(UINT16 op);
	void bisb_ixd_ix(UINT16 op);
	void bisb_ixd_ixd(UINT16 op);
	void sub_rg_rg(UINT16 op);
	void sub_rg_rgd(UINT16 op);
	void sub_rg_in(UINT16 op);
	void sub_rg_ind(UINT16 op);
	void sub_rg_de(UINT16 op);
	void sub_rg_ded(UINT16 op);
	void sub_rg_ix(UINT16 op);
	void sub_rg_ixd(UINT16 op);
	void sub_rgd_rg(UINT16 op);
	void sub_rgd_rgd(UINT16 op);
	void sub_rgd_in(UINT16 op);
	void sub_rgd_ind(UINT16 op);
	void sub_rgd_de(UINT16 op);
	void sub_rgd_ded(UINT16 op);
	void sub_rgd_ix(UINT16 op);
	void sub_rgd_ixd(UINT16 op);
	void sub_in_rg(UINT16 op);
	void sub_in_rgd(UINT16 op);
	void sub_in_in(UINT16 op);
	void sub_in_ind(UINT16 op);
	void sub_in_de(UINT16 op);
	void sub_in_ded(UINT16 op);
	void sub_in_ix(UINT16 op);
	void sub_in_ixd(UINT16 op);
	void sub_ind_rg(UINT16 op);
	void sub_ind_rgd(UINT16 op);
	void sub_ind_in(UINT16 op);
	void sub_ind_ind(UINT16 op);
	void sub_ind_de(UINT16 op);
	void sub_ind_ded(UINT16 op);
	void sub_ind_ix(UINT16 op);
	void sub_ind_ixd(UINT16 op);
	void sub_de_rg(UINT16 op);
	void sub_de_rgd(UINT16 op);
	void sub_de_in(UINT16 op);
	void sub_de_ind(UINT16 op);
	void sub_de_de(UINT16 op);
	void sub_de_ded(UINT16 op);
	void sub_de_ix(UINT16 op);
	void sub_de_ixd(UINT16 op);
	void sub_ded_rg(UINT16 op);
	void sub_ded_rgd(UINT16 op);
	void sub_ded_in(UINT16 op);
	void sub_ded_ind(UINT16 op);
	void sub_ded_de(UINT16 op);
	void sub_ded_ded(UINT16 op);
	void sub_ded_ix(UINT16 op);
	void sub_ded_ixd(UINT16 op);
	void sub_ix_rg(UINT16 op);
	void sub_ix_rgd(UINT16 op);
	void sub_ix_in(UINT16 op);
	void sub_ix_ind(UINT16 op);
	void sub_ix_de(UINT16 op);
	void sub_ix_ded(UINT16 op);
	void sub_ix_ix(UINT16 op);
	void sub_ix_ixd(UINT16 op);
	void sub_ixd_rg(UINT16 op);
	void sub_ixd_rgd(UINT16 op);
	void sub_ixd_in(UINT16 op);
	void sub_ixd_ind(UINT16 op);
	void sub_ixd_de(UINT16 op);
	void sub_ixd_ded(UINT16 op);
	void sub_ixd_ix(UINT16 op);
	void sub_ixd_ixd(UINT16 op);
};


extern const device_type T11;


#endif /* __T11_H__ */