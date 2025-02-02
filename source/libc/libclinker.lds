OUTPUT_FORMAT("elf64-x86-64","elf64-x86-64","elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(__libc_start_main)
SECTIONS
{
    . = 0x400000;
 
 	.text :
	{
		_text = .;
		*(.text)
		_etext = .;
	}

	. = ALIGN(8);

	.data :
	{
		_data = .;
		*(.data)		
		_edata = .;
	}

	.rodata : 
	{
		_rodata = .;	
		*(.rodata)
		_erodata = .;
	}

	.bss :
	{
		_bss = .;
		*(.bss)
		_ebss = .;
	}

	_end = .;
 
}
