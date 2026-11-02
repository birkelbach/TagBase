/*  TagBase - An open source Tag Data Storage Library
 *  Copyright (c) 2026 Phil Birkelbach
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <common.h>
#include <ctype.h>
#include <tagbase.h>


/* This function finds the next '.' in "name" and replaces it with '\0'
 * If there is no '.' it returns NULL */
static char *
_split_tagname(char *name)
{
    char *nextname = NULL;
    int n = 0;

    while(name[n] != '.' && name[n] != '\0') {
        n++;
    }

    if(name[n] == '.') { /* If we have more members in the string */
        name[n] = '\0';
        nextname = name + n + 1; /* This points to the next character after the '.' */
    }
    return nextname;
}

/* Looks for a number within [ ] and returns that as an index
 * if it finds problems within the [ ] it returns ERR_ARG if
 * there are no [ ] then it returns ERR_NOTFOUND which is not
 * really an error in this case but we need to know anyway.
 * It if it finds a '[' it replaces it with a '\0' so that
 * the calling function will just see the string */
static inline int
_get_index(char *str)
{
    int i, k, len, index;
    len = strlen(str);
    for(i = 0; i < len && str[i] != '['; i++);
    if(i < len) {
        str[i] = '\0'; /* To end the string */
        for(k = i+1; k < len && str[k] != ']'; k++) {
            if(str[k] < '0' || str[k] > '9') return ERR_ARG;
        }
        index = atoi(str + i + 1);
    } else {
        index = ERR_NOTFOUND;
    }
    return index;
}

/* called when a bit index is found in the tag string.  i.e. tagname.3.  The
 * function determines if all is okay and if so it finishes the handle definition
 * and returns 0 otherwise it returns an error */
static int
_parse_bit_index(tag_db_t *db, tag_type type, tag_handle_t *h, char *digit, int index, int count)
{
    char *endptr;
    int size;

    if(type == TYPE_BOOL || type == TYPE_TIME ||
       type == TYPE_REAL || type == TYPE_LREAL ||
       IS_CUSTOM(type)) {
           return ERR_ARG;
       }
    if(index < 0) index = 0;
    h->bit = strtol(digit, &endptr, 10);
    /* This indicates that there is extra text past the number */
    if(endptr[0] != '\0') {
        return ERR_ARG;
    }
    size = type_size(db, type);
    if(h->bit > size*8) {
        return ERR_2BIG;
    }
    if(count == 0) count = 1;
    if((h->bit + count) > size*8) {
        return ERR_2BIG;
    }
    h->byte += size * index + (h->bit / 8);
    h->bit = h->bit % 8;
    h->type = TYPE_BOOL;
    h->size = (h->bit + count - 1) / 8 - (h->bit / 8) + 1;
    h->count = count;
    return 0;
}

