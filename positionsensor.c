//
// positionsensor.c
//
// rotation sensor using step response
//
// Credits:
//   Neil Gershenfeld
//   11/6/11
//
//   (c) Massachusetts Institute of Technology 2011
//   Permission granted for experimental and personal use;
//   license for commercial sale available from MIT.
//
//   Rotation calculation
//   Creative Commons CC BY-SA
//     Andrew Leek
//     Daniel Barreiro
//   FabLab Sitges
//   April 2014

#include <avr/io.h>
#include <util/delay.h>

#define output(directions,pin) (directions |= pin) // set port direction for output
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~(pin))) // clear port pin
#define pin_test(pins,pin) (pins & pin) // test for port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set
#define bit_delay_time 8.5 // bit delay for 115200 with overhead
#define bit_delay() _delay_us(bit_delay_time) // RS232 bit delay
#define half_bit_delay() _delay_us(bit_delay_time/2) // RS232 half bit delay

#define batch_size 100          // number of readings averaged per transmission

#define adc_offset_delay() _delay_us(8) // settle delay

#define serial_port PORTA
#define serial_direction DDRA
#define serial_pin_out (1 << PA4)

#define pulsing_pad_port PORTA
#define pulsing_pad_direction DDRA
#define pulsing_pin1 (1 << PA3) // Red wire (upper right pad)
#define pulsing_pin2 (1 << PA2) // Yellow wire (bottom right pad)
#define pulsing_pin3 (1 << PA1) // Grey wire (bottom left pad)
#define pulsing_pin4 (1 << PA0) // Black wire (top left pad)

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

// Macro to make it easier to call put_char
#define putChar(chr) put_char(&serial_port, serial_pin_out, chr)

// converts an unsinged integer to hex (preceeded by 0x) and sends it via serial
/*
void put_hex(uint16_t value) {
	int8_t shift,
		chr;
	putChar('0');
	putChar('x');
	for (shift = 12; shift >= 0; shift -=4) {
		chr = ((value >> shift) & 0xf) + '0';
		if (chr > '9') chr += 7;
		putChar(chr);
	}
}
*/

void put_uint16(uint16_t value) {
	char out[5] = {'0','0','0','0','0'};
	int8_t digit;
	for (digit = 4; digit >= 0; digit--) {
		out[digit] = (value % 10) + '0';
		value /= 10;
	}
	for (digit = 0;digit < 5; digit++) {
		putChar(out[digit]);
	}
}


// Just a simple mechanism to refer to the pins that correspond to the pulsing pads		
uint8_t pulsing_pins[4] = {
	pulsing_pin1,
	pulsing_pin2,
	pulsing_pin3,
	pulsing_pin4
};

/* Structure for the decoding table.

The readouts from the pads produce patterns that allow to decode their position.

At any one time, one pad will not have any sensing pad above.  
That pad will produce a reading of zero, signalled by a Z in the table below.
The postion of this zero changes.  The Z then will be in the position where
the zero is expected to be.
All patterns have a Z somewhere.

Sometimes a pad is fully overlapping the sensing pad. That is represented by
an H for half (more on that later).  

Sometimes the gap in between the two sensing pads is on top of a pulsing pad.
Since the gap in between the sensing pads is half a pulsing pads, they
end up getting a quarter of the signal, this they are signalled by a Q for quarter.

Patterns may have H or Q, or they may not.

The reason why a full coverage gets an H for half is that there are two 
sensing pads so out of the total value for all the readings, only
one sensing pad out of two can cover a pulsing pad
so it can only get half the total signal.

Then, there will be pads that are partially covered by the pulsing pads.
They are marked by an N  (change it to X if you will, anyway, they are variable).

As it turns out, there are four pads and the values of those pads can be
any combination of Z, H, Q or N.  Being 4 possible values, they can be
encoded into 2 bits, times 4 pads, fits nicely into 8 bits.

The patterns of which pulsing pads cover what sensing pads change every 30 degrees.
There is a particular pattern exactly at every 30 degree multiple which I call the
simple pattern.  The patterns for positions multuple of 30degree are stored in the
patternS (for simple) entry.

When you get one of those patterns, the angle is 30 degrees times the multiplier

In between each of those positions multiple of 30 degrees there is another pattern
which I call the complex one.  The pattern is stored in the patternC member.
The angle comes out of reading the values of a couple of pads, 
indicated in pad1 and pad2 and multuplying each by a multiplier, mult1, mult2.
This calculation is in function decodeComplex below.

The table of decode structs must be ended by a 0.

There is a good change that the whole table can be calculated from some algorithm.
At this point is irrelevant since when decoding, searching the table is the fastest.
*/
typedef struct {
	uint8_t patternS;
	uint8_t multiplier;
	uint8_t patternC;
	uint8_t pad1;
	int8_t mult1;
	uint8_t pad2;
	int8_t mult2;
} decodeStruct;

