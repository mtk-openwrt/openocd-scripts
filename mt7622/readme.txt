Step to load u-boot via JTAG

1. Connect to MT7622 board using mt7622.cfg

   If the bootloader is broken, make sure BootROM has output "System halt!" before moving to step 2

2. Halt the target, and then disable watchdog immediately, and then set work-area
	> halt
	> mww 0x10212000 0x22000000
	> mt7622.cpu0 configure -work-area-phys 0x101000 -work-area-size 8096

   Since MMU can't be disabled in OpenOCD, please make sure the MMU is disabled shown in output, otherwise the following operations may fail

   If the output shows the CPU is in AArch64 state, goto step 7
   If the output shows the CPU is in ARM state, goto step 6
   If the output shows the CPU is in Thumb state, goto step 5
   If no output for CPU state, but an error "Error: Failed to read ESR_EL3 register", continue

4. Read the CPSR register
	> reg cpsr

   If bit 5 (T, Thumb) is not set, goto step 6

5. Switch CPU to ARM state
	> reg cpsr 0x1d3

6. Switch CPU to AArch64 state
	> load_image switch_mode_32_64.bin 0x100000 bin
	> load_image aarch64_stall.bin 0x100100 bin
	> reg pc 0x100000
	> resume

   switch_mode_32_64.bin is used to switch CPU from AArch32 to AArch64
   aarch64_stall.bin is used to catch the CPU after switched to AArch64

   After resume, there may be an error "mt7622.cpu0: Timeout waiting for resumex", just ignore it

	> halt

   Once the CPU is halted, the output should shows the CPU is now in AArch64 state

7. Load and run ram-boot ATF BL2
	> load_image bl2-1c.bin 0x201000 bin
	> reg pc 0x201000
	> resume

   If the MT7622 board is using only one DDR chip, use bl2-1c.bin.
   If the MT7622 board is using two DDR chip, use bl2-2c.bin.

   The last output of BL2 should be "INFO:    EMI: complex R/W mem test passed"

8. Load U-Boot (included in ATF FIP)
	> halt
	> load_image fip-snand-no-bmt.bin 0x40020000 bin

9. Continue to run BL2 and FIP
	> mww 0x100200 1
	> resume

   In step 7, when "INFO:    EMI: complex R/W mem test passed" has been output,
   the BL2 stops running, and is waiting (uint32_t)0x100200 becoming non-zero

   So we can then load the FIP into DRAM before BL2 continuing running the FIP

10. All done
   Now the u-boot should be running.
