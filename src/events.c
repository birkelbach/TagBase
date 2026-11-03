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

/* This is the source file for the event notification handling routines */

#include <ctype.h>
#include <pthread.h>
#include <common.h>
#include <tagbase.h>

//pthread_mutex_t _event_lock;

/* Check to see if the tag has changed */
static inline int
_event_change(event_t *event, tag_t *tag, int offset, int size) {
    int bit, n, i, len, result;
    uint8_t *last, *that;
    uint8_t mask;
    result = 0;

    if(event->datatype == TYPE_BOOL) {
        /* TODO: Only check the bits that have changed.  For now we are just
         * looping through each bit in the event to see if anything has
         * changed.  This can be made much more efficient. */
        last = (uint8_t *)event->test;
        that = (uint8_t *)&(tag->data[event->byte]);
        bit = event->bit;
        i = 0;
        for(n = 0; n < event->count; n++) {
            mask = (0x01 << bit);
            if((last[i] & mask) != (that[i] & mask)) {
                result = 1;
            }
            bit++;
            if(bit == 8) {
                bit = 0;
                i++;
            }
            memcpy(last, that, size);
        }
    } else {
        last = (uint8_t *)event->test + MAX(0, offset - event->byte);
        that = (uint8_t *)&(tag->data[MAX(offset, event->byte)]);
        len = MIN(event->byte + event->size, offset + size) - MAX(offset, event->byte);

        for(n = 0; n < len; n++) {
            if(last[n] != that[n]) {
                result = 1;
                last[n] = that[n];
            }
        }
    }
    return result;
}

/* Checks to see if the bit is set and whether or not the event has been sent */
static inline int
_event_set(event_t *event, tag_t *tag, int offset, int size) {
    int bit, n, i, result;
    uint8_t *last, *that;
    uint8_t mask;
    result = 0;

    /* TODO: Only check the bits that have changed.  For now we are just
     * looping through each bit in the event to see if anything has
     * changed.  This can be made much more efficient. */
    last = (uint8_t *)event->test;
    that = (uint8_t *)&(tag->data[event->byte]);
    bit = event->bit;
    i = 0;
    for(n = 0; n < event->count; n++) {
        mask = (0x01 << bit);
        /* If the bit is off and the last bit is on the we fire  */
        if( (that[i] & mask) && !(last[i] & mask) ) {
            result = 1;
        }
        bit++;
        if(bit == 8) {
            bit = 0;
            i++;
        }
    }
    memcpy(last, that, size);
    return result;
}

static inline int
_event_reset(event_t *event, tag_t *tag, int offset, int size) {
    int bit, n, i, result;
    uint8_t *last, *that;
    uint8_t mask;
    result = 0;

    last = (uint8_t *)event->test;
    that = (uint8_t *)&(tag->data[event->byte]);
    bit = event->bit;
    i = 0;

    for(n = 0; n < event->count; n++) {
        mask = (0x01 << bit);
        /* If the bit is off and the last bit is on the we fire  */
        if( (!(that[i] & mask)) && (last[i] & mask) ) {
            result = 1;
        }
        bit++;
        if(bit == 8) {
            bit = 0;
            i++;
        }
    }
    memcpy(last, that, size);
    return result;
}

