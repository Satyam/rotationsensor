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
// The following convert the patterns like this:
// 0 ==> Z (zero), 3 ==> Q (quarter), 6 ==> H (half), n ==> N (number, none of the previous)
#define Z 0
#define Q 1
#define H 2 
#define N 3
#define P(a, b, c, d) (a << 6 + b << 4 + c << 2 + d)

char decoders[] = {
//	_6330: 0,
	P(H,Q,Q,Z), 0,
//	_n3n0: [[2, -1],[0, 2]],
	P(N,Q,N,Z), 2, -1, 0, 2,
//	_3360: 1,
	P(Q,Q,H,Z), -1,
//	_nn60: [[1, 0],[0, 2]],
	P(N,N,H,Z), 1, 0, 0, 2,
//	_0660: 2,
	P(Z,H,H,Z), -2,
//	_06nn: [[3, 2], [2, 4]],
	P(Z,H,N,N), 3, 2, 2, 4,
//	_0633: 3,
	P(Z,H,Q,Q), -3,
//	_0n3n: [[3, 2], [1, 5]],
	P(Z,N,Q,N), 3, 2, 1, 5,
//	_0336: 4,
	P(Z,Q,Q,H), -4,
//	_0nn6: [[2, 3], [1, 5]],
	P(Z,N,N,H), 2,3,1,5,
//	_0066: 5,
	P(Z,Z,H,H), -5,
//	_n06n: [[0, 5], [3, 7]],
	P(N,Z,H,N), 0,5,3,7,
//	_3063: 6,
	P(Q,Z,H,Q), -6,
//	_n0n3: [[0, 5], [2, 8]],
	P(N,Z,N,Q), 0,5,2,8,
//	_6033: 7,
	P(H,Z,Q,Q), -7,
	
//	_60nn: [[3, 6], [2, 8]],
	P(H,Z,N,N), 3,6,2,8,
//	_6006: 8,
	P(H,Z,Z,H), -8,
//	_nn06: [[1, 8],[0,10]],
	P(N,N,Z,H), 1,8,0,10,
//	_3306: 9,
	P(Q,Q,Z,H), -9,
//	_3n0n: [[1, 8],[3, 11]],
	P(Q,N,Z,N), 1,8,3,11,
//	_3603: 10,
	P(Q,H,Z,Q), -10,
//	_n60n: [[0,9],[3, 11]],
	P(N,H,Z,N), 0,9,3,11,
//	_6600: 11,
	P(H,H,Z,Z), -11,
//	_6nn0: [[2,11],[1,13]]
	P(H,N,N,Z), 2,11,1,13
};
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
	
		unsigned char value = up - down;
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
