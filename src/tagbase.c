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

/* This is the source file for the tagname database handling routines */

#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>
#include <common.h>
#include <tagbase.h>

/* Notes:
 * We are using an AVL tree for the tag database.  It is assumed
 * that we'll do a lot more indexing the tag datbase than we will
 * do inserting and deleting so an AVL tree makes more sense than
 * a red black tree and a btree is just overkill for in-memory
 * databases.
 */

/* Uesd for the AVL tree to order and find tags */
static int
_tag_compare_function(void *left, void *right) {
    tag_t *l = left;
    tag_t *r = right;
    return strcmp(l->name, r->name);
}

static void
_tag_free_function(void *data) {
    tag_t *tag = data;
    DF("Deleting %s", tag->name);
}

tag_db_t *
new_tagbase(void) {
    int result;
    tag_t *tag;

    tag_db_t *db = malloc(sizeof(tag_db_t));
    db->tree = avl_new_tree(_tag_compare_function);
    avl_set_free_function(db->tree,_tag_free_function);
    db->datatypes = malloc(sizeof(cdt_t) * DATATYPE_SIZE);
    if(db->datatypes == NULL) {
        //logFatal(log_TAGBASE, "Unable to allocate array for datatypes");
        free(db);
        return NULL;
    }
    db->datatype_size = DATATYPE_SIZE;
    db->datatype_index = 0;
    db->ovrdinstalled = 0;
    db->ovrdset = 0;
    db->tagcount = 0;
    //db->sentinel = 0;

    tag = tag_add_virtual(db, "_system_time", TYPE_TIME, 1, system_time, NULL, NULL, &result);
    tag = tag_add_virtual(db, "_tagcount", TYPE_UDINT, 1, get_udint, NULL, &db->tagcount, &result);
    tag = tag_add_virtual(db, "_overrides_installed", TYPE_UDINT, 1, get_udint, NULL, &db->ovrdinstalled, &result);
    tag = tag_add_virtual(db, "_overrides_set", TYPE_UDINT, 1, get_udint, NULL, &db->ovrdset, &result);
    if(tag == NULL) {
        DF("ERRORRERRRR  %d", result);
        // TODO do some error stuff...
    }
    return db;
}

void
delete_tagbase(tag_db_t *db) {
    if(db != NULL) {
        free(db->datatypes);
        free(db);
    }
}


/* Determine whether or not the tag name is okay */
int
validate_tagname(char *name)
{
    int n;
    if(strlen(name) > TAGNAME_SIZE) {
        return ERR_2BIG;
    }
    /* First character has to be a letter or '_' */
    if( !isalpha(name[0]) && name[0] != '_') {
        return ERR_ARG;
    }
    /* The rest of the name can be letters, numbers or '_' */
    for(n = 1; n < strlen(name) ; n++) {
        if( !isalpha(name[n]) && (name[n] != '_') && !isdigit(name[n]) ) {
            return ERR_ARG;
        }
    }
    return 0;
}


/* Determine the size of the tag in bytes.  It'll
 * be big trouble if the index is out of bounds.
 * This will also return 0 when the tag has been deleted
 * which is designated by zero type and zero count */
int
tag_get_size(tag_db_t *db, tag_t *tag)
{
    if(tag->type == TYPE_BOOL)
        return (tag->count-1) / 8 + 1;
    else
        return tag_type_size(db, tag->type)  * tag->count;
}

static inline void
_set_attribute(tag_t *tag, uint32_t attr) {
    /* We only let the Tag Retention attribute to be set at this point */
    if(attr & TAG_ATTR_RETAIN) {
        /* TODO: add tag retention */;
        //ret_add_tag(idx);
        tag->attr |= TAG_ATTR_RETAIN;
    } else {
        if(attr & TAG_ATTR_OWNED && attr & TAG_ATTR_READONLY) {
            tag->attr |= (TAG_ATTR_OWNED | TAG_ATTR_READONLY);
        } else if(attr & TAG_ATTR_OWNED) {
            tag->attr |= TAG_ATTR_OWNED;
        }
    }
}

