
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
test_byte(void) {
    int result;
    type_byte test;
    int id;

    xassert(tag_add(db, "test_byte", TYPE_BYTE, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_byte", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 127;
    result = tag_write(db, "test_byte", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_byte", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 127;
    result = tag_write(db, "test_byte", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 127 + (5 / 2);
    result = tag_write(db, "test_byte", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 127 - (5 / 2);
    result = tag_write(db, "test_byte", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 127;
    result = tag_write(db, "test_byte", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 127 + 5 + 0;
    result = tag_write(db, "test_byte", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 127 + 5 + 5 / 2;
    result = tag_write(db, "test_byte", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 127 + 5 * 2 + 0;
    result = tag_write(db, "test_byte", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


void
test_sint(void) {
    int result;
    type_sint test;
    int id;

    xassert(tag_add(db, "test_sint", TYPE_SINT, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_sint", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 0;
    result = tag_write(db, "test_sint", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_sint", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 0;
    result = tag_write(db, "test_sint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 0 + (5 / 2);
    result = tag_write(db, "test_sint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 0 - (5 / 2);
    result = tag_write(db, "test_sint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 0;
    result = tag_write(db, "test_sint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 0 + 5 + 0;
    result = tag_write(db, "test_sint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 0 + 5 + 5 / 2;
    result = tag_write(db, "test_sint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 0 + 5 * 2 + 0;
    result = tag_write(db, "test_sint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


void
test_int(void) {
    int result;
    type_int test;
    int id;

    xassert(tag_add(db, "test_int", TYPE_INT, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_int", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 0;
    result = tag_write(db, "test_int", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_int", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 0;
    result = tag_write(db, "test_int", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 0 + (5 / 2);
    result = tag_write(db, "test_int", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 0 - (5 / 2);
    result = tag_write(db, "test_int", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 0;
    result = tag_write(db, "test_int", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 0 + 5 + 0;
    result = tag_write(db, "test_int", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 0 + 5 + 5 / 2;
    result = tag_write(db, "test_int", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 0 + 5 * 2 + 0;
    result = tag_write(db, "test_int", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


void
test_uint(void) {
    int result;
    type_uint test;
    int id;

    xassert(tag_add(db, "test_uint", TYPE_UINT, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_uint", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 32767;
    result = tag_write(db, "test_uint", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_uint", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 32767;
    result = tag_write(db, "test_uint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 32767 + (5 / 2);
    result = tag_write(db, "test_uint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 32767 - (5 / 2);
    result = tag_write(db, "test_uint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 32767;
    result = tag_write(db, "test_uint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 32767 + 5 + 0;
    result = tag_write(db, "test_uint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 32767 + 5 + 5 / 2;
    result = tag_write(db, "test_uint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 32767 + 5 * 2 + 0;
    result = tag_write(db, "test_uint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


void
test_dint(void) {
    int result;
    type_dint test;
    int id;

    xassert(tag_add(db, "test_dint", TYPE_DINT, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_dint", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 0;
    result = tag_write(db, "test_dint", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_dint", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 0;
    result = tag_write(db, "test_dint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 0 + (5 / 2);
    result = tag_write(db, "test_dint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 0 - (5 / 2);
    result = tag_write(db, "test_dint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 0;
    result = tag_write(db, "test_dint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 0 + 5 + 0;
    result = tag_write(db, "test_dint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 0 + 5 + 5 / 2;
    result = tag_write(db, "test_dint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 0 + 5 * 2 + 0;
    result = tag_write(db, "test_dint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


void
test_udint(void) {
    int result;
    type_udint test;
    int id;

    xassert(tag_add(db, "test_udint", TYPE_UDINT, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_udint", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 2147483647UL;
    result = tag_write(db, "test_udint", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_udint", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 2147483647UL;
    result = tag_write(db, "test_udint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 2147483647UL + (5 / 2);
    result = tag_write(db, "test_udint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 2147483647UL - (5 / 2);
    result = tag_write(db, "test_udint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 2147483647UL;
    result = tag_write(db, "test_udint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 2147483647UL + 5 + 0;
    result = tag_write(db, "test_udint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 2147483647UL + 5 + 5 / 2;
    result = tag_write(db, "test_udint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 2147483647UL + 5 * 2 + 0;
    result = tag_write(db, "test_udint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


void
test_lint(void) {
    int result;
    type_lint test;
    int id;

    xassert(tag_add(db, "test_lint", TYPE_LINT, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_lint", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 0LL;
    result = tag_write(db, "test_lint", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_lint", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 0LL;
    result = tag_write(db, "test_lint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 0LL + (5 / 2);
    result = tag_write(db, "test_lint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 0LL - (5 / 2);
    result = tag_write(db, "test_lint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 0LL;
    result = tag_write(db, "test_lint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 0LL + 5 + 0;
    result = tag_write(db, "test_lint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 0LL + 5 + 5 / 2;
    result = tag_write(db, "test_lint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 0LL + 5 * 2 + 0;
    result = tag_write(db, "test_lint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


void
test_ulint(void) {
    int result;
    type_ulint test;
    int id;

    xassert(tag_add(db, "test_ulint", TYPE_ULINT, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_ulint", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 9223372036854775807ULL;
    result = tag_write(db, "test_ulint", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_ulint", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 9223372036854775807ULL;
    result = tag_write(db, "test_ulint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 9223372036854775807ULL + (5 / 2);
    result = tag_write(db, "test_ulint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 9223372036854775807ULL - (5 / 2);
    result = tag_write(db, "test_ulint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 9223372036854775807ULL;
    result = tag_write(db, "test_ulint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 9223372036854775807ULL + 5 + 0;
    result = tag_write(db, "test_ulint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 9223372036854775807ULL + 5 + 5 / 2;
    result = tag_write(db, "test_ulint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 9223372036854775807ULL + 5 * 2 + 0;
    result = tag_write(db, "test_ulint", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


void
test_real(void) {
    int result;
    type_real test;
    int id;

    xassert(tag_add(db, "test_real", TYPE_REAL, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_real", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 3.141592;
    result = tag_write(db, "test_real", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_real", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 3.141592;
    result = tag_write(db, "test_real", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 3.141592 + (5 / 2);
    result = tag_write(db, "test_real", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 3.141592 - (5 / 2);
    result = tag_write(db, "test_real", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 3.141592;
    result = tag_write(db, "test_real", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 3.141592 + 5 + 0.00001;
    result = tag_write(db, "test_real", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 3.141592 + 5 + 5 / 2;
    result = tag_write(db, "test_real", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 3.141592 + 5 * 2 + 0.00001;
    result = tag_write(db, "test_real", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


void
test_lreal(void) {
    int result;
    type_lreal test;
    int id;

    xassert(tag_add(db, "test_lreal", TYPE_LREAL, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_lreal", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = 3.141592;
    result = tag_write(db, "test_lreal", 0, &test);
    xassert(result == 0);

    test = 5; /* This will be the increment */
    id = tag_event_add(db, "test_lreal", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = 3.141592;
    result = tag_write(db, "test_lreal", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = 3.141592 + (5 / 2);
    result = tag_write(db, "test_lreal", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = 3.141592 - (5 / 2);
    result = tag_write(db, "test_lreal", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = 3.141592;
    result = tag_write(db, "test_lreal", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = 3.141592 + 5 + 0.00001;
    result = tag_write(db, "test_lreal", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = 3.141592 + 5 + 5 / 2;
    result = tag_write(db, "test_lreal", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = 3.141592 + 5 * 2 + 0.00001;
    result = tag_write(db, "test_lreal", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}


int
main(int argc, char *argv[]) {
    db = new_tagbase();
    xassert(db != NULL);

    test_byte();
    test_sint();
    test_int();
    test_uint();
    test_dint();
    test_udint();
    test_lint();
    test_ulint();
    test_real();
    test_lreal();

    exit(0);
}

