// license:BSD-3-Clause
// copyright-holders:Joakim Larsson Edstr??m
/***************************************************************************

    Force SYS68K CPU-1/CPU-6 VME SBC drivers, initially based on the 68ksbc.c

    13/06/2015

 The info found on the links below is for a later revisions of the board I have
 but I hope it is somewhat compatible so I can get it up and running at least.
 My CPU-1 board has proms from 1983 and no rev markings so probably the original.

 http://bitsavers.trailing-edge.com/pdf/forceComputers/1988_Force_VMEbus_Products.pdf
 http://www.artisantg.com/info/P_wUovN.pdf

 Some info from those documents:

Address Map
----------------------------------------------------------
Address Range     Description
----------------------------------------------------------
000 000 - 000 007 Initialisation vectors from system EPROM
000 008 - 01F FFF Dynamic RAM on CPU-1 B
000 008 - 07F FFF Dynamic RAM on CPU-1 D
080 008 - 09F FFF SYSTEM EPROM Area
OAO 000 - OBF FFF USER EPROMArea
0C0 041 - 0C0 043 ACIA (P3) Host
0C0 080 - 0C0 082 ACIA (P4) Terminal
0C0 101 - 0C0 103 ACIA (P5) Remote device (eg serial printer)
0C0 401 - 0C0 42F RTC
OEO 001 - 0E0 035 PI/T (eg centronics printer)
OEO 200 - 0E0 2FF FPU
OEO 300 - 0E0 300 Reset Off
OEO 380 - 0E0 380 Reset On
100 000 - FEF FFF VMEbus addresses (A24)
FFO 000 - FFF FFF VMEbus Short I/O (A16)
----------------------------------------------------------

Interrupt sources
----------------------------------------------------------
Description                  Device  Lvl  IRQ    VME board
                             /Board      Vector  Address
----------------------------------------------------------
On board Sources
 ABORT                        Switch  7    31
 Real Time Clock (RTC)        58167A  6    30
 Parallel/Timer (PI/T)        68230   5    29
 Terminal ACIA                6850    4    28
 Remote ACIA                  6850    3    27
 Host ACIA                    6850    2    26
 ACFAIL, SYSFAIL              VME     5    29
Off board Sources (other VME boards)
 6 Port Serial I/O board      SIO     4    64-75  0xb00000
 8 Port Serial I/O board      ISIO    4    76-83  0x960000
 Disk Controller              WFC     3    119    0xb01000
 SCSI Controller              ISCSI   4    119    0xa00000
 Slot 1 Controller Board      ASCU    7    31     0xb02000
----------------------------------------------------------

10. The VMEbus
---------------
The implemented VMEbus Interface includes 24 address, 16 data,
6 address modifier and the asynchronous control signals.
A single level bus arbiter is provided to build multi master
systems. In addition to the bus arbiter, a separate slave bus
arbitration allows selection of the arbitration level (0-3).

The address modifier range .,Short 110 Access?? can be selected
via a jumper for variable system generation. The 7 interrupt
request levels of the VMEbus are fully supported from the
SYS68K1CPU-1 B/D. For multi-processing, each IRQ signal can be
enabled/disabled via a jumper field.

Additionally, the SYS68K1CPU-1 B/D supports the ACFAIL, SYSRESET,
SYSFAIL and SYSCLK signal (16 MHz).


    TODO:
    - Finish 2 x ACIA6850, host and remote interface left, terminal works
    - Finish 1 x 68230 Motorola, Parallel Interface / Timer
      - Connect Port B to a Centronics printer interface
    - Add 1 x Abort Switch
    - Add configurable serial connector between ACIA:s and
      - Real terminal emulator, ie rs232 "socket"
      - Debug console
    - Add VME bus driver

****************************************************************************/

#include "emu.h"
#include "bus/rs232/rs232.h"
#include "cpu/m68000/m68000.h"
#include "machine/mm58167.h"
#include "machine/68230pit.h"
#include "machine/6850acia.h"
#include "machine/clock.h"