static int
_queue_add(tag_db_t *db, tag_t *tag, tag_type type, unsigned int count) {
    virt_functions vf;
    tag_queue *q;
    int size;

    q = (tag_queue *)malloc(sizeof(tag_queue));
    if(q == NULL) {
        return ERR_ALLOC;
    }
    size = tag_type_size(db, type) * count;
    q->size = size;
    q->count = count;
    q->type = type;
    q->qcount = 0;
    q->qread = 0;
    q->queue = malloc(START_QUEUE_SIZE * sizeof(void *));
    if(q->queue == NULL) {
        free(q);
        return ERR_ALLOC;
    }
    /* Set the pointers to zero so we know that they are not allocated */
    bzero(q->queue, START_QUEUE_SIZE * sizeof(void *));
    q->qsize = START_QUEUE_SIZE;
    vf.rf = read_queue;
    vf.wf = write_queue;
    vf.userdata = (uint8_t *)q;
    tag->data = malloc(sizeof(virt_functions));
    if(tag->data == NULL) {
        free(q->queue);
        free(q);
        return ERR_ALLOC;
    }
    memcpy(tag->data, &vf, sizeof(virt_functions));
    tag->attr |= TAG_ATTR_VIRTUAL;
    //logInfo(log_TAGBASE, "Add Queue for tag at index %d", idx);
    return 0;
}



/* This adds a tag to the database. It takes a string for the name
 * of the tag, the type (see opendax.h for types) and a count.  If
 * count is greater than 1 an array is created.  It returns the index
 * of the newly created tag or an error. */
tag_t *
tag_add(tag_db_t *db, char *name, tag_type type, uint32_t count, uint32_t attr, int *err) {
    void *newdata;
    unsigned int size;
    tag_t *tag;

    if(count == 0) {
        //logError(log_TAGBASE, "tag_add() called with count = 0");
        if(err != NULL) *err = ERR_ARG;
        return NULL;
    }

    if(tag_get_typename(db, type) == NULL) {
        //logDebug(log_TAGBASE, "tag_add() passed an unknown datatype %x", type);
        if(err != NULL) *err = ERR_ARG; /* is the datatype valid */
        return NULL;
    }
    if(validate_tagname(name)) {
        //logError(log_TAGBASE, "%s is not a valid tag name", name);
        if(err != NULL) *err = ERR_ARG;
        return NULL;
    }

    /* Figure the size in bytes */
    if(type == TYPE_BOOL) {
        size = (count-1) / 8 + 1;
    } else {
        size = tag_type_size(db, type) * count;
    }

    /* Check for an existing tagname in the database */
    tag_t search_tag;
    search_tag.name = name;
    tag = avl_search(db->tree, &search_tag);
    if(tag != NULL ) {
        /* If the tag is identical or bigger then just return the handle */
        if(tag->type == type && tag->count >= count) {
            _set_attribute(tag, attr);
            return tag;
        } else if(tag->type == type && tag->count < count) {
            /* If the new count is greater than the existing count then lets
             try to increase the size of the tags data */
            newdata = realloc(tag->data, size);
            //TODO Set the new area to zero
            if(newdata) {
                tag->data = newdata;
                tag->count = count;
                _set_attribute(tag, attr);
                /* Since it changed we update this tag so the write event will trigger */
                //tag_write_raw(INDEX_ADDED_TAG, 0, &n, sizeof(tag_index));
                return tag;
            } else {
                //logError(log_TAGBASE, "Unable to allocate memory to grow the size of tag %s", name);
                if(err != NULL) *err = ERR_ALLOC;
                return NULL;
            }
        } else {
            //logError(log_TAGBASE, "Duplicate tag name %s", name);
            if(err != NULL) *err = ERR_DUPLICATE;
            return NULL;
        }
    }
    /* Assign everything to the new tag, copy the string and git */
    struct tag_t *new_tag = (struct tag_t*) malloc(sizeof(struct tag_t));
    pthread_mutex_init(&new_tag->lock, NULL);

    // if a memory error has occurred
    if (new_tag == NULL) {
        if(err != NULL) *err = ERR_ALLOC;
        return NULL;
    }

    new_tag->name = strdup(name);
    new_tag->data = NULL;
    new_tag->left = NULL;
    new_tag->right = NULL;

    new_tag->count = count;
    new_tag->type = type;

    /* If the type is a queue the data area will be allocated in the
     * queue_add() function instead of here */
    if(IS_QUEUE(type)) {
        _queue_add(db, new_tag, type, count);
    } else {
        /* Allocate the data area */
        if((new_tag->data = malloc(size)) == NULL) {
            if(err != NULL) *err = ERR_ALLOC;
            return NULL;
        } else {
            bzero(new_tag->data, size);
        }
    }
    new_tag->nextevent = 1;
    new_tag->events = NULL;
    new_tag->omask = NULL;
    new_tag->odata = NULL;

    avl_insert_node(db->tree, new_tag);

    /* Only if everything works will we increment the count */
    if(IS_CUSTOM(type)) {
        cdt_inc_refcount(db, type);
    }
    db->tagcount++;


    DF("Added tag %s", new_tag->name);
    return new_tag;
    // if(_db[INDEX_TAGCOUNT].data != NULL) {
    //     tag_write_raw(INDEX_TAGCOUNT, 0, &_tagcount, sizeof(tag_index));
    // }
    // _set_attribute(n, attr);

    // /* Update the '_tag_added' system tag */
    // memset(&tag_desc, 0, sizeof(tag_t));
    // *((tag_index *)tag_desc) = n;
    // *((tag_type *)&tag_desc[4]) = type;
    // *((type_udint *)&tag_desc[8]) = count;
    // *((uint16_t *)&tag_desc[12]) = attr;
    // strncpy((char *)&tag_desc[14], name, TAGNAME_SIZE + 1);
    // tag_write_raw(INDEX_ADDED_TAG, 0, tag_desc, 47);

    //return n;
}