static int
_parse_next_member(tag_db_t *db, tag_type lasttype, tag_handle_t *h, char *str, int count)
{
    int index, result, size;
    char *name = str;
    char *nextname;
    cdt_member *this;

    if(name == NULL) return ERR_ARG;

    nextname = _split_tagname(str);

    index = _get_index(name);
    if(index < 0 && index != ERR_NOTFOUND) {
        return index;
    }

    if(type_get_name(db, lasttype) == NULL) {
        return ERR_NOTFOUND; /* This is a serious problem here */
    }

    this = db->datatypes[CDT_TO_INDEX(lasttype)].members;

    while(this != NULL) {
        /* Start by adding all the bytes of the members before the
         * one we are looking for */
        if(strcmp(name, this->name)) {
            if(this->type == TYPE_BOOL) {
                h->bit += this->count;
                if(h->bit > 7) {
                    h->byte += h->bit / 8;
                    h->bit %= 8;
                }
            } else {
                /* Gotta step up one if the last member was a bool */
                if(h->type == TYPE_BOOL && h->bit > 0) { h->byte++; }
                h->byte += type_size(db, this->type) * this->count;
                h->bit = 0;
            }
            h->type = this->type; /* We need to keep track of the last type */

            this = this->next;
        } else { /* Here we have found it */
            break;
        }

    }
    if(this == NULL) return ERR_NOTFOUND;

    if(nextname) { /* Not the last item */
        if(isdigit(nextname[0])) {
            if(this->count > 1 && index <0) {
                return ERR_ARG;
            }
            return _parse_bit_index(db, this->type, h, nextname, index, count);
        }
        result = _parse_next_member(db, this->type, h, nextname, count);
        if(result) return result;
        if(index != ERR_NOTFOUND) {
            if(count == 0) count = 1;
            if((index + count) > this->count) return ERR_2BIG;
            h->byte += type_size(db, this->type) * index;
        }
    } else { /* We are the last item */
        if(h->type == TYPE_BOOL && this->type != TYPE_BOOL && h->bit > 0) {
            h->byte++;
            h->bit = 0;
        }

        h->type = this->type;
        if(index != ERR_NOTFOUND){
            if(count == 0 ) count = 1;
            if((index + count) > this->count ) return ERR_2BIG;
            if(this->type == TYPE_BOOL) {
                h->byte += index / 8;
                h->bit += index % 8;
                /* Two bits across the byte boundry require two bytes */
                h->size = (h->bit + count - 1) / 8 - (h->bit / 8) + 1;
                h->count = count;
            } else {
                size = type_size(db, this->type);
                h->size = size * count;
                h->byte += size * index;
                h->bit = 0;
                h->count = count;
            }
        } else { /* This is where no index was given */
            if(count > this->count) return ERR_2BIG;
            if(count == 0) count = this->count;
            if(this->type == TYPE_BOOL) {
                h->size = (h->bit + count - 1) / 8 - (h->bit / 8) + 1;
                h->count = count;
            } else {
                size = type_size(db, this->type);
                h->bit = 0;
                h->size = size * count;
                h->count = count;
            }
        }
    }
    return 0;
}

/* Actually finds the handle.  We don't allocate the handle or copy the
   tag name into the handle in this function so that it can be used
   internally without that overhead. */
static int
_tag_handle(tag_db_t *db, tag_handle_t *h, char *str, int strlen, int count) {
    int result, index, size;
    tag_t *tag;
    char tagname[strlen];
    char *membername;

    strcpy(tagname, str);
    membername = _split_tagname(tagname);
    index = _get_index(tagname); /* Get array index inside [] */
    if(index < 0 && index != ERR_NOTFOUND) {
        return index;
    }

    tag = tag_get_by_name(db, tagname);
    if(tag == NULL) return ERR_NOTFOUND;
    h->ptr = tag;

    if(membername) {
        /* This is the case where the .x is a number referring to a bit */
        if(isdigit(membername[0])) {
            if(tag->count > 1 && index < 0) {
                return ERR_ARG;
            }
            return _parse_bit_index(db, tag->type, h, membername, index, count);
        }
        else if(!IS_CUSTOM(tag->type)) {
            //logError(LOG_TAGBASE, "Tag %s, has no member %s", tagname, membername);
            return ERR_ARG;
        }
        if(tag->count > 1 && index == ERR_NOTFOUND) {
            //logError(LOG_TAGBASE, "Ambiguous reference in tag %s", tagname);
            return ERR_FAIL;
        }
        result = _parse_next_member(db, tag->type, h, membername, count);
        if(result) return result;
        if(index != ERR_NOTFOUND) {
            h->byte += type_size(db, tag->type) * index;
        }
    } else {
        h->type = tag->type;
        if(index != ERR_NOTFOUND){
            if(count == 0 ) count = 1;
            if((index + count) > tag->count ) return ERR_2BIG;
            if(tag->type == TYPE_BOOL) {
                h->byte = index / 8;
                h->bit = index % 8;
                //h->size = (h->bit + count - 1) / 8 - (h->bit / 8) + 1;
                h->size = (count - 1) / 8 + 1;
                h->count = count;
            } else {
                size = type_size(db, tag->type);
                h->size = size * count;
                h->byte = size * index;
                h->bit = 0;
                h->count = count;
            }
        } else { /* This is where no index [] was given */
            if(count > tag->count) return ERR_2BIG;
            if(count == 0) count = tag->count;
            if(tag->type == TYPE_BOOL) {
                h->byte = 0;
                h->bit = 0;
                h->size = (count - 1) / 8 + 1;
                h->count = count;
            } else {
                size = type_size(db, tag->type);
                h->size = size * count;
                h->byte = 0;
                h->bit = 0;
                h->count = count;
            }
        }
    }
    return 0;
}


