#ifndef MTAG
#define MTAG

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

    typedef struct { int dummy; } MTag_File;
    typedef struct { int dummy; } MTag_Tag;

    MTag_File *mtag_file_new (const char *filename);
    void mtag_file_free (MTag_File *file);
    MTag_Tag *mtag_file_tag (const MTag_File *file);
    MTag_Tag *mtag_file_get_tag (MTag_File *file, const char *id, bool create);
    const char *mtag_file_get_type (MTag_File *file);
    bool mtag_file_save (MTag_File *file);

    typedef void (*MTag_Tag_Func) (const char *key,
                                   const char *value,
                                   void *user_data);

    void mtag_tag_for_each (const MTag_Tag *tag,
                            MTag_Tag_Func func,
                            void *user_data);
    MTag_Tag *mtag_file_strip_tag (MTag_File *file,
                                   const char *id);

    char *mtag_tag_get (const MTag_Tag *tag, const char *key);
    void mtag_tag_set (MTag_Tag *tag, const char *key, const char *value);

#ifdef __cplusplus
}
#endif

#endif /* MTAG */
