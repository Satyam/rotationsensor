//
// hello.txrx.45.c
//
// step response transmit-receive hello-world
//    9600 baud FTDI interface
//
// Neil Gershenfeld
// 11/6/11
//
// (c) Massachusetts Institute of Technology 2011
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include <avr/io.h>
#include <util/delay.h>

#define output(directions,pin) (directions |= pin) // set port direction for output
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define pin_test(pins,pin) (pins & pin) // test for port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set
#define bit_delay_time 8.5 // bit delay for 115200 with overhead
#define bit_delay() _delay_us(bit_delay_time) // RS232 bit delay
#define half_bit_delay() _delay_us(bit_delay_time/2) // RS232 half bit delay
#define adc_offset_delay() _delay_us(8) // settle delay
#define settle_delay() _delay_us(1) // settle delay
#define char_delay() _delay_ms(10) // char delay
#define nloop 100 // loops to accumulate

#define serial_port PORTA
#define serial_direction DDRA
#define serial_pin_out (1 << PA4)
#define transmit_port PORTA
#define transmit_direction DDRA
#define transmit_pin1 (1 << PA3) // Red wire (upper right pad)
#define transmit_pin2 (1 << PA2) // Yellow wire (bottom right pad)
#define transmit_pin3 (1 << PA1) // Grey wire (bottom left pad)
#define transmit_pin4 (1 << PA0) // Black wire (top left pad)

void put_char(volatile unsigned char *port, unsigned char pin, char txchar) {
   //
   // send character in txchar on port pin
   //    assumes line driver (inverts bits)
   //
   // start bit
   //
   clear(*port,pin);
   bit_delay();
   //
   // unrolled loop to write data bits
   //
   if bit_test(txchar,0)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,1)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,2)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,3)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,4)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,5)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,6)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,7)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   //
   // stop bit
   //
   set(*port,pin);
   bit_delay();
   //
   // char delay
   //
   bit_delay();
   }

void getPadReading(volatile unsigned char *port, unsigned char pin, unsigned char pad) {

      // Make sure all is settled
      //_delay_ms(10);

      //clear(transmit_port, pin);
      output(transmit_direction, pin);

      // Start the conversion
      ADCSRA |= 1 << ADSC;

      // Insert a delay so that the ADC has time to latch at an offset time (basically the pads charge really fast and the ADC is super slow)
      adc_offset_delay();

      // Charge pad
      set(*port,pin);

      // We wait for the conversion
      while (ADCSRA & (1 << ADSC));

      // Send framing
      put_char(&serial_port, serial_pin_out, 1);
      put_char(&serial_port, serial_pin_out, 2);
      put_char(&serial_port, serial_pin_out, 3);
      put_char(&serial_port, serial_pin_out, 4);

      // Send out which pad we are charging
      put_char(&serial_port, serial_pin_out, pad);

      // Send out the Low and High register data from the adc read.
      put_char(&serial_port, serial_pin_out, ADCL);
      put_char(&serial_port, serial_pin_out, ADCH);

      // Start the conversion
      ADCSRA |= 1 << ADSC;

      // Insert a delay so that the ADC has time to latch at an offset time (basically the pads charge really fast or the ADC is super slow)
      adc_offset_delay();

      // Discharge pad
      clear(*port,pin);

      // We wait for the conversion
      while (ADCSRA & (1 << ADSC));

      // Send out the Low and High register data from the adc read.
      put_char(&serial_port, serial_pin_out, ADCL);
      put_char(&serial_port, serial_pin_out, ADCH);
}

int main(void) {
   //
   // main
   //
   // set clock divider to /1
   //
   CLKPR = (1 << CLKPCE);
   CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);
   //
   // initialize output pins
   //
   set(serial_port, serial_pin_out);
   output(serial_direction, serial_pin_out);

   /*clear(transmit_port, transmit_pin1);
   output(transmit_direction, transmit_pin1);
   clear(transmit_port, transmit_pin2);
   output(transmit_direction, transmit_pin2);
   clear(transmit_port, transmit_pin3);
   output(transmit_direction, transmit_pin3);
   clear(transmit_port, transmit_pin4);
   output(transmit_direction, transmit_pin4);*/

   // Make sensing ring pin input
   clear(DDRA, PA7);
   //
   // init A/D
   //
   ADMUX = (0 << REFS1) | (0 << REFS0) // Vcc ref
      | (0 << ADLAR) // right adjust (which i believe is the default)
      // left justified looks like this 15,14,13,12,11,10,9,8,7,6,-,-,-,-,-,- (good if you want to just read at 8bit res)
      // right justified looks like this -,-,-,-,-,-,9,8,7,6,5,4,3,2,1,0
      | (0 << MUX5) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // PA7
   ADCSRA = (1 << ADEN) // enable
      | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128
   //
   // main loop
   //
   while (1) {

      //unsigned char ten = 0b1010;
      //unsigned char hundered = 0b1100100;

      //put_char(&serial_port, serial_pin_out, '1');
      //put_char(&serial_port, serial_pin_out, '10');
      /*put_char(&serial_port, serial_pin_out, 1);
      put_char(&serial_port, serial_pin_out, 2);
      put_char(&serial_port, serial_pin_out, 3);
      put_char(&serial_port, serial_pin_out, 4);
      put_char(&serial_port, serial_pin_out, 10);
      put_char(&serial_port, serial_pin_out, 15);
      put_char(&serial_port, serial_pin_out, 30);
      put_char(&serial_port, serial_pin_out, 100);*/
      //put_char(&serial_port, serial_pin_out, '\n');
      //char_delay();
      //char_delay();
      //getPadReading(&transmit_port,transmit_pin1,1,1);
      //getPadReading(&transmit_port,transmit_pin1,1,2);
      //getPadReading(&transmit_port,transmit_pin1,1,4);
      //getPadReading(&transmit_port,transmit_pin1,1,6);
      //getPadReading(&transmit_port,transmit_pin1,1,8);
      /*getPadReading(&transmit_port,transmit_pin1,1,10);
      getPadReading(&transmit_port,transmit_pin1,1,12);
      getPadReading(&transmit_port,transmit_pin1,1,14);
      getPadReading(&transmit_port,transmit_pin1,1,16);
      getPadReading(&transmit_port,transmit_pin1,1,18);*/
      //getPadReading(&transmit_port,transmit_pin1,1,5);
      //getPadReading(&transmit_port,transmit_pin1,1,10);
      //getPadReading(&transmit_port,transmit_pin1,1,100);

      getPadReading(&transmit_port,transmit_pin1,1);
      getPadReading(&transmit_port,transmit_pin2,2);
      getPadReading(&transmit_port,transmit_pin3,3);
      getPadReading(&transmit_port,transmit_pin4,4);
      }
   }
