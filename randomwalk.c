#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

static void normalize(unsigned int **field, size_t height, size_t width, size_t max) {
	size_t i, j;
	
	for(i = 0; i < height; ++i)
		for(j = 0; j < width; ++j)
			field[i][j] = (field[i][j] * 255) / max;
}

static void export_ppm(const char *filename, unsigned int **field, size_t height, size_t width) {
	FILE *file;
	size_t i, j;
	
	file = fopen(filename, "w");
	fprintf(file, "P6\n%zu %zu\n255\n", width, height);
	
	for(i = 0; i < height; ++i) {
		for(j = 0; j < width; ++j) {
			fputc(field[i][j], file);
			fputc(field[i][j], file);
			fputc(field[i][j], file);
		}
	}
	fclose(file);
}

#define INVALID_VALUE(var) do {\
	if((var) <= 0) { \
		fprintf(stderr, "Invalid value for " #var ": %zd\n", (var)); \
		return 1;\
	}\
} while(0)

int main(int argc, char **argv) {
	int c;
	size_t width = 1440, height = 900;
	unsigned int seed = 0;
	const char *output_name = "out.ppm";
	size_t steps;
	unsigned int **field;
	size_t x_pos, y_pos;
	size_t i;
	unsigned int maximum_value;
	
	while((c = getopt(argc, argv, "h:w:s:o:")) != -1) {
		switch(c) {
		case 'h':
			height = strtoul(optarg, NULL, 10);
			INVALID_VALUE(height);
			break;
		case 'w':
			width = strtoul(optarg, NULL, 10);
			INVALID_VALUE(width);
			break;
		case 's':
			seed = strtoul(optarg, NULL, 10);
			break;
		case 'o':
			output_name = optarg;
			break;
		}
	}
	
	if(seed == 0)
		srand(time(NULL));
	
	if(optind >= argc) {
		fputs("No number of steps given!\n", stderr);
		return 1;
	}
	
	steps = strtol(argv[optind], NULL, 10);
	INVALID_VALUE(steps);
	printf("%zu steps\n", steps);
	
	field = malloc(height * sizeof(*field));
	for(i = 0; i < height; ++i) {
		field[i] = malloc(width * sizeof(**field));
		memset(field[i], 0, width * sizeof(**field));
	}
	
	x_pos = width / 2;
	y_pos = height / 2;
	maximum_value = 0;
	
	for(i = 0; i < steps; ++i) {
		field[y_pos][x_pos]++;
		if(field[y_pos][x_pos] > maximum_value)
			maximum_value = field[y_pos][x_pos];
		
		switch(rand() % 4) {
		case 0: // right
			x_pos++;
			x_pos %= width;
			break;
		case 1: // left
			if(x_pos <= 0)
				x_pos = width;
			x_pos--;
			break;
		case 2: // up
			y_pos++;
			y_pos %= height;
			break;
		case 3: // down
			if(y_pos <= 0)
				y_pos = height;
			y_pos--; break;
		}
	}
	
	normalize(field, height, width, maximum_value);
	export_ppm(output_name, field, height, width);
	
	return 0;
}