static inline int
_generic_compare(tag_type datatype, void *data1, void *data2) {
    tag_type_union u1, u2;
    int result;

    switch(datatype) {
        case TYPE_BYTE:
            u1.type_byte = *(type_byte *)data1;
            u2.type_byte = *(type_byte *)data2;
            if(u1.type_byte < u2.type_byte) result = -1;
            else if(u1.type_byte > u2.type_byte) result = 1;
            else result = 0;
            return result;
        case TYPE_SINT:
        case TYPE_CHAR:
            u1.type_sint = *(type_sint *)data1;
            u2.type_sint = *(type_sint *)data2;
            if(u1.type_sint < u2.type_sint) return -1;
            else if(u1.type_sint > u2.type_sint) return 1;
            else return 0;
        case TYPE_UINT:
        case TYPE_WORD:
            u1.type_uint = *(type_uint *)data1;
            u2.type_uint = *(type_uint *)data2;
            if(u1.type_uint < u2.type_uint) return -1;
            else if(u1.type_uint > u2.type_uint) return 1;
            else return 0;
        case TYPE_INT:
            u1.type_int = *(type_int *)data1;
            u2.type_int = *(type_int *)data2;
            if(u1.type_int < u2.type_int) return -1;
            else if(u1.type_int > u2.type_int) return 1;
            else return 0;
        case TYPE_UDINT:
        case TYPE_DWORD:
        case TYPE_TIME:
            u1.type_udint = *(type_udint *)data1;
            u2.type_udint = *(type_udint *)data2;
            if(u1.type_udint < u2.type_udint) return -1;
            else if(u1.type_udint > u2.type_udint) return 1;
            else return 0;
        case TYPE_DINT:
            u1.type_dint = *(type_dint *)data1;
            u2.type_dint = *(type_dint *)data2;
            if(u1.type_dint < u2.type_dint) return -1;
            else if(u1.type_dint > u2.type_dint) return 1;
            else return 0;
        case TYPE_ULINT:
        case TYPE_LWORD:
            u1.type_ulint = *(type_ulint *)data1;
            u2.type_ulint = *(type_ulint *)data2;
            if(u1.type_ulint < u2.type_ulint) return -1;
            else if(u1.type_ulint > u2.type_ulint) return 1;
            else return 0;
        case TYPE_LINT:
            u1.type_lint = *(type_lint *)data1;
            u2.type_lint = *(type_lint *)data2;
            if(u1.type_lint < u2.type_lint) return -1;
            else if(u1.type_lint > u2.type_lint) return 1;
            else return 0;
        case TYPE_REAL:
            u1.type_real = *(type_real *)data1;
            u2.type_real = *(type_real *)data2;
            if(u1.type_real < u2.type_real) return -1;
            else if(u1.type_real > u2.type_real) return 1;
            else return 0;
        case TYPE_LREAL:
            u1.type_lreal = *(type_lreal *)data1;
            u2.type_lreal = *(type_lreal *)data2;
            if(u1.type_lreal < u2.type_lreal) return -1;
            else if(u1.type_lreal > u2.type_lreal) return 1;
            else return 0;
    }
    DF("OH CRAP, OH CRAP, OH CRAP!!!!!");
    return -2;
}

static int
_event_compare(event_t *event, tag_t *tag, int offset, int size, int compare) {
    int n, inc, bit, len, result;
    uint8_t *this, *that;
    uint8_t mask;
    result = 0;

    inc = TYPESIZE(event->datatype) / 8;
    /* For these 'this' is a bit field */
    bit = MAX(0, (offset - event->byte) / inc);
    this = (uint8_t *)event->test;
    that = (uint8_t *)&(tag->data[MAX(offset, event->byte)]);
    len = MIN(event->byte + event->size, offset + size) - MAX(offset, event->byte);
    for(n = 0; n < len; n += inc) {
        mask = 0x01<<(bit%8);
        if(_generic_compare(event->datatype, event->data, &(that[n])) == compare) {
            if(!(this[bit/8] & mask)) {
                this[bit/8] |= mask;
                result = 1;
            }
        } else {
            this[bit/8] &= ~mask; /* Unset flag so event will fire next time */
        }
        bit++;
    }
    return result;
}

/* This function calculates whether the deadband has been exceeded for each of the
 * different datatypes.  Returns 1 if the deadband has been exceeded and 0 otherwise */
