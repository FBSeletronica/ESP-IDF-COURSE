O `picocom` é um terminal serial simples e eficiente para Linux, útil para comunicação  com microcontroladores, placas de desenvolvimento (ESP32, STM32, Arduino, Raspberry Pi) e equipamentos industriais via portas seriais na linha de comandos do Linux. Ele é uma alternativa mais leve ao `minicom` e ao `screen`.

## Instalação do picocom

No Linux, você pode instalar o `picocom` via gerenciador de pacotes:

```bash
sudo apt update && sudo apt install picocom -y
```


Após a instalação, verifique se o `picocom` está disponível:

```bash
picocom --version
```

## Identificando a Porta Serial

Antes de conectar ao dispositivo, descubra a porta serial atribuída pelo sistema:

```bash
ls /dev/ttyUSB*   # Para dispositivos USB (ex: adaptadores USB-Serial)
ls /dev/ttyS*     # Para portas seriais nativas
ls /dev/ttyACM*   # Para placas com interface CDC (ex: Arduino Leonardo, ESP32-S3)
```

Você também pode usar o `dmesg` para verificar logs recentes:

```bash
dmesg | grep tty
```

Se a porta não estiver aparecendo, verifique permissões:

```bash
ls -l /dev/ttyUSB0
```

Caso o seu usuário não tenha acesso, adicione-o ao grupo `dialout`:

```bash
sudo usermod -aG dialout $USER
```

Depois, reinicie a sessão para aplicar as permissões.


```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```


## Conectando ao Dispositivo

A sintaxe básica para abrir uma conexão é:


```bash
picocom -b <baudrate> /dev/<porta>
```

### Exemplo:

Conectar-se a um ESP32 na porta `/dev/ttyUSB0`  com baud rate de 115200:


```bash
picocom -b 115200 /dev/ttyUSB0
```


### Opções úteis:

- `-b 115200` → Define a velocidade da comunicação (baud rate)
- `-r` → Ativa comunicação **raw**, evitando caracteres extras
- `-l` → Ativa o modo "local echo" (útil se os caracteres não estiverem aparecendo)
- `-f` → Define controle de fluxo (`-f h` para hardware, `-f n` para nenhum)
- `-q` → Inicia sem exibir mensagens de conexão

Exemplo de conexão com fluxo de hardware desativado:


```bash
picocom -b 9600 -f n /dev/ttyUSB0
```


### Comandos do `picocom`

Os comandos do `picocom` são ativados pressionando **Ctrl + A** seguido de outra tecla.  
**Exemplo**: Para sair do `picocom`, pressione **Ctrl + A**, depois **Ctrl + X**.

|Comando|Ação|
|---|---|
|**Ctrl + A, Ctrl + X**|Sai do `picocom`|
|**Ctrl + A, Ctrl + Q**|Sai sem resetar a porta serial|
|**Ctrl + A, Ctrl + B**|Define um novo baud rate|
|**Ctrl + A, Ctrl + U**|Aumenta o baud rate (baud-up)|
|**Ctrl + A, Ctrl + D**|Diminui o baud rate (baud-down)|
|**Ctrl + A, Ctrl + I**|Altera o número de bits de dados|
|**Ctrl + A, Ctrl + J**|Altera o número de bits de parada|
|**Ctrl + A, Ctrl + F**|Alterna o modo de controle de fluxo|
|**Ctrl + A, Ctrl + Y**|Alterna o modo de paridade|
|**Ctrl + A, Ctrl + P**|Pulsa o DTR (Data Terminal Ready)|
|**Ctrl + A, Ctrl + T**|Alterna o estado do DTR|
|**Ctrl + A, Ctrl + G**|Alterna o estado do RTS (Request to Send)|
|**Ctrl + A, Ctrl + C**|Alterna o eco local|
|**Ctrl + A, Ctrl + W**|Escreve dados em formato hexadecimal|
|**Ctrl + A, Ctrl + S**|Envia um arquivo|
|**Ctrl + A, Ctrl + R**|Recebe um arquivo|
|**Ctrl + A, Ctrl + V**|Mostra as configurações da porta serial|
|**Ctrl + A, Ctrl + H**|Exibe a lista de comandos disponíveis|

## Finalizando a Sessão

Para sair corretamente do `picocom`:

1. **CTRL + A**, depois **CTRL + Q** (sai sem resetar a porta)
2. **CTRL + A**, depois **CTRL + X** (sai e reseta a porta)

Caso o terminal fique preso, pressione `CTRL + Z`, encontre o **PID** e mate o processo:


```bash
ps aux | grep picocom
kill -9 <PID>
```



Fonte: Manual do Picocom: [picocom(8) - Linux man page](https://linux.die.net/man/8/picocom)