/* This function adds a virtual tag to the system.  When this tag is read, the data will
 * come from the function given by rf. And when written the data will be passed to wf.  If
 * rf == NULL then the tag will be write only and reads will return an error and likewise if
 * wf == NULL then the tag will be read only and writes will return an error.
 */
tag_t *
tag_add_virtual(tag_db_t *db, char *name, tag_type type, unsigned int count, vfunction *rf, vfunction *wf, void *udata, int *err) {
    tag_t *tag;
    virt_functions vf;

    tag = tag_add(db, name, type, count, 0, err);
    if(tag == NULL) return NULL; /* err should have been set inside tag_add() */
    /* We just allocated this data but that was just for convenience */
    free(tag->data);
    vf.rf = rf;
    vf.wf = wf;
    vf.userdata = udata;
    tag->data = malloc(sizeof(virt_functions));
    if(tag->data == NULL) {
        if(err != NULL) *err = ERR_ALLOC;
        return NULL;
    }
    memcpy(tag->data, &vf, sizeof(virt_functions));
    tag->attr |= TAG_ATTR_VIRTUAL;
    if(wf == NULL) tag->attr |= TAG_ATTR_READONLY;
    return tag;
}

const char *tag_get_name(tag_t *tag) {
    return (const char *)tag->name;
}


// /* Allocates the database array and the index.  Then creates the
//    system tags  */
// int
// initialize_tagbase(void) {
//     int result;
//     tag_type tag_type;
//     uint64_t starttime;

//     /* Allocate the primary database */
//     _db = calloc(TAGLIST_SIZE, sizeof(tag_t));
//     if(_db == NULL) {
//         logFatal(log_TAGBASE, "Unable to allocate the database");
//         return ERR_FAIL;
//     }
//     _dbsize = TAGLIST_SIZE;
//     /* Allocate the index */
//     _index = (tag_index_t *)calloc(TAGLIST_SIZE, sizeof(tag_index_t));
//     if(_index == NULL) {
//         logFatal(log_TAGBASE, "Unable to allocate the database");
//         return ERR_FAIL;
//     }

//     logVerbose(log_TAGBASE, "Database created with size = %d", _dbsize);
//     /* Creates the system tags */

