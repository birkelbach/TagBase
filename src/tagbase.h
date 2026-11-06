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

#ifndef __TAGBASE_H__
#define __TAGBASE_H__

#include <stdint.h>

/* General Return Values */
#define ERR_OKAY             0
#define ERR_OK               0
#define ERR_FAIL            -1
#define ERR_ARG             -2
#define ERR_ALLOC           -3
#define ERR_FULL            -4
#define ERR_2BIG            -5
#define ERR_NOT_IMPLEMENTED -6
#define ERR_NOTFOUND        -7
#define ERR_ILLEGAL         -8
#define ERR_EMPTY           -9
#define ERR_READONLY        -10
#define ERR_WRITEONLY       -11
#define ERR_DUPLICATE       -12
#define ERR_DELETED         -13
#define ERR_TIMEOUT         -14
#define ERR_EXPIRED         -15


/* Tag attribute bits */
#define TAG_ATTR_READONLY   0x0001 /* Read only tag */
#define TAG_ATTR_VIRTUAL    0x0002 /* Tag is a virtual tag */
#define TAG_ATTR_RETAIN     0x0004 /* Tag will be retained */
#define TAG_ATTR_OVR_SET    0x0008 /* Tag override is set */
#define TAG_ATTR_SPECIAL    0x0010 /* Special tags have a read/write hook function */
#define TAG_ATTR_OWNED      0x0020 /* Owned tags can not be deleted */
#define TAG_ATTR_EVENT      0x2000 /* Tag has at least one event */
#define TAG_ATTR_OVERRIDE   0x4000 /* Tag has override installed */


/* Event Types */
#define EVENT_READ     0x01 /* Called before a tag is read - Not implemented */
#define EVENT_WRITE    0x02 /* When a tag is written whether or not it has changed */
#define EVENT_CHANGE   0x03 /* When a change to the tags value has occurred */
#define EVENT_SET      0x04 /* Bit set to 1*/
#define EVENT_RESET    0x05 /* Bit cleared to 0*/
#define EVENT_EQUAL    0x06 /* Equal to */
#define EVENT_GREATER  0x07 /* Greater Than */
#define EVENT_LESS     0x08 /* Less Than */
#define EVENT_DEADBAND 0x09 /* Changed by X amount since last event */
#define EVENT_DELETED  0x0A /* Tag gets deleted */

/* Event Options */
#define EVENT_OPT_SEND_DATA  0x01 /* Send the affected data with the event */

/* Defines the maximum length of a tagname */
#ifndef TAGNAME_SIZE
 #define TAGNAME_SIZE 32
#endif

#define IS_CUSTOM(TYPE) ((TYPE) & TYPE_CUSTOM)
#define IS_QUEUE(TYPE) ((TYPE) & TYPE_QUEUE)

/* 8 Bit */
#define TYPE_BYTE_MIN    0
#define TYPE_BYTE_MAX    255
#define TYPE_SINT_MIN   -128
#define TYPE_SINT_MAX    127
/* 16 Bit */
#define TYPE_WORD_MIN    0
#define TYPE_WORD_MAX    65535
#define TYPE_UINT_MIN    TYPE_WORD_MIN
#define TYPE_UINT_MAX    TYPE_WORD_MAX
#define TYPE_INT_MIN    -32768
#define TYPE_INT_MAX     32767
/* 32 Bit */
#define TYPE_DWORD_MIN   0
#define TYPE_DWORD_MAX   4294967295
#define TYPE_DINT_MIN   -2147483648
#define TYPE_DINT_MAX    2147483647
#define TYPE_UDINT_MIN   TYPE_DWORD_MIN
#define TYPE_UDINT_MAX   TYPE_DWORD_MAX
/* 64 Bit */
#define TYPE_LWORD_MIN    0
#define TYPE_LWORD_MAX    18446744073709551615ULL
#define TYPE_LINT_MIN    -9223372036854775808LL
#define TYPE_LINT_MAX     9223372036854775807LL
#define TYPE_ULINT_MIN    TYPE_LWORD_MIN
#define TYPE_ULINT_MAX    TYPE_LWORD_MAX
#define TYPE_TIME_MIN     TYPE_LINT_MIN
#define TYPE_TIME_MAX     TYPE_LINT_MAX
/* Floating Point */
#define TYPE_REAL_MIN   -FLT_MAX
#define TYPE_REAL_MAX    FLT_MAX
#define TYPE_LREAL_MIN  -DBL_MAX
#define TYPE_LREAL_MAX   DBL_MAX

