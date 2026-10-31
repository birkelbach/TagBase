#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <debug.h>
#include <tagbase.h>

static void
_print_tag(tag_t *tag, void *udata) {
    DF("Tag %s", tag_get_name(tag));
}

int
main(int argc, char *argv[]) {
    tag_db_t *db;
    tag_t *tag;
    db = new_tagbase();

    char *taglist[] = {"Apples", "Blueberries", "Cherries",
                      "Eggs", "Chocolate", "Waffles",
                      "PanCakes", "Syrup", "Cherios",
                      "Grits", NULL};

    /* Add the tags */
    int n=0;
    while(taglist[n] != NULL) {
        tag = tag_add(db, taglist[n++], TYPE_INT, 1, 0, NULL);
        assert(tag != NULL);
    }

    // TODO  Figure out a way to test that we get these in order.
    tag_list(db, _print_tag, NULL);

    /* Make sure they are in there */
    n=0;
    while(taglist[n] != NULL) {
        DF("Checking for %s", taglist[n]);
        tag = tag_get_by_name(db, taglist[n++]);
        assert(tag != NULL);
    }

    // TODO Delete some / add some - make sure they match

    // TODO Delete them all and make sure they are all gone
    exit(0);
}
