//luzes sitio 

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// PINOS 
#define PINO_PIR     14
#define PINO_LDR     34
#define PINO_RELE_1  26
#define PINO_RELE_2  27
#define PINO_RELE_3  25
#define PINO_LED      2

// CONFIGURAÇÕES DO WIFI
const char* WIFI_NOME  = "";
const char* WIFI_SENHA = "";

// OBJETOS
WebServer server(80);
Preferences prefs;

// VARIÁVEIS CONFIGURÁVEIS
int  limiteLDR      = 2000;
int  tempoLuzAcesa  = 30;

// VARIÁVEIS
unsigned long tempoDeteccao         = 0;
unsigned long ultimoPisca           = 0;
unsigned long ultimaVerificacaoWifi = 0;
unsigned long ultimaLeituraPIR      = 0;
unsigned long ultimaDeteccao        = 0; 

bool luzAcesa1  = false;
bool luzAcesa2  = false;
bool luzAcesa3  = false;
bool modoManual = false;

//html
String paginaHTML() {
  int valorLDR = analogRead(PINO_LDR);
  bool mov     = digitalRead(PINO_PIR);
  bool eNoite  = valorLDR < limiteLDR;

  String html = R"rawhtml(
<!DOCTYPE html>
<html lang='pt-br'>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>MANGUEIRA</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family: Arial, sans-serif; background: #1a1a2e; color: #eee; padding: 20px; }
    h1 { text-align: center; color: #e94560; margin-bottom: 20px; font-size: 22px; }
    .card { background: #16213e; border-radius: 12px; padding: 16px; margin-bottom: 16px; }
    .card h2 { font-size: 16px; color: #e94560; margin-bottom: 12px; }
    .status { display: flex; justify-content: space-between; margin-bottom: 8px; }
    .badge { padding: 4px 12px; border-radius: 20px; font-size: 13px; font-weight: bold; }
    .verde { background: #2ecc71; color: #000; }
    .vermelho { background: #e74c3c; color: #fff; }
    .amarelo { background: #f39c12; color: #000; }
    label { display: block; margin-bottom: 6px; font-size: 14px; color: #aaa; margin-top: 10px; }
    input[type=range] { width: 100%; margin-bottom: 4px; }
    .valor { text-align: right; font-size: 13px; color: #e94560; margin-bottom: 8px; }
    button { width: 100%; padding: 12px; border: none; border-radius: 8px; font-size: 15px; font-weight: bold; cursor: pointer; margin-top: 8px; }
    .btn-salvar   { background: #e94560; color: #fff; }
    .btn-ligar    { background: #2ecc71; color: #000; }
    .btn-desligar { background: #e74c3c; color: #fff; }
    .btn-auto     { background: #3498db; color: #fff; }
    .btn-amarelo  { background: #f39c12; color: #000; }
    .grupo { display: flex; gap: 8px; margin-top: 8px; }
    .grupo button { margin-top: 0; }
    .separador { border: none; border-top: 1px solid #0f3460; margin: 14px 0; }
    .secao { color: #aaa; font-size: 13px; margin-top: 12px; margin-bottom: 4px; }
  </style>
</head>
<body>
  <h1>💡 MANGUEIRA</h1>

  <div class='card'>
    <h2>📊 Status Atual</h2>
    <div class='status'>
      <span>PIR:</span>
      <span class='badge )rawhtml";
  html += mov ? "verde'>DETECTADO" : "vermelho'>NENHUM";
  html += R"rawhtml('</span></div>
    <div class='status'>
      <span>Período:</span>
      <span class='badge )rawhtml";
  html += eNoite ? "amarelo'>🌙 NOITE" : "verde'>☀️ DIA";
  html += R"rawhtml('</span></div>
    <div class='status'>
      <span>Lâmpadas 220V:</span>
      <span class='badge )rawhtml";
  html += luzAcesa1 ? "verde'>ACESAS" : "vermelho'>APAGADAS";
  html += R"rawhtml('</span></div>
    <div class='status'>
      <span>Freezer:</span>
      <span class='badge )rawhtml";
  html += luzAcesa2 ? "verde'>ACESO" : "vermelho'>APAGADO";
  html += R"rawhtml('</span></div>
    <div class='status'>
      <span>Lâmpada de Fora:</span>
      <span class='badge )rawhtml";
  html += luzAcesa3 ? "verde'>ACESA" : "vermelho'>APAGADA";
  html += R"rawhtml('</span></div>
    <div class='status'>
      <span>Modo:</span>
      <span class='badge )rawhtml";
  html += modoManual ? "amarelo'>MANUAL" : "verde'>AUTOMÁTICO";
  html += R"rawhtml('</span></div>
    <div class='status'><span>Valor LDR:</span><span>)rawhtml";
  html += String(valorLDR);
  html += R"rawhtml(</span></div>
  </div>

  <!-- CONFIGURAÇÕES -->
  <div class='card'>
    <h2>⚙️ Configurações</h2>
    <form action='/salvar' method='GET'>

      <label>🌙 Limite Dia/Noite: <span id='vLDR'>)rawhtml";
  html += String(limiteLDR);
  html += R"rawhtml(</span></label>
      <input type='range' name='ldr' min='0' max='4095' step='10' value=')rawhtml";
  html += String(limiteLDR);
  html += R"rawhtml(' oninput="document.getElementById('vLDR').innerText=this.value">
      <div class='valor'>Valor atual do LDR: )rawhtml";
  html += String(valorLDR);
  html += R"rawhtml( | Abaixo do limite = NOITE</div>

      <label>⏱️ Tempo com luz acesa: <span id='vTempo'>)rawhtml";
  html += String(tempoLuzAcesa);
  html += R"rawhtml(</span> segundos</label>
      <input type='range' name='tempo' min='10' max='600' value=')rawhtml";
  html += String(tempoLuzAcesa);
  html += R"rawhtml(' oninput="document.getElementById('vTempo').innerText=this.value">
      <div class='valor'>10s até 10 minutos</div>

      <button type='submit' class='btn-salvar'>💾 Salvar Configurações</button>
    </form>
  </div>

  <!-- CONTROLE MANUAL -->
  <div class='card'>
    <h2>🕹️ Controle Manual</h2>
    <div class='grupo'>
      <a href='/ligar-tudo' style='width:100%'><button class='btn-ligar'>💡 Ligar Tudo</button></a>
      <a href='/desligar-tudo' style='width:100%'><button class='btn-desligar'>🌑 Desligar Tudo</button></a>
    </div>
    <div class='separador'></div>
    <p class='secao'>4 Lâmpadas</p>
    <div class='grupo'>
      <a href='/ligar-lampadas' style='width:100%'><button class='btn-amarelo'>💡 Ligar</button></a>
      <a href='/desligar-lampadas' style='width:100%'><button class='btn-desligar'>🌑 Desligar</button></a>
    </div>
    <p class='secao'>Freezer</p>
    <div class='grupo'>
      <a href='/ligar-freezer' style='width:100%'><button class='btn-amarelo'>💡 Ligar</button></a>
      <a href='/desligar-freezer' style='width:100%'><button class='btn-desligar'>🌑 Desligar</button></a>
    </div>
    <p class='secao'>Lâmpada de Fora</p>
    <div class='grupo'>
      <a href='/ligar-fora' style='width:100%'><button class='btn-amarelo'>💡 Ligar</button></a>
      <a href='/desligar-fora' style='width:100%'><button class='btn-desligar'>🌑 Desligar</button></a>
    </div>
    <div class='separador'></div>
    <a href='/auto'><button class='btn-auto'>🤖 Voltar ao Automático</button></a>
  </div>

  <script>setTimeout(() => location.reload(), 60000);</script>
</body>
</html>
)rawhtml";
  return html;
}

//rotas
void handleRoot() { server.send(200, "text/html", paginaHTML()); }

void handleSalvar() {
  if (server.hasArg("ldr"))   limiteLDR    = server.arg("ldr").toInt();
  if (server.hasArg("tempo")) tempoLuzAcesa = server.arg("tempo").toInt();
  prefs.begin("config", false);
  prefs.putInt("limiteLDR",  limiteLDR);
  prefs.putInt("tempoLuz",   tempoLuzAcesa);
  prefs.end();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLigarTudo() {
  modoManual = true;
  luzAcesa1 = luzAcesa2 = luzAcesa3 = true;
  digitalWrite(PINO_RELE_1, LOW);
  digitalWrite(PINO_RELE_2, LOW);
  digitalWrite(PINO_RELE_3, LOW);
  server.sendHeader("Location", "/"); server.send(303);
}

void handleDesligarTudo() {
  modoManual = true;
  luzAcesa1 = luzAcesa2 = luzAcesa3 = false;
  digitalWrite(PINO_RELE_1, HIGH);
  digitalWrite(PINO_RELE_2, HIGH);
  digitalWrite(PINO_RELE_3, HIGH);
  server.sendHeader("Location", "/"); server.send(303);
}

void handleLigarLampadas() {
  modoManual = true; luzAcesa1 = true;
  digitalWrite(PINO_RELE_1, LOW);
  server.sendHeader("Location", "/"); server.send(303);
}
void handleDesligarLampadas() {
  modoManual = true; luzAcesa1 = false;
  digitalWrite(PINO_RELE_1, HIGH);
  server.sendHeader("Location", "/"); server.send(303);
}
void handleLigarFreezer() {
  modoManual = true; luzAcesa2 = true;
  digitalWrite(PINO_RELE_2, LOW);
  server.sendHeader("Location", "/"); server.send(303);
}
void handleDesligarFreezer() {
  modoManual = true; luzAcesa2 = false;
  digitalWrite(PINO_RELE_2, HIGH);
  server.sendHeader("Location", "/"); server.send(303);
}
void handleLigarFora() {
  modoManual = true; luzAcesa3 = true;
  digitalWrite(PINO_RELE_3, LOW);
  server.sendHeader("Location", "/"); server.send(303);
}
void handleDesligarFora() {
  modoManual = true; luzAcesa3 = false;
  digitalWrite(PINO_RELE_3, HIGH);
  server.sendHeader("Location", "/"); server.send(303);
}
void handleAuto() {
  modoManual = false;
  server.sendHeader("Location", "/"); server.send(303);
}

//setup
void setup() {
  Serial.begin(115200);

  pinMode(PINO_PIR,    INPUT);
  pinMode(PINO_LDR,    INPUT);
  pinMode(PINO_RELE_1, OUTPUT);
  pinMode(PINO_RELE_2, OUTPUT);
  pinMode(PINO_RELE_3, OUTPUT);
  pinMode(PINO_LED,    OUTPUT);

  digitalWrite(PINO_RELE_1, HIGH);
  digitalWrite(PINO_RELE_2, HIGH);
  digitalWrite(PINO_RELE_3, HIGH);
  digitalWrite(PINO_LED,    LOW);

  prefs.begin("config", true);
  limiteLDR     = prefs.getInt("limiteLDR",   2000);
  tempoLuzAcesa = prefs.getInt("tempoLuz",    30);
  prefs.end();

  Serial.print("Conectando ao WiFi");
  WiFi.begin(WIFI_NOME);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
  Serial.print("Acesse: http://");
  Serial.println(WiFi.localIP());

  server.on("/",                handleRoot);
  server.on("/salvar",          handleSalvar);
  server.on("/ligar-tudo",      handleLigarTudo);
  server.on("/desligar-tudo",   handleDesligarTudo);
  server.on("/ligar-lampadas",  handleLigarLampadas);
  server.on("/desligar-lampadas", handleDesligarLampadas);
  server.on("/ligar-freezer",   handleLigarFreezer);
  server.on("/desligar-freezer",handleDesligarFreezer);
  server.on("/ligar-fora",      handleLigarFora);
  server.on("/desligar-fora",   handleDesligarFora);
  server.on("/auto",            handleAuto);
  server.begin();

  Serial.println("Servidor iniciado!");
}

//loop
void loop() {
  server.handleClient();

  // Reconexão WiFi
  if (millis() - ultimaVerificacaoWifi >= 10000) {
    ultimaVerificacaoWifi = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Reconectando...");
      WiFi.disconnect();
      WiFi.begin(WIFI_NOME);
    }
  }

  // LED azul 
  if (millis() - ultimoPisca >= 10000) {
    ultimoPisca = millis();
    digitalWrite(PINO_LED, HIGH);
    delay(200);
    digitalWrite(PINO_LED, LOW);
  }

  // Lógica automática
  if (!modoManual) {
    int valorLDR = analogRead(PINO_LDR);
    bool eNoite  = valorLDR < limiteLDR;

    // pir
    bool mov = false;
    if (millis() - ultimaLeituraPIR >= 1000) {
      ultimaLeituraPIR = millis();
      mov = digitalRead(PINO_PIR);
      Serial.print("PIR: "); Serial.print(mov ? "SIM" : "NAO");
      Serial.print(" | LDR: "); Serial.print(valorLDR);
      Serial.print(" | "); Serial.println(eNoite ? "NOITE" : "DIA");
    }

    // Lâmpada de fora 
    if (eNoite && !luzAcesa3) {
      Serial.println(">> NOITE → ACENDENDO LÂMPADA DE FORA");
      digitalWrite(PINO_RELE_3, LOW);
      luzAcesa3 = true;
    } else if (!eNoite && luzAcesa3) {
      Serial.println(">> DIA → APAGANDO LÂMPADA DE FORA");
      digitalWrite(PINO_RELE_3, HIGH);
      luzAcesa3 = false;
    }

    // PIR detectou
    if (mov && eNoite && (millis() - ultimaDeteccao >= 60000)) {
      Serial.println(">> MOVIMENTO + NOITE → ACENDENDO LÂMPADAS E FREEZER");
      digitalWrite(PINO_RELE_1, LOW);
      digitalWrite(PINO_RELE_2, LOW);
      luzAcesa1 = luzAcesa2 = true;
      tempoDeteccao  = millis();
      ultimaDeteccao = millis();
    }

    // Apaga lâmpadas e freezer 
    if ((luzAcesa1 || luzAcesa2) &&
        (millis() - tempoDeteccao >= (unsigned long)tempoLuzAcesa * 1000)) {
      Serial.println(">> Tempo esgotado → APAGANDO LÂMPADAS E FREEZER");
      digitalWrite(PINO_RELE_1, HIGH);
      digitalWrite(PINO_RELE_2, HIGH);
      luzAcesa1 = luzAcesa2 = false;
    }
  }

  delay(500);
}
