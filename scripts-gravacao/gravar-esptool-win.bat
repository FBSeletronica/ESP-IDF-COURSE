@echo off
setlocal enabledelayedexpansion

REM Configurações Fixas
set CHIP=esp32s2
set PORT=COM4
set BAUDRATE=460800    

REM Diretório dos arquivos binários e log
set BIN_DIR=bin
set LOGFILE=flash_log.txt

echo ========================== >> %LOGFILE%
echo Início da Gravação: %DATE% %TIME% >> %LOGFILE%
echo ========================== >> %LOGFILE%

REM Definição dos arquivos do firmware
set BOOTLOADER=%BIN_DIR%\bootloader.bin
set PARTITIONS=%BIN_DIR%\partition-table.bin
set FIRMWARE=%BIN_DIR%\DHT11.bin

:flash_loop
cls
echo ===========================================
echo  Gravando Firmware no %CHIP% - Porta %PORT%
echo ===========================================
echo Conecte a placa e pressione Enter para iniciar...
pause >nul

REM Gravar firmware e apagar a memória antes (usando --erase-all)
echo Apagando flash e gravando firmware...
python -m esptool --chip %CHIP% --port %PORT% --baud %BAUDRATE% --before default_reset --after no_reset write_flash --erase-all ^
    --flash_mode dio --flash_freq 80m --flash_size 4MB ^
    0x1000 %BOOTLOADER% ^
    0x10000 %FIRMWARE% ^
    0x8000 %PARTITIONS%
if %errorlevel% neq 0 (
    echo [%DATE% %TIME%] [ERRO] Falha na gravação do firmware na porta %PORT%! >> %LOGFILE%
    echo [ERRO] Falha na gravação do firmware!
    pause
    exit /b
)

echo [%DATE% %TIME%] [SUCESSO] %CHIP% gravado corretamente. >> %LOGFILE%
echo [SUCESSO] %CHIP% gravado corretamente.

echo ===========================================
echo Gravacao concluída. Remova a placa e pressione Enter para a próxima.
echo ===========================================
pause >nul
goto flash_loop
