#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

enum method {
	OVERFLOW,
	SATURATION
};

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

#define increment_overflow(x, m) do {\
	++(x);\
	(x) %= (m);\
} while(0)

#define decrement_overflow(x, m) do {\
	if((x) <= 0)\
		(x) = (m);\
	--(x);\
} while(0)

#define increment_sat(x, m) do {\
	if((x) < (m))\
		++(x);\
} while(0)

#define decrement_sat(x, m) do {\
	if((x) > (m))\
		--(x);\
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
	enum method calculation_method = OVERFLOW;
	
	while((c = getopt(argc, argv, "h:w:s:o:a")) != -1) {
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
		case 'a':
			calculation_method = SATURATION;
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
	
	printf("width: %zu\nheight: %zu\nsteps: %zu\noutput: %s\n", width, height, steps, output_name);
	
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
			switch(calculation_method) {
			case OVERFLOW: increment_overflow(x_pos, width); break;
			case SATURATION: increment_sat(x_pos, width - 1); break;
			}
			break;
		case 1: // left
			switch(calculation_method) {
			case OVERFLOW: decrement_overflow(x_pos, width); break;
			case SATURATION: decrement_sat(x_pos, 0); break;
			}
			break;
		case 2: // up
			switch(calculation_method) {
			case OVERFLOW: increment_overflow(y_pos, height); break;
			case SATURATION: increment_sat(y_pos, height - 1); break;
			}
			break;
		case 3: // down
			switch(calculation_method) {
			case OVERFLOW: decrement_overflow(y_pos, height); break;
			case SATURATION: decrement_sat(y_pos, 0); break;
			}
			break;
		}
	}
	
	normalize(field, height, width, maximum_value);
	export_ppm(output_name, field, height, width);
	
	return 0;
}
