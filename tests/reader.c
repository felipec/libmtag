#include <stdio.h>

#include "mtag.h"

static void
each_tag (const char *key,
          const char *value,
          void *user_data)
{
    printf ("%s: \"%s\"\n", key, value);
}

inline void
print_tag (const mtag_tag_t *tag)
{
    mtag_tag_for_each (tag, each_tag, NULL);
}

void
test_file (const char *fn)
{
    mtag_file_t *file;
    mtag_tag_t *tag;

    file = mtag_file_new (fn);

    tag = mtag_file_tag (file);
    print_tag (tag);

    tag = mtag_file_get_tag (file, "id3v2", false);
    print_tag (tag);

    tag = mtag_file_get_tag (file, "id3v1", false);
    print_tag (tag);

    tag = mtag_file_get_tag (file, "ape", false);
    print_tag (tag);

    tag = mtag_file_get_tag (file, "xc", false);
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
