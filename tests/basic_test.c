#include <stdlib.h>
#include <stdio.h>
#include <tagbase.h>
#include <testing.h>

/* This is just a place where we put development tests as
   we are building features, before we build more formal
   tests.
*/

void print_tag(tag_t *tag, void *udata) {
    printf("tag %s\n", tag_get_name(tag));
}

int
main(int argc, char *argv[]) {
    type_time time;
    type_udint check;
    int result;

    tag_db_t *db;
    db = new_tagbase();
    xassert(db != NULL);

    tag_list(db, print_tag, NULL);

    result = tag_read(db, "_system_time", 0, &time);
    TP("time = %ld", time);

    result = tag_read(db, "_tagcount", 0, &check);
    TP("tag count = %d", check);

    exit(0);
}
