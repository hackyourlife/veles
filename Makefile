#!/bin/make
export	PREFIX	:=	
export	CC	:=	$(PREFIX)gcc
export	LD	:=	$(CC)
export	OBJCOPY	:=	$(PREFIX)objcopy
export	NM	:=	$(PREFIX)nm
export	SIZE	:=	$(PREFIX)size
export	BIN2O	:=	bin2o
export	GLSLANG	:=	glslangValidator

export	NDEBUG

#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------
TARGET		:=	veles
INCLUDES	:=	include
SOURCES		:=	src
GLSLSOURCES	:=	glsl
BUILD		:=	build

ASAN		:=	#-fsanitize=address
OPT		:=	-O3 -g

ifdef NDEBUG
DEBUG		:=	-DNDEBUG
else
DEBUG		:=
endif

CFLAGS		:=	$(OPT) -Wall -std=gnu99 \
			-ffunction-sections -fdata-sections \
			$(INCLUDE) -DUNIX \
			-D_XOPEN_SOURCE=600 -D_DEFAULT_SOURCE \
			-DGL_GLEXT_PROTOTYPES -DVT240_NO_BUFFER \
			$(DEBUG) $(ASAN)

LIBS		:=	-lGL -lglut -lm
LDFLAGS		:=	-Wl,-x -Wl,--gc-sections $(OPT) $(ASAN)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
GLSLFILES	:=	$(foreach dir,$(GLSLSOURCES),$(notdir $(wildcard $(dir)/*.glsl)))

ifneq ($(BUILD),$(notdir $(CURDIR)))
#-------------------------------------------------------------------------------
export	DEPSDIR	:=	$(CURDIR)/$(BUILD)
export	OFILES	:=	$(CFILES:.c=.o) $(GLSLFILES:.glsl=.o)
export	VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(GLSLSOURCES),$(CURDIR)/$(dir)) $(CURDIR)
export	INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
				-I$(CURDIR)/$(BUILD)
export	OUTPUT	:=	$(CURDIR)/$(TARGET)

.PHONY: $(BUILD) clean all

$(BUILD):
	@echo compiling...
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo "[CLEAN]"
	@rm -rf $(BUILD) $(TFILES) $(OFILES) demo

$(TARGET): $(TFILES)

else

#-------------------------------------------------------------------------------
# main target
#-------------------------------------------------------------------------------
.PHONY: all

all: $(OUTPUT)

$(OUTPUT): $(TARGET).elf
	@cp $(TARGET).elf $(OUTPUT)

%.o: %.c
	@echo "[CC]    $(notdir $@)"
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@
	@#$(CC) -S $(CFLAGS) -o $(@:.o=.s) $< # create assembly file

%.o: %.glsl
	@echo "[GLSL]  $(notdir $@)"
	@$(GLSLANG) $<
	@$(BIN2O) -t -l$(subst .,_,$(basename $@)) -i$< -o$@

$(TARGET).elf: $(OFILES)
	@echo "[LD]    $(notdir $@)"
	@$(LD) $(LDFLAGS) $(OFILES) $(LIBS) -o $@ -Wl,-Map=$(@:.elf=.map)

-include $(DEPSDIR)/*.d

#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------
