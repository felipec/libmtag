#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mtag.h"

static int set_flag = 0;
static int get_flag = 0;
static int get_all_flag = 0;

static char *key;
static char *value;
static char *type;
static char *filename;
static const char *error;

static void
print_field(const mtag_tag_t *tag,
	    const char *field)
{
	printf("%s: \"%s\"\n", field, mtag_tag_get(tag, field));
}

static void
print_tag(const mtag_tag_t *tag)
{
	print_field(tag, "title");
	print_field(tag, "artist");
	print_field(tag, "album");
	print_field(tag, "comment");
	print_field(tag, "genre");
	print_field(tag, "year");
	print_field(tag, "track");
}

static void
set_key(char *_key)
{
	if (key) {
		error = "Bad arguments";
		return;
	}
	key = strdup(_key);
}

static void
set_value(char *_value)
{
	if (value) {
		error = "Bad arguments";
		return;
	}
	value = strdup(_value);
}

static void
set_type(char *_type)
{
	if (type) {
		error = "Bad arguments";
		return;
	}
	type = strdup(_type);
}

static void
set_filename(char *_filename)
{
	if (filename) {
		error = "Bad arguments";
		return;
	}
	filename = strdup(_filename);
}

static void
set(char *_key,
    char *_value)
{
	set_key(_key);
	set_value(_value);
}

static void
do_get(mtag_file_t *file,
       mtag_tag_t *tag)
{
	if (key)
		print_field(tag, key);
	else
		print_tag(tag);
}

static void
do_set(mtag_file_t *file,
       mtag_tag_t *tag)
{
	if (key) {
		/** @todo: report success */
		mtag_tag_set(tag, key, value);
		mtag_file_save(file);
	}
	else
		error = "Bad key";
}

static void
show_tag(mtag_file_t *file,
	 const char *tag_id)
{
	mtag_tag_t *tag;

	tag = mtag_file_get_tag(file, tag_id, false);

	if (tag) {
		printf("== %s ==\n", tag_id);
		do_get(file, tag);
	}
}

int
main(int argc,
     char *argv[])
{
	int r = 0;
	int option_index = 0;
	static struct option long_options[] = {
		{ "set", no_argument, &set_flag, 1 },
		{ "get", no_argument, &get_flag, 1 },
		{ "artist", required_argument, 0, 'a' },
		{ "title", required_argument, 0, 't' },
		{ "key", required_argument, 0, 'k' },
		{ "value", required_argument, 0, 'v' },
		{ "type", required_argument, 0, 'p' },
		{ "file", required_argument, 0, 'f' },
		{ 0, 0, 0, 0 }
	};

	while (true) {
		char c;

		c = getopt_long(argc, argv, "sgla:t:k:v:p:f:", long_options, &option_index);

		if (c == -1)
			break;

		switch (c) {
			case 's': set_flag = 1; break;
			case 'g': get_flag = 1; break;
			case 'l': get_all_flag = 1; break;
			case 'a': set("artist", optarg); break;
			case 't': set("title", optarg); break;
			case 'k': set_key(optarg); break;
			case 'v': set_value(optarg); break;
			case 'p': set_type(optarg); break;
			case 'f': set_filename(optarg); break;
			case '?': error = "Bad arguments"; break;
		}

		if (error)
			break;
	}

	if (set_flag && get_flag)
		error = "Bad arguments";

	if (!filename)
		error = "Bad arguments: No filename specified";

	if (!error) {
		mtag_file_t *file;
		mtag_tag_t *tag;

		file = mtag_file_new(filename);

		if (file) {
			if (get_all_flag) {
				printf("= %s =\n", basename(filename));
				show_tag(file, "id3v1");
				show_tag(file, "id3v2");
				show_tag(file, "ape");
			}
			else {
				if (type)
					tag = mtag_file_get_tag(file, type, false);
				else
					tag = mtag_file_tag(file);

				if (tag) {
					if (get_flag)
						do_get(file, tag);

					if (set_flag)
						do_set(file, tag);
				}
				else
					error = "Bad tag";
			}

			mtag_file_free(file);
		}
		else
			error = "Bad file";
	}

	if (error) {
		printf ("Error: %s\n", error);
		r = -1;
	}

	free(key);
	free(value);
	free(type);
	free(filename);

	return r;
}
