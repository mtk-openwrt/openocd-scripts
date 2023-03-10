	.syntax unified

	movw	r0, #:lower16:0x10400038
	movt	r0, #:upper16:0x10400038
	movw	r1, #:lower16:0x100100
	movt	r1, #:upper16:0x100100
	str	r1, [r0]

	mrc	p15, 0, r0, c12, c0, 2
	orr	r0, r0, #1
	mcr	p15, 0, r0, c12, c0, 2

	dsb
	isb

	mrc	p15, 0, r0, c12, c0, 2
	orr	r0, r0, #2
	mcr	p15, 0, r0, c12, c0, 2

	dsb
	isb

1:	wfi
	b	1b
