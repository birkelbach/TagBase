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


/* These two static functions destroy the cdt that is
 * passed as *cdt to _cdt_destroy.  _cdt_member_destroy
 * is a static function to free the member list */
static void
_cdt_member_destroy(cdt_member *member) {
    if(member->next != NULL) _cdt_member_destroy(member->next);
    if(member->name != NULL) free(member->name);
    free(member);
}

static inline void
_cdt_destroy(cdt_t *cdt) {
    if(cdt->members != NULL) _cdt_member_destroy(cdt->members);
    if(cdt->name != NULL ) free(cdt->name);
}

/* Receives a definition string in the form of "Name,Type,Count" and
 * appends that member to the compound datatype passed as *cdt.  Returns
 * 0 on success and dax error code on failure */
static int
cdt_append(tag_db_t *db, cdt_t *cdt, char *str)
{
    int retval, count;
    cdt_member *this, *new;
    char *name, *typestr, *countstr, *last;
    tag_type type;

    /* Parse the description string */
    name = strtok_r(str, ",", &last);
    if(name == NULL) return ERR_ARG;
    typestr = strtok_r(NULL, ",", &last);
    if(typestr == NULL) return ERR_ARG;
    countstr = strtok_r(NULL, ",", &last);
    if(countstr == NULL) return ERR_ARG;
    count = strtol(countstr, NULL, 0);

    /* Check that the name is okay */
    if( (retval = validate_tagname(name)) ) {
        return retval;
    }

    /* Check that the type is valid */
    if( (type = type_get_type(db, typestr)) == 0 ) {
        return ERR_ARG;
    }

    /* Check for duplicates */
    this = cdt->members;
    while(this != NULL) {
        if( !strcasecmp(name, this->name) ) {
            //logError(LOG_TAGBASE, "cdt_append() Duplicate name given");
            return ERR_DUPLICATE;
        }
        this = this->next;
    }

    /* Allocate the new member */
    new = malloc(sizeof(cdt_member));
    if(new == NULL) {
        //logError(LOG_TAGBASE, "Unable to allocate memory for new CDT member");
        return ERR_ALLOC;
    }
    /* Assign everything to the new datatype member */
    new->name = strdup(name);
    if(new->name == NULL) {
        free(new);
        //logError(LOG_TAGBASE, "Unable to allocate memory for the name of the new CDT member");
        return ERR_ALLOC;
    }
    new->type = type;
    new->count = count;
    new->next = NULL;

    /* Find the end of the linked list and put it there */
    if(cdt->members == NULL) {
        cdt->members = new;
    } else {
        this = cdt->members;

        while(this->next != NULL) {
            this = this->next;
        }
        this->next = new;
    }

    return 0;
}


/* Creates a compound datatype using the definition string in *str.
 * Returns the positive index of the newly created datatype if
 * sucessful or 0 on failure. If *error is not NULL any error codes
 * will be placed there otherwise zero will assigned to error. This
 * function uses strtok_r so the passed string can't be constant. */
tag_type
cdt_create(tag_db_t *db, char *str, int *error) {
    int result;
    char *name, *member, *last, *tmp, *serial;
    cdt_t cdt;
    cdt_t *new_datatype;
    tag_type type;

    /* We need a duplicate of the string to help check if this exact
     * CDT has been added before.  In that case we don't return error */
    tmp = strdup(str);
    if(tmp == NULL) {
        if(error != NULL) *error = ERR_ALLOC;
        return 0;
    }
    /* This messes with *str.  It puts a '\0' everywhere there is a ':'
     * and that is okay because the calling function doesn't need it anymore
     * except for printing the name so this works out okay. */
    name = strtok_r(tmp, ":", &last);
    /* Check that the name is okay */
    if( (result = validate_tagname(name)) ) {
        if(error != NULL) *error = result;
        free(tmp);
        return 0;
    }
    if((type = type_get_type(db, name))) {
        serialize_datatype(db, type, &serial);
        if(strcmp(serial, str)) { /* This means the two CDT's are not equal */
            if(error != NULL) *error = ERR_DUPLICATE;
            free(tmp);
            return 0;
        } else { /* If it's the same then just return it */
            free(tmp);
            return type;
        }
    }
    /* Initialize the new datatype */
    cdt.name = strdup(name);
    if(cdt.name == NULL) {
        if(error != NULL) *error = ERR_ALLOC;
        return 0;
    }
    cdt.members = NULL;

    while((member = strtok_r(NULL, ":", &last))) {
        result = cdt_append(db, &cdt, member);
        if(result) {
            _cdt_destroy(&cdt);
            if(error != NULL) *error = result;
            free(tmp);
            return 0;
        }
    }
    free(tmp);

    /* Do we have space in the array */
    if(db->datatype_index == db->datatype_size) {
        /* Allocate more space for the array */
        new_datatype = realloc(db->datatypes, (db->datatype_size + DATATYPE_SIZE) * sizeof(cdt_t));

        if(new_datatype != NULL) {
            db->datatypes = new_datatype;
            db->datatype_size += DATATYPE_SIZE;
        } else {
            if(error) *error = ERR_ALLOC;
            return 0;
        }
    }

    /* Add the datatype */
    db->datatypes[db->datatype_index].name = cdt.name;
    db->datatypes[db->datatype_index].members = cdt.members;
    db->datatypes[db->datatype_index].refcount = 0;
    db->datatypes[db->datatype_index].flags = 0;
    db->datatype_index++;

    if(error) *error = 0;
    //logDebug(LOG_TAGBASE, "Custom Datatype %s created as type=0x%X", cdt.name, CDT_TO_TYPE((_datatype_index - 1)));
    return CDT_TO_TYPE((db->datatype_index - 1));
}

