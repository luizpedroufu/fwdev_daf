/* Linker Script com Símbolos de Inicialização */
ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 786K
}

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    *(.text*)
  } > FLASH

  /* Seção de dados inicializados */
  .data :
  {
    _sdata = .;         /* Define o símbolo _sdata = início da .data na RAM */
    *(.data*)
    _edata = .;         /* Define o símbolo _edata = fim da .data na RAM */
  } > RAM AT > FLASH   /* Aloca na RAM, mas armazena os valores iniciais na FLASH */
  _sidata = LOADADDR(.data); /* Define _sidata = endereço dos valores na FLASH */

  /* Seção de dados não inicializados (zerados) */
  .bss :
  {
    _sbss = .;          /* Define _sbss = início da .bss na RAM */
    *(.bss*)
    *(COMMON)
    _ebss = .;          /* Define _ebss = fim da .bss na RAM */
  } > RAM

  _estack = ORIGIN(RAM) + LENGTH(RAM);
}