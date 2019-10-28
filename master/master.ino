
// Protocol for I2C:
// [] indicates a byte, so [a][b] indicates 2 bytes in succession
//protocol: [command][parameter 1]...[parameter n]   //amount of parameters depend on command
// [command] 1: read from address, 2: write to address, 3:analogRead, 4:digitalRead, 5:digitalWrite
//
// command 1 parameters: [read_address], return value: [data]
// command 2 parameters: [write_address][byte]
// command 3 parameters: [pin], return value: [high byte][low byte]
// command 4 parameters: [pin], returns: [bool]
// command 5 parameters: [pin][value]

#define SLAVE_ADDR 8

// Wire Master Reader
#include <Wire.h>


bool led = true;
int incrementer;

void setup() {
  Wire.begin(); // no address, since this is a master
  Serial.begin(9600); // start serial for output
  incrementer = 0;
}

void loop() {

  Serial.println(readByteSlave(SLAVE_ADDR, 1));   //reads a byte at memory_address 1

  writeByteSlave(SLAVE_ADDR, 1, incrementer);
  incrementer++;

  Serial.print("analogReadSlave A0: ");
  Serial.println(analogReadSlave(SLAVE_ADDR, A0));  //reads from analogPort A0 on the slave

  Serial.print("digitalReadSlave, pin 12: ");
  Serial.println(digitalReadSlave(SLAVE_ADDR, 12));

  digitalWriteSlave(SLAVE_ADDR, 13, led); //writes true/false to pin 13 on the other arduino (blinks the onboard LED)
  led = !led;

  Serial.println(""); // to easier read in terminal
  delay(500);
}

int analogReadSlave(byte address, byte pin)
{
  Wire.beginTransmission(address);
  Wire.write(3); //analogRead command
  Wire.write(pin); //first parameter
  Wire.endTransmission();

  //we need an answer, two bytes long
  Wire.requestFrom(address, 2);

  byte data[2];
  byte i = 0;
  while (Wire.available())
  {
    data[i] = Wire.read(); //get the data
    i++;
  }

  //the received data is [high byte][low byte], so it must be put back to an int:
  int final_value;
  final_value += data[0] << 8;  //high byte
  final_value += data[1]; //low byte

  return final_value;
}

byte digitalReadSlave(byte address, byte pin)
{
  Wire.beginTransmission(address);
  Wire.write(4); // digitalRead command
  Wire.write(pin);
  Wire.endTransmission();

  //we need an answer, 1 byte long
  Wire.requestFrom(address, 1);

  byte data;
  while (Wire.available())
    data = Wire.read();
    
  return data;
}

void digitalWriteSlave(byte address, byte pin, byte value)
{
  Wire.beginTransmission(address);
  Wire.write(5); //digitalWrite command
  Wire.write(pin); //first parameter
  Wire.write(value); //second parameter
  Wire.endTransmission();
}

byte readByteSlave(byte address, byte read_addr)
{
  Wire.beginTransmission(address);
  Wire.write(1); //read command
  Wire.write(read_addr);  ////first parameter
  Wire.endTransmission();

  //we want an answer, request it, its size is 1 byte:
  Wire.requestFrom(address, 1);

  byte data;
  while (Wire.available())
    data = Wire.read(); //get the answer back

  return data;
}

void writeByteSlave(byte address, byte write_addr, byte data)
{
  Wire.beginTransmission(address);
  Wire.write(2);  //write command
  Wire.write(write_addr);
  Wire.write(data);
  Wire.endTransmission();
}
