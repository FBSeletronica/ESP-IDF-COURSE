import os
import sys
import time
import subprocess

# Configurações Fixas
CHIP = "esp32s2"
PORT = "COM4"  # Para Linux/macOS, use algo como "/dev/ttyUSB0"
BAUDRATE = "460800"

# Diretório dos arquivos binários e log
BIN_DIR = "bin"
LOGFILE = "flash_log.txt"

# Definição dos arquivos do firmware
BOOTLOADER = os.path.join(BIN_DIR, "bootloader.bin")
PARTITIONS = os.path.join(BIN_DIR, "partition-table.bin")
FIRMWARE = os.path.join(BIN_DIR, "DHT11.bin")

# Função para escrever no log
def log_message(message):
    timestamp = time.strftime("[%Y-%m-%d %H:%M:%S]")
    log_entry = f"{timestamp} {message}\n"
    print(log_entry, end="")  # Exibe no terminal
    with open(LOGFILE, "a") as log_file:
        log_file.write(log_entry)

# Verificar se os arquivos existem
for file in [BOOTLOADER, PARTITIONS, FIRMWARE]:
    if not os.path.exists(file):
        log_message(f"[ERRO] Arquivo {file} não encontrado!")
        sys.exit(1)

while True:
    os.system("cls" if os.name == "nt" else "clear")  # Limpa a tela

    print("===========================================")
    print(f" Gravando Firmware no {CHIP} - Porta {PORT}")
    print("===========================================")
    input("Conecte a placa e pressione Enter para iniciar...")

    # Apagar flash e gravar firmware
    log_message("Apagando flash e gravando firmware...")
    command = [
        "python", "-m", "esptool",
        "--chip", CHIP,
        "--port", PORT,
        "--baud", BAUDRATE,
        "--before", "default_reset",
        "--after", "no_reset",
        "write_flash", "--erase-all",
        "--flash_mode", "dio",
        "--flash_freq", "80m",
        "--flash_size", "4MB",
        "0x1000", BOOTLOADER,
        "0x10000", FIRMWARE,
        "0x8000", PARTITIONS
    ]

    result = subprocess.run(command, stdout=sys.stdout, stderr=sys.stderr)

    if result.returncode != 0:
        log_message(f"[ERRO] Falha na gravação do firmware na porta {PORT}!")
        sys.exit(1)

    log_message(f"[SUCESSO] {CHIP} gravado corretamente na porta {PORT}.")

    print("===========================================")
    print("Gravação concluída. Remova a placa e pressione Enter para a próxima.")
    print("===========================================")
    input()
