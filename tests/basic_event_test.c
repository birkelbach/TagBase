#include <testing.h>

/* This is a very basic test of reading and writing to the database */

tag_db_t *db;

int test;

static void _event_function(void *data) {
    TP("here");
    test++;
}


int
main(int argc, char *argv[]) {
    int result;
    type_int buff;

    db = new_tagbase();
    xassert(tag_add(db, "test_int", TYPE_INT, 16, 0x0000, NULL) != NULL);
    result = tag_event_add(db, "test_int[0]", 1, EVENT_CHANGE, NULL, _event_function, NULL);
    xassert(result > 0);
    buff = 16;
    result = tag_write(db, "test_int[0]", 1, &buff);
    xassert(result == 0);
    xassert(test == 1);

    exit(0);
}

