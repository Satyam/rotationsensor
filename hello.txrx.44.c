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
#define settle_delay() _delay_us(100) // settle delay
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
      //
      static unsigned char count;
      static uint16_t up,down;
      //
      // accumulate
      //
      up = 0;
      down = 0;

      clear(*port,pin);
      output(transmit_direction, pin);

      for (count = 0; count < nloop; ++count) { 
         //
         // settle, charge
         //
         settle_delay();
         set(*port,pin);
         //
         // initiate conversion
         //
         ADCSRA |= (1 << ADSC);
         //
         // wait for completion
         //
         while (ADCSRA & (1 << ADSC))
            ;
         //
         // save result
         //
         up += ADC;
         //
         // settle, discharge
         //
         settle_delay();
         clear(*port,pin);
         //
         // initiate conversion
         //
         ADCSRA |= (1 << ADSC);
         //
         // wait for completion
         //
         while (ADCSRA & (1 << ADSC))
            ;
         //
         // save result
         //
         down += ADC;
         }
      //
      // send framing
      //
      put_char(&serial_port, serial_pin_out, 1);
      char_delay();
      put_char(&serial_port, serial_pin_out, 2);
      char_delay();
      put_char(&serial_port, serial_pin_out, 3);
      char_delay();
      put_char(&serial_port, serial_pin_out, 4);
      //
      // send result
      //
      put_char(&serial_port, serial_pin_out, pad);
      char_delay();
      put_char(&serial_port, serial_pin_out, (up & 255));
      char_delay();
      put_char(&serial_port, serial_pin_out, ((up >> 8) & 255));
      char_delay();
      put_char(&serial_port, serial_pin_out, (down & 255));
      char_delay();
      put_char(&serial_port, serial_pin_out, ((down >> 8) & 255));
      char_delay();
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
   //clear(transmit_port, transmit_pin1);
   //output(transmit_direction, transmit_pin1);
   //
   // init A/D
   //
   ADMUX = (0 << REFS1) | (0 << REFS0) // Vcc ref
      | (0 << ADLAR) // right adjust
      | (0 << MUX5) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // PA7
   ADCSRA = (1 << ADEN) // enable
      | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128
   //
   // main loop
   //
   while (1) {

      getPadReading(&transmit_port,transmit_pin1,1);
      getPadReading(&transmit_port,transmit_pin2,2);
      getPadReading(&transmit_port,transmit_pin3,3);
      getPadReading(&transmit_port,transmit_pin4,4);
      }
   }