/*!
 * This function retrieves a handle to the tag or tag fragment that is
 * represented by 'str'.  The handle is a complete representation of where
 * the data is located in the server.  It is passed to the reading/writing
 * functions to retrieve the data. If count is 0 then a handle to the whole tag
 * or tag member is returned.  This function returns a pinter to a handle that
 * is allocted and needs to be freed using the tag_freehandle() function.
 *
 * @param db Pointer to the tag database
 * @param str The string that represents the tag or tag fragment that we wish
 *            to get a handle to.
 * @param count The number of items of the tag that we wish to identify.  If set
 *              to zero the entire tag will be assumed.
 * @param err Pointer to an error
 * @returns New tag handle on success or NULL on error and err is set if not NULL
 */
tag_handle_t *
tag_gethandle(tag_db_t *db, char *str, int count, int *err) {
    tag_handle_t *h;
    int result;
    if(db == NULL || str == NULL) {
        if(err != NULL) *err = ERR_ARG;
        return NULL;
    }
    h = calloc(1, sizeof(tag_handle_t));

    result = _tag_handle(db, h, str, strlen(str) + 1, count);
    if(result) {
        if(err != NULL) *err = result;
        free(h);
        return NULL;
    }
    h->str = strdup(str);
    return h;
}

void
tag_freehandle(tag_handle_t *h) {
    if(h != NULL) {
        if(h->str != NULL) {
            free(h->str);
        }
        free(h);
    }
}


/*!
 * Higher level tag reading function.  This function uses
 * a tag handle to read the data.  The passed data buffer
 * must be as large as the .size attribute in the handle
 *
 * @param db Pointer to the tag database
 * @param h Handle to the tag or tag fragment to be read
 * @param data Buffer to hold the returned data
 * @returns Zero on success or error code otherwise
 */
int
tag_read_handle(tag_db_t *db, tag_handle_t *h, void *data) {
    uint8_t temp[h->size+1];
    int result;
    int i;

    /* This takes care of the case of the bit being offset */
    if(h->type == TYPE_BOOL && h->bit > 0) {
        if(h->bit > 7) return ERR_ARG;
        h->size = ((h->count + h->bit)-1)/8 + 1;
        result = tag_read_raw(db, h->ptr, h->byte, temp, h->size);
        if(result) return result;
        bzero(data, h->size);
        i = h->bit;
        for(int n = 0; n < h->count; n++) {
            if( (0x01 << (i % 8)) & ((uint8_t *)temp)[(i / 8)] ) {
                ((uint8_t *)data)[n / 8] |= (1 << (n % 8));
            }
            i++;
        }
    } else {
        result = tag_read_raw(db, h->ptr, h->byte, data, h->size);
    }
    return result;
}


/*!
 * Higher level tag writing function.  This function uses
 * a tag handle to write the data.  The data that is passed
 * must contain enough data to satisfy the write
 *
 * @param db Pointer to the tag database
 * @param h Handle to the tag or tag fragment to be written
 * @param data Pointer to the data buffer
 * @returns Zero on success or error code otherwise
 */