//     result = init_types();
//     if(result) return result;

//     char tag_type_cdt[256];
//     snprintf(tag_type_cdt, 256, "_tag_desc:index,DINT,1:type,UDINT,1:count,UDINT,1:attributes,UINT,1:name,CHAR,%d", TAGNAME_SIZE + 1);
//     tag_type = cdt_create(tag_type_cdt, NULL);
//     if(tag_type == 0) {
//         logFatal(log_TAGBASE, "Unable to create default datatypes");
//         return ERR_FAIL;
//     }

//     /* TODO: instead of using all these constants we should probably just add
//      * the tags and store the indexes for the ones that we need. */
//     result = _create_system_tag("_tagcount", TYPE_DINT, 1, INDEX_TAGCOUNT);
//     if(result) return result;
//     result = _create_system_tag("_lastindex", TYPE_DINT, 1, INDEX_LASTINDEX);
//     if(result) return result;
//     result = _create_system_tag("_tag_added", tag_type, 1, INDEX_ADDED_TAG);
//     if(result) return result;
//     result = _create_system_tag("_tag_deleted", tag_type, 1, INDEX_DELETED_TAG);
//     if(result) return result;
//     result = _create_system_tag("_dbsize", TYPE_DINT, 1, INDEX_DBSIZE);
//     if(result) return result;
//     result = _create_system_tag("_starttime", TYPE_TIME, 1, INDEX_STARTED);
//     if(result) return result;
//     result = _create_system_tag("_overrides_installed", TYPE_DINT, 1, INDEX_OVRD_INSTALLED);
//     if(result) return result;
//     result = _create_system_tag("_overrides_set", TYPE_DINT, 1, INDEX_OVRD_SET);
//     if(result) return result;
//     virtual_tag_add("_time", TYPE_TIME, 1, server_time, NULL);
//     starttime = xtime();
//     tag_write_raw(INDEX_STARTED, 0, &starttime, sizeof(uint64_t));
//     set_dbsize(_dbsize);

//     logInfo(log_TAGBASE, "Tag Database Successfully Initialized");
//     return ERR_OK;
// }



int
tag_set_attribute(tag_t *tag, uint32_t attr) {
    tag->attr |= attr;
    return 0;
}

int
tag_clr_attribute(tag_t *tag, uint32_t attr) {
    tag->attr &= ~attr;
    return 0;
}


/* Deletes the tag given my index.  The tags position in the _db array is not
 * moved.  The name and data fields are freed and set to NULL.  The events
 * and the mappings are also freed and removed.  The item in the index is also
 * removed.  After a tag is deleted the index will always be smaller than the
 * database.  We cannot remove the items from the database and reuse them
 * because some modules may have stored an index to that tag and may try to
 * access it in the future.  If the tag is now different then bad things could
 * happen.
 */
int
tag_delete(tag_db_t *db, tag_t *tag) {
    event_del_check(tag); /* Check to see if we have a deleted event for this tag */
    avl_delete_node(db->tree, tag);
    if(IS_CUSTOM(tag->type)) {
        cdt_dec_refcount(db, tag->type);
    }
    // if(_db[idx].attr & TAG_ATTR_RETAIN) {
    //     ret_del_tag(idx);
    // }


    events_del_all(tag->events);
    tag->events = NULL;

    // map_del_all(_db[idx].mappings);
    // _db[idx].mappings = NULL;
    // _del_index(_db[idx].name);
    // logDebug(log_TAGBASE, "Tag deleted with name = %s", _db[idx].name);

    /* Update the '_tag_deleted' system tag */
    // memset(&tag_desc, 0, sizeof(tag_t));
    // *((tag_index *)tag_desc) = idx;
    // *((tag_type *)&tag_desc[4]) = _db[idx].type;
    // *((type_udint *)&tag_desc[8]) = _db[idx].count;
    // *((uint16_t *)&tag_desc[12]) = _db[idx].attr;
    // strncpy((char *)&tag_desc[14], _db[idx].name, TAGNAME_SIZE + 1);
    // tag_write_raw(INDEX_DELETED_TAG, 0, tag_desc, 47);

    if(tag->name != NULL) {
        free(tag->name);
    }
    if(tag->data != NULL) {
        free(tag->data);
    }
    tag->name = NULL;
    tag->data = NULL;
    tag->attr = 0;
    free(tag);

    db->tagcount--;
    // if(_db[INDEX_TAGCOUNT].data != NULL) {
    //     tag_write_raw(INDEX_TAGCOUNT, 0, &_tagcount, sizeof(tag_index));
    // }

    return 0;
}