// Macros to make it easy to define the patterns for the decodeStruct entries
// There is code below that assume these macros have these values.  Don't change them.
#define Z 3
#define Q 1
#define H 2 
#define N 0
// P for pattern, encodes the pattern into a single byte
#define P(a, b, c, d) ((a << 6) + (b << 4) + (c << 2) + d)

// table of decodeStruct entries.  
// MUST be ended with a zero
static uint8_t decoders[] = {
	P(H,Q,Q,Z), 0,
	P(N,Q,N,Z), 2, -1, 0, 2,
	P(Q,Q,H,Z), 1,
	P(N,N,H,Z), 1, 0, 0, 2,
	P(Z,H,H,Z), 2,
	P(Z,H,N,N), 3, 2, 2, 4,
	P(Z,H,Q,Q), 3,
	P(Z,N,Q,N), 3, 2, 1, 5,
	P(Z,Q,Q,H), 4,
	P(Z,N,N,H), 2, 3, 1, 5,
	P(Z,Z,H,H), 5,
	P(N,Z,H,N), 0, 5, 3, 7,
	P(Q,Z,H,Q), 6,
	P(N,Z,N,Q), 0, 5, 2, 8,
	P(H,Z,Q,Q), 7,
	P(H,Z,N,N), 3, 6, 2, 8,
	P(H,Z,Z,H), 8,
	P(N,N,Z,H), 1, 8, 0, 10,
	P(Q,Q,Z,H), 9,
	P(Q,N,Z,N), 1, 8, 3, 11,
	P(Q,H,Z,Q), 10,
	P(N,H,Z,N), 0, 9, 3, 11,
	P(H,H,Z,Z), 11,
	P(H,N,N,Z), 2, 11, 1, 13,
	0
};


// The values read for each pad
uint16_t values[4]; 

// The accumulated values, differences (see decodeComplex), 
// number of successful reads and the margin on each pattern recognition
// added up for each batch of batch_size iterations
uint16_t diffs, finds, margins;

// In order to keep the most information we gather, value will
// hold the angle in tenths of a degree. 
// Also, since we have two separate readings, instead of averaging
// the two readings and possibly lossing information by ignoring the
// fraction I will accumulate both readings and average them all at once.
uint32_t value;

// return the absolute value of an integer
uint16_t abs (int16_t v) {
	return (v < 0?-v:v);
};

// Given a pattern, searches for a match in the decoders table
// and accumulates the calculated angle into value.
// Returns true if succcessful, false if the pattern could not be found.
int8_t decodeSimple (uint8_t pattern, uint16_t total, uint16_t quarter) {
	decodeStruct *p;
	
	for (p = (decodeStruct*) decoders; p->patternS; p++) {
		if (p->patternS == pattern) {
			// Times 30 because this patterns occur every 30 degrees.
			// Times 2 to make it as if I had two readings, like in the complex case
			// times 10 to turn it into tenths of degrees.
			// The compiler solves this multiplication of constants so it takes no time
			// at the chip end.
			value += p->multiplier * 30 * 2 * 10;
			return 1;
		}
	}
	return 0;
}

