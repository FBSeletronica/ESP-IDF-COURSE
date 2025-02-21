#!/bin/bash

# Configurações para o ESP32-S2 com ESP-PROG usando OpenOCD
OPENOCD_PATH="$HOME/.espressif/tools/openocd-esp32/v0.12.0-esp32-20240821/openocd-esp32/bin/openocd"
INTERFACE_CONFIG="interface/ftdi/esp32_devkitj_v1.cfg"
TARGET_CONFIG="target/esp32s2.cfg"

# Caminho dos arquivos binários
BOOTLOADER_BIN="$HOME/esp/ESP-IDF-COURSE/scripts-gravacao/bin/bootloader.bin"
PARTITION_TABLE_BIN="$HOME/esp/ESP-IDF-COURSE/scripts-gravacao/bin/partition-table.bin"
APPLICATION_BIN="$HOME/esp/ESP-IDF-COURSE/scripts-gravacao/bin/DHT11.bin"

while true; do
    clear
    echo "Iniciando gravação..."
    
    # Executa o OpenOCD e grava os arquivos diretamente
    "$OPENOCD_PATH" -f "$INTERFACE_CONFIG" -f "$TARGET_CONFIG" \
        -c "init" \
        -c "reset halt" \
        -c "flash write_image erase $BOOTLOADER_BIN 0x1000" \
        -c "flash write_image erase $PARTITION_TABLE_BIN 0x8000" \
        -c "flash write_image erase $APPLICATION_BIN 0x10000" \
        -c "reset run" \
        -c "shutdown"
    
    # Mensagem de conclusão e solicitação para troca de placa
    echo "Programação concluída."
    read -p "Troque a placa e pressione ENTER para continuar..."
done