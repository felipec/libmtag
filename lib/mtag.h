/*
 * Copyright (C) 2007-2009 Felipe Contreras
 *
 * This code is licenced under the LGPLv2.1.
 */

#ifndef MTAG_H
#define MTAG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

	typedef struct { int dummy; } mtag_file_t;
	typedef struct { int dummy; } mtag_tag_t;

	mtag_file_t *mtag_file_new(const char *filename);
	void mtag_file_free(mtag_file_t *file);
	mtag_tag_t *mtag_file_tag(const mtag_file_t *file);
	mtag_tag_t *mtag_file_get_tag(mtag_file_t *file,
				      const char *id,
				      bool create);
	const char *mtag_file_get_type(mtag_file_t *file);
	bool mtag_file_save(mtag_file_t *file);

	typedef void (*mtag_tag_func_t)(const char *key,
					const char *value,
					void *user_data);

	void mtag_tag_for_each(const mtag_tag_t *tag,
			       mtag_tag_func_t func,
			       void *user_data);
	void mtag_file_strip_tag(mtag_file_t *file,
				 const char *id);

	char *mtag_tag_get(const mtag_tag_t *tag,
			   const char *key);
	void mtag_tag_set(mtag_tag_t *tag,
			  const char *key,
			  const char *value);

#ifdef __cplusplus
}
#endif

#endif /* MTAG_H */
