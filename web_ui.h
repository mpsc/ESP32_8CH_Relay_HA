#pragma once
/*
 * Interface web (HTML/CSS/JS) embutida em PROGMEM.
 * Placeholders substituídos em runtime:
 *   %CSS% %TITLE% %NAME% %SSID% %PASSPH% %MHOST% %MPORT% %MUSER% %MPASSPH%
 *   %NAMES% %INV% %REST% %DISC% %INTRO% %BACK%
 */

const char PAGE_CSS[] PROGMEM = R"rawliteral(
<style>
:root{
  --bg:#e8ebef; --panel:#f8f9fb; --panel2:#eef1f4; --line:#d4dae1; --line2:#c3cbd4;
  --tx:#181f28; --mut:#5b6673; --bus:#20272f; --bustx:#cfd8e2;
  --amber:#c06a12; --amber2:#e79a3c; --glow:rgba(192,106,18,.28);
  --off:#93a0ac; --ok:#2c8f60; --bad:#c0392b;
  --mono:ui-monospace,"Cascadia Mono","SFMono-Regular",Consolas,monospace;
  --sans:system-ui,-apple-system,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;
  --r:10px;
}
@media (prefers-color-scheme:dark){
:root{
  --bg:#0f141a; --panel:#171e26; --panel2:#1c242d; --line:#2a3540; --line2:#36434f;
  --tx:#e6eef6; --mut:#8b98a6; --bus:#0a0e12; --bustx:#b9c5d1;
  --amber:#f0a03c; --amber2:#ffc272; --glow:rgba(240,160,60,.34);
  --off:#54626f; --ok:#3fb27f; --bad:#e5544b;
}}
*{box-sizing:border-box}
html{-webkit-text-size-adjust:100%}
body{margin:0;background:var(--bg);color:var(--tx);
  font-family:var(--sans);font-size:15px;line-height:1.45;
  padding:18px 14px calc(28px + env(safe-area-inset-bottom))}
.wrap{max-width:760px;margin:0 auto}
h1{font-size:19px;margin:0;letter-spacing:-.01em}
.sub{color:var(--mut);font-size:12.5px;margin:2px 0 0}
a{color:inherit}

/* ---- cabeçalho ---- */
.top{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:14px}
.brand{display:flex;align-items:center;gap:12px;min-width:0}
.mark{flex:none;width:40px;height:40px;border-radius:11px;background:var(--bus);color:var(--bustx);
  display:grid;place-items:center;font-family:var(--mono);font-weight:700;font-size:17px;
  box-shadow:inset 0 0 0 1px rgba(255,255,255,.06)}

/* ---- barra de status (data em mono) ---- */
.bus{display:flex;flex-wrap:wrap;gap:0;background:var(--bus);color:var(--bustx);
  border-radius:var(--r);overflow:hidden;margin-bottom:16px}