#define BAUDGEN_CLOCK XTAL_1_8432MHz
/*
 The baudrate on the Force68k CPU-1 to CPU-6 is generated by a
 Motorola 14411 bitrate generator, the CPU-6 documents matches the circuits
 that I could find on the CPU-1 board. Here how I calculated the clock for
 the factory settings. No need to add selectors until terminal.c supports
 configurable baudrates. Fortunality CPU-1 was shipped with 9600N8!

 From the documents:

  3 RS232C interfaces, strap selectable baud rate from 110-9600 or 600-19200 baud

  Default Jumper Settings of B7:
  --------------------------------
  GND           10 - 11 RSA input on 14411
  F1 on 14411    1 - 20 Baud selector of the terminal port
  F1 on 14411    3 - 18 Baud selector of the host port
  F1 on 14411    5 - 16 Baud selector of the remote port

 The RSB input on the 14411 is kept high always so RSA=0, RSB=1 and a 1.8432MHz crystal
 generates 153600 on the F1 output pin which by default strapping is connected to all
 three 6850 acias on the board. These can be strapped separatelly to speedup downloads.

 The selectable outputs from 14411, F1-F16:
 X16 RSA=0,RSB=1: 153600, 115200, 76800, 57600, 38400, 28800, 19200, 9600, 4800, 3200, 2153.3, 1758.8, 1200, 921600, 1843000
 X64 RSA=1,RSB=1: 614400, 460800, 307200, 230400, 153600, 115200, 76800, 57600, 38400, 28800, 19200, 9600, 4800, 921600, 1843000

 However, the datasheet says baudrate is strapable for 110-9600 but the output is 153600
 so the system rom MUST setup the acia to divide by 16 to generate the correct baudrate.

*/
#define ACIA_CLOCK (BAUDGEN_CLOCK / 12)

class force68k_state : public driver_device
{
public:
	force68k_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
			//      m_rtc(*this, "rtc")
			m_maincpu(*this, "maincpu"),
			m_rtc(*this, "rtc"),
			m_pit(*this, "pit"),
			m_aciahost(*this, "aciahost"),
			m_aciaterm(*this, "aciaterm"),
			m_aciaremt(*this, "aciaremt")
	{
	}

	DECLARE_READ16_MEMBER(bootvect_r);
	virtual void machine_start();
	DECLARE_WRITE_LINE_MEMBER(write_aciahost_clock);
	DECLARE_WRITE_LINE_MEMBER(write_aciaterm_clock);
	DECLARE_WRITE_LINE_MEMBER(write_aciaremt_clock);

private:
	required_device<cpu_device> m_maincpu;
	required_device<mm58167_device> m_rtc;
	required_device<pit68230_device> m_pit;
	required_device<acia6850_device> m_aciahost;
	required_device<acia6850_device> m_aciaterm;
	required_device<acia6850_device> m_aciaremt;

	// Pointer to System ROMs needed by bootvect_r
	UINT16  *m_sysrom;
};

static ADDRESS_MAP_START(force68k_mem, AS_PROGRAM, 16, force68k_state)
		ADDRESS_MAP_UNMAP_HIGH
		AM_RANGE(0x000000, 0x000007) AM_ROM AM_READ(bootvect_r) /* Vectors mapped from System EPROM */
		AM_RANGE(0x000008, 0x01ffff) AM_RAM /* DRAM */
		AM_RANGE(0x080000, 0x09ffff) AM_ROM /* System EPROM Area */