static inline int
_generic_deadband(event_t *event, void *data1, void *data2) {
    tag_type_union diff, db;

    switch(event->datatype) {
        case TYPE_BYTE:
            db.type_byte = *(type_byte *)event->data;
            diff.type_byte = MAX(*(type_byte *)data1, *(type_byte *)data2) - MIN(*(type_byte *)data1, *(type_byte *)data2);
            if(diff.type_byte >= db.type_byte) return 1;
            else return 0;
        case TYPE_SINT:
        case TYPE_CHAR:
            db.type_sint = *(type_sint *)event->data;
            diff.type_sint = *(type_sint *)data1 - *(type_sint *)data2;
            if(ABS(diff.type_sint) >= db.type_sint) return 1;
            else return 0;
        case TYPE_UINT:
        case TYPE_WORD:
            db.type_uint = *(type_uint *)event->data;
            diff.type_uint = MAX(*(type_uint *)data1,*(type_uint *)data2) - MIN(*(type_uint *)data1,*(type_uint *)data2);
            if(diff.type_uint >= db.type_uint) return 1;
            else return 0;
        case TYPE_INT:
            db.type_int = *(type_int *)event->data;
            diff.type_int = *(type_int *)data1 - *(type_int *)data2;
            if(ABS(diff.type_int) >= db.type_int) return 1;
            else return 0;
        case TYPE_UDINT:
        case TYPE_DWORD:
        case TYPE_TIME:
            db.type_udint = *(type_udint *)event->data;
            diff.type_udint = MAX(*(type_udint *)data1, *(type_udint *)data2) - MIN(*(type_udint *)data1, *(type_udint *)data2);
            if(diff.type_udint >= db.type_udint) return 1;
            else return 0;
        case TYPE_DINT:
            db.type_dint = *(type_dint *)event->data;
            diff.type_dint = *(type_dint *)data1 - *(type_dint *)data2;
            if(ABS(diff.type_dint) >= db.type_dint) return 1;
            else return 0;
        case TYPE_ULINT:
        case TYPE_LWORD:
            db.type_ulint = *(type_ulint *)event->data;
            diff.type_ulint = MAX(*(type_ulint *)data1, *(type_ulint *)data2) - MIN(*(type_ulint *)data1, *(type_ulint *)data2);
            if(diff.type_ulint >= db.type_ulint) return 1;
            else return 0;
        case TYPE_LINT:
            db.type_lint = *(type_lint *)event->data;
            diff.type_lint = *(type_lint *)data1 - *(type_lint *)data2;
            if(ABS(diff.type_lint) >= db.type_lint) return 1;
            else return 0;
        case TYPE_REAL:
            db.type_real = *(type_real *)event->data;
            diff.type_real = *(type_real *)data1 - *(type_real *)data2;
            if(ABS(diff.type_real) >= db.type_real) return 1;
            else return 0;
        case TYPE_LREAL:
            db.type_lreal = *(type_lreal *)event->data;
            diff.type_lreal = *(type_lreal *)data1 - *(type_lreal *)data2;
            if(ABS(diff.type_lreal) >= db.type_lreal) return 1;
            else return 0;
    }
    DF("OH CRAP, OH CRAP, OH CRAP!!!!!");
    return -2;
}


static inline int
_event_deadband(event_t *event, tag_t *tag, int offset, int size) {
    int n, inc, len, result;
    uint8_t *this, *that;
    result = 0;

    inc = TYPESIZE(event->datatype) / 8;
    this = (uint8_t *)event->test + MAX(0, offset - event->byte);
    that = (uint8_t *)&(tag->data[MAX(offset, event->byte)]);
    len = MIN(event->byte + event->size, offset + size) - MAX(offset, event->byte);

    for(n = 0; n < len; n += inc) {
        if(_generic_deadband(event, &this[n], &that[n])) {
            result = 1;
            memcpy(&this[n], &that[n], inc);
        }
    }
    return result;
}



/* This function is called when the area of data is affected by the write
 * and it determines whether the event should fire or not.  Return 1 if the
 * event hits and 0 otherwise.  There are no errors */
