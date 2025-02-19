#!/bin/bash

# Configurações Fixas
CHIP="esp32s2"
PORT="/dev/ttyACM0"  # Ajuste conforme necessário
BAUDRATE=460800

# Diretório dos arquivos binários e log
BIN_DIR="bin"
LOGFILE="flash_log.txt"

echo "==========================" >> "$LOGFILE"
echo "Início da Gravação: $(date)" >> "$LOGFILE"
echo "==========================" >> "$LOGFILE"

# Definição dos arquivos do firmware
BOOTLOADER="$BIN_DIR/bootloader.bin"
PARTITIONS="$BIN_DIR/partition-table.bin"
FIRMWARE="$BIN_DIR/DHT11.bin"

# Verificar se os arquivos existem
if [[ ! -f "$BOOTLOADER" ]]; then
    echo "[ERRO] Arquivo bootloader.bin não encontrado na pasta $BIN_DIR!" | tee -a "$LOGFILE"
    exit 1
fi

if [[ ! -f "$PARTITIONS" ]]; then
    echo "[ERRO] Arquivo partitions.bin não encontrado na pasta $BIN_DIR!" | tee -a "$LOGFILE"
    exit 1
fi

if [[ ! -f "$FIRMWARE" ]]; then
    echo "[ERRO] Arquivo firmware.bin não encontrado na pasta $BIN_DIR!" | tee -a "$LOGFILE"
    exit 1
fi

while true; do
    clear
    echo "==========================================="
    echo " Gravando Firmware no $CHIP - Porta $PORT"
    echo "==========================================="
    echo "Conecte a placa e pressione Enter para iniciar..."
    read -r

    # Gravar firmware e apagar a memória antes (usando --erase-all)
    echo "Apagando flash e gravando firmware..."
    python -m esptool --chip "$CHIP" --port "$PORT" --baud "$BAUDRATE" --before default_reset --after no_reset write_flash --erase-all \
        --flash_mode dio --flash_freq 80m --flash_size 4MB \
        0x1000 "$BOOTLOADER" \
        0x10000 "$FIRMWARE" \
        0x8000 "$PARTITIONS"

    if [[ $? -ne 0 ]]; then
        echo "[$(date)] [ERRO] Falha na gravação do firmware na porta $PORT!" | tee -a "$LOGFILE"
        echo "[ERRO] Falha na gravação do firmware!"
        exit 1
    fi

    echo "[$(date)] [SUCESSO] $CHIP gravado corretamente na porta $PORT." | tee -a "$LOGFILE"
    echo "[SUCESSO] $CHIP gravado corretamente na porta $PORT."

    echo "==========================================="
    echo "Gravação concluída. Remova a placa e pressione Enter para a próxima."
    echo "==========================================="
    read -r
done