.bus .cell{flex:1 1 auto;min-width:130px;padding:9px 13px;border-right:1px solid rgba(255,255,255,.07)}
.bus .cell:last-child{border-right:0}
.bus .k{font-size:10.5px;letter-spacing:.04em;color:#8fa0b0}
.bus .v{font-family:var(--mono);font-size:13px;margin-top:2px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.bus .v.ok::before,.bus .v.bad::before{content:"";display:inline-block;width:7px;height:7px;border-radius:50%;
  margin-right:7px;vertical-align:middle;background:#5b6673}
.bus .v.ok::before{background:#3fb27f;box-shadow:0 0 7px #3fb27f}
.bus .v.bad::before{background:#e5544b}

/* ---- grid de canais ---- */
.grid{display:grid;grid-template-columns:1fr;gap:8px}
@media(min-width:600px){.grid{grid-template-columns:1fr 1fr}}
.ch{display:flex;align-items:center;gap:12px;background:var(--panel);
  border:1px solid var(--line);border-left:3px solid var(--line2);
  border-radius:var(--r);padding:11px 13px;transition:border-color .18s,background .18s}
.ch.on{border-left-color:var(--amber)}
.ch .n{flex:none;width:26px;height:26px;border-radius:7px;background:var(--panel2);
  border:1px solid var(--line);display:grid;place-items:center;font-family:var(--mono);
  font-size:12.5px;color:var(--mut)}
.ch.on .n{background:var(--amber);border-color:var(--amber);color:#fff}
.ch .meta{min-width:0;flex:1}
.ch .nm{font-size:14.5px;font-weight:600;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.ch .pin{font-family:var(--mono);font-size:11px;color:var(--mut);margin-top:1px}
.lamp{flex:none;width:11px;height:11px;border-radius:50%;background:var(--off);
  box-shadow:inset 0 0 0 1px rgba(0,0,0,.25);transition:background .18s,box-shadow .18s}
.ch.on .lamp{background:var(--amber2);box-shadow:0 0 0 3px var(--glow),0 0 10px var(--amber2)}

/* ---- switch tipo rocker ---- */
.rock{flex:none;position:relative;width:52px;height:30px;border:0;padding:0;cursor:pointer;
  border-radius:16px;background:var(--off);transition:background .18s;outline-offset:2px}
.rock span{position:absolute;top:3px;left:3px;width:24px;height:24px;border-radius:50%;background:#fff;
  box-shadow:0 1px 3px rgba(0,0,0,.35);transition:transform .18s}
.rock[aria-pressed="true"]{background:var(--amber)}
.rock[aria-pressed="true"] span{transform:translateX(22px)}
.rock:focus-visible{outline:2px solid var(--amber2)}

/* ---- botões ---- */
.bar{display:flex;flex-wrap:wrap;gap:8px;margin:16px 0}
.btn{font:inherit;font-size:14px;background:var(--amber);color:#fff;border:0;border-radius:9px;
  padding:10px 15px;cursor:pointer;text-decoration:none;display:inline-block}
.btn.ghost{background:transparent;color:var(--tx);border:1px solid var(--line2)}
.btn.sm{padding:8px 12px;font-size:13px}
.btn:active{transform:translateY(1px)}

/* ---- diagnóstico ---- */
details{background:var(--panel);border:1px solid var(--line);border-radius:var(--r);padding:0 14px;margin-top:4px}
summary{cursor:pointer;font-size:14px;padding:13px 0;font-weight:600}
summary::marker{color:var(--mut)}
.kv{display:flex;justify-content:space-between;gap:14px;padding:8px 0;border-top:1px solid var(--line);font-size:13.5px}
.kv .k{color:var(--mut)}
.kv .v{font-family:var(--mono);font-size:12.5px;text-align:right;word-break:break-all;user-select:all}
.topic{padding:9px 0;border-top:1px solid var(--line);font-family:var(--mono);font-size:12px;color:var(--mut);word-break:break-all}
.topic b{color:var(--tx);font-weight:600;font-family:var(--sans);font-size:13px}

/* ---- formulário (config) ---- */
form{margin:0}
fieldset{border:1px solid var(--line);border-radius:var(--r);padding:6px 15px 16px;margin:0 0 14px;background:var(--panel)}
legend{font-size:12px;font-weight:700;letter-spacing:.03em;color:var(--mut);padding:0 6px}
label{display:block;font-size:12.5px;color:var(--mut);margin:12px 0 5px}
label.chk{display:flex;align-items:center;gap:9px;color:var(--tx);font-size:14px}
label.chk input{width:auto;margin:0}
input{width:100%;font:inherit;font-size:14.5px;padding:10px 11px;border-radius:8px;
  border:1px solid var(--line2);background:var(--bg);color:var(--tx)}
input:focus-visible{outline:2px solid var(--amber2);border-color:var(--amber)}
.two{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.nets{display:flex;flex-direction:column;gap:6px;margin:4px 0 6px}
button.net{font:inherit;font-size:13.5px;background:var(--panel2);color:var(--tx);border:1px solid var(--line);
  border-radius:8px;padding:10px;display:flex;justify-content:space-between;gap:10px;cursor:pointer;text-align:left}
button.net:hover{border-color:var(--amber)}
.note{color:var(--mut);font-size:12.5px;margin:6px 0 0}
.foot{color:var(--mut);font-size:12px;text-align:center;margin-top:22px;font-family:var(--mono)}
@media (prefers-reduced-motion:reduce){*{transition:none !important;animation:none !important}}
@keyframes rise{from{opacity:0;transform:translateY(6px)}to{opacity:1;transform:none}}
.ch{animation:rise .3s ease both}
</style>
)rawliteral";

/* ============================ PAINEL ============================ */
const char PAGE_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="pt-BR"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#20272f"><title>%TITLE%</title>%CSS%</head><body>
<div class="wrap">
  <header class="top">
    <div class="brand"><div class="mark">8</div>
      <div><h1 id="dev">ESP32 8CH</h1><p class="sub" id="sub">Quadro de saídas</p></div></div>
    <a class="btn ghost sm" href="/config">Ajustes</a>
  </header>

  <section class="bus">
    <div class="cell"><div class="k">REDE</div><div class="v" id="ssid">-</div></div>
    <div class="cell"><div class="k">IP</div><div class="v" id="ip">-</div></div>
    <div class="cell"><div class="k">SINAL</div><div class="v" id="rssi">-</div></div>
    <div class="cell"><div class="k">MQTT</div><div class="v" id="mqtt">-</div></div>
    <div class="cell"><div class="k">LIGADO HÁ</div><div class="v" id="up">-</div></div>
  </section>

  <section class="grid" id="grid"></section>

  <div class="bar">
    <button class="btn" onclick="all('ON')">Ligar todas</button>
    <button class="btn ghost" onclick="all('OFF')">Desligar todas</button>
  </div>

  <details>
    <summary>Diagnóstico MQTT</summary>
    <div id="minfo"></div>
    <div id="topics"></div>
  </details>

  <p class="foot" id="foot"></p>
</div>
<script>
const $=id=>document.getElementById(id);
let built=false;
function el(t,c,x){const e=document.createElement(t);if(c)e.className=c;if(x!=null)e.textContent=x;return e;}

function build(outs){
  const g=$('grid');g.innerHTML='';
  outs.forEach((o,i)=>{
    const row=el('div','ch');row.dataset.i=i;
    row.appendChild(el('span','n',i+1));
    const meta=el('div','meta');
    meta.appendChild(el('div','nm',o.name));
    meta.appendChild(el('div','pin','GPIO '+o.pin));
    row.appendChild(meta);
    row.appendChild(el('span','lamp'));
    const b=el('button','rock');b.type='button';b.setAttribute('aria-label','Alternar '+o.name);
    b.appendChild(el('span'));
    b.onclick=()=>set(i, b.getAttribute('aria-pressed')!=='true');
    row.appendChild(b);
    g.appendChild(row);
  });
  built=true;
}
function paint(i,on){
  const row=document.querySelector('.ch[data-i="'+i+'"]');if(!row)return;
  row.classList.toggle('on',!!on);
  row.querySelector('.rock').setAttribute('aria-pressed',on?'true':'false');
}
function kv(box,k,v,mono){
  const r=el('div','kv');r.appendChild(el('span','k',k));
  const val=el('span','v',v);if(mono)val.style.fontFamily='var(--mono)';r.appendChild(val);box.appendChild(r);
}
function uptime(s){
  const d=Math.floor(s/86400),h=Math.floor(s%86400/3600),m=Math.floor(s%3600/60);
  return (d?d+'d ':'')+h+'h '+m+'m';
}
async function loadInfo(){
  try{
    const m=await (await fetch('/api/mqtt')).json();
    const box=$('minfo');box.innerHTML='';
    kv(box,'Broker',m.host?m.host+':'+m.port:'(não configurado)');
    kv(box,'Usuário',m.user||'(sem autenticação)');
    kv(box,'Client ID',m.clientId);
    kv(box,'Prefixo discovery',m.prefix);
    kv(box,'Memória livre',m.heap+' bytes');
    const tp=$('topics');tp.innerHTML='';
    m.topics.forEach(t=>{
      const d=el('div','topic');
      d.appendChild(el('b',null,t.ch+'. '+t.name));
      d.appendChild(el('div',null,'estado: '+t.state));
      d.appendChild(el('div',null,'comando: '+t.set));
      tp.appendChild(d);
    });
    const ex=el('div','topic');
    ex.appendChild(el('div',null,'Teste (terminal): mosquitto_pub -h '+(m.host||'BROKER')+
      (m.user?' -u '+m.user+' -P SENHA':'')+' -t '+m.topics[0].set+' -m ON'));
    tp.appendChild(ex);
  }catch(e){}
}
async function load(){
  try{
    const s=await (await fetch('/api/state')).json();
    if(!built)build(s.outs);
    $('dev').textContent=s.name;
    $('sub').textContent='Quadro de saídas · '+s.ip;
    document.title=s.name;
    s.outs.forEach((o,i)=>paint(i,o.on));
    $('ssid').textContent=s.ssid;
    $('ip').textContent=s.ip;
    $('rssi').textContent=s.ap?'-':s.rssi+' dBm';
    const mq=$('mqtt');mq.textContent=s.mqttMsg;mq.className='v '+(s.mqtt?'ok':'bad');
    $('up').textContent=uptime(s.uptime);
    $('foot').textContent=s.name+' · firmware '+s.fw+' · '+s.chip;
    if(!window._info){window._info=1;loadInfo();}
  }catch(e){$('mqtt').textContent='sem conexão';$('mqtt').className='v bad';}
}
async function set(i,on){paint(i,on);await fetch('/api/out?ch='+(i+1)+'&state='+(on?'ON':'OFF'));load();}
async function all(v){await fetch('/api/all?state='+v);load();}
load();setInterval(load,3000);
</script></body></html>
)rawliteral";

/* ============================ CONFIG ============================ */
const char PAGE_CONFIG[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="pt-BR"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#20272f"><title>%TITLE%</title>%CSS%</head><body>
<div class="wrap">
  <header class="top">
    <div class="brand"><div class="mark">8</div>
      <div><h1>%TITLE%</h1><p class="sub">%INTRO%</p></div></div>
    %BACK%
  </header>

  <form method="POST" action="/save">
    <fieldset>
      <legend>Dispositivo</legend>
      <label>Nome exibido no painel e no Home Assistant</label>
      <input name="name" value="%NAME%" maxlength="40">
    </fieldset>

    <fieldset>
      <legend>Rede Wi-Fi</legend>
      <label>Redes encontradas</label>
      <div class="nets" id="nets"><span class="note">Procurando...</span></div>
      <button type="button" class="btn ghost sm" onclick="scan(true)">Procurar novamente</button>
      <label>Rede (SSID)</label><input name="ssid" id="ssid" value="%SSID%" required>
      <label>Senha da rede</label><input name="pass" id="pass" type="password" placeholder="%PASSPH%">
    </fieldset>

    <fieldset>
      <legend>MQTT · Home Assistant</legend>
      <label>Servidor (IP do Home Assistant)</label><input name="mhost" value="%MHOST%" placeholder="192.168.0.10">
      <div class="two">
        <div><label>Porta</label><input name="mport" type="number" value="%MPORT%" min="1" max="65535"></div>
        <div><label>Prefixo descoberta</label><input name="disc" value="%DISC%"></div>
      </div>
      <label>Usuário</label><input name="muser" value="%MUSER%" autocomplete="off">
      <label>Senha</label><input name="mpass" type="password" placeholder="%MPASSPH%" autocomplete="off">
      <p class="note">Deixe o servidor vazio para usar o painel web sem MQTT.</p>
    </fieldset>

    <fieldset>
      <legend>Saídas</legend>
      %NAMES%
    </fieldset>

    <fieldset>
      <legend>Comportamento</legend>
      <label class="chk"><input type="checkbox" name="inv" %INV%> Relé ativo em nível baixo (invertido)</label>
      <label class="chk"><input type="checkbox" name="rest" %REST%> Restaurar estado das saídas ao reiniciar</label>
      <p class="note">Marque "invertido" se os relés ligarem desligados (placas de relé comuns são ativas em LOW).</p>
    </fieldset>

    <button type="submit" class="btn">Salvar e reiniciar</button>
  </form>
  <p class="foot">%NAME%</p>
</div>
<script>
const $=id=>document.getElementById(id);
async function scan(force){
  const box=$('nets');box.innerHTML='<span class="note">Procurando redes...</span>';
  try{
    let r=await (await fetch('/api/scan'+(force?'?rescan=1':''))).json();
    let n=0;
    while(r.scanning && n++<20){await new Promise(x=>setTimeout(x,1000));r=await (await fetch('/api/scan')).json();}
    box.innerHTML='';
    if(!r.nets||!r.nets.length){box.innerHTML='<span class="note">Nenhuma rede encontrada.</span>';return;}
    r.nets.forEach(net=>{
      const b=document.createElement('button');b.type='button';b.className='net';
      const a=document.createElement('span');a.textContent=net.ssid+(net.lock?'  (protegida)':'');
      const c=document.createElement('span');c.style.color='var(--mut)';c.textContent=net.rssi+' dBm';
      b.appendChild(a);b.appendChild(c);
      b.onclick=()=>{$('ssid').value=net.ssid;$('pass').focus();};
      box.appendChild(b);
    });
  }catch(e){box.innerHTML='<span class="note">Falha ao procurar redes.</span>';}
}
scan(false);
</script></body></html>
)rawliteral";

/* Página pós-gravação */
const char PAGE_SAVED[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="pt-BR"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Salvo</title>%CSS%</head><body>
<div class="wrap">
  <header class="top"><div class="brand"><div class="mark">8</div>
    <div><h1>Configuração salva</h1><p class="sub">O dispositivo está reiniciando</p></div></div></header>
  <fieldset>
    <p style="margin-top:14px">Conecte seu celular ou computador de volta à sua rede Wi-Fi normal e acesse:</p>
    <p class="kv"><span class="k">Endereço local</span><span class="v">http://%MDNS%.local</span></p>
    <p class="note">Se o endereço .local não abrir, procure o IP do dispositivo no roteador ou no Home Assistant.</p>
  </fieldset>
  <a class="btn" href="/">Abrir painel</a>
</div></body></html>
)rawliteral";
