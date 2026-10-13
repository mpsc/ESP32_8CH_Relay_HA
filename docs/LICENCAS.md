# Licenças e créditos

Este documento reúne as licenças do **próprio projeto** e os **créditos de
terceiros** (bibliotecas, plataforma e marcas) usados no firmware.

> Este projeto é **independente** e **não é afiliado, patrocinado ou endossado**
> pela Espressif Systems, pelo Arduino ou pelo Home Assistant.

## 1. Licença do projeto

O firmware e a interface web deste repositório são originais e distribuídos sob
a licença **MIT** — Copyright (c) 2026 **Marcos (mpsc)**. O texto completo está
em [`../LICENSE`](../LICENSE).

Isso significa que você pode usar, copiar, modificar e redistribuir este código,
inclusive comercialmente, desde que **mantenha o aviso de copyright e a licença**
originais.

## 2. Bibliotecas de terceiros

O repositório **não redistribui** o código dessas bibliotecas — cada pessoa as
instala pela Arduino IDE ou pelo `arduino-cli`. Ainda assim, elas são parte do
build e vão **linkadas no binário final**. Abaixo, os créditos e licenças:

| Componente | Autor / origem | Licença |
|---|---|---|
| **Arduino core for ESP32** (`esp32:esp32`) | Espressif Systems / comunidade Arduino | **LGPL-2.1** (core) e **Apache-2.0** (componentes do ESP-IDF) |
| **PubSubClient** | Nick O'Leary | **MIT** |
| **ArduinoJson** | Benoit Blanchon | **MIT** |

As demais APIs usadas (`WiFi`, `WebServer`, `DNSServer`, `ESPmDNS`,
`Preferences`, `ESP32`/`ESP`) fazem parte do **core ESP32**, sob a licença do
próprio core (LGPL-2.1 / Apache-2.0), e não de código deste repositório.

> Como essas bibliotecas **não são redistribuídas** aqui, a obrigação principal
> ao publicar/derivar este projeto é **creditá-las** e respeitar as licenças
> originais caso você decida embutir o código delas.

## 3. Recursos visuais e de interface

- A interface web (`web_ui.h`) foi escrita do zero e usa **apenas fontes do
  sistema** (`-apple-system`, `Segoe UI`, `Roboto`, `sans-serif`). **Não há
  fontes, ícones ou imagens de terceiros** embutidos.
- As capturas de tela em [`images/`](images/) são do próprio projeto e usam
  **dados fictícios**.

## 4. Marcas registradas

As marcas abaixo pertencem aos respectivos titulares e são citadas apenas de
forma **descritiva** (indicação de compatibilidade), sem sugerir vínculo ou
endosso:

| Marca | Titular |
|---|---|
| **ESP32** / **Espressif** | Espressif Systems (Shanghai) Co. Ltd. |
| **Arduino** | Arduino S.r.l. / Arduino AG |
| **Home Assistant** | Open Home Foundation / Nabu Casa |
| **MQTT** | OASIS Open (especificação) |

## 5. Isenção de responsabilidade

O software é fornecido "como está", sem garantias de qualquer tipo. Este projeto
comuta **cargas elétricas** e **não** se responsabiliza por danos a equipamentos,
instalações ou pessoas. Trabalhe com **110/220 V somente se tiver qualificação** —
veja [HARDWARE.md](HARDWARE.md).

---

> Nada aqui constitui aconselhamento jurídico. Para uso comercial ou em caso de
> dúvida sobre licenças, consulte um profissional.
