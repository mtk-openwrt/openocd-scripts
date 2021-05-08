Step to load u-boot via JTAG

1. Connect to MT7629 board using mt7629.cfg

   If the bootloader is broken, make sure BootROM has output "System halt!" before moving to step 2

2. Halt the target, and then disable watchdog immediately, and then set work-area
	> halt
	> mww 0x10212000 0x22000000
	> mt7629.cpu0 configure -work-area-phys 0x101000 -work-area-size 8096

3. Disable MMU
	> set cp [arm mrc 15 0 1 0 0]
	> set cp [expr ($cp & ~1)]
	> arm mcr 15 0 1 0 0 $cp

   From the output of halt command:
     If the output shows the CPU is in ARM state, goto step 5
     If the output shows the CPU is in Thumb state, continue

4. Switch CPU to ARM state
	> reg cpsr 0x1d3

5. Load and run ram-boot ATF BL2
	> load_image bl2.bin 0x201000 bin
	> reg pc 0x201000
	> resume

   The last output of BL2 should be "[EMI]rank0 size auto detect: <ram size>"

6. Load U-Boot (included in ATF FIP)
	> halt
	> load_image fip-snand.bin 0x40020000 bin

   If the MT7629 board is using SPI-NAND flash, use fip-snand.bin.
   If the MT7629 board is using SPI-NOR flash, use fip-snor.bin.

7. Continue to run BL2 and FIP
	> mww 0x100200 1
	> resume

   In step 5, when "[EMI]rank0 size auto detect: <ram size>" has been output,
   the BL2 stops running, and is waiting (uint32_t)0x100200 becoming non-zero

   So we can then load the FIP into DRAM before BL2 continuing running the FIP

8. All done
   Now the u-boot should be running.
