Step to load u-boot via JTAG

1. Connect to MT7628 board using mt7628.cfg

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
	> mt7628.cpu0 configure -work-area-phys 0xa0001000 -work-area-size 4096

7. Load and run ram-boot U-Boot
	> load_image <u-boot-bin> 0x80200000 bin
	> reg pc 0x80200000
	> resume

   Please replace <u-boot-bin> with the actual u-boot binary file.

   There is one pre-built u-boot binary file:

   - u-boot-mt7628-ram.bin	for generic boards
				U-Boot config file: mt7628_rfb_defconfig

   You can also use u-boot.bin from your customized source.

8. All done
   Now the u-boot should be running.

---
Note:
   The pre-built u-boot binary can be built from:
   https://gitlab.denx.de/u-boot/u-boot
