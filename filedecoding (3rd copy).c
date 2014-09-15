#include <stdio.h>
#include <stdlib.h>

char verbose = 0;
// The following convert the patterns like this:
// 0 ==> Z (zero), 3 ==> Q (quarter), 6 ==> H (half), n ==> N (number, none of the previous)
#define Z 3
#define Q 1
#define H 2 
#define N 0
#define P(a, b, c, d) ((a << 6) + (b << 4) + (c << 2) + d)

static unsigned char decoders[] = {
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

typedef struct {
	unsigned char patternS;
	unsigned char multiplier;
	unsigned char patternC;
	unsigned char pad1;
	char mult1;
	unsigned char pad2;
	char mult2;
} decodeStruct;

//float calibration[4] = {1.0381429282, 1.0679601315, 0.989620176, 0.9175252495};

unsigned int values[4];

unsigned int r1 = 0, r2 = 0, e = 0, f = 0, m = 0;

int abs (int v) {
	return (v < 0?-v:v);
};

int decodeSimple (unsigned char pattern, int total) {
	decodeStruct *p;
	int value;
	
	for (p = (decodeStruct*) decoders; p->patternS; p++) {
		//printf("patterns %x == %x ? \n",((simplePattern*)p)->pattern, pattern);
		if (p->patternS == pattern) {
			//printf("found with margin: %d, pattern %x,", margin, pattern);
			//printf("Simple multiplier: %d\n", ((simplePattern*)p)->multiplier);
			value = p->multiplier * 30;
			if (verbose) printf("Simple: %d\n", value);
			r1 += value;
			r2 += value;
			return 1;
		}
	}
	return 0;
}

int decodeComplex(unsigned char pattern, int total) {
	decodeStruct *p;
	int value1, value2;
	
	for (p = (decodeStruct*) decoders; p->patternS; p++) {
		if (p->patternC == pattern) {
			//printf("complex: [%d, %d], [%d, %d]\n",((complexPattern*)p)->pad1, ((complexPattern*)p)->mult1, ((complexPattern*)p)->pad2, ((complexPattern*)p)->mult2);
			value1 = (values[p->pad1] * 4  + p->mult1 * total)  * 30 / total;
			value2 = (p->mult2 * total - values[p->pad2] * 4)  * 30 / total;
			if (verbose) printf("Complex: %d, %d, %d\n", value1, value2, abs(value1 - value2));
			r1 += value1;
			r2 += value2;
			e += abs(value1 - value2);
			return 1;
		} 
	}
	return 0;
}

void convert () {
	int pad, 
		min = 99999,
		total = 0,
		half,
		quarter,
		margin,
		found = 0,
		numN = 0;
	unsigned char pattern = 0,
	extraPad, extra;
	
	
	for (pad = 0;pad < 4; pad++) {
		if (values[pad] < min) {
			min = values[pad];
		}
	}
	for (pad = 0; pad < 4; pad++) {
		values[pad] -= min;
		total += values[pad];
	}
	if (verbose) printf("scaled : %d, %d, %d, %d\n",  values[0], values[1], values[2], values[3]);
	half = total >> 1;
	quarter = half >> 1;
	if (verbose) printf("Min: %d, total: %d, half: %d, quarter: %d\n", min, total, half, quarter);
	for (margin = 0; !found && margin < (total >> 3); margin++) {
		for (extraPad = -1; extraPad < 4; extraPad++) {

			numN = pattern = 0;
			for (pad = 0; pad < 4; pad++) {
				extra = (pad == extraPad?1,0);
				pattern <<=2;
				if (abs(values[pad]) < margin + extra) pattern+=Z;
				else if (abs(values[pad] - quarter) < margin + extra) pattern += Q;
				else if (abs(values[pad] - half) < margin + extra) 	pattern += H;
				else numN++;
				//printf("pad %d, value %d, pattern %x\n", pad, values[pad], pattern);
			}

			//printf("margin %d, pattern %x, ", margin, pattern);
			switch (numN) {
				case 0: // simple pattern
					found += decodeSimple(pattern, total);
					break;
				case 1: // needs patching
	//				for (pad = 0; pad < 4; pad++) {
	//					found += decodeComplex(pattern & (~(3<< (pad * 2))), total);
	//				}
					break;
				case 2: // complex pattern
					found += decodeComplex(pattern, total);
					break;
				case 3: // retry
					break;
			}
		}
		if (found) {
			m += margin;
			f++;
		}
	}
	if (verbose && !found) printf("Not found\n");
};


int main (int argc,char *argv[]) {
	unsigned int pad;
	unsigned int hi, lo;
	unsigned int pads = 0;
	unsigned int count = 0;
	
	verbose = argc > 1;
	while (scanf("%u%u%u", &pad, &hi, &lo) == 3) {
		pads++;
		pad--;
//		pad = 3 - pad;
//		if (pad == 3) pad = 2;
//		if (pad == 2) pad = 3;
			
//		values[pad] = (int)((hi - lo) * calibration[pad]);
		values[pad] = hi - lo;
		if (pads == 4) {
			count++;
			if (verbose) printf("\n%d : %d, %d, %d, %d\n", count, values[0], values[1], values[2], values[3]);
			pads = 0;
			
			convert();
			//return 0;
		}
		
	}
	if (verbose) printf("samples: %d\n",count);
	if (f) {
		printf("result: over %d, found: %3.2f%%, val: %d (%d), diff: %d, margin: %d\n",
			   count, 
			   (double)f * 100 /count,  
			   (r1 + r2) /2 / f, 
			   (r1 + r2) /2 / f - 240, 
			   e / f, 
			   m / f
		);
	} else {
		puts("none found");
	}
	
	return 0;
};