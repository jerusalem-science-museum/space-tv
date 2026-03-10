#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>

// ======================
// IR protocols
// ======================
#define DECODE_DENON
#define DECODE_JVC
#define DECODE_KASEIKYO
#define DECODE_PANASONIC
#define DECODE_LG
#define DECODE_NEC
#define DECODE_SAMSUNG
#define DECODE_SONY
#define DECODE_RC5
#define DECODE_RC6
#define DECODE_BOSEWAVE
#define DECODE_LEGO_PF
#define DECODE_MAGIQUEST
#define DECODE_WHYNTER
#define DECODE_FAST
#define DECODE_DISTANCE_WIDTH
#define DECODE_HASH

// ======================
// OLED config
// ======================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C   // souvent 0x3C, parfois 0x3D

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ======================
// IR config
// ======================
#define IR_RECEIVE_PIN 2
#define ENABLE_LED_FEEDBACK true

// ======================
// Helpers
// ======================
String protocolToString(decode_type_t protocol) {
    switch (protocol) {
        case NEC: return "NEC";
        case SONY: return "SONY";
        case RC5: return "RC5";
        case RC6: return "RC6";
        case SAMSUNG: return "SAMSUNG";
        case LG: return "LG";
        case JVC: return "JVC";
        case PANASONIC: return "PANASONIC";
        case KASEIKYO: return "KASEIKYO";
        case DENON: return "DENON";
        case SHARP: return "SHARP";
        case ONKYO: return "ONKYO";
        case APPLE: return "APPLE";
        case BOSEWAVE: return "BOSE";
        case LEGO_PF: return "LEGO";
        case MAGIQUEST: return "MAGIQUEST";
        case WHYNTER: return "WHYNTER";
        case FAST: return "FAST";
        case UNKNOWN: return "UNKNOWN";
        default: return "OTHER";
    }
}

void showMessage(String line1, String line2 = "", String line3 = "", String line4 = "") {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println(line1);

    if (line2.length()) {
        display.setCursor(0, 16);
        display.println(line2);
    }

    if (line3.length()) {
        display.setCursor(0, 32);
        display.println(line3);
    }

    if (line4.length()) {
        display.setCursor(0, 48);
        display.println(line4);
    }

    display.display();
}

void showIRData() {
    String proto = protocolToString(IrReceiver.decodedIRData.protocol);

    char addressStr[20];
    char commandStr[20];
    char rawStr[20];

    sprintf(addressStr, "Addr: 0x%X", IrReceiver.decodedIRData.address);
    sprintf(commandStr, "Cmd : 0x%X", IrReceiver.decodedIRData.command);
    sprintf(rawStr, "Raw : 0x%lX", IrReceiver.decodedIRData.decodedRawData);

    showMessage(
        "Proto: " + proto,
        String(addressStr),
        String(commandStr),
        String(rawStr)
    );
}

void setup() {
    Serial.begin(115200);

    // OLED init
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("OLED not found"));
        while (true);
    }

    display.clearDisplay();
    display.display();

    showMessage("Demarrage...", "IR + OLED");

    Serial.println(F("START"));
    Serial.println(F("IR Receiver + OLED"));

    // IR init
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    delay(1000);
    showMessage("Pret", "Attente signal IR", "Pin IR: 2");
}

void loop() {
    if (IrReceiver.decode()) {

        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println(F("Unknown protocol"));
            showMessage("Signal inconnu", "ou bruit");
            IrReceiver.resume();
            return;
        }

        bool isRepeat = (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT);

        IrReceiver.printIRResultShort(&Serial);
        IrReceiver.printIRSendUsage(&Serial);
        Serial.println();

        if (isRepeat) {
            Serial.println(F("Repeat received"));
            showMessage(
                "Repeat recu",
                "Proto: " + protocolToString(IrReceiver.decodedIRData.protocol),
                "Cmd : 0x" + String(IrReceiver.decodedIRData.command, HEX)
            );
        } else {
            showIRData();

            if (IrReceiver.decodedIRData.command == 0x10) {
                Serial.println(F("Received command 0x10."));
            } else if (IrReceiver.decodedIRData.command == 0x11) {
                Serial.println(F("Received command 0x11."));
            }
        }

        IrReceiver.resume();
    }
}