#include <stdio.h>

#include "mtag.h"

void
print_field (const MTag_Tag *tag, const char *field)
{
	printf ("%s: \"%s\"\n", field, mtag_tag_get (tag, field));
}

void
print_tag (const MTag_Tag *tag)
{
	if (tag)
	{
		print_field (tag, "title");
		print_field (tag, "artist");
		print_field (tag, "album");
		print_field (tag, "comment");
		print_field (tag, "genre");
		print_field (tag, "year");
		print_field (tag, "track");
	}
}

void
test_file (const char *fn)
{
	MTag_File *file;
	MTag_Tag *tag;

	file = mtag_file_new (fn);

	tag = mtag_file_tag (file);
	print_tag (tag);

	tag = mtag_file_get_tag (file, "id3v2");
	print_tag (tag);

	tag = mtag_file_get_tag (file, "id3v1");
	print_tag (tag);

	tag = mtag_file_get_tag (file, "ape");
	print_tag (tag);

	tag = mtag_file_get_tag (file, "xc");
	print_tag (tag);

#if 0
	{
		mtag_tag_set (tag, "comment", "foo");
		mtag_file_save (file);
	}
#endif

	mtag_file_free (file);
}

int
main (int argc,
	  char *argv[])
{
	if (argc > 1)
	{
		test_file (argv[1]);
	}

	return 0;
}
