@echo off
REM Configurações para o ESP32-S2 com ESP-PROG usando OpenOCD
set OPENOCD_PATH=c:\Users\fabio\.espressif\tools\openocd-esp32\v0.12.0-esp32-20240821\openocd-esp32\bin\openocd.exe
set INTERFACE_CONFIG=interface/ftdi/esp32_devkitj_v1.cfg
set TARGET_CONFIG=target/esp32s2.cfg

REM Caminho dos arquivos binários
set BOOTLOADER_BIN=C:/Users/fabio/esp/ESP-IDF-COURSE/scripts-gravacao/bin/bootloader.bin
set PARTITION_TABLE_BIN=C:/Users/fabio/esp/ESP-IDF-COURSE/scripts-gravacao/bin/partition-table.bin
set APPLICATION_BIN=C:/Users/fabio/esp/ESP-IDF-COURSE/scripts-gravacao/bin/DHT11.bin

:LOOP
cls
REM Executa o OpenOCD e grava os arquivos diretamente
"%OPENOCD_PATH%" -f "%INTERFACE_CONFIG%" -f "%TARGET_CONFIG%" ^
    -c "init" ^
    -c "reset halt" ^
    -c "flash write_image erase %BOOTLOADER_BIN% 0x1000" ^
    -c "flash write_image erase %PARTITION_TABLE_BIN% 0x8000" ^
    -c "flash write_image erase %APPLICATION_BIN% 0x10000" ^
    -c "reset run" ^
    -c "shutdown"

REM Mensagem de conclusão e solicitação para troca de placa
echo Programação concluída.
echo Troque a placa e pressione ENTER para continuar...
pause >nul
goto LOOP
