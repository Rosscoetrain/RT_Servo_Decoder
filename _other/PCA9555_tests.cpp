PCA9555 expander(0x20); // Default address with A0, A1, A2 to GND

void setup() {
  Serial.begin(115200);
  Wire.begin(); 
  
  // Init: Pins 0-7 as OUTPUT (0), Pins 8-15 as INPUT (1)
  // Binary: 1111 1111 0000 0000 -> 0xFF00
  expander.init(0xFF00); 
  Serial.println("PCA9555 Initialised");
}

void loop() {
  // Test setAll: Blink Port 0
  expander.setAll(0x00FF);
  delay(500);
  expander.setAll(0x0000);
  delay(500);

  // Test setPin and getPin: Mirror pin 8 to pin 0
  bool pin8Status = expander.getPin(8);
  expander.setPin(0, pin8Status);

  // Test getAll: Print full state
  uint16_t allPins = expander.getAll();
  Serial.print("Current State: 0x");
  Serial.println(allPins, HEX);
}



////////////////////////////////////////////////////////////////////


const uint8_t interruptPin = 2; 
volatile bool deviceChanged = false;
PCA9555 expander(0x20);

// Interrupt Service Routine (ISR)
// Keeps code minimal: just set a flag
void IRAM_ATTR pcalnterrupt() {
  deviceChanged = true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Nano D2 supports FALLING interrupts
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), pcalnterrupt, FALLING);

  // Init all pins as INPUTs (0xFFFF)
  expander.init(0xFFFF);
  
  Serial.println("System Ready. Change an input state to trigger interrupt...");
}

void loop() {
  if (deviceChanged) {
    // Clear flag
    deviceChanged = false;

    // Read the device to see what changed and clear the INT hardware pin
    uint16_t state = expander.clearInterrupt();

    Serial.print("Interrupt Detected! Port State: 0x");
    Serial.println(state, HEX);
  }
}


/////////////////////////////////////////////////////////////////////////



PCA9555_NoWire expander(A4, A5); // Using A4 (SDA) and A5 (SCL) manually

void setup() {
  Serial.begin(115200);
  expander.init(0x0000); // Set all as outputs
}

void loop() {
  expander.setAll(0xFFFF);
  delay(1000);
  expander.setAll(0x0000);
  delay(1000);
}



//////////////////////////////////////////////////////////////////////////////////




PCA9555_Fast expander(0x20);

void setup() {
  Serial.begin(115200);
  expander.init(0x0000); // All outputs
}

void loop() {
  // Rapid toggle test
  expander.writeReg16(0x02, 0xFFFF);
  expander.writeReg16(0x02, 0x0000);
}