/* This function incrememnts the reference counter for the
 * compound data type.  It assumes that the type is valid, if
 * the type is not valid, bad things will happen */
void
cdt_inc_refcount(tag_db_t *db, tag_type type) {
    db->datatypes[CDT_TO_INDEX(type)].refcount++;
}

/* This funtion decrements the reference counter */
void
cdt_dec_refcount(tag_db_t *db, tag_type type) {
    if(db->datatypes[CDT_TO_INDEX(type)].refcount != 0) {
        db->datatypes[CDT_TO_INDEX(type)].refcount--;
    }
}


/* Returns the type of the datatype with given name
 * If the datatype isn't found it returns 0 */
tag_type
type_get_type(tag_db_t *db, char *name) {
    int n;

    if(name == NULL) return 0;
    if(!strcasecmp(name, "BOOL"))
        return TYPE_BOOL;
    if(!strcasecmp(name, "BYTE"))
        return TYPE_BYTE;
    if(!strcasecmp(name, "SINT"))
        return TYPE_SINT;
    if(!strcasecmp(name, "CHAR"))
        return TYPE_CHAR;
    if(!strcasecmp(name, "WORD"))
        return TYPE_WORD;
    if(!strcasecmp(name, "INT"))
        return TYPE_INT;
    if(!strcasecmp(name, "UINT"))
        return TYPE_UINT;
    if(!strcasecmp(name, "DWORD"))
        return TYPE_DWORD;
    if(!strcasecmp(name, "DINT"))
        return TYPE_DINT;
    if(!strcasecmp(name, "UDINT"))
        return TYPE_UDINT;
    if(!strcasecmp(name, "TIME"))
        return TYPE_TIME;
    if(!strcasecmp(name, "REAL"))
        return TYPE_REAL;
    if(!strcasecmp(name, "LWORD"))
        return TYPE_LWORD;
    if(!strcasecmp(name, "LINT"))
        return TYPE_LINT;
    if(!strcasecmp(name, "ULINT"))
        return TYPE_ULINT;
    if(!strcasecmp(name, "LREAL"))
        return TYPE_LREAL;

    for(n = 0; n < db->datatype_index; n++) {
        if(!strcasecmp(name, db->datatypes[n].name)) {
            return CDT_TO_TYPE(n);
        }
    }
    return 0;
}

/* Returns a pointer to the name of the datatype given
 * by 'type'.  Returns NULL on failure */
const char *
type_get_name(tag_db_t *db, tag_type type)
{
    int index;

    if(IS_CUSTOM(type)) {
        index = CDT_TO_INDEX(type);
        if(index >= 0 && index < db->datatype_index) {
            return db->datatypes[index].name;
        } else {
            return NULL;
        }
        return NULL;
    } else {
        switch (type) {
            case TYPE_BOOL:
                return "BOOL";
            case TYPE_BYTE:
                return "BYTE";
            case TYPE_SINT:
                return "SINT";
            case TYPE_CHAR:
                return "CHAR";
            case TYPE_WORD:
                return "WORD";
            case TYPE_INT:
                return "INT";
            case TYPE_UINT:
                return "UINT";
            case TYPE_DWORD:
                return "DWORD";
            case TYPE_DINT:
                return "DINT";
            case TYPE_UDINT:
                return "UDINT";
            case TYPE_TIME:
                return "TIME";
            case TYPE_REAL:
                return "REAL";
            case TYPE_LWORD:
                return "LWORD";
            case TYPE_LINT:
                return "LINT";
            case TYPE_ULINT:
                return "ULINT";
            case TYPE_LREAL:
                return "LREAL";
            default:
                return NULL;
        }
    }
}

