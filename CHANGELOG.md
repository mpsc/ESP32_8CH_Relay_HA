# Changelog

Todas as mudanças relevantes deste projeto são documentadas neste arquivo.

O formato segue o [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/)
e o projeto adere ao [Versionamento Semântico](https://semver.org/lang/pt-BR/).

## [Não lançado]

## [1.0.1] - 2026-10-13

### Corrigido

- **Botões "Ligar todas" / "Desligar todas" não funcionavam.** O nome `all` usado
  nos handlers `onclick` inline do HTML colidia com a propriedade legada
  `document.all` do navegador (que é resolvida antes do escopo global). A função
  foi renomeada para `setAll` e os botões passaram a ser vinculados por
  JavaScript, fora do escopo problemático dos handlers inline.

## [1.0.0] - 2026-10-13

### Adicionado

- Controle de 8 saídas (relés) com painel web responsivo (tema claro/escuro automático).
- Acionamento individual por canal e comandos "Ligar todas" / "Desligar todas".
- MQTT com auto-descoberta para Home Assistant: 8 `switch`, sensores de sinal
  Wi-Fi, tempo ligado e endereço IP, além de um botão de reiniciar.
- Disponibilidade via LWT (`online`/`offline`) e reanúncio em `homeassistant/status`.
- Portal cativo para configuração inicial (rede `ESP32-8CH-XXXX`, senha `12345678`).
- Configuração pela página web salva em NVS: Wi-Fi, servidor MQTT, nomes das
  saídas, polaridade dos relés e restauração de estado após reinício.
- Descoberta de redes Wi-Fi pela interface.
- Suporte a mDNS (`http://<id>.local`).
- API HTTP em JSON para integrações.

[1.0.1]: https://github.com/mpsc/ESP32_8CH_Relay_HA/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/mpsc/ESP32_8CH_Relay_HA/releases/tag/v1.0.0
