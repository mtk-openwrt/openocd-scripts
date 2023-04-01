Steps to load u-boot via JTAG

== Note ==
It's suggested to use OpenOCD 0.12.0 or later version.

1. Connect to MT7988 board using mt7621.cfg

2. Halt the target
	> halt

   If you want to use "reset halt", please modify mt7621.cfg to make sure only c0v0 is enabled

3. Initialize CPU & DDR
	> pre_init
	> load_stage_bin # Load mt7621_stage_sram.bin to SRAM and wait for modification
	> set_baudrate 115200 # The baudrate of the output of mt7621_stage_sram.bin. Default is 57600 if not set
	> set_cpu_freq 880
	> set_ddr_freq 1200 # You should set <=1066 for DDR2

   Now we're going to set DDR timing parameter. Do ONLY ONE of the following command according to its comment:
	> set_ddr2_timing 64 # DDR2 64MiB/512Mb
	> set_ddr2_timing 128 # DDR2 128MiB/1024Mb
	> set_ddr3_timing 128 # DDR3 128MiB/1024Mb
	> set_ddr3_timing 128_kgd # MT7621DA only
	> set_ddr3_timing 256 # DDR3 256MiB/2048Mb
	> set_ddr3_timing 512 # DDR3 512MiB4096Mb

   Reset commands to finish initialization:
	> run_cpu_ddr_init # Run mt7621_stage_sram.bin
	> reset_cache # Initialize L1 & L2 cache
	> post_init

4. Set work-area
	> mt7621.cpu0 configure -work-area-phys 0xA0000000 -work-area-size 0x100

5. Load and run U-Boot
	> load_image u-boot-mt7621-snor.bin 0x80200000 bin
	> resume 0x80200000

   For board using SPI-NOR, use u-boot-mt7621-snor.bin.
   For board using SPI-NAND, use u-boot-mt7621-nand.bin.

   If you want to use your customized u-boot, please use u-boot.bin from compiled u-boot source directory.
   (Only mainline u-boot is supported by this guide)

9. All done
   Now the u-boot should be running.