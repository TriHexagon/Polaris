PROJ_NAME = kernel

DEVICE = stm32f4discovery
#DEVICE = STM32F429Discovery

#aviable defines: DEBUG, RAMMODE, NOFPU, NOMPU
DEFINES = DEBUG RAMMODE DEVICE=$(DEVICE)

# Linkerfile settings
ifeq ( $(filter RAMMODE,$(DEFINES)), )
LINKERFILE = devices/$(DEVICE)/rom.ld
else
LINKERFILE = devices/$(DEVICE)/ram.ld
endif

OBJ_DIR = obj/
BIN_DIR = bin/
INC_DIRS = inc cmsis/inc/ devices/$(DEVICE)/inc

SRCS  = $(wildcard src/*.s)
SRCS += $(wildcard src/*.c)
SRCS += $(wildcard cmsis/src/*.c)
SRCS += $(wildcard devices/$(DEVICE)/src/*.c)
SRCS += $(wildcard devices/$(DEVICE)/src/*.s)

OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(basename $(SRCS))))

CFLAGS = -mcpu=cortex-m4 -mlittle-endian -mthumb -nostdlib -std=c99

# DEBUG settings
ifeq ( $(filter DEBUG,$(DEFINES)), )
CFLAGS += -O2
else
CFLAGS += -O0 -g
endif

# FPU settings
ifeq ( $(filter NOFPU,$(DEFINES)), )
CFLAGS += -mfloat-abi=soft
else
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard #or softfp?
endif

LFLAGS = -Wl,--gc-sections $(CFLAGS)

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
MKDIR = mkdir
RM = rm

# RULES #
.PHONY: all
all: $(OBJS)
all: $(BIN_DIR)$(PROJ_NAME).elf
all: $(BIN_DIR)$(PROJ_NAME).bin

# C Sources
$(OBJ_DIR)%.o: %.c
	@$(MKDIR) -p $(dir $@)
	$(CC) $< -o $@ -c $(CFLAGS) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INC_DIRS))

# Assembler Sources
$(OBJ_DIR)%.o: %.s
	@$(MKDIR) -p $(dir $@)
	$(CC) $< -o $@ -c $(CFLAGS) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INC_DIRS))

%.elf: $(OBJS)
	@$(MKDIR) -p $(dir $@)
	$(LD) $(OBJS) -o $@ $(LFLAGS) -T$(LINKERFILE)

%.bin: %.elf
	@$(MKDIR) -p $(dir $@)
	$(OBJCOPY) -Obinary $< $@

$(OBJS): Makefile

# Other
.PHONY: load
load:
	../../KernelLoader $(BIN_DIR)$(PROJ_NAME).bin

.PHONY: debug
debug:
	openocd -f stm32f4discovery.cfg &
	arm-none-eabi-gdb $(BIN)$(PROJ_NAME).elf
	fg %1

.PHONY: clean
clean:
	$(RM) -rf $(wildcard $(OBJ_DIR)*) $(wildcard $(BIN_DIR)*)
