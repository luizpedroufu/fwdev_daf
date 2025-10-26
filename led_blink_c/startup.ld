ENTRY(Reset_Handler)

MEMORY
{
  FLASH(rx): ORIGIN = 0x08000000, LENGTH = 2048K
  SRAM(rwx): ORIGIN = 0x20000000, LENGTH = 786K
}

SECTIONS
{
  .text :
  {
    KEEP(*(.isr_vectors))
    *(.text)
    . = ALIGN(4);
    *(.rodata)
    . = ALIGN(4);
    _la_data = .;
  }> FLASH

  .data :
  {
    _sdata = .;
    *(.data)
    . = ALIGN(4);
    _edata = .;
  }> SRAM AT> FLASH

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    . = ALIGN(4);
    _ebss = .;
  }> SRAM
}