static int
_event_hit(event_t *event, tag_t *tag, int offset, int size) {
    switch(event->eventtype) {
        case EVENT_WRITE:
            return 1;
        case EVENT_CHANGE:
            return _event_change(event, tag, offset, size);
        case EVENT_SET:
            return _event_set(event, tag, offset, size);
        case EVENT_RESET:
            return _event_reset(event, tag, offset, size);
        case EVENT_EQUAL:
            return _event_compare(event, tag, offset, size, 0);
        case EVENT_GREATER:
            return _event_compare(event, tag, offset, size, -1);
        case EVENT_LESS:
            return _event_compare(event, tag, offset, size, 1);
        case EVENT_DEADBAND:
            return _event_deadband(event, tag, offset, size);
    }
    return 0;
}

/* This function checks to see if an event has occurred.  It should be
 * called from the tag_write() function or the tag_mask_write() function.
 * If it decides that there is an event match to the data area given then
 * it will call the send_event() function to send the event message to
 * the proper module. This function assumes that the events that are stored
 * with events that make sense so it does no checking.  There is no return type
 * because there are no possible errors, and no information to pass back. */
/* TODO: The event list should be sorted in increasing order of the bottom of
 * the range so that this function would not have to run through all of the
 * events.  For now we keep it simple. */
void
event_check(tag_t *tag, int offset, int size) {
    event_t *this;

    this = tag->events;

    pthread_mutex_unlock(&tag->lock);
    while(this != NULL) {
        /* This is to check whether the the data ranges intersect.  If this
         * test passes then we have manipulated the data associated with
         * this event. */
        if(offset <= (this->byte + this->size - 1) && (offset + size -1 ) >= this->byte) {
            if(this->inhibit == false && _event_hit(this, tag, offset, size)) {
                this->inhibit = true;
                this->notify(this->userdata);
                this->inhibit = false;
            }
        }
        this = this->next;
    }
    pthread_mutex_lock(&tag->lock);
    return;
}

/* This function checks to see if the tag has a deleted event.  This
 * should only be called from the tag_delete() function */
void
event_del_check(tag_t *tag) {
    event_t *this;

    this = tag->events;

    while(this != NULL) {
        /* Look for the tag delete event. */
        if(this->eventtype == EVENT_DELETED) {
            this->notify(this->userdata);
        }
        this = this->next;
    }
    return;
}

static inline int
_verify_event_type(tag_type ttype, int etype)
{
    /* All datatypes can use Write or Change */
    if(etype == EVENT_WRITE || etype == EVENT_CHANGE) {
        return 0;
    }
    /* Only BOOL can use Set and Reset */
    if(etype == EVENT_SET || etype == EVENT_RESET) {
        if(ttype == TYPE_BOOL) {
            return 0;
        } else {
            //logError(LOG_TAGBASE, "Only BOOL tags are allowed to use SET or RESET events");
            return -1;
        }
    }
    /* Booleans, Reals and Custom Datatypes Can't use Equal */
    if(etype == EVENT_EQUAL) {
        if(ttype == TYPE_REAL || ttype == TYPE_LREAL ||
           ttype == TYPE_BOOL || ttype >= TYPE_CUSTOM) {
            //logError(LOG_TAGBASE, "EQUAL event not allowed for BOOL, REAL, LREAL or Custom types");
            return -1;
        } else {
            return 0;
        }
    }
    /* At this point the only ones left are < > and deadband.  All
     * except Booleans and Custom datatypes can use these */
    if(ttype == TYPE_BOOL || ttype >= TYPE_CUSTOM) {
        //logError(LOG_TAGBASE, "GREATER, LESS and DEADBAND events not allowed for BOOL and Custom types");
        return -1;
    } else {
        return 0;
    }
    /* If we get here then we were given an unknown event type */
    //logError(LOG_TAGBASE, "Unknown datatype in new event");
    return -1;
}

/* This function figures out how big the *data and *test memory blocks need
 * to be in the event and assigns the proper data to them.  The *data area
 * is to store the data from the user that is associated with the event, such
 * as the number that the variables are supposed to be less than.  The *test
 * area is to store state data such as a bitmap to show the last values of
 * the tag to determine if a set event should be issued. It is assumed that
 * the events make sense at this point. Returns 0 on success and an error
 * code otherwise. */