/* Finds a tag based on it's name.  Basically just a wrapper for _get_by_name().
 * Fills in the structure 'tag' and returns zero on success */
tag_t *
tag_get_by_name(tag_db_t *db, char *name) {
    tag_t search_tag;
    search_tag.name = name;
    return avl_search(db->tree, &search_tag);
}

/* Iterator for listing tags.  The function f will be called for each tag
   with the tag and the udata */
void
tag_list(tag_db_t *db, void (*f)(tag_t *, void *), void *udata) {
    avl_inorder(db->tree, (void (*)(void *, void *))f, udata);
}


/* Returns true if the tag at the given index is read only */
int
tag_is_readonly(tag_t *tag) {
    return (tag->attr & TAG_ATTR_READONLY );
}

/* Returns true if the tag at the given index is a virtual tag */
int
tag_is_virtual(tag_t *tag) {
    return (tag->attr & TAG_ATTR_VIRTUAL );
}

/* Returns true if tag at the given index is a queue */
int
tag_is_queue(tag_t *tag) {
    return IS_QUEUE(tag->type);
}

/* Returns true if tag at the given index is owned */
int
tag_is_owned(tag_t *tag) {
    return (tag->attr & TAG_ATTR_OWNED );
}


static int
_tag_read_raw(tag_db_t *db, tag_t *tag, int offset, void *data, int size) {
    virt_functions *vf;
    int n;
    uint8_t x, y;

    /* determine if it's a virtual tag */
    if(tag->attr & TAG_ATTR_VIRTUAL) {
        /* virtual tags read their data from a function that should be
         * pointed to by the *data pointer */
        vf = (virt_functions *)tag->data;
        if(vf->rf == NULL) return ERR_WRITEONLY;
        return vf->rf(tag, offset, data, size, vf->userdata);
    } else {
        /* Bounds check size */
        if( (offset + size) > tag_get_size(db, tag)) {
            return ERR_2BIG;
        }
        if(tag->data == NULL) {
            return ERR_DELETED;
        }
        if(tag->attr & TAG_ATTR_OVR_SET) {
            for(n=0; n<size; n++) {
                 x = tag->odata[n+offset] & tag->omask[n+offset];
                 y = tag->data[n+offset] & ~tag->omask[n+offset];
                 ((uint8_t *)data)[n] = x | y;
            }
        } else {
            /* Copy the data into the right place. */
            memcpy(data, &(tag->data[offset]), size);
        }

        /* If the tag is special then call the hook */
        // if(tag->attr & TAG_ATTR_SPECIAL) {
        //     result = special_tag_read(tag, offset, data, size);
        //     if(result) {
        //         return result;
        //     }
        // }
    }

    return 0;
}


/*!
 * Low level tag read function.
 *
 * This function reads the data raw
 *
 * @param idx The tags index in the tag database
 * @param offset The byte offset in the tags data area
 * @param data Pointer to where the return data should be stored
 * @param size The amount of data to read
 * @returns Zero on success or an error code otherwise
 */
int
tag_read_raw(tag_db_t *db, tag_t *tag, int offset, void *data, int size) {
    int result;


    pthread_mutex_lock(&tag->lock);
    result = _tag_read_raw(db, tag, offset, data, size);
    pthread_mutex_unlock(&tag->lock);

    return result;
}


