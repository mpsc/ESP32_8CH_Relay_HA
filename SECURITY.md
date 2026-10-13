# Política de segurança

## Escopo

Este projeto é um firmware para **ESP32** que controla relés e se comunica por
Wi-Fi e MQTT. Esta política cobre o código-fonte deste repositório.

## Versões suportadas

Apenas a versão mais recente publicada (`main` / última *release*) recebe
correções de segurança.

| Versão | Suportada |
|--------|-----------|
| 1.0.x  | ✅        |
| < 1.0  | ❌        |

## Relatando uma vulnerabilidade

Por favor **não abra uma issue pública** para vulnerabilidades de segurança.
Relate de forma privada:

- Use a aba **Security → Report a vulnerability** do repositório no GitHub, ou
- Entre em contato diretamente com o mantenedor.

Inclua, se possível:

- Descrição do problema e seu impacto.
- Passos para reproduzir ou uma prova de conceito.
- Versão do firmware, placa e ambiente.

O retorno costuma ocorrer em alguns dias. Correções serão publicadas assim que
possível, com o devido crédito (se desejado).

## Boas práticas de implantação

Este é um dispositivo de automação residencial. Para uma implantação segura:

- **Senhas:** troque a senha do portal cativo (`12345678` é apenas o padrão de
  fábrica) e use credenciais MQTT dedicadas.
- **Rede:** mantenha o dispositivo em uma rede Wi-Fi confiável; a interface web
  não usa autenticação e fica acessível a quem estiver na mesma rede.
- **MQTT:** prefira um broker com autenticação e, quando possível, TLS.
- **Elétrica:** relés podem comutar **tensão de rede (110/220 V)**. Trabalhe com
  o circuito desenergizado e siga as normas locais.

## Segredos no repositório

Este repositório **não deve conter** credenciais reais. Wi-Fi e MQTT são
configurados em tempo de execução e gravados apenas na NVS do dispositivo.
Nunca faça *commit* de senhas, tokens ou chaves.