// Given a pattern, it searches for a match in the decoders table.
// Complex patterns always have a Z, one of H or Q and two N.
// The angle can be calculated independently from any of those Ns
// Here we calculate them both and accumulate the average to 
// produce the angle for the batch.
// We also accumulate the difference in between those two values
// to have a measure of the confidence of the reading.
// If the pattern could not be found, it returns false.
int8_t decodeComplex(uint8_t pattern, uint16_t total, uint16_t quarter) {
	decodeStruct *p;
	int32_t value1, value2;
	
	for (p = (decodeStruct*) decoders; p->patternS; p++) {
		if (p->patternC == pattern) {
			// times 120 to turn it into degreees, 
			// times 10 to make it tenths of degrees
			
			// TODO check whether the type-casting can be dropped, I think it is not required.
			value1 = (int32_t)(values[p->pad1] + p->mult1 * quarter)  * 120 * 10 / total;
			value2 = (int32_t)(p->mult2 * quarter - values[p->pad2])  * 120 * 10 / total;
			
			/*
			Alternative: 
				Instead of averaging the two values read
				and report the difference, 
				you might want to simply reject readings
				where the difference is greater than a
				certain value.
				To do that, simply return 0 when the
				error is unacceptable and accumulate nothing
			*/
			value += value1 + value2;
			diffs += abs(value1 - value2);
			return 1;
		} 
	}
	return 0;
}

