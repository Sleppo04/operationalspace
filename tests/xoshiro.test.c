#include "../src/logic/rand/xoshiro256.h"

#include <stdio.h>

#define ROWS 1000
#define COLS 1000

int main()
{
    FILE* f = fopen("output.pgm", "w");
	fprintf(f, "P2\n#Comment\n%d %d\n255\n", ROWS, COLS);
    xoshiro256_state_t state = {.state = {1, 2, 3, 4}};
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			uint64_t result = xoshiro256_next(&state);
			fprintf(f, "%i ", (unsigned char) (result / (UINT64_MAX / 256)));
		}
		fprintf(f, "\n");
	}
	fclose(f);
}