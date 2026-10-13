# Como contribuir

Obrigado pelo interesse em contribuir com o **ESP32 · 8 Saídas (Relés)**! Este é um
projeto pessoal, mas sugestões, correções e melhorias são muito bem-vindas.

## Antes de abrir uma issue

- Verifique se já existe uma issue semelhante em aberto.
- Consulte o [Manual de utilização](docs/MANUAL.md) e a
  [solução de problemas](README.md#solução-de-problemas).
- Para bugs, inclua o máximo de contexto possível (veja o template de issue).

## Fluxo de contribuição (GitHub Flow)

1. Faça um *fork* do repositório.
2. Crie um branch descritivo a partir de `main`:
   ```bash
   git checkout -b fix/descricao-curta
   # ou
   git checkout -b feat/nova-funcionalidade
   ```
3. Faça as alterações seguindo o estilo do projeto (veja abaixo).
4. Compile localmente antes de enviar:
   ```bash
   arduino-cli compile --fqbn esp32:esp32:esp32 .
   ```
   Ajuste o caminho do sketch conforme descrito em
   [docs/INSTALACAO.md](docs/INSTALACAO.md#compilar-por-linha-de-comando).
5. Faça *commit* com uma mensagem clara (veja o padrão abaixo).
6. Envie o branch e abra um *Pull Request* explicando o que mudou e por quê.

## Padrão de mensagens de commit

Recomenda-se o estilo [Conventional Commits](https://www.conventionalcommits.org/pt-br/):

```
<tipo>(<escopo>): <descrição curta>

[corpo opcional]
```

Tipos comuns: `feat`, `fix`, `docs`, `refactor`, `chore`, `ci`.

Exemplos:

- `feat(mqtt): adiciona sensor de consumo`
- `fix(web): corrige botão de ligar todas`
- `docs: atualiza manual de utilização`

## Estilo de código

- **Idioma dos comentários e da documentação:** português (pt-BR).
- **Indentação:** 2 espaços; sem tabulações.
- **Fim de linha:** LF.
- Mantenha as funções pequenas e com nomes claros.
- Preserve a estrutura e as convenções já existentes no firmware.
- Textos embutidos na interface web ficam em `web_ui.h` (PROGMEM).

## Testes

Não há suíte de testes automatizados. Antes de abrir o PR:

- Compile sem *warnings* novos.
- Teste o painel web, os comandos individuais e os "ligar/desligar todas".
- Se possível, valide a auto-descoberta no Home Assistant.
- O workflow de CI (`.github/workflows/compile.yml`) também compila o sketch.

## Licença

Ao contribuir, você concorda que sua contribuição será licenciada sob a
licença [MIT](LICENSE) do projeto.
