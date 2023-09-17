#include "../src/logic/rand/opensimplex.c"
#include <stdio.h>
#include <float.h>

#define ROWS 1000
#define COLS 1000

int main()
{
	FILE* f = fopen("output.pgm", "w");
	fprintf(f, "P2\n#Comment\n%d %d\n255\n", ROWS, COLS);
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			float result = OpenSimplex_2DNoise_ImprovedX(998123, i, j);
			fprintf(f, "%i ", (int) (fabs(result) * 256));
		}
		fprintf(f, "\n");
	}
	fclose(f);
}
