#include <stdlib.h>
#include <stdio.h>
#include <tagbase.h>
#include <testing.h>

int
main(int argc, char *argv[]) {
    tag_db_t *db;
    db = new_tagbase();
    xassert(db != NULL);
    exit(0);
}
