#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdint.h>

#define FLAG_OPTATIVE 1<<1
#define FLAG_REQUIRED 1<<2
#define FLAG_POSITIONAL 1<<0

#define TYPE_BOOL 0
#define TYPE_INT 1
#define TYPE_STRING 2
#define TYPE_FLOAT 3

struct st_option {
	const char flagshort;
	const char *flaglong;
	const char *description;
	const char *placeholder;
	uint8_t flags;
	uint8_t type;
	void * data;
	const char *def;
};

/*
 * Print a well formatted usage using the options structure. Also provide
 * the name of the program to be shown in the listing.
 */
void options_usage(struct st_option *options, const char * program);

/*
 * Parse the command line using the options structure. After parsing,
 * set the value for the assignated value of each option. Return
 * error if the command line arguments are not compliant with the
 * structure options.
 */
int options_parse(struct st_option *options, int argc, char *argv[]);

/*
 * This variable holds the last error obtained when parsing the
 * structure of options with the command line. Print it after options_parse
 * returns != 0, and before calling it again.
 */
extern char option_err_msg[128];

#endif
