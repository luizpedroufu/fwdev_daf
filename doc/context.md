# Troca de contexto no Cortex M4

> [!NOTE]
> :brain:

## Modos de Operação no Cortex-M4

O núcleo Cortex-M4 opera em dois modos principais:

- **Thread Mode**: é o modo padrão de execução do firmware, onde o código da aplicação principal e tarefas de RTOS são executadas. Pode operar em nível privilegiado ou não privilegiado.
- **Handler Mode**: ativado automaticamente durante o atendimento de exceções e interrupções. Sempre executa em nível privilegiado.

Essa distinção é fundamental para garantir isolamento entre o código de aplicação e os serviços de sistema ou tratadores de interrupção.




## Pilhas: MSP e PSP

O Cortex-M4 possui dois ponteiros de pilha:

- **MSP (Main Stack Pointer)**: utilizado por padrão durante o boot e por todas as exceções (Handler Mode).
- **PSP (Process Stack Pointer)**: utilizado por tarefas da aplicação (Thread Mode), geralmente sob controle de um RTOS.

### Inicialização das Pilhas

Durante o reset, o MSP é automaticamente carregado com o valor presente na primeira posição da tabela de vetores (endereço 0x00000000). O PSP deve ser inicializado manualmente pelo firmware se for utilizado.

A troca entre MSP e PSP no Thread Mode é controlada pelo registrador CONTROL. O bit 1 desse registrador determina qual pilha é usada no Thread Mode:

- 0: Thread Mode usa MSP
- 1: Thread Mode usa PSP

A troca é realizada com a instrução:

```c
__set_CONTROL(__get_CONTROL() | 0x2); // usa PSP no Thread Mode
```

Para inicializar o PSP com um valor válido, normalmente alocado estaticamente em RAM:

```c
#define STACK_SIZE 256
uint32_t psp_stack[STACK_SIZE];

void setup_psp(void) {
    uint32_t psp_start = (uint32_t)&psp_stack[STACK_SIZE];
    __set_PSP(psp_start); // Define o topo da pilha PSP
    __set_CONTROL(__get_CONTROL() | 0x2); // Ativa PSP no Thread Mode
    __ISB(); // Instruction Synchronization Barrier
}
```

Essa função configura o PSP para usar o topo de uma pilha alocada estaticamente e garante que o processador passe a utilizá-lo corretamente, com sincronização das instruções subsequentes. Essa abordagem é comum na inicialização de tarefas por um RTOS ou em firmwares com separação clara entre modos de operação.



## Salvamento de Contexto em Interrupções

Quando ocorre uma interrupção, o Cortex-M4 realiza automaticamente o empilhamento de parte do contexto da CPU na pilha ativa (sempre o MSP). Os seguintes registradores são salvos:

- xPSR: Status Register completo (APSR + EPSR + IPSR)
- PC: Program Counter
- LR: Link Register
- R12: registrador de uso geral
- R0–R3: registradores de argumentos

Esse processo é feito em hardware, de forma determinística, antes da execução do handler. Se a FPU estiver habilitada e usada, os registradores S0–S15 e FPSCR também são salvos automaticamente.

### Representação do Contexto na Pilha

```ascii
   +------------------+ <- endereço mais baixo da pilha após interrupção
   |      R0          |
   +------------------+
   |      R1          |
   +------------------+
   |      R#          |
   +------------------+
   |      R3          |
   +------------------+
   |      R12         |
   +------------------+
   |      LR          |
   +------------------+
   |      PC          |
   +------------------+
   |      xPSR        |
   +------------------+ <- valor do SP no início da ISR
```

Se a FPU for usada:

```ascii
Endereço -0x40 -> S15
         ...
         -0x04 -> S0
         +0x00 -> R0  <- SP após empilhamento automático
```

O SP aponta para o local onde R0 foi salvo. Os valores superiores à pilha (endereços menores) são usados em casos com FPU.

## Retorno de Interrupções

O retorno de uma exceção pode ocorrer implicitamente ao final do handler ou ser acionado explicitamente com a instrução `BX LR`. Em ambos os casos, o valor contido no registrador LR durante a exceção é um valor especial conhecido como `EXC_RETURN`. Este valor codifica informações sobre o contexto salvo e como ele deve ser restaurado.

