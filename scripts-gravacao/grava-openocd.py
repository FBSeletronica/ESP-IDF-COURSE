import os
import subprocess

def gravar_firmware():
    OPENOCD_PATH = "C:/Users/fabio/.espressif/tools/openocd-esp32/v0.12.0-esp32-20240821/openocd-esp32/bin/openocd.exe"
    INTERFACE_CONFIG = "interface/ftdi/esp32_devkitj_v1.cfg"
    TARGET_CONFIG = "target/esp32s2.cfg"

    BOOTLOADER_BIN = "C:/Users/fabio/esp/ESP-IDF-COURSE/scripts-gravacao/bin/bootloader.bin"
    PARTITION_TABLE_BIN = "C:/Users/fabio/esp/ESP-IDF-COURSE/scripts-gravacao/bin/partition-table.bin"
    APPLICATION_BIN = "C:/Users/fabio/esp/ESP-IDF-COURSE/scripts-gravacao/bin/DHT11.bin"

    command = [
        OPENOCD_PATH, "-f", INTERFACE_CONFIG, "-f", TARGET_CONFIG,
        "-c", "init",
        "-c", "reset halt",
        "-c", f"flash write_image erase {BOOTLOADER_BIN} 0x1000",
        "-c", f"flash write_image erase {PARTITION_TABLE_BIN} 0x8000",
        "-c", f"flash write_image erase {APPLICATION_BIN} 0x10000",
        "-c", "reset run",
        "-c", "shutdown"
    ]

    subprocess.run(command)

if __name__ == "__main__":
    while True:
        os.system("cls" if os.name == "nt" else "clear")
        print("Iniciando gravação...")
        gravar_firmware()
        print("Programação concluída.")
        input("Troque a placa e pressione ENTER para continuar...")
