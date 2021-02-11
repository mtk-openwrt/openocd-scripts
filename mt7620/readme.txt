Step to load u-boot via JTAG

1. Connect to MT7620 board using mt7620.cfg

2. Halt the target
	> halt

	or

	> reset halt

3. Initialize CPU clock
	> cpu_pll_init

4. Initialize DRAM
	> dram_init <mb>

   Please replace <mb> with the actual DRAM size in MiB (e.g. 64)

5. Set work-area
	> mt7620.cpu0 configure -work-area-phys 0xa0001000 -work-area-size 4096

7. Load and run ram-boot U-Boot
	> load_image <u-boot-bin> 0x80200000 bin
	> reg pc 0x80200000
	> resume

   Please replace <u-boot-bin> with the actual u-boot binary file.

   There are two pre-built u-boot binary files:

   - u-boot-mt7620-ram.bin		for boards using built-in FE switch
					U-Boot config file: mt7620_rfb_defconfig
   - u-boot-mt7620-mt7530-ram.bin	for boards using external MT7530 switch
					U-Boot config file: mt7620_mt7530_rfb_defconfig

   You can also use u-boot.bin from your customized source.

8. All done
   Now the u-boot should be running.

---
Note:
   All pre-built u-boot binaries can be built from:
   https://gitlab.denx.de/u-boot/u-boot