//  AM_RANGE(0x0a0000, 0x0bffff) AM_ROM /* User EPROM Area   */
	AM_RANGE(0x0c0040, 0x0c0041) AM_DEVREADWRITE8("aciahost", acia6850_device, status_r, control_w, 0x00ff)
	AM_RANGE(0x0c0042, 0x0c0043) AM_DEVREADWRITE8("aciahost", acia6850_device, data_r, data_w, 0x00ff)
	AM_RANGE(0x0c0080, 0x0c0081) AM_DEVREADWRITE8("aciaterm", acia6850_device, status_r, control_w, 0xff00)
	AM_RANGE(0x0c0082, 0x0c0083) AM_DEVREADWRITE8("aciaterm", acia6850_device, data_r, data_w, 0xff00)
	AM_RANGE(0x0c0100, 0x0c0101) AM_DEVREADWRITE8("aciaremt", acia6850_device, status_r, control_w, 0x00ff)
	AM_RANGE(0x0c0102, 0x0c0103) AM_DEVREADWRITE8("aciaremt", acia6850_device, data_r, data_w, 0x00ff)
		AM_RANGE(0x0c0400, 0x0c042f) AM_DEVREADWRITE8("rtc", mm58167_device, read, write, 0x00ff)
		AM_RANGE(0x0e0000, 0x0e0035) AM_DEVREADWRITE8("pit", pit68230_device, data_r, data_w, 0x00ff)
//      AM_RANGE(0x0e0200, 0x0e0380) AM_READWRITE(fpu_r, fpu_w) /* optional FPCP 68881 FPU interface */
//      AM_RANGE(0x100000, 0xfeffff) /* VMEbus Rev B addresses (24 bits) */
//      AM_RANGE(0xff0000, 0xffffff) /* VMEbus Rev B addresses (16 bits) */
ADDRESS_MAP_END

/* Input ports */
static INPUT_PORTS_START( force68k )
INPUT_PORTS_END

void force68k_state::machine_start()
{
	m_sysrom = (UINT16*)(memregion("maincpu")->base() + 0x080000);
}

READ16_MEMBER(force68k_state::bootvect_r)
{
		return m_sysrom[offset];
}

WRITE_LINE_MEMBER(force68k_state::write_aciahost_clock)
{
		m_aciahost->write_txc(state);
	m_aciahost->write_rxc(state);
}

WRITE_LINE_MEMBER(force68k_state::write_aciaterm_clock)
{
		m_aciaterm->write_txc(state);
	m_aciaterm->write_rxc(state);
}

WRITE_LINE_MEMBER(force68k_state::write_aciaremt_clock)
{
		m_aciaremt->write_txc(state);
	m_aciaremt->write_rxc(state);
}

static MACHINE_CONFIG_START( fccpu1, force68k_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, XTAL_16MHz / 2)
	MCFG_CPU_PROGRAM_MAP(force68k_mem)

		/* P3/Host Port config */
	MCFG_DEVICE_ADD("aciahost", ACIA6850, 0)
		MCFG_DEVICE_ADD("aciahost_clock", CLOCK, ACIA_CLOCK)
	MCFG_CLOCK_SIGNAL_HANDLER(WRITELINE(force68k_state, write_aciahost_clock))

		/* P4/Terminal Port config */
	MCFG_DEVICE_ADD("aciaterm", ACIA6850, 0)

	MCFG_ACIA6850_TXD_HANDLER(DEVWRITELINE("rs232trm", rs232_port_device, write_txd))
	MCFG_ACIA6850_RTS_HANDLER(DEVWRITELINE("rs232trm", rs232_port_device, write_rts))

	MCFG_RS232_PORT_ADD("rs232trm", default_rs232_devices, "terminal")
	MCFG_RS232_RXD_HANDLER(DEVWRITELINE("aciaterm", acia6850_device, write_rxd))
	MCFG_RS232_CTS_HANDLER(DEVWRITELINE("aciaterm", acia6850_device, write_cts))

		MCFG_DEVICE_ADD("aciaterm_clock", CLOCK, ACIA_CLOCK)
	MCFG_CLOCK_SIGNAL_HANDLER(WRITELINE(force68k_state, write_aciaterm_clock))

		/* P5/Remote Port config */
	MCFG_DEVICE_ADD("aciaremt", ACIA6850, 0)