// /* Returns a pointer to the CDT given by 'type' */
// cdt_t *
// cdt_get_entry(tag_type type) {
//     int index;

//     index = CDT_TO_INDEX(type);
//     if(IS_CUSTOM(type) && index > 0 && index < _datatype_size) {
//         return &_datatypes[index];
//     } else {
//         return NULL;
//     }
// }


/* Adds a member to the datatype referenced by it's array index 'cdt_index'.
 * Returns 0 on success, nonzero error code on failure. */
// int
// cdt_add_member(datatype *cdt, char *name, tag_type type, unsigned int count)
// {
//     int retval;
//     cdt_member *this, *new;

//     /* Check that the name is okay */
//     if( (retval = _validate_name(name)) ) {
//         return retval;
//     }
//     /* Check that the type is valid */
//     if( _checktype(type) ) {
//         logError(LOG_TAGBASE, "cdt_add_member() - datatype 0x%X does not exist");
//         return ERR_ARG;
//     }

//     /* Check for duplicates */
//     this = cdt->members;
//     while(this != NULL) {
//         if( !strcasecmp(name, this->name) ) {
//             logError(LOG_TAGBASE, "cdt_add_member() - name %s already exists", name);
//             return ERR_DUPLICATE;
//         }
//         this = this->next;
//     }

//     /* Allocate the new member */
//     new = malloc(sizeof(cdt_member));
//     if(new == NULL) {
//         logError(LOG_TAGBASE, "cdt_add_member() - Unable to allocate memory for new datatype member %s", name);
//         return ERR_ALLOC;
//     }
//     /* Assign everything to the new datatype member */
//     new->name = strdup(name);
//     if(new->name == NULL) {
//         free(new);
//         logError(LOG_TAGBASE, "cdt_add_member() - Unable to allocate memory for member name %s", name);
//         return ERR_ALLOC;
//     }
//     new->type = type;
//     new->count = count;
//     new->next = NULL;

//     /* Find the end of the linked list and put it there */
//     if(cdt->members == NULL) {
//         cdt->members = new;
//     } else {
//         this = cdt->members;

//         while(this->next != NULL) {
//             this = this->next;
//         }
//         this->next = new;
//     }

//     return 0;
// }

/* Retrieves a pointer to the datatype array element identified by type
 * returns NULL on failure and if the error pointer is not NULL the error
 * code will be placed there. */
// static cdt_t *
// get_cdt_pointer(tag_type type, int *error)
// {
//     int index;

//     if(error != NULL) *error = 0;
//     if(IS_CUSTOM(type)) {
//         index = CDT_TO_INDEX(type);
//         if(index < _datatype_size && _datatypes[index].name != NULL) {
//             return &_datatypes[index];
//         } else {
//             *error = ERR_ARG;
//             return NULL;
//         }
//     } else {
//         *error = ERR_ARG;
//         return NULL;
//     }
// }


// /*!
//  * This is the compound data type iterator.  If type is a CDT then this
//  * function iterates over each member of the data type and calls 'callback'
//  * with the cdt_iter structure and passes back the udata pointer as well.
//  * If type is 0 then it iterates over the list of data types.  In this case
//  * the name and type fields in cdt_iter are the only things that are relevant.
//  *
//  * @param type The data type that we wish to iterate over.  If set to zero
//  *             This will iterate over the list of CDTs in the server
//  * @param udata Pointer to user data that will be passed to the callback
//  * @param callback Function that will be called for each member in the CDT
//  * @returns Zero on success or an error code otherwise
//  */
// int
// cdt_iter(tag_type type, void *udata, void (*callback)(cdt_iter_t member, void *udata))
// {
//     cdt_t *dt;
//     cdt_member *this;
//     cdt_iter_t iter;
//     int result;
//     int byte = 0;
//     int bit = 0;
//     int index = 0;

//     if(type == 0) { /* iterate through the custom types */
//         dt = get_cdt_pointer(CDT_TO_TYPE(index), &result);

//         while(result == 0) {
//             iter.name = dt->name;
//             iter.type = CDT_TO_TYPE(index);
//             iter.count = iter.byte = iter.bit = 0;
//             callback(iter, udata);