int
tag_write_handle(tag_db_t *db, tag_handle_t *h, void *data) {
    int result;
    size_t size;
    uint8_t *temp;
    uint8_t *mask;
    int i;

    if(tag_is_readonly(h->ptr)) return ERR_READONLY;
    if(h->type == TYPE_BOOL && h->bit > 0) {
        if(h->bit > 7) return ERR_ARG;
        size = ((h->count + h->bit)-1)/8 + 1;
        temp = alloca(size);
        mask = alloca(size);

        bzero(mask, size);
        bzero(temp, size);

        i = h->bit % 8;
        for(int n = 0; n < h->count; n++) {
            if( (0x01 << (n % 8)) & ((uint8_t *)data)[n / 8] ) {
                ((uint8_t *)temp)[i / 8] |= (0x01 << (i % 8));
            }
            mask[i / 8] |= (0x01 << (i % 8));
            i++;
        }
        result = tag_mask_write_raw(db, h->ptr, h->byte, temp, mask, size );
    } else {
        result = tag_write_raw(db, h->ptr, h->byte, data, h->size);
    }
    //map_check(h->index, h->byte, h->size);
    return result;
}


/*!
 * Higher level tag masked write function.  This function uses
 * a tag handle to write the data.  The data that is passed
 * must contain enough data to satisfy the write.  Only data
 * that corresponds to bits that are set in the mask will be
 * written to the tagbase.
 *
 * @param db Pointer to the tag database
 * @param h Handle to the tag or tag fragment to be written
 * @param data Pointer to the data buffer
 * @param mask Pointer to the mask buffer
 * @returns Zero on success or error code otherwise
 */
int
tag_mask_write_handle(tag_db_t *db, tag_handle_t *h, void *data, void *mask) {
    size_t size;
    uint8_t *temp;
    uint8_t *masktemp;
    int i;

    if(tag_is_readonly(h->ptr)) return ERR_READONLY;
    if(h->type == TYPE_BOOL && (h->bit > 0 || h->count % 8 )) {
        if(h->bit > 7) return ERR_ARG;
        size = ((h->count + h->bit)-1)/8 + 1;
        temp = alloca(size);
        masktemp = alloca(size);
        bzero(temp, size);
        bzero(masktemp, size);

        i = h->bit % 8;
        for(int n = 0; n < h->count; n++) {
            if( (0x01 << (n % 8)) & ((uint8_t *)data)[n / 8] ) {
                ((uint8_t *)temp)[i / 8] |= (1 << (i % 8));
            }
            if( (0x01 << (n % 8)) & ((uint8_t *)mask)[n / 8] ) {
                ((uint8_t *)masktemp)[i / 8] |= (1 << (i % 8));
            }
            i++;
        }
        return tag_mask_write_raw(db, h->ptr, h->byte, temp, masktemp, size);
    } else {
        return tag_mask_write_raw(db, h->ptr, h->byte, data, mask, h->size);
    }
    return 0;
}


int
tag_read(tag_db_t *db, char *name, int count, void *data) {
    tag_handle_t h;
    int result;
    result = _tag_handle(db, &h, name, strlen(name), count);
    if(result) return result;

    return tag_read_handle(db, &h, data);
}


int
tag_write(tag_db_t *db, char *name, int count, void *data) {
    tag_handle_t h;
    int result;
    result = _tag_handle(db, &h, name, strlen(name), count);
    if(result) return result;

    return tag_write_handle(db, &h, data);
}


int
tag_mask_write(tag_db_t *db, char *name, int count, void *data, void *mask) {
    tag_handle_t h;
    int result;
    result = _tag_handle(db, &h, name, strlen(name), count);
    if(result) return result;

    return tag_mask_write_handle(db, &h, data, mask);
}


/* This is here so we can use _tag_handle */
int
event_add(tag_db_t *db, char *name, int count, int event_type, void *data, event_function f, void *udata) {
    tag_handle_t h;
    int result;
    result = _tag_handle(db, &h, name, strlen(name), count);
    if(result) return result;

    return event_add_handle(db, &h, event_type, data, f, udata);
}
