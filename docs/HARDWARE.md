# Hardware, ligação e segurança

## Alvo

| Item | Valor |
|---|---|
| Placa | **ESP32-D0WD-V3** (ESP32 clássico, dual-core, 240 MHz) |
| Memória | 4 MB de flash |
| Conexão USB | Conversor USB-serial CP2102 (ou CH340) |
| Tensão lógica | 3,3 V |

> O firmware também deve compilar para outras placas ESP32 clássicas
> (`esp32:esp32:esp32`). Variantes S2/S3/C3 exigiriam ajuste de pinos e do
> FQBN de compilação.

## Pinagem das saídas

Todas as 8 saídas são **somente saída digital**:

| Saída | GPIO | Observações |
|:---:|:---:|---|
| 1 | **16** | — |
| 2 | **17** | — |
| 3 | **18** | — |
| 4 | **19** | — |
| 5 | **23** | — |
| 6 | **25** | Também DAC1 (usado aqui como saída digital) |
| 7 | **26** | Também DAC2 (usado aqui como saída digital) |
| 8 | **27** | — |

Pinos escolhidos por segurança: **nenhum** é pino de *strapping* (0, 2, 12, 15)
nem pertence à flash interna (6–11), evitando comportamento indesejado no boot.

## Ligação de um módulo de relés

1. **VCC** do módulo → **5 V** do ESP32 (ou fonte externa de 5 V, recomendado
   para módulos com muitas bobinas).
2. **GND** do módulo → **GND** do ESP32 (**GND comum é obrigatório**).
3. **IN1…IN8** → **GPIO 16, 17, 18, 19, 23, 25, 26, 27** (na ordem das saídas).

```
ESP32                      Módulo de relés
 GPIO16 ── IN1                ┌───────────┐
 GPIO17 ── IN2                │  RELÉ 1..8│   COM / NO / NC
 GPIO18 ── IN3   ─────────────┤ IN1..IN8  │   → cargas
 GPIO19 ── IN4                │           │
 GPIO23 ── IN5                │ VCC  GND  │
 GPIO25 ── IN6                └─┬─────┬───┘
 GPIO26 ── IN7                  │     │
 GPIO27 ── IN8                5V    GND
```

### Nível de acionamento (ativo em LOW x HIGH)

A maioria dos módulos de relé é **acionada em nível BAIXO** (a entrada vai a
GND para ligar). Se, com o painel mostrando "desligado", os relés estiverem
energizados, marque a opção **"Relé ativo em nível baixo (invertido)"** na
página de configuração. Isso ajusta tudo por software, sem regravar.

## Alimentação

- Alimentar o ESP32 pela USB é suficiente para testes.
- Com **8 relés** acionados simultaneamente, a corrente das bobinas pode
  ultrapassar o que o regulador da placa fornece. Para uso definitivo, use
  **fonte externa de 5 V** para o módulo e garanta o **GND comum**.
- Adicione capacitores de desacoplamento (ex.: 470–1000 µF) junto à alimentação
  do módulo de relés para reduzir ruído e quedas de tensão.

## ⚠️ Segurança elétrica

- Relés podem comutar **tensão de rede (110/220 V AC)**. Isso é **perigoso**.
- Trabalhe sempre com o circuito **desenergizado** e respeite as normas locais
  (no Brasil, NBR 5410).
- Prefira módulos de relé com **optoacoplador** e mantenha o lado de alta
  tensão fisicamente separado do lado de baixa tensão.
- Para cargas indutivas (motores, bombas, contatores), use **snubber**
  (RC) e/ou relés adequados; o arco de comutação desgasta contatos comuns.
- Este projeto não tem certificação elétrica. Use por sua conta e risco.