/* typedefs to the basic DAX_? data types */
typedef uint8_t    type_byte;
typedef int8_t     type_sint;
typedef char       type_char;
typedef uint16_t   type_word;
typedef int16_t    type_int;
typedef uint16_t   type_uint;
typedef uint32_t   type_dword;
typedef int32_t    type_dint;
typedef uint32_t   type_udint;
typedef int64_t    type_time;
typedef float      type_real;
typedef uint64_t   type_lword;
typedef int64_t    type_lint;
typedef uint64_t   type_ulint;
typedef double     type_lreal;

typedef type_dint tag_index;
typedef uint32_t tag_type;

/* Object Pointers */
/*! Opaque pointer for a database object */
typedef struct tag_db_t tag_db_t;

/*! Opaque pinter for tag object */
typedef struct tag_t tag_t;

/*! Opaque pointer for tag group */
typedef struct tag_group_id tag_group_id;

/*! Opaque pointer for tag handle */
typedef struct tag_handle_t tag_handle_t;

/*! Opaque pointer to a custom data type */
typedef struct cdt_t cdt_t;

/*! Custom Datatype Iterator */
typedef struct cdt_iter {
    const char *name;
    tag_type type;
    int count;
    int byte;
    int bit;
} cdt_iter_t;


/* Easy way to store base datatypes.  Doesn't include BOOL */
typedef union tag_type_union {
    type_byte   type_byte;
    type_sint   type_sint;
    type_char   type_char;
    type_word   type_word;
    type_int    type_int;
    type_uint   type_uint;
    type_dword  type_dword;
    type_dint   type_dint;
    type_udint  type_udint;
    type_time   type_time;
    type_lword  type_lword;
    type_lint   type_lint;
    type_ulint  type_ulint;
    type_real   type_real;
    type_lreal  type_lreal;
} tag_type_union;

/* Data type definitions */
#define TYPE_BOOL    0x0010
/* 8 Bits */
#define TYPE_BYTE    0x0003
#define TYPE_SINT    0x0013
#define TYPE_CHAR    0x0023
/* 16 Bits */
#define TYPE_WORD    0x0004
#define TYPE_INT     0x0014
#define TYPE_UINT    0x0024
/* 32 Bits */
#define TYPE_DWORD   0x0005
#define TYPE_DINT    0x0015
#define TYPE_UDINT   0x0025
#define TYPE_REAL    0x0035
/* 64 Bits */
#define TYPE_LWORD   0x0006
#define TYPE_LINT    0x0016
#define TYPE_ULINT   0x0026
#define TYPE_TIME    0x0036 /* milliseconds since the epoch */
#define TYPE_LREAL   0x0046

/* The highest order bit shows that it's a custom datatype */
#define TYPE_CUSTOM  0x80000000
/* If this bit is set then the datatype is a queue */
#define TYPE_QUEUE   0x40000000

/* The size of the base datatypes are based on the lower 4 bits.  A
 * simple bit shift will give the size of the datatype */
#define TYPE_1BIT    0x0000
#define TYPE_8BITS   0x0003
#define TYPE_16BITS  0x0004
#define TYPE_32BITS  0x0005
#define TYPE_64BITS  0x0006

#define TYPE_64_ONES 0xFFFFFFFFFFFFFFFFULL


/*! Macro to get the size of the datatype in bits*/
#define TYPESIZE(TYPE) (0x0001 << (TYPE & 0x0F))

/* Flag bits for the tag data groups */
#define GRP_FLAG_NOT_EMPTY  0x01


