// - Board: Arduino Uno
// - Shield: AC logger

#include <Arduino.h>
#include <stdlib.h>
#include "avr_io_bitfields.h"

#include "packets.hpp"

volatile bool to_send = false;
volatile u8 mux_select = 0;

volatile smpl_pkg pkg = {
	.magic = MAGIC,
	.id = 0,
	.val_array = {0},
	.err = 0
};

static void print_reg(const char *name, u8 r) {
	Serial.print(name);
	Serial.print(" = 0b");
	for (int i = 7; i >= 0; i--)
	{
		if ((r >> i) & 1)
		{
			Serial.print('1');
		}
		else
		{
			Serial.print('0');
		}
		if (i == 4)
		{
			Serial.print('_');
		}
	}
	Serial.println();
}

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
	u16 sample = adc.r.adcw;

	pkg.val_array[mux_select] = sample;

	mux_select++;

	// Steer to nexmux_selecttrue;
	if (mux_select == 6){
		mux_select = 0;
		to_send = true;
	}

	delay(10);
	
	set_adc_mux(mux_select);

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
	OCR1A = F_CPU / (50 * 32 * 2) * 8;

	TIMSK1 |= (1 << OCIE1A); // IRQ      //TIMSK1- timer interrupt mask register
	sei();					 // set global interrupt enabled
}

void loop() {
	if(to_send){
		to_send = false;

		pkg.err = false;

		if(to_send){
			pkg.err = true;
		}

		pkg.id++;

		Serial.write("[0] = ");
		String data0 = String(pkg.val_array[0]);
		Serial.println(data0);
		Serial.write("\n");

		Serial.write("[1] = ");
		String data1 = String(pkg.val_array[1]);
		Serial.println(data1);
		Serial.write("\n");

		Serial.write("[2] = ");
		String data2 = String(pkg.val_array[2]);
		Serial.println(data2);
		Serial.write("\n");

		Serial.write("[3] = ");
		String data3 = String(pkg.val_array[3]);
		Serial.println(data3);
		Serial.write("\n");

		Serial.write("[4] = ");
		String data4 = String(pkg.val_array[4]);
		Serial.println(data4);
		Serial.write("\n");

		Serial.write("[5] = ");
		String data5 = String(pkg.val_array[5]);
		Serial.println(data5);
		Serial.write("\n");

		

		delay(1);
		//uint16_t reading = pkg.val_array[0];
		//Serial.write("aleksa\n");

		//Serial.write((uint8_t *)&pkg, sizeof(pkg));

	}
	//Serial.write("LOOP");
}
