
#include <tagbase.h>
#include <testing.h>

static int check_value;
tag_db_t *db;

static void
_event_callback(void *data) {
    TP("Event called - %ld", (long int)data);
    check_value = (long int)data;
}


void
single_test_byte(void) {
    int result;
    type_byte test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_byte", TYPE_BYTE, 1, 0, NULL);
    xassert(tag != NULL);
    test = 127;

    id[0] = tag_event_add(db, "test_byte", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test_byte", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test_byte", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 127 - 1;
    tag_write(db, "test_byte", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 127 + 1;
    tag_write(db, "test_byte", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 127;
    tag_write(db, "test_byte", 0, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_byte");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 127 - 1;
    tag_write(db, "test_byte", 0, &test);
    xassert(check_value == 0x00);

    test = 127 + 1;
    tag_write(db, "test_byte", 0, &test);
    xassert(check_value == 0x00);

    test = 127;
    tag_write(db, "test_byte", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_byte(void) {
    int result;
    type_byte test;
    //tag_handle_t h;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_byte", TYPE_BYTE, 10, 0, NULL);
    xassert(tag != NULL);
    test = 127;

    id[0] = tag_event_add(db, "test2_byte[5]", 1, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_byte[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_byte[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 127 - 1;
    tag_write(db, "test2_byte[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 127 + 1;
    tag_write(db, "test2_byte[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 127;
    tag_write(db, "test2_byte[5]", 1, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 127 - 1;
    tag_write(db, "test2_byte[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 127 + 1;
    tag_write(db, "test2_byte[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 127;
    tag_write(db, "test2_byte[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 127 - 1;
    tag_write(db, "test2_byte[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 127 + 1;
    tag_write(db, "test2_byte[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 127;
    tag_write(db, "test2_byte[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_byte");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 127 - 1;
    tag_write(db, "test2_byte[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 127 + 1;
    tag_write(db, "test2_byte[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 127;
    tag_write(db, "test2_byte[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 127;

    id[0] = tag_event_add(db, "test2_byte", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_byte", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_byte", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 127 - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_byte[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 127 + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_byte[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 127;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_byte[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_EQUAL);
        check_value = 0x00;
        tag_write(db, s, 1, &test);  // Repeat and make sure the event does not fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


void
single_test_sint(void) {
    int result;
    type_sint test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_sint", TYPE_SINT, 1, 0, NULL);
    xassert(tag != NULL);
    test = 0;

    id[0] = tag_event_add(db, "test_sint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test_sint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test_sint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 0 - 1;
    tag_write(db, "test_sint", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 0 + 1;
    tag_write(db, "test_sint", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 0;
    tag_write(db, "test_sint", 0, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_sint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 0 - 1;
    tag_write(db, "test_sint", 0, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test_sint", 0, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test_sint", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_sint(void) {
    int result;
    type_sint test;
    //tag_handle_t h;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_sint", TYPE_SINT, 10, 0, NULL);
    xassert(tag != NULL);
    test = 0;

    id[0] = tag_event_add(db, "test2_sint[5]", 1, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_sint[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_sint[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 0 - 1;
    tag_write(db, "test2_sint[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 0 + 1;
    tag_write(db, "test2_sint[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 0;
    tag_write(db, "test2_sint[5]", 1, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 0 - 1;
    tag_write(db, "test2_sint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test2_sint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test2_sint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 - 1;
    tag_write(db, "test2_sint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test2_sint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test2_sint[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_sint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 0 - 1;
    tag_write(db, "test2_sint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test2_sint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test2_sint[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 0;

    id[0] = tag_event_add(db, "test2_sint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_sint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_sint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 0 - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_sint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 0 + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_sint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 0;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_sint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_EQUAL);
        check_value = 0x00;
        tag_write(db, s, 1, &test);  // Repeat and make sure the event does not fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


void
single_test_int(void) {
    int result;
    type_int test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_int", TYPE_INT, 1, 0, NULL);
    xassert(tag != NULL);
    test = 0;

    id[0] = tag_event_add(db, "test_int", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test_int", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test_int", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 0 - 1;
    tag_write(db, "test_int", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 0 + 1;
    tag_write(db, "test_int", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 0;
    tag_write(db, "test_int", 0, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_int");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 0 - 1;
    tag_write(db, "test_int", 0, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test_int", 0, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test_int", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_int(void) {
    int result;
    type_int test;
    //tag_handle_t h;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_int", TYPE_INT, 10, 0, NULL);
    xassert(tag != NULL);
    test = 0;

    id[0] = tag_event_add(db, "test2_int[5]", 1, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_int[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_int[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 0 - 1;
    tag_write(db, "test2_int[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 0 + 1;
    tag_write(db, "test2_int[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 0;
    tag_write(db, "test2_int[5]", 1, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 0 - 1;
    tag_write(db, "test2_int[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test2_int[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test2_int[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 - 1;
    tag_write(db, "test2_int[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test2_int[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test2_int[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_int");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 0 - 1;
    tag_write(db, "test2_int[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test2_int[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test2_int[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 0;

    id[0] = tag_event_add(db, "test2_int", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_int", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_int", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 0 - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_int[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 0 + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_int[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 0;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_int[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_EQUAL);
        check_value = 0x00;
        tag_write(db, s, 1, &test);  // Repeat and make sure the event does not fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


void
single_test_uint(void) {
    int result;
    type_uint test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_uint", TYPE_UINT, 1, 0, NULL);
    xassert(tag != NULL);
    test = 32767;

    id[0] = tag_event_add(db, "test_uint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test_uint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test_uint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 32767 - 1;
    tag_write(db, "test_uint", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 32767 + 1;
    tag_write(db, "test_uint", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 32767;
    tag_write(db, "test_uint", 0, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_uint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 32767 - 1;
    tag_write(db, "test_uint", 0, &test);
    xassert(check_value == 0x00);

    test = 32767 + 1;
    tag_write(db, "test_uint", 0, &test);
    xassert(check_value == 0x00);

    test = 32767;
    tag_write(db, "test_uint", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_uint(void) {
    int result;
    type_uint test;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_uint", TYPE_UINT, 10, 0, NULL);
    xassert(tag != NULL);
    test = 32767;

    id[0] = tag_event_add(db, "test2_uint[5]", 1, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_uint[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_uint[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 32767 - 1;
    tag_write(db, "test2_uint[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 32767 + 1;
    tag_write(db, "test2_uint[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 32767;
    tag_write(db, "test2_uint[5]", 1, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 32767 - 1;
    tag_write(db, "test2_uint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 32767 + 1;
    tag_write(db, "test2_uint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 32767;
    tag_write(db, "test2_uint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 32767 - 1;
    tag_write(db, "test2_uint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 32767 + 1;
    tag_write(db, "test2_uint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 32767;
    tag_write(db, "test2_uint[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_uint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 32767 - 1;
    tag_write(db, "test2_uint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 32767 + 1;
    tag_write(db, "test2_uint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 32767;
    tag_write(db, "test2_uint[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 32767;

    id[0] = tag_event_add(db, "test2_uint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_uint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_uint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 32767 - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_uint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 32767 + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_uint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 32767;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_uint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_EQUAL);
        check_value = 0x00;
        tag_write(db, s, 1, &test);  // Repeat and make sure the event does not fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


void
single_test_dint(void) {
    int result;
    type_dint test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_dint", TYPE_DINT, 1, 0, NULL);
    xassert(tag != NULL);
    test = 0;

    id[0] = tag_event_add(db, "test_dint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test_dint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test_dint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 0 - 1;
    tag_write(db, "test_dint", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 0 + 1;
    tag_write(db, "test_dint", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 0;
    tag_write(db, "test_dint", 0, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_dint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 0 - 1;
    tag_write(db, "test_dint", 0, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test_dint", 0, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test_dint", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_dint(void) {
    int result;
    type_dint test;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_dint", TYPE_DINT, 10, 0, NULL);
    xassert(tag != NULL);
    test = 0;

    id[0] = tag_event_add(db, "test2_dint[5]", 1, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_dint[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_dint[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 0 - 1;
    tag_write(db, "test2_dint[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 0 + 1;
    tag_write(db, "test2_dint[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 0;
    tag_write(db, "test2_dint[5]", 1, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 0 - 1;
    tag_write(db, "test2_dint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test2_dint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test2_dint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 - 1;
    tag_write(db, "test2_dint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test2_dint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test2_dint[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_dint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 0 - 1;
    tag_write(db, "test2_dint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 0 + 1;
    tag_write(db, "test2_dint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 0;
    tag_write(db, "test2_dint[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 0;

    id[0] = tag_event_add(db, "test2_dint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_dint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_dint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 0 - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_dint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 0 + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_dint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 0;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_dint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_EQUAL);
        check_value = 0x00;
        tag_write(db, s, 1, &test);  // Repeat and make sure the event does not fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


void
single_test_udint(void) {
    int result;
    type_udint test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_udint", TYPE_UDINT, 1, 0, NULL);
    xassert(tag != NULL);
    test = 2147483647UL;

    id[0] = tag_event_add(db, "test_udint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test_udint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test_udint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 2147483647UL - 1;
    tag_write(db, "test_udint", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 2147483647UL + 1;
    tag_write(db, "test_udint", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 2147483647UL;
    tag_write(db, "test_udint", 0, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_udint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 2147483647UL - 1;
    tag_write(db, "test_udint", 0, &test);
    xassert(check_value == 0x00);

    test = 2147483647UL + 1;
    tag_write(db, "test_udint", 0, &test);
    xassert(check_value == 0x00);

    test = 2147483647UL;
    tag_write(db, "test_udint", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_udint(void) {
    int result;
    type_udint test;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_udint", TYPE_UDINT, 10, 0, NULL);
    xassert(tag != NULL);
    test = 2147483647UL;

    id[0] = tag_event_add(db, "test2_udint[5]", 1, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_udint[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_udint[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 2147483647UL - 1;
    tag_write(db, "test2_udint[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 2147483647UL + 1;
    tag_write(db, "test2_udint[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 2147483647UL;
    tag_write(db, "test2_udint[5]", 1, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 2147483647UL - 1;
    tag_write(db, "test2_udint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 2147483647UL + 1;
    tag_write(db, "test2_udint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 2147483647UL;
    tag_write(db, "test2_udint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 2147483647UL - 1;
    tag_write(db, "test2_udint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 2147483647UL + 1;
    tag_write(db, "test2_udint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 2147483647UL;
    tag_write(db, "test2_udint[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_udint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 2147483647UL - 1;
    tag_write(db, "test2_udint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 2147483647UL + 1;
    tag_write(db, "test2_udint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 2147483647UL;
    tag_write(db, "test2_udint[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 2147483647UL;

    id[0] = tag_event_add(db, "test2_udint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_udint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_udint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 2147483647UL - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_udint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 2147483647UL + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_udint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 2147483647UL;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_udint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_EQUAL);
        check_value = 0x00;
        tag_write(db, s, 1, &test);  // Repeat and make sure the event does not fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


void
single_test_lint(void) {
    int result;
    type_lint test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_lint", TYPE_LINT, 1, 0, NULL);
    xassert(tag != NULL);
    test = 0LL;

    id[0] = tag_event_add(db, "test_lint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test_lint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test_lint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 0LL - 1;
    tag_write(db, "test_lint", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 0LL + 1;
    tag_write(db, "test_lint", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 0LL;
    tag_write(db, "test_lint", 0, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_lint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 0LL - 1;
    tag_write(db, "test_lint", 0, &test);
    xassert(check_value == 0x00);

    test = 0LL + 1;
    tag_write(db, "test_lint", 0, &test);
    xassert(check_value == 0x00);

    test = 0LL;
    tag_write(db, "test_lint", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_lint(void) {
    int result;
    type_lint test;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_lint", TYPE_LINT, 10, 0, NULL);
    xassert(tag != NULL);
    test = 0LL;

    id[0] = tag_event_add(db, "test2_lint[5]", 1, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_lint[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_lint[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 0LL - 1;
    tag_write(db, "test2_lint[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 0LL + 1;
    tag_write(db, "test2_lint[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 0LL;
    tag_write(db, "test2_lint[5]", 1, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 0LL - 1;
    tag_write(db, "test2_lint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0LL + 1;
    tag_write(db, "test2_lint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0LL;
    tag_write(db, "test2_lint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 0LL - 1;
    tag_write(db, "test2_lint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 0LL + 1;
    tag_write(db, "test2_lint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 0LL;
    tag_write(db, "test2_lint[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_lint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 0LL - 1;
    tag_write(db, "test2_lint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 0LL + 1;
    tag_write(db, "test2_lint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 0LL;
    tag_write(db, "test2_lint[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 0LL;

    id[0] = tag_event_add(db, "test2_lint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_lint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_lint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 0LL - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_lint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 0LL + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_lint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 0LL;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_lint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_EQUAL);
        check_value = 0x00;
        tag_write(db, s, 1, &test);  // Repeat and make sure the event does not fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


void
single_test_ulint(void) {
    int result;
    type_ulint test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_ulint", TYPE_ULINT, 1, 0, NULL);
    xassert(tag != NULL);
    test = 9223372036854775807ULL;

    id[0] = tag_event_add(db, "test_ulint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test_ulint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test_ulint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 9223372036854775807ULL - 1;
    tag_write(db, "test_ulint", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 9223372036854775807ULL + 1;
    tag_write(db, "test_ulint", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 9223372036854775807ULL;
    tag_write(db, "test_ulint", 0, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_ulint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 9223372036854775807ULL - 1;
    tag_write(db, "test_ulint", 0, &test);
    xassert(check_value == 0x00);

    test = 9223372036854775807ULL + 1;
    tag_write(db, "test_ulint", 0, &test);
    xassert(check_value == 0x00);

    test = 9223372036854775807ULL;
    tag_write(db, "test_ulint", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_ulint(void) {
    int result;
    type_ulint test;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_ulint", TYPE_ULINT, 10, 0, NULL);
    xassert(tag != NULL);
    test = 9223372036854775807ULL;

    id[0] = tag_event_add(db, "test2_ulint[5]", 1, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_ulint[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_ulint[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 9223372036854775807ULL - 1;
    tag_write(db, "test2_ulint[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 9223372036854775807ULL + 1;
    tag_write(db, "test2_ulint[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = 9223372036854775807ULL;
    tag_write(db, "test2_ulint[5]", 1, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 9223372036854775807ULL - 1;
    tag_write(db, "test2_ulint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 9223372036854775807ULL + 1;
    tag_write(db, "test2_ulint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 9223372036854775807ULL;
    tag_write(db, "test2_ulint[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 9223372036854775807ULL - 1;
    tag_write(db, "test2_ulint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 9223372036854775807ULL + 1;
    tag_write(db, "test2_ulint[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 9223372036854775807ULL;
    tag_write(db, "test2_ulint[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_ulint");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 9223372036854775807ULL - 1;
    tag_write(db, "test2_ulint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 9223372036854775807ULL + 1;
    tag_write(db, "test2_ulint[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 9223372036854775807ULL;
    tag_write(db, "test2_ulint[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 9223372036854775807ULL;

    id[0] = tag_event_add(db, "test2_ulint", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_ulint", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_ulint", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = 9223372036854775807ULL - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_ulint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 9223372036854775807ULL + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_ulint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 9223372036854775807ULL;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_ulint[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_EQUAL);
        check_value = 0x00;
        tag_write(db, s, 1, &test);  // Repeat and make sure the event does not fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


void
single_test_real(void) {
    int result;
    type_real test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_real", TYPE_REAL, 1, 0, NULL);
    xassert(tag != NULL);
    test = 3.141592;

    id[0] = tag_event_add(db, "test_real", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[1] = tag_event_add(db, "test_real", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<2;n++) {
        xassert(id[n] > 0);
    }

    test = 3.141592 - 1;
    tag_write(db, "test_real", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 3.141592 + 1;
    tag_write(db, "test_real", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_real");
    for(int n=0;n<2;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 3.141592 - 1;
    tag_write(db, "test_real", 0, &test);
    xassert(check_value == 0x00);

    test = 3.141592 + 1;
    tag_write(db, "test_real", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_real(void) {
    int result;
    type_real test;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_real", TYPE_REAL, 10, 0, NULL);
    xassert(tag != NULL);
    test = 3.141592;
    id[0] = tag_event_add(db, "test2_real[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[1] = tag_event_add(db, "test2_real[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<2;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 3.141592 - 1;
    tag_write(db, "test2_real[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 3.141592 + 1;
    tag_write(db, "test2_real[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 3.141592 - 1;
    tag_write(db, "test2_real[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 3.141592 + 1;
    tag_write(db, "test2_real[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 3.141592 - 1;
    tag_write(db, "test2_real[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 3.141592 + 1;
    tag_write(db, "test2_real[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_real");
    for(int n=0;n<2;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 3.141592 - 1;
    tag_write(db, "test2_real[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 3.141592 + 1;
    tag_write(db, "test2_real[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 3.141592;

    id[0] = tag_event_add(db, "test2_real", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[1] = tag_event_add(db, "test2_real", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<2;n++) {
        xassert(id[n] > 0);
    }

    test = 3.141592 - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_real[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 3.141592 + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_real[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


void
single_test_lreal(void) {
    int result;
    type_lreal test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_lreal", TYPE_LREAL, 1, 0, NULL);
    xassert(tag != NULL);
    test = 3.141592;
    id[0] = tag_event_add(db, "test_lreal", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[1] = tag_event_add(db, "test_lreal", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<2;n++) {
        xassert(id[n] > 0);
    }

    test = 3.141592 - 1;
    tag_write(db, "test_lreal", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 3.141592 + 1;
    tag_write(db, "test_lreal", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_lreal");
    for(int n=0;n<2;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = 3.141592 - 1;
    tag_write(db, "test_lreal", 0, &test);
    xassert(check_value == 0x00);

    test = 3.141592 + 1;
    tag_write(db, "test_lreal", 0, &test);
    xassert(check_value == 0x00);
}


void
array_test_lreal(void) {
    int result;
    type_lreal test;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_lreal", TYPE_LREAL, 10, 0, NULL);
    xassert(tag != NULL);
    test = 3.141592;

    id[0] = tag_event_add(db, "test2_lreal[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[1] = tag_event_add(db, "test2_lreal[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<2;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = 3.141592 - 1;
    tag_write(db, "test2_lreal[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = 3.141592 + 1;
    tag_write(db, "test2_lreal[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    test = 3.141592 - 1;
    tag_write(db, "test2_lreal[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 3.141592 + 1;
    tag_write(db, "test2_lreal[4]", 1, &test);
    xassert(check_value == 0x00);

    test = 3.141592 - 1;
    tag_write(db, "test2_lreal[6]", 1, &test);
    xassert(check_value == 0x00);

    test = 3.141592 + 1;
    tag_write(db, "test2_lreal[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_lreal");
    for(int n=0;n<2;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = 3.141592 - 1;
    tag_write(db, "test2_lreal[5]", 1, &test);
    xassert(check_value == 0x00);

    test = 3.141592 + 1;
    tag_write(db, "test2_lreal[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = 3.141592;

    id[0] = tag_event_add(db, "test2_lreal", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[1] = tag_event_add(db, "test2_lreal", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<2;n++) {
        xassert(id[n] > 0);
    }

    test = 3.141592 - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_lreal[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = 3.141592 + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_lreal[%d]", n);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}


int
main(int argc, char *argv[]) {

    db = new_tagbase();

    xassert(db != NULL);

    single_test_byte();
    array_test_byte();
    single_test_sint();
    array_test_sint();
    single_test_int();
    array_test_int();
    single_test_uint();
    array_test_uint();
    single_test_dint();
    array_test_dint();
    single_test_udint();
    array_test_udint();
    single_test_lint();
    array_test_lint();
    single_test_ulint();
    array_test_ulint();
    single_test_real();
    array_test_real();
    single_test_lreal();
    array_test_lreal();

    exit(0);
}

