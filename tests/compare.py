#!/usr/bin/python3

# This is a Python script to generate C code tests for the three compare
# event types.  This should be run and redirected to event_compare_tests.c
# It is primarily for int data types since real data types don't allow the
# equal event.  The lines are commented out that generate these tests but they
# will fail when the EQUAL event is created.

single_test_function = """
void
single_test_{type}(void) {
    int result;
    type_{type} test;
    int id[3];
    tag_t *tag;

    tag = tag_add(db, "test_{type}", TYPE_{TYPE}, 1, 0, NULL);
    xassert(tag != NULL);
    test = {val};
    //result = tag_gethandle(&h, "test_{type}", 0);
    //xassert(result == 0);
    id[0] = tag_event_add(db, "test_{type}", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test_{type}", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test_{type}", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = {val} - 1;
    tag_write(db, "test_{type}", 0, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = {val} + 1;
    tag_write(db, "test_{type}", 0, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = {val};
    tag_write(db, "test_{type}", 0, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test_{type}");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure the events are deleted */
    test = {val} - 1;
    tag_write(db, "test_{type}", 0, &test);
    xassert(check_value == 0x00);

    test = {val} + 1;
    tag_write(db, "test_{type}", 0, &test);
    xassert(check_value == 0x00);

    test = {val};
    tag_write(db, "test_{type}", 0, &test);
    xassert(check_value == 0x00);
}
"""


array_test_function = """
void
array_test_{type}(void) {
    int result;
    type_{type} test;
    //tag_handle_t h;
    int id[3];
    char s[128];
    tag_t *tag;

    tag = tag_add(db, "test2_{type}", TYPE_{TYPE}, 10, 0, NULL);
    xassert(tag != NULL);
    test = {val};
    //result = tag_gethandle(&h, "test2_{type}[5]", 1);
    //xassert(result == 0);
    id[0] = tag_event_add(db, "test2_{type}[5]", 1, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_{type}[5]", 1, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_{type}[5]", 1, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }
    /* Check that the event fires like we'd think */
    test = {val} - 1;
    tag_write(db, "test2_{type}[5]", 1, &test);
    xassert(check_value == EVENT_LESS);
    check_value = 0x00;

    test = {val} + 1;
    tag_write(db, "test2_{type}[5]", 1, &test);
    xassert(check_value == EVENT_GREATER);
    check_value = 0x00;

    test = {val};
    tag_write(db, "test2_{type}[5]", 1, &test);
    xassert(check_value == EVENT_EQUAL);
    check_value = 0x00;

    /* Make sure that it does not fire with adjacent values */
    //result = tag_gethandle(&h, "test2_{type}[4]", 1);

    test = {val} - 1;
    tag_write(db, "test2_{type}[4]", 1, &test);
    xassert(check_value == 0x00);

    test = {val} + 1;
    tag_write(db, "test2_{type}[4]", 1, &test);
    xassert(check_value == 0x00);

    test = {val};
    tag_write(db, "test2_{type}[4]", 1, &test);
    xassert(check_value == 0x00);

    //result = tag_gethandle(&h, "test2_{type}[6]", 1);

    test = {val} - 1;
    tag_write(db, "test2_{type}[6]", 1, &test);
    xassert(check_value == 0x00);

    test = {val} + 1;
    tag_write(db, "test2_{type}[6]", 1, &test);
    xassert(check_value == 0x00);

    test = {val};
    tag_write(db, "test2_{type}[6]", 1, &test);
    xassert(check_value == 0x00);

    /* Delete all of these events */
    tag = tag_get_by_name(db, "test2_{type}");
    for(int n=0;n<3;n++) {
        result = tag_event_del(tag, id[n]);
        xassert(result == 0);
    }
    /* Make sure they are deleted */
    check_value = 0x00;
    test = {val} - 1;
    tag_write(db, "test2_{type}[5]", 1, &test);
    xassert(check_value == 0x00);

    test = {val} + 1;
    tag_write(db, "test2_{type}[5]", 1, &test);
    xassert(check_value == 0x00);

    test = {val};
    tag_write(db, "test2_{type}[5]", 1, &test);
    xassert(check_value == 0x00);

    /* Now we set the compare events to the whole tag array */
    test = {val};
    //result = tag_gethandle(&h, "test2_{type}", 0);
    //xassert(result == 0);
    id[0] = tag_event_add(db, "test2_{type}", 0, EVENT_EQUAL, &test, _event_callback, (void *)EVENT_EQUAL);
    id[1] = tag_event_add(db, "test2_{type}", 0, EVENT_GREATER, &test, _event_callback, (void *)EVENT_GREATER);
    id[2] = tag_event_add(db, "test2_{type}", 0, EVENT_LESS, &test, _event_callback, (void *)EVENT_LESS);
    for(int n=0;n<3;n++) {
        xassert(id[n] > 0);
    }

    test = {val} - 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_{type}[%d]", n);
        //tag_gethandle(&h, s, 1);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_LESS);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = {val} + 1;
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_{type}[%d]", n);
        //tag_gethandle(&h, s, 1);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_GREATER);
        check_value = 0x00;
        tag_write(db, s, 1, &test); // Make sure it doesn't fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }

    test = {val};
    for(int n=0;n<10;n++) {
        sprintf(s, "test2_{type}[%d]", n);
        //tag_gethandle(&h, s, 1);
        tag_write(db, s, 1, &test);
        xassert(check_value == EVENT_EQUAL);
        check_value = 0x00;
        tag_write(db, s, 1, &test);  // Repeat and make sure the event does not fire again
        xassert(check_value == 0x00);
        check_value = 0x00;
    }
}
"""

tests = []

tests.append({'{type}': 'byte', '{TYPE}': 'BYTE', '{val}': '127'})
tests.append({'{type}': 'sint', '{TYPE}': 'SINT', '{val}': '0'})
tests.append({'{type}': 'int', '{TYPE}': 'INT', '{val}': '0'})
tests.append({'{type}': 'uint', '{TYPE}': 'UINT', '{val}': '32767'})
tests.append({'{type}': 'dint', '{TYPE}': 'DINT', '{val}': '0'})
tests.append({'{type}': 'udint', '{TYPE}': 'UDINT', '{val}': '2147483647UL'})
tests.append({'{type}': 'lint', '{TYPE}': 'LINT', '{val}': '0LL'})
tests.append({'{type}': 'ulint', '{TYPE}': 'ULINT', '{val}': '9223372036854775807ULL'})
tests.append({'{type}': 'real', '{TYPE}': 'REAL', '{val}': '3.141592'})
tests.append({'{type}': 'lreal', '{TYPE}': 'LREAL', '{val}': '3.141592'})

header = """
#include <tagbase.h>
#include <testing.h>

static int check_value;
tag_db_t *db;

static void
_event_callback(void *data) {
    TP("Event called - %ld", (long int)data);
    check_value = (long int)data;
}
"""

print(header)

for test in tests:
    s = single_test_function
    for k,v in test.items():
        s = s.replace(k, v)
    print(s)

    s = array_test_function
    for k,v in test.items():
        s = s.replace(k, v)
    print(s)

main = """
int
main(int argc, char *argv[]) {

    db = new_tagbase();

    xassert(db != NULL);
"""

print(main)

for test in tests:
    print("    single_test_{type}();".replace("{type}", test["{type}"]))
    print("    array_test_{type}();".replace("{type}", test["{type}"]))


footer = """
    exit(0);
}
"""

print(footer)