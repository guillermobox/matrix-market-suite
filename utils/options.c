#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "options.h"

#define IT_RECEIVES_INPUT(opt) (opt->type & (TYPE_INT | TYPE_STRING | TYPE_FLOAT))
#define REMOVE(arraylist, i, len) \
do {\
	int _i;\
	for (_i = i + 1; _i < len + 1; _i++) {\
		arraylist[_i - 1] = arraylist[_i];\
	}\
	len--;\
} while(0);

char option_err_msg[128];

char * format_option(struct st_option *option)
{
	static char buffer[128];
	char flagname[32];

	if (option->flaglong) {
		snprintf(flagname, 32, "  -%c, --%s", option->flagshort, option->flaglong);
	} else {
		snprintf(flagname, 32, "  -%c", option->flagshort);
	}
	if (option->placeholder) {
		if (IT_RECEIVES_INPUT(option)) {
			snprintf(flagname + strlen(flagname), 32 - strlen(flagname), " <%s>", option->placeholder);
		}
	} else {
		if (option->type == TYPE_INT)
			strcat(flagname, " <integer>");
		else if (option->type == TYPE_FLOAT)
			strcat(flagname, " <float>");
		else if (option->type == TYPE_STRING)
			strcat(flagname, " <string>");
	}
	snprintf(buffer, 128, "%-32s %s", flagname, option->description);
	return buffer;
};

void options_usage(struct st_option *options, const char * program)
{
	struct st_option * option = options;

	printf("Usage: %s [OPTIONS] ", program);
	for (option = options; option->description != NULL; option++) {
		if (option->flags == (FLAG_POSITIONAL | FLAG_REQUIRED))
			printf("%s ", option->flaglong);
		if (option->flags == (FLAG_POSITIONAL | FLAG_OPTATIVE))
			printf("[%s] ", option->flaglong);
	}
	printf("\n\nWhere the available options are:\n\n");
	for (option = options; option->description != NULL; option++) {
		if (!(option->flags & FLAG_POSITIONAL))
			puts(format_option(option));
	}
	printf("\n");
};

static int parse_value(struct st_option *option, const char *value, int found)
{
	if (!found && option->flags & FLAG_REQUIRED && !option->def) {
		sprintf(option_err_msg, "Flag %c is required", option->flagshort);
		return 1;
	}
	if (!found && option->def) {
		value = option->def;
		found = 1;
	}
	if (option->type == TYPE_BOOL) {
		if (found)
			*((int*)option->data) = 1;
		else
			*((int*)option->data) = 0;
	} else if (option->type == TYPE_INT) {
		if (found) {
			if (value) {
				long int number;
				char * ptr;
				ptr = (char *) value;
				number = strtol(value, &ptr, 10);
				if (ptr == value || *ptr != '\0') {
					sprintf(option_err_msg, "Variable for flag %c has to be an integer", option->flagshort);
					return 1;
				}
				*((int*)option->data) = number;
			} else {
				sprintf(option_err_msg, "Flag %c needs an argument", option->flagshort);
				return 1;
			}
		} else
			*((int*)option->data) = 0;
	} else if (option->type == TYPE_FLOAT) {
		if (found) {
			if (value) {
				double number;
				char * ptr;
				ptr = (char *) value;
				number = strtod(value, &ptr);
				if (ptr == value || *ptr != '\0') {
					sprintf(option_err_msg, "Variable for flag %c has to be a floating point number", option->flagshort);
					return 1;
				}
				*((double*)option->data) = number;
			} else {
				sprintf(option_err_msg, "Flag %c needs an argument", option->flagshort);
				return 1;
			}
		} else
			*((double*)option->data) = 0;
	} else if (option->type == TYPE_STRING) {
		if (found) {
			if (value) {
				*((char**)option->data) = malloc(strlen(value) + 1);
				strcpy( *((char**)option->data), value);
			} else {
				sprintf(option_err_msg, "Flag %c needs an argument", option->flagshort);
				return 1;
			}
		} else
			*((char**)option->data) = NULL;
	}
	return 0;
};

int options_parse(struct st_option *options, int argc, char *argv[])
{
	struct st_option * option = options;
	const char * value = NULL;
	int found, argi, err;

	for (option = options; option->description != NULL; option++) {
		if (option->flags & FLAG_POSITIONAL) continue;
		char buf[3], longbuf[32];
		snprintf(buf, 3, "-%c", option->flagshort);
		snprintf(longbuf, 32, "--%s", option->flaglong);
		found = 0;
		value = NULL;

		for (argi = 1; argi < argc; argi++) {
			if (strncmp(argv[argi], buf, 3) == 0 || strncmp(argv[argi], longbuf, 32) == 0) {
				REMOVE(argv, argi, argc);
				found = 1;
				if (IT_RECEIVES_INPUT(option)) {
					value = argv[argi];
					REMOVE(argv, argi, argc);
				}
				break;
			}
		}

		if (option->data) {
			err = parse_value(option, value, found);
			if (err) return 1;
		};

	}

	for (argi = 1; argi < argc; argi++) {
		if (argv[argi][0] == '-') {
			sprintf(option_err_msg, "Unknown or repeated option: %s", argv[argi]);
			return 1;
		}
	};

	argi = 1;
	for (option = options; option->description != NULL; option++) {
		if (!(option->flags & FLAG_POSITIONAL)) continue;
		value = argv[argi];
		if (value == NULL) {
			sprintf(option_err_msg, "Positional argument %s required, but not found", option->flaglong);
			return 1;
		}
		err = parse_value(option, value, value != NULL);
		if (err) return 1;
		argi++;
	};

	return 0;
};
