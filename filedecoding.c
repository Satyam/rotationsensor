#include <stdio.h>
// #include <stdlib.h>

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned long int uint32_t;
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed long int int32_t;
#define UINT16_MAX 0xffff


int verbose;

typedef struct {
	uint8_t patternS;
	uint8_t multiplier;
	uint8_t patternC;
	uint8_t pad1;
	int8_t mult1;
	uint8_t pad2;
	int8_t mult2;
} decodeStruct;

// The following convert the patterns like this:
// 0 ==> Z (zero), 3 ==> Q (quarter), 6 ==> H (half), n ==> N (number, none of the previous)
#define Z 3
#define Q 1
#define H 2 
#define N 0
#define P(a, b, c, d) ((a << 6) + (b << 4) + (c << 2) + d)

static uint8_t decoders[] = {
//	_6330: 0,
	P(H,Q,Q,Z),  0,
	
//	_n3n0: [[2, -1],[0, 2]],
	P(N,Q,N,Z), 2, -1, 0, 2,
	
//	_3360: 1,
	P(Q,Q,H,Z),  1,
	
//	_nn60: [[1, 0],[0, 2]],
	P(N,N,H,Z), 1, 0, 0, 2,
	
//	_0660: 2,
	P(Z,H,H,Z),  2,
	
//	_06nn: [[3, 2], [2, 4]],
	P(Z,H,N,N), 3, 2, 2, 4,
	
//	_0633: 3,
	P(Z,H,Q,Q),  3,
	
//	_0n3n: [[3, 2], [1, 5]],
	P(Z,N,Q,N), 3, 2, 1, 5,
	
//	_0336: 4,
	P(Z,Q,Q,H),  4,
	
//	_0nn6: [[2, 3], [1, 5]],
	P(Z,N,N,H), 2,3,1,5,
	
//	_0066: 5,
	P(Z,Z,H,H),  5,
	
//	_n06n: [[0, 5], [3, 7]],
	P(N,Z,H,N), 0,5,3,7,
	
//	_3063: 6,
	P(Q,Z,H,Q),  6,
	
//	_n0n3: [[0, 5], [2, 8]],
	P(N,Z,N,Q), 0,5,2,8,
	
//	_6033: 7,
	P(H,Z,Q,Q),  7,
	
//	_60nn: [[3, 6], [2, 8]],
	P(H,Z,N,N), 3,6,2,8,
	
//	_6006: 8,
	P(H,Z,Z,H),  8,
	
//	_nn06: [[1, 8],[0,10]],
	P(N,N,Z,H), 1,8,0,10,
	
//	_3306: 9,
	P(Q,Q,Z,H),  9,
	
//	_3n0n: [[1, 8],[3, 11]],
	P(Q,N,Z,N), 1,8,3,11,
	
//	_3603: 10,
	P(Q,H,Z,Q),  10,
	
//	_n60n: [[0,9],[3, 11]],
	P(N,H,Z,N), 0,9,3,11,
	
//	_6600: 11,
	P(H,H,Z,Z),  11,
	
//	_6nn0: [[2,11],[1,13]]
	P(H,N,N,Z), 2,11,1,13,
	
	0
};



//float calibration[4] = {1.0381429282, 1.0679601315, 0.989620176, 0.9175252495};

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
// This one comes standard in regular C
//uint16_t abs (int16_t v) {
//
//	return (v < 0?-v:v);
//};

int8_t decodeSimple (uint8_t pattern, uint16_t total) {
	decodeStruct *p;
	if (verbose > 1) printf("Trying simple with pattern %x\n", pattern);
	for (p = (decodeStruct*) decoders; p->patternS; p++) {
		if (verbose > 1) printf("patterns %x == %x ? \n",p->patternS, pattern);
		if (p->patternS == pattern) {
			if (verbose > 1) printf("Simple multiplier: %d\n", p->multiplier);
			value += p->multiplier * 30 * 2 * 10;

			if (verbose > 0) printf("Simple: %d\n", p->multiplier * 30 * 2 * 10);
			return 1;
		}
	}
	return 0;
}