static int
_set_event_data(tag_db_t *db, event_t *event, tag_t *tag, void *data) {
    int datasize = 0, testsize = 0, size;

    /* Figure out how much memory to allocate */
    switch(event->eventtype) {
        case EVENT_WRITE: /* We don't need any of this to detect write events */
            datasize = 0;
            testsize = 0;
            break;
        case EVENT_CHANGE:
            datasize = 0;
            if(event->datatype == TYPE_BOOL) {
                testsize = (event->count + event->bit - 1)/8 + 1;
            } else {
                testsize = tag_type_size(db, event->datatype) * event->count;
            }
            break;
        case EVENT_DEADBAND:
            size = tag_type_size(db, event->datatype);
            datasize = size;
            testsize = size * event->count;
            break;
        case EVENT_SET:
        case EVENT_RESET:
            datasize = 0;
            testsize = (event->count + event->bit - 1)/8 + 1;
            break;
        case EVENT_EQUAL:
        case EVENT_GREATER:
        case EVENT_LESS:
            datasize = tag_type_size(db, event->datatype);
            testsize = (event->count - 1)/8 + 1;
            break;
    }
    /* Allocate the memory that we need */
    if(datasize > 0) {
        event->data = malloc(datasize);
        if(event->data == NULL) {
            //logError(LOG_TAGBASE, "event_add() - Unable to allocate memory for event data");
            return ERR_ALLOC;
        }
    } else {
        event->data = NULL;
    }

    if(testsize > 0) {
        event->test = malloc(testsize);
        if(event->test == NULL) {
            if(event->data != NULL) free(event->data);
            //logError(LOG_TAGBASE, "event_add() - Unable to allocate memory for event test data");
            return ERR_ALLOC;
        }
    } else {
        event->test = NULL;
    }

    switch(event->eventtype) {
        case EVENT_WRITE:
           break;
        case EVENT_DEADBAND:
            /* Copy the existing tag data into the *data area */
            memcpy(event->data, data, datasize);
            memcpy(event->test, &tag->data[event->byte], testsize);
            break;
        case EVENT_CHANGE:
            memcpy(event->test, &tag->data[event->byte], testsize);
            break;
        case EVENT_SET:
        case EVENT_RESET:
            memcpy(event->test, &tag->data[event->byte], testsize);
            break;
        case EVENT_EQUAL:
        case EVENT_GREATER:
        case EVENT_LESS:
            memcpy(event->data, data, datasize);
            bzero(event->test, testsize);
            break;
    }


    return 0;
}

/* Add the event defined.  Return the event id. 'h' is a handle to the tag
 * data that the event is tied too.  'event_type' is the type of event (see
 * common.h for #defines.  'data' is any data that may need to be
 * passed such as values for deadband.  Returns the id of the event */