/* Some Macros for manipulating CDT types */
#define CDT_TO_INDEX(TYPE) (TYPE & ~TYPE_CUSTOM)
#define CDT_TO_TYPE(INDEX) (INDEX | TYPE_CUSTOM)

// #define CDT_FLAGS_RETAINED 0x80;

typedef void (*event_function)(void *);
/* type definition of the virtual tag function prototype */
typedef int vfunction(tag_t *tag, int offset, void *data, int size, void *userdata);

/* Tag Database Handling Functions */
tag_db_t *new_tagbase(void);
void delete_tagbase(tag_db_t *db);

tag_t *tag_add(tag_db_t *db, char *name, tag_type type, uint32_t count, uint32_t attr, int *err);
int tag_set_attribute(tag_t *tag, uint32_t attr);
int tag_clr_attribute(tag_t *tag, uint32_t attr);

tag_t *tag_add_virtual(tag_db_t *db, char *name, tag_type type, uint32_t count, vfunction *rf, vfunction *wf, void *udata, int *err);
int tag_delete(tag_db_t *db, tag_t *tag);
tag_t *tag_get_by_name(tag_db_t *db, char *name);
tag_handle_t *tag_gethandle(tag_db_t *db, char *str, int count, int *err);
void tag_list(tag_db_t *db, void (*f)(tag_t *, void *), void *udata);
const char *tag_get_name(tag_t *tag);
int tag_is_readonly(tag_t *tag);
int tag_is_virtual(tag_t *tag);
int tag_is_queue(tag_t *tag);
int tag_is_owned(tag_t *tag);
int tag_get_size(tag_db_t *db, tag_t *tag);

/* Database reading and writing functions */
int tag_read_handle(tag_db_t *db, tag_handle_t *h, void *data);
int tag_write_handle(tag_db_t *db, tag_handle_t *h, void *data);
int tag_mask_write_handle(tag_db_t *db, tag_handle_t *h, void *data, void *mask);
int tag_read(tag_db_t *db, char *name, int count, void *data);
int tag_write(tag_db_t *db, char *name, int count, void *data);
int tag_mask_write(tag_db_t *db, char *name, int count, void *data, void *mask);
int tag_lock(tag_t *tag);
int tag_unlock(tag_t *tag);

/* Custom DataType functions */
tag_type tag_cdt_create(tag_db_t *db, char *str, int *error);
cdt_t *tag_cdt_new(char *name, int *err);
int tag_cdt_add_member(tag_db_t *db, cdt_t *cdt, char *name, tag_type type, uint32_t count);
int tag_cdt_finalize(tag_db_t *db, cdt_t *cdt);
void tag_cdt_free(cdt_t *cdt);

tag_type tag_get_type(tag_db_t *db, char *name);
const char *tag_get_typename(tag_db_t *db, tag_type type);
// cdt_t *cdt_get_entry(tag_type type);
int tag_cdt_iter(tag_db_t *db, tag_type type, void *udata, void (*callback)(cdt_iter_t member, void *udata));
int tag_type_size(tag_db_t *db, tag_type type);

/* The event functions */
int tag_event_add_handle(tag_db_t *db, tag_handle_t *h, int event_type, void *data, event_function f, void *udata);
int tag_event_add(tag_db_t *db, char *name, int count, int event_type, void *data, event_function f, void *udata);
int tag_event_del(tag_t *, int id);
int tag_event_opt(tag_t *, int id, uint32_t options);
void *tag_event_get_userdata(tag_t *, int id, int *res);

int override_add(tag_db_t *db, tag_t *tag, int offset, void *data, void *mask, int size);
int override_del(tag_db_t *db, tag_t *tag, int offset, void *mask, int size);
int override_get(tag_db_t *db, tag_t *tag, int offset, int size, void *data, void *mask);
int override_set(tag_db_t *db, tag_t *tag, uint8_t flag);

// /* Tag retention functions */
// #define RET_FLAG_DELETED 0x01
//
// int ret_init(char *filename);
// int ret_add_tag(int index);
// int ret_del_tag(int index);
// int ret_tag_write(int index);
// int ret_close(void);


#endif /* !__TAGBASE_H__ */
