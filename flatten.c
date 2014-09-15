#include <stdio.h>
#include <stdlib.h>

int pad, hi, lo, values[4],his[4], los[4], pads= 0, count = 0;
int main () {
	while (scanf("%u%u%u", &pad, &hi, &lo) == 3) {
		count++;
		pads++;
		pad--;
		his[pad] = hi;
		los[pad] = lo;
		values[pad] = hi - lo;
		if (pads == 4) {
			printf("%d", count);
			for (pad = 0; pad < 4; pad++) {
				printf(",%d, %d, %d", his[pad], los[pad], values[pad]);
			}
			printf("\n");
			
			pads = 0;
		}
		
	}
	return 0;
};