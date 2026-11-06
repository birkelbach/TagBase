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

 * This header contains the private type definitions for the tagbase library.
 */

#ifndef __COMMON_H
#define __COMMON_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#include <tagbase.h>
#include <avl_tree.h>
#include "config.h"

/* Initial size of the custom data types array in each data base*/
#define DATATYPE_SIZE 8

/*!
 * This structure is used to define a specific tag or a part of the tag for
 * reading and writing.
 */
struct  tag_handle_t {
    tag_t *ptr;            /*!< Pointer to the Tag */
    char *str;             /*!< This string is the string that created the handle*/
    uint32_t count;        /*!< The number of items represented by the handle */
    uint32_t byte;         /*!< The byte offset where the data block starts */
    unsigned char bit;     /*!< The bit offset */
    uint32_t size;         /*!< The total size of the data block in bytes */
    tag_type type;         /*!< The data type of the block */
};


/* This is the compound datatype member definition.  The
 * members are represented as a linked list */
typedef struct cdt_member {
    char *name;
    uint32_t type;
    uint32_t count;
    struct cdt_member *next;
} cdt_member;


/* This is the structure that represents the container for each
 * datatype. */
struct cdt_t {
    char *name;
    uint8_t flags;
    unsigned int refcount; /* Number of tags of this type */
    cdt_member *members;
};


typedef struct event_t {
    int id;                 /* Unique identifier for this event definition */
    bool inhibit;           /* Used to inhibit the event while the event is running */

    int byte;               /* The byte offset where the data block starts */
    unsigned char bit;      /* The bit offset */
    int count;              /* The number of items represented by the handle */
    uint32_t size;          /* The total size of the data block in bytes */
    uint32_t options;       /* Options for the event */
    tag_type datatype;      /* The data type of the block */
    int eventtype;          /* The type of event */
    void *data;             /* Data given by module */
    void *test;             /* Internal data, depends on event type */
    event_function notify;  /* Callback function for this event */
    void *userdata;         /* user data that is associated with the callback function */
    struct event_t *next;
} event_t;


/* This is the internal structure for an individual tag. */
struct tag_t {
    tag_type type;
    uint16_t attr;
    unsigned int count;
    pthread_mutex_t lock;
    char *name;
    int nextevent;             /* Counter for keeping track of event IDs */
    event_t *events;           /* Linked list of events */
    uint8_t *data;
    uint8_t *omask;            /* Override mask pointer */
    uint8_t *odata;            /* Override data pointer */
    uint32_t ret_file_pointer; /* Pointer to the data area of the tag retention file */
    /* AVL Tree Stuff */
    struct tag_t *left;
    struct tag_t *right;
    int ht;
};


struct tag_db_t {
    uint32_t count;
    cdt_t *datatypes;
    unsigned int datatype_size;
    unsigned int datatype_index;  /* Next datatype index */
    uint32_t ovrdinstalled;
    uint32_t ovrdset;
    uint32_t tagcount;
    /* sentinel is incremented when tags are deleted.  Handles will contain the sentinel
       and any handle with a sentinel less than this number will have to be recreated */
    //unsigned int sentinel;
    avl_tree_t *tree; /* Tag AVL Tree */
};


/* tagtype.c */
int validate_tagname(char *name);
void cdt_inc_refcount(tag_db_t *db, tag_type type);
void cdt_dec_refcount(tag_db_t *db, tag_type type);
int serialize_datatype(tag_db_t *db, tag_type type, char **str);


/* tagbase.c */
int tag_read_raw(tag_db_t *db, tag_t *tag, int offset, void *data, int size);
int tag_write_raw(tag_db_t *db, tag_t *tag, int offset, void *data, int size);
int tag_mask_write_raw(tag_db_t *db, tag_t *tag, int offset, void *data, void *mask, int size);

/* events.c*/
void event_check(tag_t *tag, int offset, int size);
void event_del_check(tag_t *tag);
int events_del_all(event_t *head);


/* queue handling functions */
int write_queue(tag_t *tag, int offset, void *data, int size, void *userdata);
int read_queue(tag_t *tag, int offset, void *data, int size, void *userdata);


/* virtual.c */
int system_time(tag_t *tag, int offset, void *data, int size, void *userdata);
int get_udint(tag_t *tag, int offset, void *data, int size, void *userdata);

/* utility.c */
time_t get_time(void);

/* This structure is used to hold the two virtual functions
 * that would define a virtual tag as well as the userdata for
 * that tag.
 * rf = the read function
 * wf = the write function
 * userdata = userdata that may or may not be needed.
 * If rf is NULL then the tag is write only
 * and if rf is NULL then the tag is read only
 */
typedef struct virt_functions {
    vfunction *rf;
    vfunction *wf;
    void *userdata;
} virt_functions;

#define START_QUEUE_SIZE 16
#define MAX_QUEUE_SIZE 1024

/* This structure represents a single tag queue.  A copy of
 * this would be placed in the *userdata pointer of the virt_function
 * structure in the data area of the tag.
 */
typedef struct tag_queue {
    tag_type type;   /* Type of the tag items */
    uint32_t count;  /* The number of tags of each item */
    uint32_t size;   /* The size of the tag item */
    int qsize;       /* Total size of the queue in number of tag items */
    int qcount;      /* Current number of items in the queue */
    int qread;       /* Nest item that needs to be read */
    void **queue;    /* Pointer to the actual queue */
} tag_queue;



#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif
#ifndef ABS
#define ABS(a)     (((a) < 0) ? -(a) : (a))
#endif

#include <debug.h>

#endif /* !__COMMON_H */

