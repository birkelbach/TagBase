#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <debug.h>
#include <tagbase.h>
#include <testing.h>

const char *lastone;

static void
_print_tag(tag_t *tag, void *udata) {
    /* Check that they are coming in order */
    if(lastone != NULL) {
        xassert(strcmp(tag_get_name(tag), lastone) > 0);
        lastone = tag_get_name(tag);
    }
    DF("Tag %s", tag_get_name(tag));
}

static void
_tag_delete_test(tag_t *tag, void *udata) {
    DF("Tag %s", tag_get_name(tag));
    //assert(strcmp(tag_get_name(tag), "Chocolate") != 0);
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
        tag = tag_add(db, taglist[n], TYPE_INT, n+1, 0, NULL);
        n++;
        assert(tag != NULL);
    }

    tag_list(db, _print_tag, NULL);

    /* Make sure they are in there */
    n=0;
    while(taglist[n] != NULL) {
        DF("Checking for %s", taglist[n]);
        tag = tag_get_by_name(db, taglist[n++]);
        assert(tag != NULL);
    }

    tag = tag_get_by_name(db, "Chocolate");
    xassert(tag != NULL);
    xassert(tag_delete(db, tag) == 0);
    tag_list(db, _tag_delete_test, NULL);

    // TODO Delete them all and make sure they are all gone
    exit(0);
}
