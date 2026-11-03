#include <stdlib.h>
#include <stdio.h>
#include <tagbase.h>
#include <testing.h>

tag_db_t *db;

int
main(int argc, char *argv[]) {
    tag_t *tag;
    type_int test;

    db = new_tagbase();
    xassert(db != NULL);

    tag = tag_add(db, "test_int", TYPE_INT, 4, 0, NULL);
    xassert(tag != NULL);
    test = 4321;
    tag_write(db, "test_int[0]", 1, &test);

    test = 1234;
    override_add(db, tag, 0, &test, NULL, 2);

    tag_read(db, "test_int[0]", 1, &test);
    xassert(test == 4321);

    override_set(db, tag, 1);
    tag_read(db, "test_int[0]", 1, &test);
    xassert(test == 1234);

    override_set(db, tag, 0);
    tag_read(db, "test_int[0]", 1, &test);
    xassert(test == 4321);


    exit(0);
}