#define PRINTER 0
#if PRINTER
	MCFG_ACIA6850_TXD_HANDLER(DEVWRITELINE("rs232rmt", rs232_port_device, write_txd))
	MCFG_ACIA6850_RTS_HANDLER(DEVWRITELINE("rs232rmt", rs232_port_device, write_rts))

	MCFG_RS232_PORT_ADD("rs232rmt", default_rs232_devices, "printer")
	MCFG_RS232_RXD_HANDLER(DEVWRITELINE("aciaremt", acia6850_device, write_rxd))
	MCFG_RS232_CTS_HANDLER(DEVWRITELINE("aciaremt", acia6850_device, write_cts))
#endif

		MCFG_DEVICE_ADD("aciaremt_clock", CLOCK, ACIA_CLOCK)
	MCFG_CLOCK_SIGNAL_HANDLER(WRITELINE(force68k_state, write_aciaterm_clock))

	/* RTC Real Time Clock device */
	MCFG_DEVICE_ADD("rtc", MM58167, XTAL_32_768kHz)

	/* PIT Parallel Interface and Timer device, assuming strapped for on board clock */
	MCFG_DEVICE_ADD("pit", PIT68230, XTAL_16MHz / 2)

MACHINE_CONFIG_END

#if 0

static MACHINE_CONFIG_START( fccpu6, force68k_state )
	MCFG_CPU_ADD("maincpu", M68000, XTAL_8MHz)  /* Jumper B10 Mode B */
	MCFG_CPU_PROGRAM_MAP(force68k_mem)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( fccpu6a, force68k_state )
	MCFG_CPU_ADD("maincpu", M68000, XTAL_12_5MHz) /* Jumper B10 Mode A */
	MCFG_CPU_PROGRAM_MAP(force68k_mem)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( fccpu6v, force68k_state )
	MCFG_CPU_ADD("maincpu", M68010, XTAL_8MHz)  /* Jumper B10 Mode B */
	MCFG_CPU_PROGRAM_MAP(force68k_mem)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( fccpu6va, force68k_state )
	MCFG_CPU_ADD("maincpu", M68010, XTAL_12_5MHz) /* Jumper B10 Mode A */
	MCFG_CPU_PROGRAM_MAP(force68k_mem)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( fccpu6vb, force68k_state )
	MCFG_CPU_ADD("maincpu", M68010, XTAL_12_5MHz) /* Jumper B10 Mode A */
	MCFG_CPU_PROGRAM_MAP(force68k_mem)
MACHINE_CONFIG_END
#endif

/* ROM definitions */
ROM_START( fccpu1 )
	ROM_REGION(0x1000000, "maincpu", 0)

		ROM_LOAD16_BYTE( "fccpu1V1.0L.j8.bin", 0x080001, 0x2000, CRC(3ac6f08f) SHA1(502f6547b508d8732bd68bbbb2402d8c30fefc3b) )
		ROM_LOAD16_BYTE( "fccpu1V1.0L.j9.bin", 0x080000, 0x2000, CRC(035315fb) SHA1(90dc44d9c25d28428233e6846da6edce2d69e440) )
