#include "pio_encoder.h"
#include "Adafruit_TinyUSB.h"

#define SW    6
#define BT1   2
#define BT2   3

PioEncoder encoder(7);
long oldPosition  = -999;
bool btSwPressed, bt1Pressed, bt2Pressed = false;

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_GAMEPAD()
};

// USB HID object
Adafruit_USBD_HID usb_hid;
hid_gamepad_report_t gp;

void setup() {
  // Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  Serial.begin(115200);
  pinMode(SW, INPUT_PULLUP);
  pinMode(BT1, INPUT_PULLUP);
  pinMode(BT2, INPUT_PULLUP);
  encoder.begin();

  // Setup HID
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

 // Reset buttons
  Serial.println("No pressing buttons");
  gp.x = 0;
  gp.y = 0;
  gp.z = 0;
  gp.rz = 0;
  gp.rx = 0;
  gp.ry = 0;
  gp.hat = 0;
  gp.buttons = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(1000);

  Serial.println("Adafruit TinyUSB HID Gamepad example");
}

void loop() {

  #ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
  #endif

  if (!TinyUSBDevice.mounted()) {
    return;
  }

  if (!usb_hid.ready()) return;

  long newPosition = encoder.getCount();
  if (newPosition != oldPosition) {
    if(newPosition > oldPosition) {
        gp.hat = 1; // GAMEPAD_HAT_UP;
    usb_hid.sendReport(0, &gp, sizeof(gp));
    delay(200);
      gp.hat = 0; // GAMEPAD_HAT_CENTERED;
  usb_hid.sendReport(0, &gp, sizeof(gp));
    }
    else {
        gp.hat = 5; // GAMEPAD_HAT_UP;
    usb_hid.sendReport(0, &gp, sizeof(gp));
    delay(200);
      gp.hat = 0; // GAMEPAD_HAT_CENTERED;
  usb_hid.sendReport(0, &gp, sizeof(gp));
    }
    oldPosition = newPosition;
    Serial.println(newPosition);
  }

//BTSW
int btSwStatus = digitalRead(SW);
  if(btSwStatus == 0 && !btSwPressed) {
    Serial.println("SWPressed");
    btSwPressed = true;
        gp.buttons = 1;
    usb_hid.sendReport(0, &gp, sizeof(gp));
  }
  if(btSwStatus == 1 && btSwPressed) {
    gp.buttons = 0;
    usb_hid.sendReport(0, &gp, sizeof(gp));
    btSwPressed = false;
    Serial.println("SWReleased");
  }

//BT1
int bt1Status = digitalRead(BT1);
  if(bt1Status == 0 && !bt1Pressed) {
    Serial.println("1Pressed");
    bt1Pressed = true;
        gp.buttons = 2;
    usb_hid.sendReport(0, &gp, sizeof(gp));
  }
  if(bt1Status == 1 && bt1Pressed) {
    gp.buttons = 0;
    usb_hid.sendReport(0, &gp, sizeof(gp));
    bt1Pressed = false;
    Serial.println("1Released");
  }

  int bt2Status = digitalRead(BT2);
  if(bt2Status == 0 && !bt2Pressed) {
    Serial.println("2Pressed");
    bt2Pressed = true;
        gp.buttons = 8;
    usb_hid.sendReport(0, &gp, sizeof(gp));
  }
  if(bt2Status == 1 && bt2Pressed) {
    gp.buttons = 0;
    usb_hid.sendReport(0, &gp, sizeof(gp));
    bt2Pressed = false;
    Serial.println("2Released");
  }

}
