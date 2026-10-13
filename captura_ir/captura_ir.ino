/*
 * Captura de códigos IR — ESP32
 * Aponte o controle original para o receptor e aperte os botões.
 * O protocolo e o código aparecem no Monitor Serial (115200).
 *
 * Receptor IR (VS1838B / TSOP) com saída no GPIO 14.
 */

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t PINO_RECEPTOR = 14;
const uint16_t TAMANHO_BUFFER = 1024;   // ar-condicionado manda quadros longos
const uint8_t  TIMEOUT_MS = 50;

IRrecv irrecv(PINO_RECEPTOR, TAMANHO_BUFFER, TIMEOUT_MS, true);
decode_results resultado;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
  Serial.println("Pronto. Aperte um botão do controle.");
}

void loop() {
  if (irrecv.decode(&resultado)) {
    Serial.println(resultToHumanReadableBasic(&resultado));
    Serial.println(resultToSourceCode(&resultado));  // array raw, útil para AC
    Serial.println("----------------------------------------");
    irrecv.resume();
  }
}