int
tag_event_add_handle(tag_db_t *db, tag_handle_t *h, int event_type, void *data, event_function f, void *udata) {
    event_t *head, *new;
    int result;

    /* No events can be assigned to virtual tags unless it's a queue */
    if(tag_is_virtual(h->ptr) && ! tag_is_queue(h->ptr)) {
        //logError(LOG_TAGBASE, "Events cannot be added to virtual tags");
        return ERR_ILLEGAL;
    }
    /* If the tag is a queue then we can only do write events for
     * the entire tag not a partial handle. */
    if(tag_is_queue(h->ptr)) {
        if(h->byte != 0 || event_type != EVENT_WRITE ||
           h->size != (tag_get_size(db, h->ptr) * h->ptr->count)) {
            //logError(LOG_TAGBASE, "Only write events can be assigned for a queue");
            return ERR_ILLEGAL;
        }
    }
    /* Bounds check size */
    if( (h->byte + h->size) > tag_get_size(db, h->ptr)) {
        //logError(LOG_TAGBASE, "Size of the affected data in the new event is too large");
        return ERR_2BIG;
    }
    if(_verify_event_type(h->type, event_type)) {
        /* error log handled in verify_event_type() function */
        return ERR_ARG;
    }
    /* If everything is okay then allocate the new event. */
    new = calloc(1, sizeof(event_t));
    if(new == NULL) {
        //logError(LOG_TAGBASE, "event_add() - Unable to allocate memory for new event");
        return ERR_ALLOC;
    }
    new->id = h->ptr->nextevent++;
    new->byte = h->byte;
    new->bit = h->bit;
    new->size = h->size;
    new->count = h->count;
    new->options = 0x0000;
    new->datatype = h->type;
    new->eventtype = event_type;
    new->notify = f;
    new->userdata = udata;
    new->inhibit = false;
    result = _set_event_data(db, new, h->ptr, data);
    if(result) {
        free(new);
        return result;
    }

    head = h->ptr->events;
    /* If the list is empty put it on top */
    if(head == NULL) {
        new->next = NULL;
        h->ptr->events = new;
        h->ptr->attr |= TAG_ATTR_EVENT; /* Add attribute flag to show we have at least one event */
    } else {
        /* For now we are not going to sort these events.  The right optimization
         * would be to sort by the bottom of the range.  This way the
         * _event_check() function could stop once the top of the updated
         * range is greater than the bottom of the range of the event. */
        new->next = h->ptr->events;
        h->ptr->events = new;
    }
    //logInfo(LOG_TAGBASE, "Event added index: %d, id: %d", h->index, new->id);
    return new->id;
}


/* Frees the memory associated with an event.  Pass a NULL pointer
 * and bad things will happen. */
static void
_free_event(event_t *event) {
    if(event->data != NULL) {
        free(event->data);
        event->data = NULL;
    }
    if(event->test != NULL) {
        free(event->test);
        event->test = NULL;
    }
    free(event);
}

static int
_find_event(event_t **event, tag_t *tag, int id) {
	event_t *this;

    this = tag->events;
    if(this == NULL) return ERR_NOTFOUND;
    while(this->id != id) {
    	if(this->next == NULL) return ERR_NOTFOUND;
    	this = this->next;
    }
    *event = this;
	return 0;
}

int
tag_event_del(tag_t *tag, int id) {
    event_t *this, *last;
    int result = ERR_NOTFOUND;

    this = tag->events;
    if(this == NULL) return ERR_NOTFOUND;
    if(this->id == id) {
        tag->events = this->next;
        _free_event(this);
        result = ERR_OK;
    } else {
        last = this;
        this = this->next;
        while(this != NULL) {
            if(this->id == id) {
                last->next = this->next;
                _free_event(this);
                result = ERR_OK;
                break; /* Uggg */
            }
            last = this;
            this = this->next;
        }
    }
    /* If there are no more events on this tag then reset the attibute flag */
    if(tag->events == NULL) {
        tag->attr &= ~TAG_ATTR_EVENT;
    }
    //logInfo(LOG_TAGBASE, "Deleted event index: %d, id: %d", index, id);
    return result;
}

/* Traverse the linked list of events and delete them all */
int
events_del_all(event_t *head) {
    event_t *this, *next;
    this = head;
    while(this != NULL) {
        next = this->next;
        _free_event(this);
        this = next;
    }
    return 0;
}

int
tag_event_opt(tag_t *tag, int id, uint32_t options) {
    event_t *event;
    int result;

    result = _find_event(&event, tag, id);
    if(result) {
        //logError(LOG_TAGBASE, "event_opt() - event not found");
        return result;
    }

    event->options = options;
    return 0;
}

/* Returns the userdata from the given event.  Returns NULL on
   error and if res is not NULL any errors will be written there.
   It is possible that the userdata itself is NULL and this can
   be used to differentiate errors from null userdata */
void *
tag_event_get_userdata(tag_t *tag, int id, int *res) {
    event_t *event;
    int result;

    result = _find_event(&event, tag, id);
    if(result) {
        //logError(LOG_TAGBASE, "event_opt() - event not found");
        if(res != NULL) {
            *res = result;
        }
        return NULL;
    }
    if(res != NULL) {
        *res = ERR_OK;
    }
    return event->userdata;
}
