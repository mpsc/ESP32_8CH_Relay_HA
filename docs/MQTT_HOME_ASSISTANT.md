# MQTT e Home Assistant

O firmware publica a **auto-descoberta (MQTT Discovery)** de forma que as 8
saídas e os sensores apareçam **automaticamente** no Home Assistant — sem
editar o `configuration.yaml`.

## Pré-requisitos

- Um broker MQTT (ex.: **Mosquitto**) integrado ao Home Assistant.
- No Home Assistant: **Configurações → Dispositivos e serviços → Adicionar
  integração → MQTT** (aponte para o mesmo broker).
- No firmware: informe **IP, porta, usuário e senha** do broker na página de
  configuração.

> Se você usa o **Mosquitto add-on** do Home Assistant, o IP costuma ser o do
> próprio Home Assistant (ex.: `192.168.0.35`) e a porta `1883`.

## Identificação do dispositivo

O `device_id` é derivado do MAC e tem o formato `esp32_8ch_XXXXXX` (ex.:
`esp32_8ch_124b00`). Todos os tópicos usam esse identificador.

- **Base:** `esp32/<device_id>`
- **Nome no HA:** o que você definiu em "Nome exibido no painel"

## Tópicos

| Tópico | Direção | Payload | Retido | Descrição |
|---|---|:---:|:---:|---|
| `esp32/<id>/status` | publica (LWT) | `online` / `offline` | ✅ | Disponibilidade do dispositivo |
| `esp32/<id>/out<N>/state` | publica | `ON` / `OFF` | ✅ | Estado atual da saída N |
| `esp32/<id>/out<N>/set` | assina | `ON` / `OFF` | — | Comando para a saída N |
| `esp32/<id>/rssi` | publica | ex.: `-62` | ✅ | Sinal Wi-Fi (dBm) |
| `esp32/<id>/uptime` | publica | segundos | ✅ | Tempo ligado |
| `esp32/<id>/ip` | publica | ex.: `192.168.0.102` | ✅ | Endereço IP |
| `esp32/<id>/cmd/restart` | assina | `PRESS` | — | Reinicia o dispositivo |

`<N>` vai de **1 a 8**.

## Entidades criadas no Home Assistant

| Entidade | Tipo | Tópico de descoberta |
|---|---|---|
| Saída 1…8 (nomes configurados) | `switch` | `<prefixo>/switch/<id>/outN/config` |
| Sinal Wi-Fi | `sensor` (signal_strength) | `<prefixo>/sensor/<id>/rssi/config` |
| Tempo ligado | `sensor` (duration) | `<prefixo>/sensor/<id>/uptime/config` |
| Endereço IP | `sensor` (diagnostic) | `<prefixo>/sensor/<id>/ip/config` |
| Reiniciar | `button` | `<prefixo>/button/<id>/restart/config` |

O **prefixo de descoberta** é configurável (padrão: `homeassistant`).

Todas as entidades ficam agrupadas sob **um único dispositivo**, com suporte a
`configuration_url` (link direto para o painel web a partir do HA).

## Testes manuais com `mosquitto_pub/sub`

Ligar a saída 1:

```bash
mosquitto_pub -h 192.168.0.35 -u USUARIO -P SENHA \
  -t esp32/esp32_8ch_124b00/out1/set -m ON
```

Desligar todas (via API HTTP):

```bash
curl "http://192.168.0.102/api/all?state=OFF"
```

Acompanhar o estado:

```bash
mosquitto_sub -h 192.168.0.35 -u USUARIO -P SENHA \
  -t "esp32/esp32_8ch_124b00/#" -v
```

## Reanúncio da descoberta

- A descoberta é publicada automaticamente ao conectar e sempre que o Home
  Assistant envia `online` no tópico `homeassistant/status`.
- Você também pode forçar pela página web ou pelo endpoint
  `GET /api/discovery`.

## Cartão de exemplo no Lovelace

```yaml
type: entities
title: Quadro de saídas
entities:
  - switch.teste_1
  - switch.saida_2
  - switch.saida_3
  - switch.saida_4
  - switch.saida_5
  - switch.saida_6
  - switch.saida_7
  - switch.saida_8
```

> Os `entity_id` são gerados pelo Home Assistant a partir dos nomes. Confirme
> em **Ferramentas de desenvolvedor → Estados**.

## Remover entidades órfãs

Se você renomear saídas ou trocar o prefixo, entidades antigas podem ficar
"órfãs". Remova-as na página **MQTT** da integração
(Configurações → Dispositivos e serviços → MQTT → tópicos descobertos).
