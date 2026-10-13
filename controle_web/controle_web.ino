/*
 * Controle Remoto IR via Web — ESP32
 * Autor: Lucas Carcerani — Carcerani Energia e Automação
 *
 * Abre uma página web na rede local com botões que enviam comandos
 * infravermelho (TV Samsung; ar-condicionado em desenvolvimento).
 *
 * Biblioteca: IRremoteESP8266 (David Conran / crankyoldgit)
 * Placa testada: ESP32-S3 Dev Module
 *
 * Ligação recomendada (alcance de vários metros):
 *   GPIO 4 -> resistor 1 kΩ -> base de um transistor NPN (BC337 ou 2N2222)
 *   Emissor -> GND
 *   5V -> resistor 47 Ω -> LED IR (anodo) ; LED IR (catodo) -> coletor
 * Ligar o LED direto no GPIO funciona, mas com alcance de poucos centímetros.
 */

#include <WiFi.h>
#include <WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "config.h"  // copie config.example.h para config.h

const uint16_t PINO_LED_IR = 4;

IRsend irsend(PINO_LED_IR);
WebServer server(80);

struct Comando {
  const char* id;
  const char* rotulo;
  uint32_t codigo;   // protocolo SAMSUNG, 32 bits
};

// Códigos capturados do controle original com captura_ir.ino
const Comando COMANDOS_TV[] = {
  {"power",  "Liga / Desliga", 0xE0E040BF},
  {"vol_up", "Volume +",       0xE0E0E01F},
  {"vol_dn", "Volume −",       0xE0E0D02F},
};

const char PAGINA[] PROGMEM = R"HTML(
<!doctype html><html lang="pt-BR"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Controle IR</title>
<style>
 body{font-family:system-ui,sans-serif;background:#1f2326;color:#e8eaea;margin:0;
      display:flex;flex-direction:column;align-items:center;gap:16px;padding:32px 16px}
 h1{font-size:18px;font-weight:600;letter-spacing:.04em;text-transform:uppercase;margin:0}
 .grade{display:grid;grid-template-columns:repeat(2,140px);gap:12px}
 button{height:64px;border:1px solid #4a5156;border-radius:8px;background:#2b3034;
        color:#e8eaea;font-size:16px;cursor:pointer}
 button:active{background:#3a4146}
 .power{grid-column:span 2;background:#5a2a24;border-color:#8a3f35}
 #msg{font-size:13px;color:#9aa3a8;min-height:1em}
</style></head><body>
<h1>TV · Sala</h1>
<div class="grade">
 <button class="power" onclick="enviar('power')">Liga / Desliga</button>
 <button onclick="enviar('vol_dn')">Volume −</button>
 <button onclick="enviar('vol_up')">Volume +</button>
</div>
<div id="msg"></div>
<script>
 function enviar(c){
  fetch('/enviar?cmd='+c).then(r=>r.text()).then(t=>{document.getElementById('msg').textContent=t})
   .catch(()=>{document.getElementById('msg').textContent='Sem resposta do ESP32'});
 }
</script></body></html>
)HTML";

void paginaInicial() {
  server.send_P(200, "text/html; charset=utf-8", PAGINA);
}

void enviarComando() {
  String cmd = server.arg("cmd");
  for (const Comando& c : COMANDOS_TV) {
    if (cmd == c.id) {
      irsend.sendSAMSUNG(c.codigo, 32);
      Serial.printf("Enviado %s (0x%08X)\n", c.id, c.codigo);
      server.send(200, "text/plain; charset=utf-8", String("Enviado: ") + c.rotulo);
      return;
    }
  }
  server.send(400, "text/plain; charset=utf-8", "Comando desconhecido");
}

void setup() {
  Serial.begin(115200);
  irsend.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_SENHA);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nAbra no navegador: http://%s\n", WiFi.localIP().toString().c_str());

  server.on("/", paginaInicial);
  server.on("/enviar", enviarComando);
  server.begin();
}

void loop() {
  server.handleClient();
}
