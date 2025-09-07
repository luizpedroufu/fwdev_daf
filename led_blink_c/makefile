# Nome do programa

PROGNAME := main

# Ferramentas do toolchain

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
CP = arm-none-eabi-objcopy
RM = rm -rf

# Diretorios arquivos objeto e de lista de dependencias serao salvos

OBJDIR = build
DEPDIR = .deps

# Arquivos a serem compilados

SRCS = main.c

#Flags do compilador

CFLAGS = -mcpu=cortex-m33 -mthumb -O0 -g -Wall
DEPFLAGS = -MMD -MP -MF $(DEPDIR)/$*.d
LFLAGS = -nostdlib -T startup.ld -Wl,-Map=main.map

# Gera lista de arquivos objeto e cria diretorio onde serao salvos

OBJS = $(patsubst %, $(OBJDIR)/%.o, $(basename $(SRCS)))
$(shell mkdir -p $(dir $(OBJS)) > /dev/null)

# Gera lista de arquivos de lista dependencia e cria diretorio onde serao salvos

DEPS = $(patsubst %, $(DEPDIR)/%.d, $(basename $(SRCS)))
$(shell mkdir -p $(dir $(DEPS)) > /dev/null)

all: $(OBJS) $(PROGNAME).elf $(PROGNAME).bin

$(PROGNAME).elf: $(OBJS)
	$(LD) $(LFLAGS) -o $@ $^

$(PROGNAME).bin: $(PROGNAME).elf
	$(CP) -O binary $^ $@

$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	$(CC) -c $(CFLAGS) $(DEPFLAGS) $< -o $@

# Cria um novo target para cada arquivo de dependencia possivel

$(DEPS):

# Inclui conteudo dos arquivos de dependencia

-include $(DEPS)

.PHONY: clean
clean:
	$(RM) $(OBJDIR) $(DEPDIR)
	$(RM) $(PROGNAME).elf $(PROGNAME).bin $(PROGNAME).map