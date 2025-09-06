
ENTRY(Reset_Handler)

MEMORY
{
  RAM    (xrw)    : ORIGIN = 0x20000000,   LENGTH = 32K
  FLASH    (rx)    : ORIGIN = 0x8000000,   LENGTH = 128K
}

/* Sections */
SECTIONS
{
  .isr_area : 
  { 
    . = ALIGN(4);
    KEEP(*(.isr_vectors))
    . = ALIGN(4);
  } >FLASH
  
  .text : 
  { 
    . = ALIGN(4);
  	*(.text*) 
  	. = ALIGN(4);
  } >FLASH
  
  __start_flash_data = LOADADDR(.data);
  
  .data : 
  { 
    . = ALIGN(4);
    __start_ram_data = .;
  	*(.data*)
  	. = ALIGN(4);
  	__end_ram_data = .; 
  } > RAM AT >FLASH

  
  .rodata : 
  { 
    . = ALIGN(4);
  	*(.rodata*) 
  	. = ALIGN(4);
  } >FLASH
  
  .bss  : 
  { 
    . = ALIGN(4);
    __start_ram_bss = .;
  	*(.bss*)  
  	. = ALIGN(4);
  	__end_ram_bss = .;
  } >RAM
}
