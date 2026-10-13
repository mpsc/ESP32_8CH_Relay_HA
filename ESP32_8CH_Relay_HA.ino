/*
 * ESP32 · 8 saídas (relés) — Painel web + MQTT com auto-descoberta Home Assistant
 *                       + portal cativo para configuração inicial.
 *
 * Hardware alvo (verificado em COM3):
 *   - ESP32-D0WD-V3 (ESP32 clássico, dual-core), 4 MB flash, USB-CP2102
 *   - 8 saídas (somente output): GPIO 16, 17, 18, 19, 23, 25, 26, 27
 *
 * Bibliotecas necessárias:
 *   - PubSubClient  (Nick O'Leary)   2.8
 *   - ArduinoJson   (Benoit Blanchon) 7.x
 *   WiFi, WebServer, DNSServer, Preferences, ESPmDNS e ArduinoJson vêm do core ESP32.
 *
 * Compilação (arduino-cli embutido no Arduino IDE):
 *   esp32:esp32:esp32  (core 2.0.17)
 *
 * Primeira conexão (portal cativo):
 *   1. Sem Wi-Fi salvo, o dispositivo cria a rede "ESP32-8CH-XXXX" (senha: 12345678).
 *   2. Ao conectar, a página de configuração abre sozinha (ou acesse http://192.168.4.1).
 *   3. Escolha a rede, informe o MQTT (IP do Home Assistant) e salve.
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include "web_ui.h"

// ------------------------------------------------------------------ Constantes
#define FW_VERSION   "1.0.1"
#define NUM_OUT      8
#define AP_PASS      "12345678"     // >= 8 caracteres (WPA2)
#define AP_RETRY_MS  300000UL       // 5 min sem cliente no AP -> tenta reconectar
#define MQTT_BUF     1280

// Saídas (pinagem comprovada do projeto de 8 canais)
const uint8_t OUT_PINS[NUM_OUT] = {16, 17, 18, 19, 23, 25, 26, 27};

// ------------------------------------------------------------------ Objetos
Preferences  prefs;
WebServer    server(80);
DNSServer    dns;
WiFiClient   net;
PubSubClient mqtt(net);

// ------------------------------------------------------------------ Configuração
char     devId[24];
String   devName    = "ESP32 8 Canais";
String   wifiSsid, wifiPass;
String   mqttHost, mqttUser, mqttPass;
uint16_t mqttPort   = 1883;
String   discPrefix = "homeassistant";
bool     invertOut    = false;   // true = relé ativo em nível BAIXO
bool     restoreState = true;
String   outName[NUM_OUT];

// ------------------------------------------------------------------ Estado
bool          outState[NUM_OUT];
bool          apMode = false;
unsigned long apLastClient = 0, lastMqttTry = 0, lastPub = 0, lastWifiTry = 0;

// ------------------------------------------------------------------ Tópicos MQTT
String tBase, tAvail, tRssi, tUptime, tIp, tRestart;
String tOutState[NUM_OUT], tOutCmd[NUM_OUT];
const char* HA_STATUS_TOPIC = "homeassistant/status";

// ================================================================== Utilidades
String htmlEsc(String s) {
  s.replace("&", "&amp;");
  s.replace("\"", "&quot;");
  s.replace("<", "&lt;");
  s.replace(">", "&gt;");
  return s;
}

String selfUrl() {
  IPAddress ip = apMode ? WiFi.softAPIP() : WiFi.localIP();
  return "http://" + ip.toString() + "/";
}

const char* mqttStatusText() {
  if (apMode)            return "Modo AP";
  if (mqttHost.isEmpty())return "Não configurado";
  if (mqtt.connected())  return "Conectado";
  switch (mqtt.state()) {
    case -4: return "Timeout";
    case -3: return "Conexão perdida";
    case -2: return "Broker inacessível";
    case  1: return "Protocolo inválido";
    case  2: return "Client ID rejeitado";
    case  3: return "Broker indisponível";
    case  4: return "Usuário/senha incorretos";
    case  5: return "Não autorizado";
    default: return "Desconectado";
  }
}

// ================================================================== Persistência
void loadConfig() {
  prefs.begin("cfg", true);
  devName    = prefs.getString("name",  devName);
  wifiSsid   = prefs.getString("ssid",  "");
  wifiPass   = prefs.getString("pass",  "");
  mqttHost   = prefs.getString("mhost", "");
  mqttPort   = prefs.getUShort("mport", 1883);
  mqttUser   = prefs.getString("muser", "");
  mqttPass   = prefs.getString("mpass", "");
  discPrefix = prefs.getString("disc",  "homeassistant");
  invertOut    = prefs.getBool("inv",  false);
  restoreState = prefs.getBool("rest", true);

  uint8_t mask = prefs.getUChar("mask", 0);
  for (int i = 0; i < NUM_OUT; i++) {
    outName[i]  = prefs.getString(("n" + String(i)).c_str(), "Saída " + String(i + 1));
    outState[i] = restoreState ? ((mask >> i) & 1) : false;
  }
  prefs.end();
}

void saveConfig() {
  prefs.begin("cfg", false);
  prefs.putString("name",  devName);
  prefs.putString("ssid",  wifiSsid);
  prefs.putString("pass",  wifiPass);
  prefs.putString("mhost", mqttHost);
  prefs.putUShort("mport", mqttPort);
  prefs.putString("muser", mqttUser);
  prefs.putString("mpass", mqttPass);
  prefs.putString("disc",  discPrefix);
  prefs.putBool("inv",  invertOut);
  prefs.putBool("rest", restoreState);
  for (int i = 0; i < NUM_OUT; i++) prefs.putString(("n" + String(i)).c_str(), outName[i]);
  prefs.end();
}

void saveStateMask() {
  uint8_t mask = 0;
  for (int i = 0; i < NUM_OUT; i++) if (outState[i]) mask |= (1 << i);
  prefs.begin("cfg", false);
  prefs.putUChar("mask", mask);
  prefs.end();
}

// ================================================================== Saídas
void writeOut(int i) {
  digitalWrite(OUT_PINS[i], (outState[i] != invertOut) ? HIGH : LOW);
}

void initOutputs() {
  for (int i = 0; i < NUM_OUT; i++) {
    digitalWrite(OUT_PINS[i], (outState[i] != invertOut) ? HIGH : LOW);
    pinMode(OUT_PINS[i], OUTPUT);
  }
}

void publishOut(int i) {
  if (!mqtt.connected()) return;
  mqtt.publish(tOutState[i].c_str(), outState[i] ? "ON" : "OFF", true);
}

void setOut(int i, bool on) {
  if (i < 0 || i >= NUM_OUT || outState[i] == on) return;
  outState[i] = on;
  writeOut(i);
  saveStateMask();
  publishOut(i);
  Serial.printf("Saída %d -> %s\n", i + 1, on ? "ON" : "OFF");
}

// ================================================================== MQTT
void addDeviceBlock(JsonObject dev) {
  dev["identifiers"].to<JsonArray>().add(devId);
  dev["name"]             = devName;
  dev["manufacturer"]     = "DIY";
  dev["model"]            = "ESP32 8CH Relay";
  dev["sw_version"]       = FW_VERSION;
  dev["hw_version"]       = ESP.getChipModel();
  if (!apMode) dev["configuration_url"] = "http://" + WiFi.localIP().toString();
}

void announce(const char* component, const String& objectId, JsonDocument& d) {
  String topic = discPrefix + "/" + component + "/" + devId + "/" + objectId + "/config";
  String payload;
  serializeJson(d, payload);
  mqtt.publish(topic.c_str(), payload.c_str(), true);
}

void publishDiscovery() {
  if (!mqtt.connected()) return;

  for (int i = 0; i < NUM_OUT; i++) {
    JsonDocument d;
    d["name"]                 = outName[i];
    d["unique_id"]            = String(devId) + "_out" + String(i + 1);
    d["object_id"]            = String(devId) + "_out" + String(i + 1);
    d["command_topic"]        = tOutCmd[i];
    d["state_topic"]          = tOutState[i];
    d["availability_topic"]   = tAvail;
    d["payload_available"]    = "online";
    d["payload_not_available"]= "offline";
    d["payload_on"]  = "ON";
    d["payload_off"] = "OFF";
    d["state_on"]    = "ON";
    d["state_off"]   = "OFF";
    d["icon"]        = "mdi:electric-switch";
    addDeviceBlock(d["device"].to<JsonObject>());
    announce("switch", "out" + String(i + 1), d);
  }

  { // Sinal Wi-Fi
    JsonDocument d;
    d["name"]              = "Sinal Wi-Fi";
    d["unique_id"]         = String(devId) + "_rssi";
    d["state_topic"]       = tRssi;
    d["availability_topic"]= tAvail;
    d["device_class"]      = "signal_strength";
    d["unit_of_measurement"]="dBm";
    d["state_class"]       = "measurement";
    d["entity_category"]   = "diagnostic";
    addDeviceBlock(d["device"].to<JsonObject>());
    announce("sensor", "rssi", d);
  }
  { // Tempo ligado
    JsonDocument d;
    d["name"]              = "Tempo ligado";
    d["unique_id"]         = String(devId) + "_uptime";
    d["state_topic"]       = tUptime;
    d["availability_topic"]= tAvail;
    d["device_class"]      = "duration";
    d["unit_of_measurement"]="s";
    d["state_class"]       = "total_increasing";
    d["entity_category"]   = "diagnostic";
    d["icon"]              = "mdi:timer-outline";
    addDeviceBlock(d["device"].to<JsonObject>());
    announce("sensor", "uptime", d);
  }
  { // Endereço IP
    JsonDocument d;
    d["name"]              = "Endereço IP";
    d["unique_id"]         = String(devId) + "_ip";
    d["state_topic"]       = tIp;
    d["availability_topic"]= tAvail;
    d["entity_category"]   = "diagnostic";
    d["icon"]              = "mdi:ip-network";
    addDeviceBlock(d["device"].to<JsonObject>());
    announce("sensor", "ip", d);
  }
  { // Botão reiniciar
    JsonDocument d;
    d["name"]              = "Reiniciar";
    d["unique_id"]         = String(devId) + "_restart";
    d["command_topic"]     = tRestart;
    d["availability_topic"]= tAvail;
    d["payload_press"]     = "PRESS";
    d["device_class"]      = "restart";
    d["entity_category"]   = "config";
    addDeviceBlock(d["device"].to<JsonObject>());
    announce("button", "restart", d);
  }
  Serial.println("Discovery MQTT publicado");
}

void publishState() {
  if (!mqtt.connected()) return;
  for (int i = 0; i < NUM_OUT; i++) publishOut(i);
  mqtt.publish(tRssi.c_str(),   String(WiFi.RSSI()).c_str(),               true);
  mqtt.publish(tUptime.c_str(), String(millis() / 1000).c_str(),           true);
  mqtt.publish(tIp.c_str(),     WiFi.localIP().toString().c_str(),         true);
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  String t(topic);
  String msg;
  msg.reserve(len);
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  msg.trim();

  for (int i = 0; i < NUM_OUT; i++) {
    if (t == tOutCmd[i]) {
      String u = msg; u.toUpperCase();
      if (u == "ON")       setOut(i, true);
      else if (u == "OFF") setOut(i, false);
      else if (u == "TOGGLE") setOut(i, !outState[i]);
      return;
    }
  }
  if (t == tRestart) {
    Serial.println("Reinício solicitado via MQTT");
    delay(150);
    ESP.restart();
  } else if (t == HA_STATUS_TOPIC) {
    String u = msg; u.toUpperCase();
    if (u == "ONLINE") { publishDiscovery(); mqtt.publish(tAvail.c_str(), "online", true); publishState(); }
  }
}

void mqttReconnect() {
  if (mqttHost.isEmpty() || WiFi.status() != WL_CONNECTED) return;
  if (millis() - lastMqttTry < 5000) return;
  lastMqttTry = millis();

  mqtt.setServer(mqttHost.c_str(), mqttPort);
  bool ok;
  if (mqttUser.isEmpty())
    ok = mqtt.connect(devId, nullptr, nullptr, tAvail.c_str(), 0, true, "offline");
  else
    ok = mqtt.connect(devId, mqttUser.c_str(), mqttPass.c_str(), tAvail.c_str(), 0, true, "offline");

  if (ok) {
    Serial.println("MQTT conectado");
    for (int i = 0; i < NUM_OUT; i++) mqtt.subscribe(tOutCmd[i].c_str());
    mqtt.subscribe(tRestart.c_str());
    mqtt.subscribe(HA_STATUS_TOPIC);
    publishDiscovery();
    mqtt.publish(tAvail.c_str(), "online", true);
    publishState();
  } else {
    Serial.printf("MQTT falhou (rc=%d)\n", mqtt.state());
  }
}

// ================================================================== Web: páginas
String render(const char* tpl) {
  String s = FPSTR(tpl);
  s.replace("%CSS%", FPSTR(PAGE_CSS));
  return s;
}

void handleIndex() {
  String s = render(PAGE_INDEX);
  s.replace("%TITLE%", htmlEsc(devName));
  server.send(200, "text/html; charset=utf-8", s);
}

void handleConfig() {
  String s = render(PAGE_CONFIG);
  String names;
  for (int i = 0; i < NUM_OUT; i++) {
    names += "<label>Saída " + String(i + 1) + " · GPIO " + String(OUT_PINS[i]) + "</label>";
    names += "<input name='n" + String(i) + "' value=\"" + htmlEsc(outName[i]) + "\" maxlength='30'>";
  }
  bool hasWifi = !wifiSsid.isEmpty();
  s.replace("%TITLE%", apMode ? "Configuração inicial" : "Ajustes");
  s.replace("%INTRO%", apMode
      ? "Escolha a rede Wi-Fi e informe o servidor MQTT do Home Assistant."
      : "Alterações entram em vigor após salvar. Campos de senha vazios mantêm o valor atual.");
  s.replace("%NAME%",   htmlEsc(devName));
  s.replace("%SSID%",   htmlEsc(wifiSsid));
  s.replace("%PASSPH%", hasWifi ? "(manter a atual)" : "senha da rede");
  s.replace("%MHOST%",  htmlEsc(mqttHost));
  s.replace("%MPORT%",  String(mqttPort));
  s.replace("%DISC%",   htmlEsc(discPrefix));
  s.replace("%MUSER%",  htmlEsc(mqttUser));
  s.replace("%MPASSPH%",mqttPass.isEmpty() ? "" : "(manter a atual)");
  s.replace("%NAMES%",  names);
  s.replace("%INV%",    invertOut ? "checked" : "");
  s.replace("%REST%",   restoreState ? "checked" : "");
  s.replace("%BACK%",   apMode ? "" : "<a class='btn ghost sm' href='/'>Painel</a>");
  server.send(200, "text/html; charset=utf-8", s);
}

void handleRoot() {
  if (apMode) handleConfig();
  else        handleIndex();
}

void handleSave() {
  if (server.arg("name").length()) devName = server.arg("name");
  wifiSsid = server.arg("ssid");
  if (server.arg("pass").length()) wifiPass = server.arg("pass");
  mqttHost = server.arg("mhost");
  int p = server.arg("mport").toInt();
  mqttPort = (p > 0 && p <= 65535) ? (uint16_t)p : 1883;
  String d = server.arg("disc");
  discPrefix = d.length() ? d : "homeassistant";
  mqttUser = server.arg("muser");
  if (server.arg("mpass").length()) mqttPass = server.arg("mpass");
  for (int i = 0; i < NUM_OUT; i++) {
    String n = server.arg("n" + String(i));
    n.trim();
    outName[i] = n.length() ? n : "Saída " + String(i + 1);
  }
  invertOut    = server.hasArg("inv");
  restoreState = server.hasArg("rest");
  saveConfig();

  String s = render(PAGE_SAVED);
  s.replace("%MDNS%", devId);
  server.send(200, "text/html; charset=utf-8", s);
  delay(1500);
  ESP.restart();
}

// ================================================================== Web: API JSON
void handleState() {
  JsonDocument d;
  d["name"]    = devName;
  d["ssid"]    = apMode ? "(modo configuração)" : WiFi.SSID();
  d["ip"]      = apMode ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
  d["rssi"]    = apMode ? 0 : WiFi.RSSI();
  d["ap"]      = apMode;
  d["mqtt"]    = mqtt.connected();
  d["mqttMsg"] = mqttStatusText();
  d["uptime"]  = millis() / 1000;
  d["heap"]    = ESP.getFreeHeap();
  d["fw"]      = FW_VERSION;
  d["chip"]    = String(ESP.getChipModel()) + " r" + String(ESP.getChipRevision());
  JsonArray outs = d["outs"].to<JsonArray>();
  for (int i = 0; i < NUM_OUT; i++) {
    JsonObject o = outs.add<JsonObject>();
    o["name"] = outName[i];
    o["pin"]  = OUT_PINS[i];
    o["on"]   = outState[i];
  }
  String out;
  serializeJson(d, out);
  server.send(200, "application/json", out);
}

void handleMqttInfo() {
  JsonDocument d;
  d["host"]   = mqttHost;
  d["port"]   = mqttPort;
  d["user"]   = mqttUser;
  d["clientId"]= devId;
  d["prefix"] = discPrefix;
  d["heap"]   = ESP.getFreeHeap();
  d["avail"]  = tAvail;
  JsonArray arr = d["topics"].to<JsonArray>();
  for (int i = 0; i < NUM_OUT; i++) {
    JsonObject t = arr.add<JsonObject>();
    t["ch"]    = i + 1;
    t["name"]  = outName[i];
    t["state"] = tOutState[i];
    t["set"]   = tOutCmd[i];
  }
  String out;
  serializeJson(d, out);
  server.send(200, "application/json", out);
}

void handleOut() {
  int ch = server.arg("ch").toInt() - 1;
  String st = server.arg("state"); st.toUpperCase();
  if (ch < 0 || ch >= NUM_OUT) { server.send(400, "text/plain", "canal inválido"); return; }
  if (st == "TOGGLE") setOut(ch, !outState[ch]);
  else                setOut(ch, st == "ON");
  server.send(200, "text/plain", outState[ch] ? "ON" : "OFF");
}

void handleAll() {
  bool on = server.arg("state") == "ON";
  for (int i = 0; i < NUM_OUT; i++) setOut(i, on);
  server.send(200, "text/plain", on ? "ON" : "OFF");
}

void handleDiscovery() {
  if (!mqtt.connected()) { server.send(503, "text/plain", "MQTT desconectado"); return; }
  publishDiscovery();
  publishState();
  server.send(200, "text/plain", "OK");
}

void handleRestart() {
  server.send(200, "text/plain", "reiniciando");
  delay(150);
  ESP.restart();
}

void handleScan() {
  if (server.hasArg("rescan") && WiFi.scanComplete() != WIFI_SCAN_RUNNING) {
    WiFi.scanDelete();
    WiFi.scanNetworks(true);
  }
  int n = WiFi.scanComplete();
  JsonDocument d;
  if (n == WIFI_SCAN_RUNNING || n == WIFI_SCAN_FAILED) {
    if (n == WIFI_SCAN_FAILED) WiFi.scanNetworks(true);
    d["scanning"] = true;
  } else {
    JsonArray nets = d["nets"].to<JsonArray>();
    for (int i = 0; i < n; i++) {
      String ssid = WiFi.SSID(i);
      if (ssid.isEmpty()) continue;
      bool dup = false;
      for (JsonObject o : nets) if (ssid == o["ssid"].as<String>()) { dup = true; break; }
      if (dup) continue;
      JsonObject o = nets.add<JsonObject>();
      o["ssid"] = ssid;
      o["rssi"] = WiFi.RSSI(i);
      o["lock"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
    }
  }
  String out;
  serializeJson(d, out);
  server.send(200, "application/json", out);
}

// ================================================================== Portal cativo
void captiveRedirect() {
  if (apMode) {
    server.sendHeader("Location", selfUrl(), true);
    server.send(302, "text/plain", "");
  } else {
    server.send(404, "text/plain", "não encontrado");
  }
}

void setupWeb() {
  server.on("/",            handleRoot);
  server.on("/config",      handleConfig);
  server.on("/save",        HTTP_POST, handleSave);
  server.on("/api/state",   handleState);
  server.on("/api/mqtt",    handleMqttInfo);
  server.on("/api/out",     handleOut);       // ?ch=1..8&state=ON|OFF|TOGGLE
  server.on("/api/all",     handleAll);       // ?state=ON|OFF
  server.on("/api/discovery", handleDiscovery);
  server.on("/api/scan",    handleScan);
  server.on("/api/restart", handleRestart);

  // Detecção de portal cativo (Android / iOS / Windows / Firefox)
  server.on("/generate_204",              captiveRedirect);
  server.on("/gen_204",                   captiveRedirect);
  server.on("/hotspot-detect.html",       captiveRedirect);
  server.on("/library/test/success.html", captiveRedirect);
  server.on("/success.txt",               captiveRedirect);
  server.on("/connecttest.txt",           captiveRedirect);
  server.on("/ncsi.txt",                  captiveRedirect);
  server.on("/fwlink",                    captiveRedirect);
  server.on("/canonical.html",            captiveRedirect);
  server.onNotFound(captiveRedirect);
  server.begin();
}

// ================================================================== Wi-Fi
void startAP() {
  apMode = true;
  apLastClient = millis();
  String apName = "ESP32-8CH-" + String(devId).substring(strlen(devId) - 4);
  WiFi.mode(WIFI_AP_STA);                      // STA ativo para poder varrer redes
  WiFi.softAP(apName.c_str(), AP_PASS);
  delay(200);
  dns.start(53, "*", WiFi.softAPIP());
  WiFi.scanNetworks(true);
  Serial.println("Portal cativo ativo. Rede: " + apName + " / http://" + WiFi.softAPIP().toString());
}

void connectWiFi() {
  if (wifiSsid.isEmpty()) { startAP(); return; }
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(devId);
  WiFi.setSleep(false);
  WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());
  Serial.print("Conectando ao Wi-Fi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) { delay(500); Serial.print("."); }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("IP: " + WiFi.localIP().toString());
    if (MDNS.begin(devId)) { MDNS.addService("http", "tcp", 80); Serial.println("mDNS: http://" + String(devId) + ".local"); }
  } else {
    Serial.println("Falha no Wi-Fi -> portal cativo");
    startAP();
  }
}

// ================================================================== Setup / Loop
void buildTopics() {
  tBase    = "esp32/" + String(devId);
  tAvail   = tBase + "/status";
  tRssi    = tBase + "/rssi";
  tUptime  = tBase + "/uptime";
  tIp      = tBase + "/ip";
  tRestart = tBase + "/cmd/restart";
  for (int i = 0; i < NUM_OUT; i++) {
    tOutState[i] = tBase + "/out" + String(i + 1) + "/state";
    tOutCmd[i]   = tBase + "/out" + String(i + 1) + "/set";
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=== ESP32 8CH Relay " FW_VERSION " ===");

  snprintf(devId, sizeof(devId), "esp32_8ch_%06x", (uint32_t)(ESP.getEfuseMac() & 0xFFFFFF));

  loadConfig();
  buildTopics();
  initOutputs();

  connectWiFi();
  setupWeb();

  mqtt.setBufferSize(MQTT_BUF);
  mqtt.setKeepAlive(30);
  mqtt.setCallback(mqttCallback);
}

void loop() {
  server.handleClient();

  if (apMode) {
    dns.processNextRequest();
    if (WiFi.softAPgetStationNum() > 0) apLastClient = millis();
    else if (!wifiSsid.isEmpty() && millis() - apLastClient > AP_RETRY_MS) ESP.restart();
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastWifiTry > 10000) { lastWifiTry = millis(); WiFi.reconnect(); }
    return;
  }

  if (!mqtt.connected()) {
    mqttReconnect();
  } else {
    mqtt.loop();
    if (millis() - lastPub > 30000) { lastPub = millis(); publishState(); }
  }
}
