/* x86_64的lds文件 */
OUTPUT_FORMAT("elf64-x86-64","elf64-x86-64","elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(micro_kernel_entry)
SECTIONS
{
    . = 0xffff800000000000 + 0x100000;
	.text :
	{
		_text = .;
		*(.text)
		_etext = .;
	}
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
		*(COMMON)
		_ebss = .;
	}
	_end = .;
}