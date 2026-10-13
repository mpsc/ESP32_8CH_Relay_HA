# Instalação e gravação

## Pré-requisitos

- **Arduino IDE** 2.x **ou** o **arduino-cli** (o Arduino IDE 2.x já traz um
  `arduino-cli` embutido).
- Core **ESP32** (`esp32:esp32`). Versão testada: **2.0.17**.
- Bibliotecas:
  - **PubSubClient** (Nick O'Leary)
  - **ArduinoJson** (Benoit Blanchon), versão **7.x**

## Estrutura do sketch

```
ESP32_8CH_Relay_HA.ino   firmware principal
web_ui.h                 interface web (HTML/CSS/JS em PROGMEM)
```

> **Importante:** o Arduino exige que a **pasta** tenha o **mesmo nome do
> arquivo `.ino`**. Este repositório já se chama `ESP32_8CH_Relay_HA`, então
> basta cloná-lo e abrir `ESP32_8CH_Relay_HA/ESP32_8CH_Relay_HA.ino`. Se você
> renomear a pasta por algum motivo, mantenha o nome idêntico ao do `.ino`.

## Compilar pela Arduino IDE

1. Instale o suporte a ESP32 (Boards Manager → procure por **esp32** by Espressif).
2. Instale as bibliotecas PubSubClient e ArduinoJson (Library Manager).
3. Abra `ESP32_8CH_Relay_HA/ESP32_8CH_Relay_HA.ino`.
4. Selecione a placa **"ESP32 Dev Module"** (ou equivalente à sua) e a porta COM.
5. Clique em **Verificar** (compilar) e depois em **Carregar** (gravar).

## Compilar por linha de comando

Usando o `arduino-cli` (ajuste os caminhos conforme a sua instalação):

```bash
arduino-cli core update-index
arduino-cli core install esp32:esp32@2.0.17
arduino-cli lib install PubSubClient
arduino-cli lib install ArduinoJson

# A pasta do sketch deve ter o mesmo nome do .ino
arduino-cli compile --fqbn esp32:esp32:esp32 ESP32_8CH_Relay_HA

# Gravar (exemplo: porta COM3 no Windows, /dev/ttyUSB0 no Linux)
arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32 ESP32_8CH_Relay_HA
```

## Descobrir a placa/porta

```bash
# Lista as portas seriais e placas detectadas
arduino-cli board list

# Confirma o chip e o tamanho da flash
esptool --port COM3 flash_id
```

Saída esperada (exemplo real):

```
Chip is ESP32-D0WD-V3 (revision v3.1)
Detected flash size: 4MB
```

## Problemas comuns ao gravar

| Sintoma | Causa provável / solução |
|---|---|
| `Failed to connect to ESP32: Timed out` | Segure o botão **BOOT**, inicie a gravação e solte; verifique o cabo (precisa ser de dados) e a porta correta. |
| `Porta ocupada` / `Access denied` | Feche o Monitor Serial da IDE e outros programas que usem a porta. |
| `main file missing from sketch` | A pasta não tem o mesmo nome do `.ino` — renomeie-a. |
| Compila mas não liga | Confira a alimentação e a polaridade dos relés (veja [HARDWARE.md](HARDWARE.md)). |
| Não aparece a rede de configuração | Confira se há uma rede Wi-Fi salva; se houver e falhar, o portal sobe após 20 s. |

## Após gravar

Na primeira vez, o dispositivo sobe em **modo portal cativo**. Siga a
[primeira configuração](../README.md#primeira-configuração-portal-cativo).
