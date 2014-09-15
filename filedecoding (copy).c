#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// The following convert the patterns like this:
// 0 ==> Z (zero), 3 ==> Q (quarter), 6 ==> H (half), n ==> N (number, none of the previous)
#define Z 3
#define Q 1
#define H 2 
#define N 0
#define P(a, b, c, d) ((a << 6) + (b << 4) + (c << 2) + d)

static unsigned char decoders[] = {
//	_6330: 0,
	P(H,Q,Q,Z), -1, 0,
	
//	_n3n0: [[2, -1],[0, 2]],
	P(N,Q,N,Z), 2, -1, 0, 2,
	
//	_3360: 1,
	P(Q,Q,H,Z), -1, 1,
	
//	_nn60: [[1, 0],[0, 2]],
	P(N,N,H,Z), 1, 0, 0, 2,
	
//	_0660: 2,
	P(Z,H,H,Z), -1, 2,
	
//	_06nn: [[3, 2], [2, 4]],
	P(Z,H,N,N), 3, 2, 2, 4,
	
//	_0633: 3,
	P(Z,H,Q,Q), -1, 3,
	
//	_0n3n: [[3, 2], [1, 5]],
	P(Z,N,Q,N), 3, 2, 1, 5,
	
//	_0336: 4,
	P(Z,Q,Q,H), -1, 4,
	
//	_0nn6: [[2, 3], [1, 5]],
	P(Z,N,N,H), 2,3,1,5,
	
//	_0066: 5,
	P(Z,Z,H,H), -1, 5,
	
//	_n06n: [[0, 5], [3, 7]],
	P(N,Z,H,N), 0,5,3,7,
	
//	_3063: 6,
	P(Q,Z,H,Q), -1, 6,
	
//	_n0n3: [[0, 5], [2, 8]],
	P(N,Z,N,Q), 0,5,2,8,
	
//	_6033: 7,
	P(H,Z,Q,Q), -1, 7,
	
//	_60nn: [[3, 6], [2, 8]],
	P(H,Z,N,N), 3,6,2,8,
	
//	_6006: 8,
	P(H,Z,Z,H), -1, 8,
	
//	_nn06: [[1, 8],[0,10]],
	P(N,N,Z,H), 1,8,0,10,
	
//	_3306: 9,
	P(Q,Q,Z,H), -1, 9,
	
//	_3n0n: [[1, 8],[3, 11]],
	P(Q,N,Z,N), 1,8,3,11,
	
//	_3603: 10,
	P(Q,H,Z,Q), -1, 10,
	
//	_n60n: [[0,9],[3, 11]],
	P(N,H,Z,N), 0,9,3,11,
	
//	_6600: 11,
	P(H,H,Z,Z), -1, 11,
	
//	_6nn0: [[2,11],[1,13]]
	P(H,N,N,Z), 2,11,1,13,
	
	0
};

float calibration[4] = {1.0381429282, 1.0679601315, 0.989620176, 0.9175252495};

unsigned int values[4];
unsigned int pad;
unsigned int hi, lo;
unsigned int accumHi[4] = {0,0,0,0}, accumLo[4] = {0,0,0,0}, accumHi2[4] = {0,0,0,0},accumLo2[4] ={0,0,0,0};
unsigned int pads = 0;
unsigned int count = 0;

unsigned r1 = 0, r2 = 0, e = 0, f = 0, m = 0;
int abs (int v) {
	return (v < 0?-v:v);
};

void convert () {
	int pad, 
		min = 99999,
		total = 0,
		half,
		quarter,
		margin,
		value1,
		value2,
		found = 0;
unsigned char *p,
		pattern = 0;
	
	
	for (pad = 0;pad < 4; pad++) {
		if (values[pad] < min) {
			min = values[pad];
		}
	}
	for (pad = 0; pad < 4; pad++) {
		values[pad] -= min;
		total += values[pad];
	}
	printf("red : %d, %d, %d, %d\n",  values[0], values[1], values[2], values[3]);
	half = total >> 1;
	quarter = half >> 1;
	printf("Min: %d, total: %d, half: %d, quarter: %d\n", min, total, half, quarter);
	for (margin = 0; !found && margin < (total / 10); margin++) {
		
		pattern = 0;
		for (pad = 0; pad < 4; pad++) {
			pattern <<=2;
			if (values[pad] == 0) pattern+=Z;
			else if (abs(values[pad] - quarter) < margin) pattern += Q;
			else if (abs(values[pad] - half) < margin) 	pattern += H;
			//printf("pad %d, value %d, pattern %x\n", pad, values[pad], pattern);
		}
		//printf("margin %d, pattern %x\n", margin, pattern);
		p = decoders;
		while (*p) {
			if (*p == pattern) {
				m += margin;
				found++;
				f++;
				printf("found with margin: %d, pattern %x,", margin, pattern);
				p++;
				if (*p < 0) {
					p++;
					value1 = *p * quarter;
					printf("%d\n", value1 * 120 / total);
					r1 += value1 * 120 / total;
					r2 += value1 * 120 / total;
				} else {
					//printf("p1: %d",*p);
					value1 = values[*p];
					p++;
					//printf("p2: %d",*p);
					value1 += (*p) * quarter;
					p++;
					//printf("p3: %d",*p);
					value2 = -values[*p];
					p++;
					//printf("p4: %d",*p);
					value2 += (*p) * quarter;
					printf("%d, %d, %d\n", value1 * 120 / total, value2 * 120 / total, abs(value1 - value2));
					r1 += value1 * 120 / total;
					r2 += value2 * 120 / total;
					e += abs(value1 - value2);
				} 
			} else {
				p++;
				p += (*p < 0?1:3);
			}
			p++;
		}
	}
	if (!found) printf("Not found\n");
};

double stdDev(int acc, int acc2, int n) {
	if (n == 0) return 0;
	return sqrt(count * acc2 - acc * acc) / n;
};
int main () {
	while (scanf("%u%u%u", &pad, &hi, &lo) == 3) {
		pads++;
		pad--;
//		pad = 3 - pad;
//		if (pad == 3) pad = 2;
//		if (pad == 2) pad = 3;
			
//		values[pad] = (int)((hi - lo) * calibration[pad]);
		values[pad] = hi - lo;
		accumHi[pad] += hi;
		accumLo[pad] += lo;
		accumHi2[pad] += hi * hi;
		accumLo2[pad] += lo * lo;
		if (pads == 4) {
			count++;
			printf("%d : %d, %d, %d, %d\n", count, values[0], values[1], values[2], values[3]);
			pads = 0;
			
			convert();
			//return 0;
		}
		
	}
	printf("samples: %d\n",count);
	for (pad = 0; pad < 4; pad++) {
		printf("Pad: %d,  Avg Hi: %d, stdDevHi %f, Avg Lo: %d, stdDevLo %f\n", 
			   pad, 
			   accumHi[pad] /count /4,
			   stdDev(accumHi[pad], accumHi2[pad], count*4), 
			   accumLo[pad] / count / 4,
			   stdDev(accumLo[pad], accumLo2[pad], count*4)
			  );
	}
	if (f) {
		printf("result: over %d, found: %d, v1: %d, v2: %d, error: %d, margin: %d\n",count, f, r1 / f, r2 / f, e / f, m / f);
	} else {
		puts("none found");
	}
	return 0;
};