
#include "packets.hpp"

#include <Arduino.h>

sample_packet pack =
{
  .magic = MAGIC,
  .id = 0
};

volatile bool sampale = false;

void setup()
{
  cli();
  TCCR1A = 0;     //0 - 0xffff
  TCCR1B = 0b001; //clk/no prescaler

  TCCR1B |= (1<<WGM12); // CTC mode.
  
  OCR1A = 5000;  //output compare register, max Timer/Counter value
  
  TIMSK1 |= (1<<OCIE1A); // IRQ      //TIMSK1- timer interrupt mask register
  sei();  //set global interrupt enabled
  
  Serial.begin(115200);
}

ISR(TIMER1_COMPA_vect)
{
   sample = true;
}

void loop() {
  if(sample){
    sample = false;
    
    pack.err = false;
    
    pack.val_array[0] = analogRead(A0);
    pack.val_array[1] = analogRead(A1);
    pack.val_array[2] = analogRead(A2);
    pack.val_array[3] = analogRead(A3);
    pack.val_array[4] = analogRead(A4);
    pack.val_array[5] = analogRead(A5);
    
    if(sample)
    {
      pack.err = true;
    }
    
    pack.id++;
    //Serial.write((uint8_t*)&pack, sizeof(sample_packet));

    Serial.write((uint8_t*)&pack.val_array[0], sizeof(pack.val_array[0]));
  
  }
}
