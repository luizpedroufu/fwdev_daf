TARGET = firmware
MCU = cortex-m33

# Ferramentas
PREFIX = arm-none-eabi-
AS = $(PREFIX)as
LD = $(PREFIX)ld
OC = $(PREFIX)objcopy

# Arquivos fonte (ALTERE PARA O NOME DO SEU ARQUIVO)
AS_SOURCES = main.s  # Mude para o nome do seu arquivo .s

# Flags
ASFLAGS = -mcpu=$(MCU) -mthumb -g
LDFLAGS = -T $(LD_SCRIPT) -Map=$(TARGET).map
OCFLAGS = -O binary

# Arquivos gerados
OBJS = $(AS_SOURCES:.s=.o)
LD_SCRIPT = stm32u5.ld
ELF = $(TARGET).elf
BIN = $(TARGET).bin

all: $(BIN)

$(BIN): $(ELF)
	$(OC) $(OCFLAGS) $< $@

$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(ELF) $(BIN) $(TARGET).map

.PHONY: all clean