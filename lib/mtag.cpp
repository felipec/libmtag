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
#include <apetag.h>

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
        unsigned char *bin_buffer;
        FILE *f;
        int r;

        f = fopen (filename, "r");
        bin_buffer = (unsigned char *) buffer;

        if (!f)
        {
            return NULL;
        }

        r = fread (buffer, 1, 0x4, f);
        if (r = 0x4)
        {
            if (strncmp (buffer, "ID3", 3) == 0)
            {
                file = reinterpret_cast<TagLib::File *>(new TagLib::MPEG::File (filename));
            }
            else if ((bin_buffer[0] == 0xFF) && ((bin_buffer[1] & 0xFB) == 0xFB))
            {
                file = reinterpret_cast<TagLib::File *>(new TagLib::MPEG::File (filename));
            }
            else if (strncmp (buffer, "OggS", 4) == 0)
            {
                file = reinterpret_cast<TagLib::File *>(new TagLib::Vorbis::File (filename));
            }
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
                   const char *id,
                   bool create)
{
    TagLib::File *f = reinterpret_cast<TagLib::File *>(file);
    TagLib::Tag *t = NULL;

    if (!t)
    {
        TagLib::MPEG::File *rf = NULL;

        if (rf = dynamic_cast<TagLib::MPEG::File *>(f))
        {
            if (strcmp (id, "id3v2") == 0)
            {
                t = (TagLib::Tag *) rf->ID3v2Tag (create);
            }
            else if (strcmp (id, "id3v1") == 0)
            {
                t = (TagLib::Tag *) rf->ID3v1Tag (create);
            }
            else if (strcmp (id, "ape") == 0)
            {
                t = (TagLib::Tag *) rf->APETag (create);
            }

            if (t && !create && t->isEmpty ())
                t = NULL;
        }
    }

    if (!t)
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

MTag_Tag *
mtag_file_strip_tag (MTag_File *file,
                     const char *id)
{
    TagLib::File *f = reinterpret_cast<TagLib::File *>(file);

    {
        TagLib::MPEG::File *rf = NULL;

        if (rf = dynamic_cast<TagLib::MPEG::File *>(f))
        {
            int to_strip = TagLib::MPEG::File::NoTags;

            if (strcmp (id, "id3v2") == 0)
            {
                to_strip |= TagLib::MPEG::File::ID3v2;
            }
            else if (strcmp (id, "id3v1") == 0)
            {
                to_strip |= TagLib::MPEG::File::ID3v1;
            }
            else if (strcmp (id, "ape") == 0)
            {
                to_strip |= TagLib::MPEG::File::APE;
            }

            rf->strip (to_strip);
        }
    }
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
    return strdup (s.toCString ());
}

inline char *
_convert (unsigned int v)
{
    char *value = NULL;
    value = (char *) malloc (10);
    snprintf (value, 10, "%d", v);
    return value;
}

/*
   4.2.1 TALB Album/Movie/Show title
   4.2.3 TBPM BPM (beats per minute)
   4.2.2 TCOM Composer
   4.2.3 TCON Content type
   4.2.4 TCOP Copyright message
   4.2.5 TDEN Encoding time
   4.2.5 TDLY Playlist delay
   4.2.5 TDOR Original release time
   4.2.5 TDRC Recording time
   4.2.5 TDRL Release time
   4.2.5 TDTG Tagging time
   4.2.2 TENC Encoded by
   4.2.2 TEXT Lyricist/Text writer
   4.2.3 TFLT File type
   4.2.2 TIPL Involved people list
   4.2.1 TIT1 Content group description
   4.2.1 TIT2 Title/songname/content description
   4.2.1 TIT3 Subtitle/Description refinement
   4.2.3 TKEY Initial key
   4.2.3 TLAN Language(s)
   4.2.3 TLEN Length
   4.2.2 TMCL Musician credits list
   4.2.3 TMED Media type
   4.2.3 TMOO Mood
   4.2.1 TOAL Original album/movie/show title
   4.2.5 TOFN Original filename
   4.2.2 TOLY Original lyricist(s)/text writer(s)
   4.2.2 TOPE Original artist(s)/performer(s)
   4.2.4 TOWN File owner/licensee
   4.2.2 TPE1 Lead performer(s)/Soloist(s)
   4.2.2 TPE2 Band/orchestra/accompaniment
   4.2.2 TPE3 Conductor/performer refinement
   4.2.2 TPE4 Interpreted, remixed, or otherwise modified by
   4.2.1 TPOS Part of a set
   4.2.4 TPRO Produced notice
   4.2.4 TPUB Publisher
   4.2.1 TRCK Track number/Position in set
   4.2.4 TRSN Internet radio station name
   4.2.4 TRSO Internet radio station owner
   4.2.5 TSOA Album sort order
   4.2.5 TSOP Performer sort order
   4.2.5 TSOT Title sort order
   4.2.1 TSRC ISRC (international standard recording code)
   4.2.5 TSSE Software/Hardware and settings used for encoding
   4.2.1 TSST Set subtitle
   4.2.2 TXXX User defined text information frame
*/

inline const char *
id3v2_tag_to_id (const char *tag_name)
{
    /* 4.2.1 TIT2 Title/songname/content description */
    if (strcmp (tag_name, "TIT2") == 0)
        return "title";

    /* 4.2.2 TPE1 Lead performer(s)/Soloist(s) */
    if (strcmp (tag_name, "TPE1") == 0)
        return "artist";

    /* 4.2.1 TALB Album/Movie/Show title */
    if (strcmp (tag_name, "TALB") == 0)
        return "album";

    /* 4.2.1 TRCK Track number/Position in set */
    if (strcmp (tag_name, "TRCK") == 0)
        return "track";

    /* 4.2.5 TDRC Recording time */
    if (strcmp (tag_name, "TDRC") == 0)
        return "year";

    /* 4.2.3 TCON Content type */
    if (strcmp (tag_name, "TCON") == 0)
        return "genre";

    /* 4.2.2 TCOM Composer */
    if (strcmp (tag_name, "TCOM") == 0)
        return "composer";

    /* 4.2.1 TIT1 Content group description */
    if (strcmp (tag_name, "TIT1") == 0)
        return "content_description";

    /* 4.2.1 TIT3 Subtitle/Description refinement */
    if (strcmp (tag_name, "TIT3") == 0)
        return "subtitle";

    /* 4.2.3 TMOO Mood */
    if (strcmp (tag_name, "TMOO") == 0)
        return "mood";

    /* 4.2.2 TPE2 Band/orchestra/accompaniment */
    if (strcmp (tag_name, "TPE2") == 0)
        return "band";

    /* 4.2.2 TPE3 Conductor/performer refinement */
    if (strcmp (tag_name, "TPE3") == 0)
        return "conductor";

    /* 4.2.2 TPE4 Interpreted, remixed, or otherwise modified by */
    if (strcmp (tag_name, "TPE4") == 0)
        return "interpreter";

    /* 4.2.2 TENC Encoded by */
    if (strcmp (tag_name, "TENC") == 0)
        return "encoded_by";

    return NULL;
}

inline const char *
ape_tag_to_id (const char *tag_name)
{
    if (strcmp (tag_name, "TITLE") == 0)
        return "title";

    if (strcmp (tag_name, "ARTIST") == 0)
        return "artist";

    if (strcmp (tag_name, "ALBUM") == 0)
        return "album";

    if (strcmp (tag_name, "TRACK") == 0)
        return "track";

    if (strcmp (tag_name, "GENRE") == 0)
        return "genre";

    if (strcmp (tag_name, "YEAR") == 0)
        return "year";

    return NULL;
}
void
mtag_tag_for_each (const MTag_Tag *tag,
                   MTag_Tag_Func func,
                   void *user_data)
{
    const TagLib::Tag *t = reinterpret_cast<const TagLib::Tag *>(tag);

    if (!func)
        return;

    {
        const TagLib::ID3v2::Tag *rt = NULL;

        if (rt = dynamic_cast<const TagLib::ID3v2::Tag *>(t))
        {
            TagLib::ID3v2::FrameList list;
            list = rt->frameList();

            TagLib::ID3v2::FrameList::ConstIterator it;
            for (it = list.begin (); it != list.end (); it++)
            {
                char *tag_name;
                tag_name = strndup ((*it)->frameID ().data(), (*it)->frameID ().size ());
                func (id3v2_tag_to_id (tag_name),
                      (*it)->toString ().toCString (),
                      user_data);
                free (tag_name);
            }

            return;
        }
    }

    {
        const TagLib::APE::Tag *rt = NULL;

        if (rt = dynamic_cast<const TagLib::APE::Tag *>(t))
        {
            TagLib::APE::ItemListMap map;
            map = rt->itemListMap();

            TagLib::APE::ItemListMap::ConstIterator it;
            for (it = map.begin (); it != map.end (); it++)
            {
                TagLib::APE::Item item;
                item = it->second;
                if (item.type() == TagLib::APE::Item::Text)
                {
                    TagLib::String tmp;
                    tmp = it->first;
                    func (ape_tag_to_id (tmp.toCString ()),
                          item.toString ().toCString (),
                          user_data);
                }
            }

            return;
        }
    }
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