static int
_tag_write_raw(tag_db_t *db, tag_t *tag, int offset, void *data, int size) {
    virt_functions *vf;

    /* determine if it's a virtual tag */
    if(tag->attr & TAG_ATTR_VIRTUAL) {
       /* virtual tags read their data from a function that should be
        * pointed to by the *data pointer */
       vf = (virt_functions *)tag->data;
       if(vf->wf == NULL) return ERR_READONLY;
       return vf->wf(tag, offset, data, size, vf->userdata);
    } else {
        /* Bounds check size */
        if( (offset + size) > tag_get_size(db, tag)) {
            return ERR_2BIG;
        }
        if(tag->data == NULL) {
            return ERR_DELETED;
        }
        // if(tag->attr & TAG_ATTR_SPECIAL) {
        //     result = special_tag_write(tag, offset, data, size);
        //     if(result) {
        //         return result;
        //     }
        // }
        /* Copy the data into the right place. */
        memcpy(&(tag->data[offset]), data, size);
        event_check(tag, offset, size);
    }
    // if(tag->attr & TAG_ATTR_RETAIN) {
    //     ret_tag_write(tag);
    // }

    return 0;
}

/*!
 * Low level tag writing function.  This function bypasses the read-only
 * attribute and writes anyway.  It will also not carry forward a data
 * mapping.  It should not be called from user facing functions.
 *
 * @param idx The tags index in the tag database
 * @param offset The byte offset in the tags data area
 * @param data Pointer to the data buffer
 * @param size The amount of data to read
 * @returns Zero on success or an error code otherwise
 */
int
tag_write_raw(tag_db_t *db, tag_t *tag, int offset, void *data, int size) {
    int result;

    pthread_mutex_lock(&tag->lock);
    result = _tag_write_raw(db, tag, offset, data, size);
    pthread_mutex_unlock(&tag->lock);

    return result;
}



static int
_tag_mask_write_raw(tag_db_t *db, tag_t *tag, int offset, void *data, void *mask, int size) {
    uint8_t *dbi, *newdata, *newmask;
    int n;

    /* We don't allow masked writes to virtual tags.  This would be
     * too ambiguous */
    if(tag->attr & TAG_ATTR_VIRTUAL) return ERR_ILLEGAL;
    /* Bounds check size */
    if( (offset + size) > tag_get_size(db, tag)) {
        return ERR_2BIG;
    }
    if(tag->data == NULL) {
        return ERR_DELETED;
    }
    // if(tag->attr & TAG_ATTR_SPECIAL) {
    //     result = special_tag_mask_write(tag, offset, data, mask, size);
    //     if(result) return result;
    // }

    dbi = &tag->data[offset];
    newdata = (uint8_t *)data;
    newmask = (uint8_t *)mask;
    for(n = 0; n < size; n++) {
        dbi[n] = (newdata[n] & newmask[n]) | (dbi[n] & ~newmask[n]);
    }
    event_check(tag, offset, size);

    // if(tag->attr & TAG_ATTR_RETAIN) {
    //     ret_tag_write(tag);
    // }

    return 0;
}

/*!
 * Low level tag masked write function.  Writes data to the data area
 * of the given tag but only if the corresponding mask bit(s) are set.
 * This function bypasses the read-only attribute and writes anyway.
 * It also does not carry forward a data mapping.  It should not
 * be called from user facing functions.
 *
 * @param idx The tags index in the tag database
 * @param offset The byte offset in the tags data area
 * @param data Pointer to the data buffer
 * @param mask Pointer to the mask data buffer
 * @param size The amount of data to read
 * @returns Zero on success or an error code otherwise
 */
int
tag_mask_write_raw(tag_db_t *db, tag_t *tag, int offset, void *data, void *mask, int size) {
    int result;


    pthread_mutex_lock(&tag->lock);
    result = _tag_mask_write_raw(db, tag, offset, data, mask, size);
    pthread_mutex_unlock(&tag->lock);

    return result;
}


int
tag_lock(tag_t *tag) {
    int result;

    result = pthread_mutex_lock(&tag->lock);
    if(result) return ERR_FAIL;
    return ERR_OK;
}

int
tag_unlock(tag_t *tag) {
    int result;

    result = pthread_mutex_unlock(&tag->lock);
    if(result) return ERR_FAIL;
    return ERR_OK;
}