int8_t decodeComplex(uint8_t pattern, uint16_t total) {
	decodeStruct *p;
	int32_t value1, value2;
	
	if (verbose > 1) printf("Trying complex with pattern %x\n", pattern);
	for (p = (decodeStruct*) decoders; p->patternS; p++) {
		if (p->patternC == pattern) {
			if (verbose > 1) printf("complex: [%d, %d], [%d, %d]\n",p->pad1, p->mult1, p->pad2, p->mult2);
			
			// TODO check whether the type-casting can be dropped, I think it is not required.
			value1 = (int32_t)(4 * values[p->pad1] + p->mult1 * total)  * 30 * 10 / total;
			value2 = (int32_t)(p->mult2 * total - 4 * values[p->pad2])  * 30 * 10 / total;
			

			if (verbose > 0) printf("Complex: %ld, %ld, %d\n", value1, value2, abs(value1 - value2));
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

void convert () {
	uint16_t min,
		total,
		half,
		quarter,
		margin,
		extra;
	
	uint8_t pad,
		found,
		numN,
		pattern;

	int8_t extraPad;
	
	
	for (min = UINT16_MAX, pad = 0; pad < 4; pad++) {
 		if (values[pad] < min) {
			min = values[pad];
		}
	}
		
	for (total = pad = 0; pad < 4; pad++) {
		values[pad] -= min;
		total += values[pad];
	}
	
	if (verbose > 0) printf("scaled : %d, %d, %d, %d\n",  values[0], values[1], values[2], values[3]);
	half = total >> 1;
	quarter = half >> 1;
	if (verbose > 0) printf("Min: %d, total: %d, half: %d, quarter: %d\n", min, total, half, quarter);
	
	for ( margin = found = 0; !found && margin < (total >> 3); margin++) {
		for (extraPad = -1; extraPad < 4 && !found; extraPad++) {
			for (numN = pattern = pad = 0; pad < 4; pad++) {
				extra = (pad == extraPad?margin + 1:margin);
				//extra = margin;
				pattern <<=2;
				if (abs(values[pad]) < extra) pattern+=Z;
				else if (abs(values[pad] - quarter) < extra) pattern += Q;
				else if (abs(values[pad] - half) < extra) 	pattern += H;
				else numN++;
				if (verbose > 1) printf("pad %d, value %d, pattern %x\n", pad, values[pad], pattern);
			}

			if (verbose > 1) printf("margin %d, pattern %x\n", margin, pattern);
			switch (numN) {
				case 0: // simple pattern
					found += decodeSimple(pattern, total);
					break;
				case 1: // needs patching
//					for (pad = 0; pad < 4 & !found; pad++) {
//						found += decodeComplex(pattern & (~(3<< (pad * 2))), total);
//					}
					break;
				case 2: // complex pattern
					found += decodeComplex(pattern, total);
					break;
				case 3: // retry
					break;
			}
			if (found) {
				margins += margin;
				finds++;
				break;
			}
		}
	}
	if (verbose && !found) printf("Not found\n");
};


int16_t main (int16_t argc,char *argv[]) {
	unsigned int pad;
	unsigned int hi, lo;
	unsigned int pads = 0;
	unsigned int count = 0;
	if (argc > 1) {
		sscanf(argv[1],"%d", &verbose);
	} else {
		verbose = 0;
	}

	while (scanf("%u%u%u", &pad, &hi, &lo) == 3) {
		pads++;
		pad--;
//		pad = 3 - pad;
//		if (pad == 3) pad = 2;
//		if (pad == 2) pad = 3;
			
//		values[pad] = (int16_t)((hi - lo) * calibration[pad]);
		values[pad] = hi - lo;
		if (pads == 4) {
			count++;
			if (verbose > 0) printf("\n%d : %d, %d, %d, %d\n", count, values[0], values[1], values[2], values[3]);
			pads = 0;
			
			convert();
			//return 0;
		}
		
	}
	if (verbose > 0) printf("samples: %d\n",count);
	if (finds) {
		printf("result: over %d, found: %3.2f%%, val: %d (%d), diff: %d, margin: %d\n",
			   count, 
			   (double)finds * 100 /count,  
			   (int)(value /20 / finds), 
			   (int)(value /20 / finds - 240), 
			   (int)(diffs / finds), 
			   (int)(margins / finds)
		);
	} else {
		puts("none found");
	}
	
	return 0;
};