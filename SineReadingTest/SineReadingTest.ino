// - Board: Arduino Uno
// - Shield: AC logger

#include <Arduino.h>
#include <stdlib.h>
#include <SPI.h>
#include <SD.h>
#include "virtuabotixRTC.h"

#include "avr_io_bitfields.h"
#include "packets.hpp"

#define CS 10
#define PKG_LEN 10

volatile bool to_send = false;
volatile u8 mux_select = 0;

//String entries[PKG_LEN];
volatile int entry_select = 0;

String entry = "";

virtuabotixRTC rtc1(6, 7, 8);

volatile smpl_pkg pkg = {
	.magic = MAGIC,
	.id = 0,
	.data = {0}
};

static void set_adc_mux(u8 mux)
{
	set_bf(adc.g.admux, {
		.mux = mux,
		.adlar = 0,	 // Left align ADC value to 8 bits from ADCH register.
		.refs = 0b01 // Vcc TODO
	});
}

ISR(TIMER1_COMPA_vect) {
	// Take prev sample.
	//Serial.println(a1);
	//a1++;
	u16 sample = adc.r.adcw;

	pkg.data.val_array[mux_select] = sample;

	mux_select++;

	// Steer to nexmux_selecttrue;
	if (mux_select == 6){
		mux_select = 0;
		to_send = true;
	}
	
	set_adc_mux(mux_select);

	delay(20);
	
	// Start new sample.
	adc.f.adsc = 1;
}

void setup() {

	Serial.begin(115200);

	// ADC setup.

	// Disable digital buffer on analog input.
	adc.r.didr_0 = 0xff;

	set_adc_mux(mux_select);

	adc.g.adcsrb.adts = 0; // Free Running mode

	set_bf(adc.g.adcsra, {
		// f_adc_in = 38.5 kHz
		// f_smpl_max = 77 kHz
		// f_smpl = 9.6MHz/16/13 = 46.154 kHz
		// f_in = 46.154/2 = 23 kHz
		.adps = 0b100, // 16 prescaler.
		.adie  = 0, // No IRQ.
		.adif  = 0, // Clear IRQ.
		.adate = 0, // Auto trigger disabled.
		.adsc = 1, // Warm up ADC, because first sampling is 25 CLKs.
		.aden = 1 // Enable ADC.
	});



#if 0
	print_reg("ADCSRA", ADCSRA);
	print_reg("ADMUX", ADMUX);
	print_reg("ADCSRB", ADCSRB);
	//print_reg("ADCSRA", ADCSRA);
	print_reg("ADCL", ADCL);
	print_reg("ADCH", ADCH);
	Serial.println();
#endif


	// TODO Nicer: use structs. <- No.
	cli();
	TCCR1A = 0;		// 0 - 0xffff
	TCCR1B = 0b001; // clk/no prescaler

	TCCR1B |= (1 << WGM12); // CTC mode.

	// output compare register, max Timer/Counter value
	// TODO Defines and explanation for these numbers.
	OCR1A = (F_CPU / (50 * 32 * 2) * 8);

	TIMSK1 |= (1 << OCIE1A); // IRQ      //TIMSK1- timer interrupt mask register
	sei();					 // set global interrupt enabled

	if(!SD.begin(CS))
	{
		Serial.println("Couldn't initialize SD Card module.\n");
		while(1) {}
	}

	//rtc1.setDS1302Time(30, 15, 13, 4, 20, 6, 2024);

	if(!SD.mkdir("aclogger"))
	{
		Serial.println("Couldn't create a folder to store the log file.\n");
	}

	
	
}

void loop() {

	rtc1.updateTime();

	if(to_send){
		
		to_send = false;
		
		pkg.data.err = false;


		pkg.id++;

		pkg.data.timestamp[0] = rtc1.seconds;
		pkg.data.timestamp[1] = rtc1.minutes;
		pkg.data.timestamp[2] = rtc1.hours;
		pkg.data.timestamp[3] = rtc1.dayofmonth;
		pkg.data.timestamp[4] = rtc1.month;

		if(!pkg.data.err)
		{
			/*
			UPIS NA SD KARTICU
			*/

			
			entry += String(pkg.id) + '\t' +
					 String(pkg.data.timestamp[0]) + '\t' + 
					 String(pkg.data.timestamp[1])+ '\t' + 
					 String(pkg.data.timestamp[2]) + '\t' +
					 String(pkg.data.timestamp[3]) + '\t' + 
					 String(pkg.data.timestamp[4]) + '\t' +
					 String(pkg.data.val_array[0]) + '\t' +
					 String(pkg.data.val_array[1]) + '\t' +
					 String(pkg.data.val_array[2]) + '\t' +
					 String(pkg.data.val_array[3]) + '\t' +
					 String(pkg.data.val_array[4]) + '\t' +
					 String(pkg.data.val_array[5]) + '\n';
					 
			Serial.println(entry);
			/*entries[entry_select] = entry;
			//Serial.println(entries[entry_select]);*/
			entry_select++;
			
		}

		if(entry_select == PKG_LEN) {
			
			//ispis svega
			File sd_fd = SD.open("aclogger/log.txt", FILE_WRITE);
			
			if(sd_fd) {
				
				sd_fd.print(entry);

				sd_fd.close();

				

			}
			else {

				Serial.println("Couldn't open log file.\n");
			}

			entry = "";
			entry_select = 0;
		}	
	}
}