Os bits do EXC_RETURN indicam:

- Qual pilha deve ser usada (MSP ou PSP)
- Se a FPU deve restaurar registradores
- Se o retorno é para modo Thread ou Handler

O `EXC_RETURN` possui o formato `0xFFFFFFXY`, onde os bits baixos (X e Y) determinam:

- **Bit [0]**: Sempre 1 (indica o modo thumb2)
- **Bit [2]**: 0 indica retorno para Handler Mode, 1 indica retorno para Thread Mode
- **Bit [3]**: 0 usa MSP como pilha no retorno, 1 usa PSP
- **Bit [4]**: 1 se FPU não está presente ou não usada, 0 se os registradores da FPU foram empilhados
- **Bits [31:5]**: Fixos em 1 (0xFFFFFF)

Tabela com alguns valores comuns:

| Valor EXC\_RETURN | Modo de retorno | Pilha usada | FPU empilhada |
| ----------------- | --------------- | ----------- | ------------- |
| 0xFFFFFFF1        | Handler Mode    | MSP         | Não           |
| 0xFFFFFFF9        | Thread Mode     | MSP         | Não           |
| 0xFFFFFFFD        | Thread Mode     | PSP         | Não           |
| 0xFFFFFFE1        | Handler Mode    | MSP         | Sim           |
| 0xFFFFFFE9        | Thread Mode     | MSP         | Sim           |
| 0xFFFFFFED        | Thread Mode     | PSP         | Sim           |

O processador interpreta esse valor automaticamente e restaura o contexto salvo na pilha, retornando a execução ao ponto onde a interrupção ocorreu. Esse mecanismo é essencial para a implementação de trocas de contexto em RTOS, pois permite alternar tarefas com mínimo overhead e sem necessidade de instruções manuais de restauração.

Se a FPU estiver habilitada e em uso, registradores de ponto flutuante (S0–S15 e FPSCR) também são empilhados automaticamente antes do empilhamento padrão, e `EXC_RETURN` refletirá isso.

Esse mecanismo baseado em `EXC_RETURN` reforça a simplicidade e a previsibilidade do modelo de exceções do Cortex-M4, garantindo que o retorno à execução normal seja realizado de forma segura e automática após a manipulação de eventos assíncronos.

Durante o retorno:

1. O Cortex-M4 avalia o valor do EXC_RETURN no LR quando a instrução `BX LR` é executada
2. Automaticamente restaura os registros salvos na pilha (em ordem inversa)
3. Atualiza o SP
4. Retoma a execução no endereço contido em PC

Esse mecanismo permite alternância eficiente entre tarefas e interrupções, sendo amplamente utilizado por sistemas operacionais embarcados para implementar troca de contexto (context switch).

A estrutura simples, previsível e em hardware do empilhamento e desempilhamento no Cortex-M4 é um dos fatores que contribuem para sua eficiência em aplicações de tempo real.

##

Ao entrar na interrupção, observar:

https://interrupt.memfault.com/blog/cortex-m-rtos-context-switching

REGISTRO CONTROL:


SFPA indica se o ponto flutuante seguro está ativo ou inativo (V8M somente)
FPCA indica se o contexto de ponto flutuante está ativo ou não
SPSEL controla qual ponteiro de pilha está em uso no modo thread (0 = MSP, 1 = PSP). Entraremos em mais detalhes a seguir. O modo handler sempre usa MSP.
nPriv controla se o modo de thread está operando como privilegiado (0) ou não (1).



Uso dos registros em EABI Cortex M

- R12: Usado como registrador de link (LR) em algumas convenções de chamada.
- R7: Usado como registrador de frame pointer (FP) em algumas convenções de chamada.
- 




https://github.com/ARM-software/abi-aa/releases

https://stackoverflow.com/questions/261419/what-registers-to-save-in-the-arm-c-calling-convention

r0-r3 are the argument and scratch registers also know as caller-saved or call-clobbered registers; r0-r1 are also the result registers. Callers must assume function-calls overwrite these registers, even if there are no arguments.
r4-r8 are callee-save registers
r9 might be a callee-save register or not (on some variants of AAPCS it is a special register)
r10-r11 are callee-save registers
r12-r15 are special registers
