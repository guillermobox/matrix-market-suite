/**
  * Copyright 2016 José Manuel Abuín Mosquera <josemanuel.abuin@usc.es>
  * 
  * This file is part of Matrix Market Suite.
  *
  * Matrix Market Suite is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * Matrix Market Suite is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with Matrix Market Suite. If not, see <http://www.gnu.org/licenses/>.
  */

#include <stdio.h>
#include <stdlib.h>
#include "CreateDenseMatrixGeneral.h"
#include "../utils/options.h"

int CreateDenseMatrixGeneral(int argc, char *argv[]) {

	FILE *output;
	unsigned long int numRows, numCols, seed, i, j;
	char * outputpath;
	double value;
	MM_typecode outputmatcode;

	int verbose;
	struct st_option options[] = {
		FLAG_BOOL('v', "verbose", "Show more info when executing", &verbose, NULL),
		PARAM_INT("rows", "Number of rows of the matrix", &numRows, NULL),
		PARAM_INT("cols", "Number of columns of the matrix", &numCols, NULL),
		PARAM_STRING("outputfilename", "Filename to write the matrix into", &outputpath, NULL),
		PARAM_INT("seed", "Seed of the random number generator", &seed, "0"),
		OPTIONS_END,
	};

	if (options_parse(options, argc, argv)) {
		options_usage(options, "./MM-Suite CreateDenseMatrixGeneral");
		puts(option_err_msg);
		return 0;
	};

	if ((output = fopen(outputpath, "w")) == NULL){
		return 0;
	}


	mm_initialize_typecode(&outputmatcode);
	mm_set_matrix(&outputmatcode);
	mm_set_coordinate(&outputmatcode);
	mm_set_real(&outputmatcode);
	mm_set_general(&outputmatcode);

	srand(seed);

	mm_write_banner(output, outputmatcode);
	mm_write_mtx_crd_size(output, numRows, numCols, numRows*numCols);
	
	for (i = 0; i < numRows; i++) {
		for (j = 0; j < numCols; j++) {
			value = ((double) rand() / (double) RAND_MAX) / 100;
			fprintf(output, "%lu %lu %lg\n", i + 1, j + 1, value);
		}
	}
	
	fclose(output);

	return 1;
};
