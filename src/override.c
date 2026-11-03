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
#include <tagbase.h>

/* Tag override functions */
int
override_add(tag_db_t *db, tag_t *tag, int offset, void *data, void *mask, int size) {
    int tag_size;

    tag_size = tag->count * tag_type_size(db, tag->type);

    if(tag->odata == NULL) {
        tag->odata = malloc(tag_size);
        if(tag->odata == NULL) return ERR_ALLOC;
        tag->omask = malloc(tag_size);
        if(tag->omask == NULL) {
            free(tag->odata);
            tag->odata = NULL;

            return ERR_ALLOC;
        }
    }
    if((offset + size) > tag_size) return ERR_2BIG;
    memcpy(&tag->odata[offset], data, size);
    if(mask) {
        memcpy(&tag->omask[offset], mask, size);
    } else {
        memset(tag->omask, 0xFF, tag_size);
    }

    /* If the flag for this tag is not already set then increment the counter */
    if(! (tag->attr & TAG_ATTR_OVERRIDE )) {
        db->ovrdinstalled++;
        // if(_db[INDEX_OVRD_INSTALLED].data != NULL) {
        //     tag_write_raw(INDEX_OVRD_INSTALLED, 0, &_ovrdinstalled, sizeof(tag_index));
        // }
    }

    tag->attr |= TAG_ATTR_OVERRIDE; /* Indicate that we have an override installed */

    return 0;
}

int
override_del(tag_db_t *db, tag_t *tag, int offset, void *mask, int size) {
    int tag_size;
    int n;

    tag_size = tag->count * tag_type_size(db, tag->type);
    if(tag->odata == NULL) {
        return ERR_FAIL;
    }
    for(n=0;n<size;n++) {
        tag->omask[n+offset] &= ~((uint8_t *)mask)[n];
        tag->odata[n+offset] &= ~((uint8_t *)mask)[n];
    }
    tag_size = tag->count * tag_type_size(db, tag->type);
    for(n=0;n<tag_size;n++) {
        if(tag->omask[n])  return 0;
    }
    /* Remove both of the flags that indicate we have an override installed or set */
    tag->attr &= ~(TAG_ATTR_OVERRIDE | TAG_ATTR_OVR_SET);
    /* If we get here then we've deleted the last of the overrides for this
     * tag so we'll free the memory */
    db->ovrdinstalled--;
    // if(_db[INDEX_OVRD_INSTALLED].data != NULL) {
    //     tag_write_raw(INDEX_OVRD_INSTALLED, 0, &_ovrdinstalled, sizeof(tag_index));
    // }
    free(tag->odata);
    tag->odata = NULL;
    free(tag->omask);
    tag->omask = NULL;

    return 0;
}


int
override_get(tag_db_t *db, tag_t *tag, int offset, int size, void *data, void *mask) {
    int tag_size;

    if(tag->data == NULL) {
        return ERR_DELETED;
    }
    tag_size = tag->count * tag_type_size(db, tag->type);
    if((offset + size) > tag_size) return ERR_2BIG;
    memcpy(data, tag->data, size);
    memcpy(mask, tag->omask, size);
    return 0;
}

int
override_set(tag_db_t *db, tag_t *tag, uint8_t flag) {
    if(flag) {
        if(tag->odata == NULL) return ERR_ILLEGAL;
        if( !(tag->attr & TAG_ATTR_OVR_SET)) {
            db->ovrdset++;
            // if(_db[INDEX_OVRD_SET].data != NULL) {
            //     tag_write_raw(INDEX_OVRD_SET, 0, &_ovrdset, sizeof(tag_index));
            // }
            tag->attr |= TAG_ATTR_OVR_SET;
        }
    } else {
        if(tag->attr & TAG_ATTR_OVR_SET) {
            db->ovrdset--;
            // if(_db[INDEX_OVRD_SET].data != NULL) {
            //     tag_write_raw(INDEX_OVRD_SET, 0, &_ovrdset, sizeof(tag_index));
            // }
            tag->attr &= ~TAG_ATTR_OVR_SET;
        }
    }
    return 0;
}