//             index++;
//             dt = get_cdt_pointer(CDT_TO_TYPE(index), &result);
//         }
//     } else { /* iterate over the members */
//         dt = get_cdt_pointer(type, &result);
//         if(dt == NULL) {
//             return result;
//         }

//         this = dt->members;
//         while(this != NULL) {
//             iter.name = this->name;
//             iter.type = this->type;
//             iter.count = this->count;
//             iter.bit = bit;
//             iter.byte = byte;
//             callback(iter, udata); /* Call the callback */
//             /* No sense in calculating the offsets if we're done */
//             if(this->next != NULL) {
//                 if(this->type == TYPE_BOOL) {
//                     bit += this->count % 8;
//                     byte += this->count / 8;
//                     if(bit > 7) { /* adjust for the overflow */
//                         bit %= 8;
//                         byte ++;
//                     }
//                     /* if the next one is not a BOOL then we should align */
//                     if(this->next->type != TYPE_BOOL && bit != 0) {
//                         bit = 0;
//                         byte++;
//                     }
//                 } else { /* Not this->type == DAX_BOOL */
//                     bit = 0;
//                     byte += type_size(this->type) * this->count;
//                 }
//             }
//             this = this->next;
//         }
//     }
//     return 0;
// }


/* This function builds a string that is a serialization of
 * the datatype and all of it's members.  The pointer passed
 * as str will be set to the string.  This pointer will have
 * to be freed by the calling program.  Returns the size
 * of the string. <0 on error */
int
serialize_datatype(tag_db_t *db, tag_type type, char **str)
{
    int size;
    char test[TAGNAME_SIZE + 1];
    cdt_member *this;
    int cdt_index;

    cdt_index = CDT_TO_INDEX(type);

    if(cdt_index < 0 || cdt_index >= db->datatype_index) {
        return ERR_ARG;
    }

    /* The first thing we do is figure out how big it
     * will all be. */
    size = strlen(db->datatypes[cdt_index].name);
    this = db->datatypes[cdt_index].members;

    while(this != NULL) {
        size += strlen(this->name);
        size += strlen(type_get_name(db, this->type));
        snprintf(test, TAGNAME_SIZE + 1, "%d", this->count);
        size += strlen(test);
        size += 3; /* This is for the ':' and the two commas */
        this = this->next;
    }
    size += 1; /* For Trailing NULL */

    *str = malloc(size);
    if(*str == NULL)
        return ERR_ALLOC;
    /* Now build the string */
    strncat(*str, db->datatypes[cdt_index].name, size -1);
    this = db->datatypes[cdt_index].members;

    while(this != NULL) {
        strncat(*str, ":", size - 1);
        strncat(*str, this->name, size - 1);
        strncat(*str, ",", size);
        strncat(*str, type_get_name(db, this->type), size - 1);
        strncat(*str, ",", size - 1);
        snprintf(test, TAGNAME_SIZE + 1, "%d", this->count);
        strncat(*str, test, size - 1);

        this = this->next;
    }
    return size;
}

/* Figures out how large the datatype is and returns
 * that size in bytes.  This function is recursive */
int
type_size(tag_db_t *db, tag_type type)
{
    int size = 0;
    int result;
    unsigned int pos = 0; /* Bit position within the data area */
    cdt_member *this;

    if( type_get_name(db, type) == NULL ) {
        return ERR_NOTFOUND;
    }

    if(IS_CUSTOM(type)) {
        this = db->datatypes[CDT_TO_INDEX(type)].members;
        while (this != NULL) {
            if(this->type == TYPE_BOOL) {
                pos += this->count; /* BOOLs are easy just add the number of bits */
            } else {
                /* Since it's not a bool we need to align to the next byte.
                 * To align it we set all the lower three bits to 1 and then
                 * increment. */
                if(pos % 8 != 0) { /* Do nothing if already aligned */
                    pos |= 0x07;
                    pos++;
                }
                if(IS_CUSTOM(this->type)) {
                    result = type_size(db, this->type);
                    pos += (result * this->count) * 8;
                } else {
                    /* This gets the size in bits */
                    pos += TYPESIZE(this->type) * this->count;
                }
            }
            this = this->next;
        }
        if(pos) {
            size = (pos - 1)/8 + 1;
        } else {
            size = 0;
        }
    } else { /* Not IS_CUSTOM() */
        size = TYPESIZE(type) / 8; /* Size in bytes */
    }
    return size;
}

