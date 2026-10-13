# API HTTP (JSON)

O firmware expõe uma API simples, usada pelo próprio painel web e disponível
para integrações externas (scripts, outros sistemas).

- **Base URL:** `http://<dispositivo>/` (ex.: `http://192.168.0.102/` ou
  `http://esp32_8ch_124b00.local/`)
- **Formato:** JSON (exceto `/save`, que é um formulário, e respostas de texto simples)
- **Autenticação:** nenhuma (a API fica acessível a quem estiver na mesma rede —
  veja [SECURITY.md](../SECURITY.md))

## Páginas

| Método | Rota | Descrição |
|---|---|---|
| `GET` | `/` | Painel de controle (ou a página de configuração, se estiver em modo AP) |
| `GET` | `/config` | Página de configuração |
| `POST` | `/save` | Salva a configuração (formulário) e reinicia |

## Endpoints de dados

### `GET /api/state`

Estado completo do dispositivo.

```json
{
  "name": "ESP32 8 Canais",
  "ssid": "MS",
  "ip": "192.168.0.102",
  "rssi": -62,
  "ap": false,
  "mqtt": true,
  "mqttMsg": "Conectado",
  "uptime": 23,
  "heap": 228100,
  "fw": "1.0.1",
  "chip": "ESP32-D0WD-V3 r3",
  "outs": [
    { "name": "Teste 1", "pin": 16, "on": false },
    { "name": "Saída 2", "pin": 17, "on": false }
  ]
}
```

### `GET /api/out?ch=<1-8>&state=<ON|OFF|TOGGLE>`

Aciona uma saída. Retorna `ON` ou `OFF` (texto simples).

```bash
curl "http://192.168.0.102/api/out?ch=1&state=ON"
curl "http://192.168.0.102/api/out?ch=1&state=TOGGLE"
```

| Parâmetro | Valores | Obrigatório |
|---|---|---|
| `ch` | 1 a 8 | sim |
| `state` | `ON`, `OFF`, `TOGGLE` | sim |

### `GET /api/all?state=<ON|OFF>`

Aciona **todas** as saídas de uma vez.

```bash
curl "http://192.168.0.102/api/all?state=ON"
curl "http://192.168.0.102/api/all?state=OFF"
```

### `GET /api/mqtt`

Informações do broker e a lista de tópicos por canal.

```json
{
  "host": "192.168.0.35",
  "port": 1883,
  "user": "mqtt",
  "clientId": "esp32_8ch_124b00",
  "prefix": "homeassistant",
  "heap": 228100,
  "avail": "esp32/esp32_8ch_124b00/status",
  "topics": [
    {
      "ch": 1,
      "name": "Teste 1",
      "state": "esp32/esp32_8ch_124b00/out1/state",
      "set": "esp32/esp32_8ch_124b00/out1/set"
    }
  ]
}
```

### `GET /api/scan`

Resultado (assíncrono) da varredura de redes Wi-Fi.

```json
{
  "nets": [
    { "ssid": "MinhaCasa", "rssi": -55, "lock": true },
    { "ssid": "Visitantes", "rssi": -72, "lock": false }
  ]
}
```

Enquanto a varredura ainda está em andamento, retorna `{ "scanning": true }`.
Use `GET /api/scan?rescan=1` para disparar uma nova varredura.

### `GET /api/discovery`

Reenvia toda a auto-descoberta MQTT e o estado atual. Requer MQTT conectado
(caso contrário, retorna HTTP 503).

```bash
curl "http://192.168.0.102/api/discovery"
```

### `GET /api/restart`

Reinicia o dispositivo.

```bash
curl "http://192.168.0.102/api/restart"
```

## Códigos de status

| Código | Significado |
|---|---|
| `200` | Sucesso |
| `400` | Parâmetros inválidos (ex.: canal fora de 1–8) |
| `404` | Rota inexistente (fora do modo portal cativo) |
| `503` | MQTT não conectado (`/api/discovery`) |

## Exemplos rápidos

PowerShell:

```powershell
Invoke-RestMethod "http://192.168.0.102/api/state"
Invoke-RestMethod "http://192.168.0.102/api/out?ch=3&state=TOGGLE"
```

Node.js:

```js
await fetch("http://192.168.0.102/api/all?state=OFF");
```
