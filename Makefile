# Joao Victor Pariz
# Projeto Final - Sensor HW-201
# SE/3 - Eletronica - Programacao Aplicada

CXX ?= g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -O2

# Toolchain de Cross-Compilacao para o processador ARM Cortex-A7 da STM32MP1
CROSS_PREFIX ?= arm-buildroot-linux-gnueabihf-
CROSS_CXX = $(CROSS_PREFIX)g++

SRCS = src/IrObstacleSensor.cpp src/main.cpp
TARGET_NATIVE = app_sensor
TARGET_ARM = app_sensor_arm

.PHONY: all cross doc clean

all: $(TARGET_NATIVE)

$(TARGET_NATIVE): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo [OK] Binario nativo para PC gerado com sucesso: $(TARGET_NATIVE)

cross: $(SRCS)
	$(CROSS_CXX) $(CXXFLAGS) -o $(TARGET_ARM) $^
	@echo [OK] Binario cross-compilado para ARM gerado com sucesso: $(TARGET_ARM)

doc:
	doxygen Doxyfile
	@echo [OK] Documentacao Doxygen gerada em docs/doxygen/html/

clean:
	rm -f $(TARGET_NATIVE) $(TARGET_ARM) $(TARGET_NATIVE).exe $(TARGET_ARM).exe *.o 2>/dev/null || del /f /q $(TARGET_NATIVE).exe $(TARGET_ARM).exe $(TARGET_NATIVE) $(TARGET_ARM) *.o 2>nul || true
