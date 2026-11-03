#!/usr/bin/python3

# This is a Python script to generate C code tests deadband events
# should be used to create the event_deadband_test.c file

test_function = """
void
test_{type}(void) {
    int result;
    type_{type} test;
    int id;

    xassert(tag_add(db, "test_{type}", TYPE_{TYPE}, 1, 0, NULL) != NULL);
    //xassert(result > 0);
    //result = tag_gethandle(&h, "test_{type}", 0);
    //xassert(result == 0);

    /* Initialize the value */
    test = {init};
    result = tag_write(db, "test_{type}", 0, &test);
    xassert(result == 0);

    test = {val}; /* This will be the increment */
    id = tag_event_add(db, "test_{type}", 0, EVENT_DEADBAND, &test, _event_callback, (void *)1);
    xassert(id > 0);

    /* Write the same value and verify that it does not fire */
    test = {init};
    result = tag_write(db, "test_{type}", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Write half the increment value and the event should not fire */
    test = {init} + ({val} / 2);
    result = tag_write(db, "test_{type}", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* go the other way */
    test = {init} - ({val} / 2);
    result = tag_write(db, "test_{type}", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* back to the start */
    test = {init};
    result = tag_write(db, "test_{type}", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* now make it go */
    test = {init} + {val} + {tweak};
    result = tag_write(db, "test_{type}", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

    /* another small move should not fire */
    test = {init} + {val} + {val} / 2;
    result = tag_write(db, "test_{type}", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x00);
    check_value = 0x00;

    /* Second fire */
    test = {init} + {val} * 2 + {tweak};
    result = tag_write(db, "test_{type}", 0, &test);
    xassert(result == 0);
    xassert(check_value == 0x01);
    check_value = 0x00;

}
"""

tests = []

tests.append({'{type}': 'byte', '{TYPE}': 'BYTE', '{init}': '127', '{val}': '5', '{tweak}': '0'})
tests.append({'{type}': 'sint', '{TYPE}': 'SINT', '{init}': '0', '{val}': '5', '{tweak}': '0'})
tests.append({'{type}': 'int', '{TYPE}': 'INT', '{init}': '0', '{val}': '5', '{tweak}': '0'})
tests.append({'{type}': 'uint', '{TYPE}': 'UINT', '{init}': '32767', '{val}': '5', '{tweak}': '0'})
tests.append({'{type}': 'dint', '{TYPE}': 'DINT', '{init}': '0', '{val}': '5', '{tweak}': '0'})
tests.append({'{type}': 'udint', '{TYPE}': 'UDINT', '{init}': '2147483647UL', '{val}': '5', '{tweak}': '0'})
tests.append({'{type}': 'lint', '{TYPE}': 'LINT', '{init}': '0LL', '{val}': '5', '{tweak}': '0'})
tests.append({'{type}': 'ulint', '{TYPE}': 'ULINT', '{init}': '9223372036854775807ULL', '{val}': '5', '{tweak}': '0'})
tests.append({'{type}': 'real', '{TYPE}': 'REAL', '{init}': '3.141592', '{val}': '5', '{tweak}': '0.00001'})
tests.append({'{type}': 'lreal', '{TYPE}': 'LREAL', '{init}': '3.141592', '{val}': '5', '{tweak}': '0.00001'})

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
    s = test_function
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
    print("    test_{type}();".replace("{type}", test["{type}"]))


footer = """
    exit(0);
}
"""

print(footer)