DEVICE = GD32F303CC
CPU = cortex-m4
FPU = fpv4-sp-d16
FLOAT_ABI = hard

PREFIX = arm-none-eabi-

CC = $(PREFIX)gcc
AS = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size

PROJECT = privateTens

# Directories
FIRMWARE = GD32F30x_Firmware_Library_V2.1.3
SOURCE_DIR = source
LIBS_DIR = tenslibs
EEPROM_DIR = eeprom_in_flash

# Output
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
LST_DIR = $(BUILD_DIR)/lst
BIN_DIR = $(BUILD_DIR)/bin

# Debug/Release
ifeq ($(BUILD),release)
    DEFINES = -DUSE_STDPERIPH_DRIVER -DGD32F30X_HD -DRELEASE
    CFLAGS = -O2 -DNDEBUG
else
    DEFINES = -DUSE_STDPERIPH_DRIVER -DGD32F30X_HD
    CFLAGS = -Og -g3
endif

# Common flags
CFLAGS += -mcpu=$(CPU) -mthumb -mfloat-abi=$(FLOAT_ABI) -mfpu=$(FPU)
CFLAGS += -Wall -Wextra -Wno-unused-parameter
CFLAGS += -fdata-sections -fno-builtin
CFLAGS += -ffunction-sections
CFLAGS += $(DEFINES)

ASFLAGS = $(CFLAGS) -x assembler-with-cpp -D__ASSEMBLY__

LDFLAGS = -mcpu=$(CPU) -mthumb -mfloat-abi=$(FLOAT_ABI) -mfpu=$(FPU)
LDFLAGS += --specs=nosys.specs -lnosys
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map
LDFLAGS += -Wl,--cref
LDFLAGS += -Wl,--defsym=_estack=0x2000c000

# Memory settings
FLASH_SIZE = 0x40000
RAM_SIZE = 0x0C000

# Startup and linker script paths
STARTUP = startup_gd32f30x_hd.S
LDSCRIPT = ldscript.ld
SYSTEM = $(FIRMWARE)/Firmware/CMSIS/GD/GD32F30x/Source/system_gd32f30x.c

# Include paths
INCLUDES = -I$(FIRMWARE)/Firmware/CMSIS
INCLUDES += -I$(FIRMWARE)/Firmware/CMSIS/GD/GD32F30x/Include
INCLUDES += -I$(FIRMWARE)/Firmware/GD32F30x_standard_peripheral/Include
INCLUDES += -I$(SOURCE_DIR)
INCLUDES += -I$(LIBS_DIR)/include
INCLUDES += -I$(EEPROM_DIR)

CFLAGS += $(INCLUDES)

# Source groups
FWLIB_SRCS = $(wildcard $(FIRMWARE)/Firmware/GD32F30x_standard_peripheral/Source/*.c)
FWLIB_SRCS := $(filter-out $(FIRMWARE)/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_can.c, $(FWLIB_SRCS))
FWLIB_SRCS := $(filter-out $(FIRMWARE)/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_enet.c, $(FWLIB_SRCS))
FWLIB_SRCS := $(filter-out $(FIRMWARE)/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_bkp.c, $(FWLIB_SRCS))
FWLIB_SRCS := $(filter-out $(FIRMWARE)/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_i2c.c, $(FWLIB_SRCS))

CMSIS_SRCS = $(FIRMWARE)/Firmware/CMSIS/GD/GD32F30x/Source/system_gd32f30x.c

HAL_SRCS = $(SOURCE_DIR)/hc595.c \
           $(SOURCE_DIR)/board.c \
           $(SOURCE_DIR)/btuart.c \
           $(SOURCE_DIR)/pwm.c

APP_SRCS = $(SOURCE_DIR)/main.c \
           $(SOURCE_DIR)/tens.c \
           $(SOURCE_DIR)/motortip.c \
           $(SOURCE_DIR)/button.c \
           $(SOURCE_DIR)/hv.c \
           $(SOURCE_DIR)/eepromemul.c \
           $(SOURCE_DIR)/lcd.c \
           $(SOURCE_DIR)/tensdata.c \
           $(SOURCE_DIR)/tensoutput.c \
           $(SOURCE_DIR)/battery.c \
           $(EEPROM_DIR)/eeprom_in_flash.c \
           $(EEPROM_DIR)/eeprom_port/eeprom_port_gd32f1.c \
           $(SOURCE_DIR)/heat.c \
           $(SOURCE_DIR)/vib.c \
           $(SOURCE_DIR)/lcdFont.c \
           $(SOURCE_DIR)/tensgui.c \
           $(SOURCE_DIR)/uidata.c \
           $(SOURCE_DIR)/rbuf.c \
           $(SOURCE_DIR)/therapy.c \
           $(SOURCE_DIR)/tensproto.c

PRESCS_SRCS = $(LIBS_DIR)/src/carrier.c \
             $(LIBS_DIR)/src/prescdata.c \
             $(LIBS_DIR)/src/prescoutput.c \
             $(LIBS_DIR)/src/rtdataoutput.c

ALL_SRCS = $(FWLIB_SRCS) $(CMSIS_SRCS) $(HAL_SRCS) $(APP_SRCS) $(PRESCS_SRCS)

OBJS = $(ALL_SRCS:%.c=$(OBJ_DIR)/%.o)
OBJS := $(OBJS:.s=$(OBJ_DIR)/%.o)
OBJS += $(OBJ_DIR)/$(STARTUP:.S=.o)

STARTUP_OBJ = $(OBJ_DIR)/$(STARTUP:.S=.o)

VPATH = $(FWLIB_SRCS:%.c=%) $(CMSIS_SRCS:%.c=%) $(SOURCE_DIR) $(LIBS_DIR)/src $(EEPROM_DIR)

.PHONY: all clean size

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR):
	mkdir -p $(OBJ_DIR) $(LST_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo Compiling $<
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.s | $(BUILD_DIR)
	@echo Assembling $<
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.S | $(BUILD_DIR)
	@echo Assembling $<
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -x assembler-with-cpp -c -o $@ $<

$(BUILD_DIR)/$(PROJECT).elf: $(OBJS)
	@echo Linking $@
	$(CC) $(OBJS) $(LDFLAGS) -T $(LDSCRIPT) -o $@

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo Creating binary $@
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	@echo Creating hex $@
	$(OBJCOPY) -O ihex $< $@

size: $(BUILD_DIR)/$(PROJECT).elf
	$(SIZE) $<

clean:
	rm -rf $(BUILD_DIR)