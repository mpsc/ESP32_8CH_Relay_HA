# ESP32 · 8 Saídas (Relés) com Painel Web + MQTT / Home Assistant

Firmware para **ESP32** que controla **8 relés (somente saídas)** com:

- **Painel web moderno** (responsivo, tema claro/escuro automático) para ligar/desligar as 8 saídas individualmente ou todas de uma vez
- **MQTT com auto-descoberta no Home Assistant** — as 8 saídas aparecem automaticamente como `switch`, além de sensores de diagnóstico (sinal Wi-Fi, tempo ligado, IP) e um botão de reiniciar
- **Portal cativo** — na primeira ligação (ou sem Wi-Fi salvo), o dispositivo cria a rede `ESP32-8CH-XXXX` e abre sozinho a página de configuração
- **Configuração 100% pela página web** — rede Wi-Fi, servidor MQTT, nomes das saídas, polaridade do relé e restauração de estado; tudo salvo na NVS (memória não volátil)
- **mDNS** — acesse pelo endereço `http://<id>.local`

## Hardware

| Item | Valor |
|---|---|
| Placa | ESP32-D0WD-V3 (ESP32 clássico, dual-core), 4 MB flash |
| Conector | USB via CP2102 (ou similar) |
| Saídas | GPIO **16, 17, 18, 19, 23, 25, 26, 27** |
| Relés | 8 módulos/módulo de 8 relés (comum ativo em nível baixo) |

> Se os relés ligarem "ao contrário", marque **"Relé ativo em nível baixo (invertido)"** na página de configuração.

## Bibliotecas

- **PubSubClient** (Nick O'Leary)
- **ArduinoJson** 7.x (Benoit Blanchon)

`WiFi`, `WebServer`, `DNSServer`, `Preferences` e `ESPmDNS` vêm do core do ESP32.

## Compilar e gravar

Com o Arduino IDE (ou o `arduino-cli` que vem embutido nele):

```bash
# Compilar (core esp32:esp32)
arduino-cli compile --fqbn esp32:esp32:esp32 ESP32_8CH_Relay_HA

# Gravar (ajuste a porta COM)
arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32 ESP32_8CH_Relay_HA
```

## Primeira configuração (portal cativo)

1. Sem Wi-Fi salvo, o ESP32 cria a rede **`ESP32-8CH-XXXX`** (senha `12345678`).
2. Conecte o celular/PC nessa rede — a página de configuração abre sozinha (ou acesse `http://192.168.4.1`).
3. Escolha a rede Wi-Fi na lista, informe o servidor MQTT (IP do Home Assistant) e salve.
4. O dispositivo reinicia e conecta à sua rede. Acesse `http://<id>.local` ou veja o IP no roteador / Home Assistant.

## Tópicos MQTT

Base: `esp32/<device_id>` (ex.: `esp32/esp32_8ch_154b00`)

| Tópico | Direção | Descrição |
|---|---|---|
| `<base>/status` | pub (LWT) | Disponibilidade `online`/`offline` |
| `<base>/outN/state` | pub | Estado da saída N (`ON`/`OFF`, retido) |
| `<base>/outN/set` | sub | Comando da saída N (`ON`/`OFF`) |
| `<base>/rssi` | pub | Sinal Wi-Fi (dBm) |
| `<base>/uptime` | pub | Tempo ligado (s) |
| `<base>/ip` | pub | Endereço IP |
| `<base>/cmd/restart` | sub | Reiniciar (`PRESS`) |

A auto-descoberta é publicada em `homeassistant/<component>/<device_id>/<entity>/config`
(o prefixo é configurável na página web).

## Endpoints da API web

| Endpoint | Descrição |
|---|---|
| `GET /api/state` | Estado completo (JSON) |
| `GET /api/out?ch=N&state=ON\|OFF\|TOGGLE` | Aciona uma saída |
| `GET /api/all?state=ON\|OFF` | Aciona todas |
| `GET /api/mqtt` | Informações e tópicos MQTT |
| `GET /api/scan[?rescan=1]` | Varredura de redes Wi-Fi |
| `GET /api/discovery` | Reenvia a auto-descoberta |
| `GET /api/restart` | Reinicia o dispositivo |

## Estrutura

```
ESP32_8CH_Relay_HA.ino   firmware (lógica, Wi-Fi, MQTT, web, portal cativo)
web_ui.h                 interface web (HTML/CSS/JS em PROGMEM)
```

## Notas

- Estado das saídas pode ser restaurado após reiniciar (configurável).
- Sem servidor MQTT configurado, o painel web funciona normalmente.
- Em modo portal cativo, após 5 minutos sem nenhum cliente conectado o dispositivo reinicia para tentar reconectar ao Wi-Fi salvo.
