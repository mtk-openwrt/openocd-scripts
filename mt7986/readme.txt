Steps to load u-boot via JTAG

== IMPORTANT ==
Since v0.12.0, reading pauth_dmask/pauth_cmask was added but fails on MediaTek chips with the following error:
Error: Failed to read pauth_dmask register
after halt. One can no longer determine the CPU state and MMU/cache state.
To fix it, you must compile your own openocd binary with patch in the patch/for-v0.12.0-and-later applied.

== Note ==
Considering that the original OpenOCD does not support disabling MMU in AArch64,
if you encountered the situation that CPU halts with MMU enabled, you can compile
your own openocd binary with all patches in the patch folder applied to the openocd
source to support disabling MMU with cache flushing.

Since v0.12.0-rc1, the "reset halt" is broken due to a related commit. To make it work again,
you can compile your own openocd binary with patch in the patch/for-v0.12.0-rc1-and-later applied.

1. Connect to MT7986 board using mt7986.cfg

2. Halt the target, and then disable watchdog immediately, and then set work-area
	> halt # You can also use "reset halt" if possible
	> mww 0x1001c000 0x22000000
	> mt7986.cpu0 configure -work-area-phys 0x101000 -work-area-size 8096

   Using "reset halt" can halt the CPU in an early stage of BootROM to prevent BL2/U-Boot from running which
   may cause possibly JTAG disabled or broken peripheral (including DRAM) initialization.

   If the output shows the CPU is in AArch64 state, goto step 6
   If the output shows the CPU is in ARM state, goto step 5
   If the output shows the CPU is in Thumb state, goto step 4
   If no output for CPU state, but an error "Error: Failed to read ESR_EL3 register", continue

3. Read the CPSR register
	> reg cpsr

   If bit 5 (T, Thumb) is not set, goto step 6

4. Switch CPU to ARM state
	> reg cpsr 0x1d3

5. Switch CPU to AArch64 state
	> load_image switch_mode_32_64.bin 0x100000 bin
	> load_image aarch64_stall.bin 0x100100 bin
	> aarch64 disable_mmu # Do this command only if CPU halts with MMU enabled
	> resume 0x100000

   switch_mode_32_64.bin is used to switch CPU from AArch32 to AArch64
   aarch64_stall.bin is used to catch the CPU after switched to AArch64

   The command "aarch64 disable_mmu" will first flush D-Cache then disable MMU.
   This may take few seconds to complete.

   After resume, there may be an error "mt7986.cpu0: Timeout waiting for resumex", just ignore it

	> halt

   Once the CPU is halted, the output should shows the CPU is now in AArch64 state

6. Load and run ram-boot ATF BL2
	> mww 0x104007f0 0x301
	> load_image bl2-ddr4.bin 0x201000 bin
	> resume 0x201000

   For board using DDR3 chip, use bl2-ddr3.bin.
   For board using DDR4 chip, use bl2-ddr4.bin.

   The last output of BL2 should be "NOTICE:  Waiting for FIP data and signal from debugger"

7. Load U-Boot (included in ATF FIP)
	> halt
	> load_image fip-spim-nand.bin 0x40020000 bin

   For board using SPI-NOR, use fip-spim-nor.bin.
   For board using SPI-NAND, use fip-spim-nand.bin.
   For board using SD with MT7986A, use fip-sd-a.bin.
   For board using SD with MT7986B/C, use fip-sd-b.bin.
   For board using eMMC with MT7986A, use fip-emmc-a.bin.
   For board using eMMC with MT7986B/C, use fip-emmc-b.bin.

8. Continue to run BL2 and FIP
	> mww 0x100200 1
	> resume

   In step 6, when "NOTICE:  Waiting for FIP data and signal from debugger" has been output,
   the BL2 stops running, and is waiting (uint32_t)0x100200 becoming non-zero

   So we can then load the FIP into DRAM before BL2 continuing running the FIP

9. All done
   Now the u-boot should be running.

Appendix
== Command line for building bl2 binaries ==
make -f Makefile CROSS_COMPILE=<toolchain-prefix> LOG_LEVEL=30 PLAT=mt7986 <dram-config> BOOT_DEVICE=ram RAM_BOOT_DEBUGGER_HOOK=1 bl2

<dram-config>:
For bl2-ddr3: DRAM_USE_DDR4=0
For bl2-ddr4: DRAM_USE_DDR4=1

Final binary is build/mt7986/release/bl2.bin