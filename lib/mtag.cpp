#include <stdbool.h>
#include <string.h>

#include "mtag.h"

#include <fileref.h>
#include <tag.h>
#include <tfile.h>
#include <tstring.h>
#include <mpegfile.h>
#include <vorbisfile.h>
#include <flacfile.h>
#include <id3v2tag.h>
#include <id3v2frame.h>

/*
 * File
 */

MTag_File *
mtag_file_new (const char *filename)
{
	TagLib::File *file = NULL;

	/* Find the file type. */
	{
		char buffer[0x4];
		FILE *f;

		f = fopen (filename, "r");

		if (!f)
		{
			return NULL;
		}

		fread (buffer, 1, 0x4, f);
		if (strncmp (buffer, "ID3", 3) == 0)
		{
			file = reinterpret_cast<TagLib::File *>(new TagLib::MPEG::File (filename));
		}
		else if ((buffer[0] & 0xFF) && (buffer[1] & 0x7F))
		{
			file = reinterpret_cast<TagLib::File *>(new TagLib::MPEG::File (filename));
		}
		else if (strncmp (buffer, "OggS", 4) == 0)
		{
			file = reinterpret_cast<TagLib::File *>(new TagLib::Vorbis::File (filename));
		}
		fclose (f);
	}

	if (!file)
	{
		file = TagLib::FileRef::create (filename);
	}

	return reinterpret_cast<MTag_File *>(file);
}

void
mtag_file_free (MTag_File *file)
{
	delete reinterpret_cast<TagLib::File *>(file);
}

MTag_Tag *
mtag_file_tag (const MTag_File *file)
{
	const TagLib::File *f = reinterpret_cast<const TagLib::File *>(file);
	return reinterpret_cast<MTag_Tag *>(f->tag ());
}

MTag_Tag *
mtag_file_get_tag (MTag_File *file,
				   const char *id)
{
	TagLib::File *f = reinterpret_cast<TagLib::File *>(file);
	TagLib::Tag *t = NULL;

	{
		TagLib::MPEG::File *rf = NULL;

		if (rf = dynamic_cast<TagLib::MPEG::File *>(f))
		{
			if (strcmp (id, "id3v2") == 0)
			{
				t = (TagLib::Tag *) rf->ID3v2Tag ();
			}
			else if (strcmp (id, "id3v1") == 0)
			{
				t = (TagLib::Tag *) rf->ID3v1Tag ();
			}
			else if (strcmp (id, "ape") == 0)
			{
				t = (TagLib::Tag *) rf->APETag ();
			}
		}
	}

	{
		TagLib::Vorbis::File *rf = NULL;

		if (rf = dynamic_cast<TagLib::Vorbis::File *>(f))
		{
			if (strcmp (id, "xc") == 0)
			{
				t = (TagLib::Tag *) rf->tag ();
			}
		}
	}

	return reinterpret_cast<MTag_Tag *>(t);
}

const char *
mtag_file_get_type (MTag_File *file)
{
	TagLib::File *f = reinterpret_cast<TagLib::File *>(file);

	{
		TagLib::MPEG::File *rf = NULL;

		if (rf = dynamic_cast<TagLib::MPEG::File *>(f))
		{
			return "audio/mpeg";
		}
	}

	{
		TagLib::Vorbis::File *rf = NULL;

		if (rf = dynamic_cast<TagLib::Vorbis::File *>(f))
		{
			return "audio/x-vorbis";
		}
	}

	{
		TagLib::FLAC::File *rf = NULL;

		if (rf = dynamic_cast<TagLib::FLAC::File *>(f))
		{
			return "audio/x-flac";
		}
	}

	return "unknown";
}

bool
mtag_file_save (MTag_File *file)
{
	return reinterpret_cast<TagLib::File *>(file)->save();
}

/*
 * Tag
 */

inline char *
_convert (TagLib::String s)
{
	return strdup (s.toCString (true));
}

inline char *
_convert (unsigned int v)
{
	char *value = NULL;
	value = (char *) malloc (10);
	snprintf (value, 10, "%d", v);
	return value;
}

char *
mtag_tag_get (const MTag_Tag *tag,
			  const char *key)
{
	const TagLib::Tag *t = reinterpret_cast<const TagLib::Tag *>(tag);
	char *value = NULL;

	if (strcmp (key, "title") == 0)
	{
		value = _convert (t->title ());
	}
	else if (strcmp (key, "artist") == 0)
	{
		value = _convert (t->artist ());
	}
	else if (strcmp (key, "album") == 0)
	{
		value = _convert (t->album ());
	}
	else if (strcmp (key, "comment") == 0)
	{
		value = _convert (t->comment ());
	}
	else if (strcmp (key, "genre") == 0)
	{
		value = _convert (t->genre ());
	}
	else if (strcmp (key, "year") == 0)
	{
		value = _convert (t->year ());
	}
	else if (strcmp (key, "track") == 0)
	{
		value = _convert (t->track ());
	}

	return value;
}

void
mtag_tag_set (MTag_Tag *tag, 
			  const char *key,
			  const char *value)
{
	TagLib::Tag *t = reinterpret_cast<TagLib::Tag *>(tag);
	TagLib::String s = TagLib::String (value, TagLib::String::UTF8);

	if (strcmp (key, "title") == 0)
	{
		t->setTitle (s);
	}
	else if (strcmp (key, "artist") == 0)
	{
		t->setArtist (s);
	}
	else if (strcmp (key, "album") == 0)
	{
		t->setAlbum (s);
	}
	else if (strcmp (key, "comment") == 0)
	{
		t->setComment (s);
	}
	else if (strcmp (key, "genre") == 0)
	{
		t->setGenre (s);
	}
	else if (strcmp (key, "year") == 0)
	{
		t->setYear (s.toInt ());
	}
	else if (strcmp (key, "track") == 0)
	{
		t->setTrack (s.toInt ());
	}
}
