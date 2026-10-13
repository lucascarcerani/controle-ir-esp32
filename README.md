# Controle Remoto IR via Web — ESP32

Transforma um ESP32 num controle remoto universal: ele **aprende os códigos** do controle original e depois **envia os comandos** a partir de uma página web aberta no celular, na rede local.

Primeiro alvo: TV Samsung. Próximo: ar-condicionado.

## Como funciona

1. **Captura** (`captura_ir/`) — um receptor IR no GPIO 14 lê o controle original e mostra protocolo e código no Monitor Serial.
2. **Envio** (`controle_web/`) — o ESP32 sobe uma página web com botões. Cada botão chama `/enviar?cmd=...` e o ESP32 dispara o código pelo LED IR.

Códigos já capturados (protocolo SAMSUNG, 32 bits):

| Botão | Código |
|---|---|
| Liga / Desliga | `0xE0E040BF` |
| Volume + | `0xE0E0E01F` |
| Volume − | `0xE0E0D02F` |

## Hardware

- ESP32-S3 Dev Module (funciona em qualquer ESP32)
- Receptor IR 38 kHz (VS1838B ou TSOP) → GPIO 14
- LED IR 940 nm + transistor NPN (BC337 ou 2N2222) → GPIO 4

### Por que usar transistor

O GPIO do ESP32 fornece poucos miliampères. Com o LED ligado direto no pino o sinal sai fraco e a TV só responde a poucos centímetros. Com um transistor, o LED recebe corrente bem maior a partir dos 5 V e o alcance passa de alguns metros.

```
GPIO 4 ──[1 kΩ]── base (BC337)
                  emissor ── GND
5V ──[47 Ω]── LED IR (+) ; LED IR (−) ── coletor
```

## Como usar

1. Instale a biblioteca **IRremoteESP8266** na Arduino IDE.
2. Grave `captura_ir.ino` e anote os códigos dos botões que quiser.
3. Coloque os códigos em `COMANDOS_TV` dentro de `controle_web.ino`.
4. Copie `config.example.h` para `config.h` com os dados do seu Wi-Fi.
5. Grave `controle_web.ino` e abra no celular o IP mostrado no Monitor Serial.

## Status

- [x] Captura dos códigos da TV
- [x] Página web enviando comandos
- [ ] Driver com transistor para maior alcance
- [ ] Captura e envio do ar-condicionado (quadros longos, protocolo Samsung AC)
- [ ] Integração MQTT com Home Assistant

## Autor

**Lucas Carcerani** — Carcerani Energia e Automação