/* COMMAND SUMMARY DESCRIPTION (From CPU-1B datasheet, ROMs were dumped
   from a CPU-1 board so some features might be missing or different)
---------------------------------------------------------------------------
   BF <address1> <address2> <data> <CR>        Block Fill memory - from addr1 through addr2 with data
   BM <address1> <address2> <address 3> <CR>   Block Move  - move from addr1 through addr2to addr3
   BR [<address> [; <count>] ... ] <CR>        Set/display Breakpoint
   BS <address1> <address2> <data> <CR>        Block Search - search addr1 through addr2 for data
   BT <address1> <address2> <CR>               Block Test of memory
   DC <expression> <CR>                        Data Conversion
   DF <CR>                                     Display Formatted registers
   DU [n] <address1> <address2>[<string>] <CR> Dump memory to object file
   GO [<address] <CR>                          Execute program
   GD [<address] <CR>                          Go Direct
   GT <address> <CR>                           Exec prog: temporary breakpoint
   HE<CR>                                      Help; display monitor commands
   LO [n] [;<options] <CR>                     Load Object file
   MD <address> [<count?? <CR>                  Memory Display
   MM <address> [<data?? [;<options?? <CR>       Memory Modify
   MS <address> <data1 > <data2> < ... <CR>    Memory Set - starting at addr with data 1. data 2 ...
   NOBR [<address> ... ] <CR>                  Remove Breakpoint
   NOPA <CR>                                   Printer Detach (Centronics on PIT/P2)
   OF <CR>                                     Offset
   PA <CR>                                     Printer Attach (Centronics on PIT/P2)
   PF[n] <CR>                                  Set/display Port Format
   RM <CR>                                     Register Modify
   TM [<exit character?? <CR>                   Transparent Mode
   TR [<count] <CR>                            Trace
   TT <address> <CR>                           Trace: temporary breakpoint
   VE [n] [<string] <CR>                       Verify memory/object file
----------------------------------------------------------------------------
  .AO - .A7 [<expression] <CR>                 Display/set address register
  .00 - .07 [<expression] <CR>                 Display/set data register
  .RO - .R6 [<expression] <CR>                 Display/set offset register
  .PC [<expression] <CR>                       Display/set program counter
  .SR [<expression] <CR>                       Display/set status register
  .SS [<expression] <CR>                       Display/set supervisor stack
  .US [<expression] <CR>                       Display/set user stack
----------------------------------------------------------------------------
   MD <address> [<count>]; D1 <CR>             Disassemble memory location
   MM <address>; DI <CR>                       Disassemble/Assemble memory location
----------------------------------------------------------------------------
*/
ROM_END

#if 0
ROM_START( fccpu6 )
	ROM_REGION(0x1000000, "maincpu", 0)
ROM_END

ROM_START( fccpu6a )
	ROM_REGION(0x1000000, "maincpu", 0)
ROM_END

ROM_START( fccpu6v )
	ROM_REGION(0x1000000, "maincpu", 0)
ROM_END

ROM_START( fccpu6va )
	ROM_REGION(0x1000000, "maincpu", 0)
ROM_END

ROM_START( fccpu6vb )
	ROM_REGION(0x1000000, "maincpu", 0)
ROM_END
#endif

/* Driver */
/*    YEAR  NAME          PARENT  COMPAT   MACHINE         INPUT     CLASS          INIT COMPANY                  FULLNAME          FLAGS */
COMP( 1983, fccpu1,            0,      0,      fccpu1,      force68k, driver_device,  0,  "Force Computers Gmbh",  "SYS68K/CPU-1",   MACHINE_NO_SOUND_HW | MACHINE_TYPE_COMPUTER )
//COMP( 1989, fccpu6,   0,      0,       fccpu6,      force68k, driver_device,  0,  "Force Computers Gmbh",  "SYS68K/CPU-6",   MACHINE_IS_SKELETON )
//COMP( 1989, fccpu6a,  0,      0,       fccpu6a,     force68k, driver_device,  0,  "Force Computers Gmbh",  "SYS68K/CPU-6a",  MACHINE_IS_SKELETON )
//COMP( 1989, fccpu6v,  0,      0,       fccpu6v,     force68k, driver_device,  0,  "Force Computers Gmbh",  "SYS68K/CPU-6v",  MACHINE_IS_SKELETON )
//COMP( 1989, fccpu6va, 0,      0,       fccpu6va,    force68k, driver_device,  0,  "Force Computers Gmbh",  "SYS68K/CPU-6va", MACHINE_IS_SKELETON )
//COMP( 1989, fccpu6vb, 0,      0,       fccpu6vb,    force68k, driver_device,  0,  "Force Computers Gmbh",  "SYS68K/CPU-6vb", MACHINE_IS_SKELETON )