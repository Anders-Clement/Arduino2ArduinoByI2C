
//Protocol can be seen in the master file
// Wire Slave Sender

#include <Wire.h>


byte read_addr, last_command, write_addr;
int led_pin = 13;

byte mem_map;

void setup() {
  Wire.begin(8); // join i2c bus with address #8
  Wire.onRequest(requestEvent); // calls requestEvent whenever data is requested
  Wire.onReceive(receiveEvent); // calls receiveEvent whenever it receives data
  pinMode(led_pin, OUTPUT);
}

void loop() {
  delay(100); // not strictly necessary
}


void requestEvent() {

  if (last_command == 1) // read command
  {
    //for now just returns fixed values, but could be made to access any values you want present in this code
    if (read_addr == 1)
      Wire.write(mem_map);
    else
      Wire.write(0);
  }
  else if (last_command == 3) //analogRead
  {
    int value = analogRead(read_addr);  //read from the pin
    //must send [high byte][low byte], so the value must be split up into two:
    byte high = (value >> 8);
    byte low = value;

    //and send it
    Wire.write(high);
    Wire.write(low);
  }
  else if (last_command == 4) // digitalRead
  {
    pinMode(read_addr, INPUT);
    byte value = digitalRead(read_addr);
    Wire.write(value);
  }
}

void receiveEvent(int numBytes) //we know how many bytes we received, whenever we receive
{
  byte data[numBytes];  //create array to store the data and get it:
  while (!Wire.available())
    delayMicroseconds(10);
  for (int i = 0; i < numBytes; i++)
    data[i] = Wire.read();

  // data now holds our packet: [command][parameter 1]...[parameter n]

  last_command = data[0]; //to use in requestEvent

  if (last_command == 1) //read command
    read_addr = data[1]; //save the read_addr so it can be returned when requested

  else if (last_command == 2) // write command
  {
    byte mem_map = data[3]; // this data can then be saved in a memory_map using write_addr
    write_addr = data[1]; // not used in this implementation
  }
  else if (last_command == 3) //analogRead
    read_addr = data[1]; //save the pin, so it can be read from when requested

  else if (last_command == 4) //digitalRead command
    read_addr = data[1]; //save the pin, so it can be read from when requested

  else if (last_command == 5) //digitalWrite command, received data: [pin][value]
    digitalWrite(data[1], data[2]);
}