// Converts a reading of the four pads into an angle and accumulates it
// for averaging over a batch of readings.
void convert () {
	uint16_t min,
		total,
		half,
		quarter,
		margin;
	
	uint8_t pad,
		found,
		numN,
		pattern;
	
	// Find the minumum value
	for (min = UINT16_MAX, pad = 0; pad < 4; pad++) {
		if (values[pad] < min) {
			min = values[pad];
		}
	}

	// Subtract the minimum found from each value
	// so we have a baseline.
	// Also calculate the total for all those values
	for (total = pad = 0; pad < 4; pad++) {
		values[pad] -= min;
		total += values[pad];
	}

	// Calculate half and quarter the total readout to
	// speed up the pattern matching below
	half = total >> 1;
	quarter = half >> 1;

	// Since the values come with noise, they will rarely
	// match any pattern so we accept a margin of error
	// due to noise.  We try a match with 0 margin but
	// if it fails, we keep increasing the margin
	// until a match is found or the margin becomes unacceptable
	for ( margin = found = 0; !found && margin < (total >> 2); margin++) {
		
		// here we try to recognize the pattern
		for (numN = pattern = pad = 0; pad < 4; pad++) {
			pattern <<=2;
			if (abs(values[pad]) < margin) pattern+=Z;
			else if (abs(values[pad] - quarter) < margin) pattern += Q;
			else if (abs(values[pad] - half) < margin) 	pattern += H;
			else {
				// pattern += N;  // since N is zero, it won't really make a difference
				numN++;
			}
		}
		
		// Valid patterns can have either 2 or no 'N's.
		// So I count the number of Ns in numN.
		switch (numN) {
			case 0: // no Ns, it is a simple pattern
				found += decodeSimple(pattern, total, quarter);
				break;
			case 1: // needs patching
				/*
				It often happens with values close the positions multiple of 30 degrees
				that a value is quite close to Z, Q or H, well within the margin accepted.
				No pattern can have just 1 N, they can have 2 or none at all.
				Here we turn each of the pad values into an N and see if we then get a match.
				*/
				for (pad = 0; pad < 4 && !found; pad++) {
					found += decodeComplex(pattern & (~(3<< (pad * 2))), total, quarter);
				}
				break;
			case 2: // two Ns, it is a complex pattern
				found += decodeComplex(pattern, total, quarter);
				break;
			case 3: // two many Ns, there is an H or Q missing somewhere, retry
				break;
		}
		// If there was a match, we accumulate the margin for this match to report the average
		// and we increment the count of finds
		if (found) {
			margins += margin;
			finds++;
		}
	}
};

	
// Reads a single pad
// This mostly relates to handling the hardware
uint16_t getPadReading(uint8_t pad) {
	
	uint16_t up;
	
	  // Not sure why - but these appear to be necessary (perhaps only when i am using the same pin continuously)
      // Maybe when switching pins I dont need the clear part (maybe its best to look at it as an init)
      clear(pulsing_pad_port,pulsing_pins[pad]);
      output(pulsing_pad_direction, pulsing_pins[pad]);
	
      // Start the conversion
      ADCSRA |= 1 << ADSC;

      // Insert a delay so that the ADC has time to latch at an offset time (basically the pads charge really fast and the ADC is super slow)
      adc_offset_delay();

      // Charge pad
      set(pulsing_pad_port,pulsing_pins[pad]);

      // We wait for the conversion
      while (ADCSRA & (1 << ADSC));

	  up = ADC;   

      // Start the conversion
      ADCSRA |= 1 << ADSC;

      // Insert a delay so that the ADC has time to latch at an offset time (basically the pads charge really fast or the ADC is super slow)
      adc_offset_delay();

      // Discharge pad
      clear(pulsing_pad_port,pulsing_pins[pad]);

      // We wait for the conversion
      while (ADCSRA & (1 << ADSC));

      // Send out the Low and High register data from the adc read.
	return up -  ADC;
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
   output(serial_direction, serial_pin_out);
   set(serial_port, serial_pin_out);

   /*output(pulsing_pad_direction, pulsing_pin1);
   clear(pulsing_pad_port, pulsing_pin1);
   output(pulsing_pad_direction, pulsing_pin2);
   clear(pulsing_pad_port, pulsing_pin2);
   output(pulsing_pad_direction, pulsing_pin3);
   clear(pulsing_pad_port, pulsing_pin3);
   output(pulsing_pad_direction, pulsing_pin4);
   clear(pulsing_pad_port, pulsing_pin4);
	*/
	
	output(pulsing_pad_direction, pulsing_pin1 | pulsing_pin2 | pulsing_pin3 | pulsing_pin4);
	clear(pulsing_pad_port, pulsing_pin1 | pulsing_pin2 | pulsing_pin3 | pulsing_pin4);
	
	// Give it a first change to settle after setting the pins as output.
	_delay_ms(10);
	
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
	   	uint8_t pad;
		uint8_t batch;
	   
	   	// zero all accumulators for averages
		value = diffs = finds = margins = 0;
	   
	   	// Repeat the calculations for batch_size times.
		for (batch = 0; batch < batch_size; batch++) {
			
			// get the readings for the four pads
		   for (pad = 0; pad < 4; pad++) {
			   values[pad] = getPadReading(pad);
		   }
			//convert a single set of readings
		   convert();
		}
	   
	   	// if there were any valid values, in the whole batch,
	   	// average them and send them out.
	   
	    // The angle is in tenths of degrees, from 0 to 3600.
	   
	   	// The format of the message is:
	    // :0x0360,0x0057,0x0002,0x0003\n
	    // It should be easily read with:
	   	// sscanf(":%i,%i,%i,%i\n", &angle, &finds,&diff, &margin);
	   
	    // If no valid readings were done in the batch
	    // a single asterisk in the line will be sent.
		if (finds) {
			putChar(':');
			// Divide by 2 because we added both readings in the complex case
			// and added twice the single value for the simple case.
			put_uint16(value / finds / 2);
			putChar(',');
			put_uint16(finds);
			putChar(',');
			put_uint16(diffs / finds / 10);
			putChar(',');
			put_uint16(margins / finds);
			putChar('\n');

		} else {
			putChar('*');
			putChar('\n');
		}
   	}
}
