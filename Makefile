# Project Name
TARGET = ili9341_test

LDFLAGS += -u _printf_float

# Sources
CPP_SOURCES = ili9341_test.cpp $(wildcard src/*.cpp)

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
