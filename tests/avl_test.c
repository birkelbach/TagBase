#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <testing.h>
#include "avl_tree.h"

struct item {
    char *name;
    int number;
};


static int
compare_function(void *left, void *right) {
    struct item *l = left;
    struct item *r = right;
    if(l->number < r->number) return -1;
    if(l->number > r->number) return 1;
    else return 0;
}

static void
print_function(void *data, void *udata) {
    struct item *i = data;
    TP("%s - %d", i->name, i->number);
}


int main(int argc, char *argv[]) {

    struct tree_t *tree = avl_new_tree(compare_function);

    struct item items[] = {{"five", 5}, {"one", 1}, {"two", 2}, {"four", 4}, {"three", 3}, {"six", 6}};

    for(int n=0; n<6; n++) {
        avl_insert_node(tree, &items[n]);
    }

    avl_inorder(tree, print_function, NULL);

    avl_delete_node(tree, &items[2]);

    TP("\nAfter deletion:");
    avl_inorder(tree, print_function, NULL);

    struct item search_item;
    search_item.name = "nope";
    search_item.number = 4;
    struct item *i = avl_search(tree, &search_item);
    xassert(i != NULL);
    TP("\nFound %s - %d", i->name, i->number);

    /* Should not find this one */
    search_item.name = "nope";
    search_item.number = 2;
    i = avl_search(tree, &search_item);
    xassert(i == NULL);

    return 0;
}
