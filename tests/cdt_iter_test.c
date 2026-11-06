
#include <tagbase.h>
#include <testing.h>

cdt_iter_t members[] = {{"Int5", TYPE_INT, 5, 0, 0},
                        {"Bool10", TYPE_BOOL, 10, 10, 0},
                        {"Dint1", TYPE_DINT, 1, 12, 0},
                        {"Dint3", TYPE_DINT, 3, 14, 0}};


void callback(cdt_iter_t member, void *udata) {
    static int x;

    xassert(strcmp(member.name, members[x++].name) == 0);
    TP("member name = %s, type = %d, count = %d", member.name, member.type, member.count);
}


int
main(int argc, char *argv[]) {
    tag_db_t *db;
    cdt_t *cdt;
    int result = 0;

    db = new_tagbase();

    cdt = tag_cdt_new("Test1", NULL);
    if(cdt == NULL) return 1;
    result += tag_cdt_add_member(db, cdt, "Int5", TYPE_INT, 5);     // 10
    result += tag_cdt_add_member(db, cdt, "Bool10", TYPE_BOOL, 10); // 2
    result += tag_cdt_add_member(db, cdt, "Dint1", TYPE_DINT, 1);   // 4
    result += tag_cdt_add_member(db, cdt, "Dint3", TYPE_DINT, 3);   // 12
    result += tag_cdt_finalize(db, cdt); /* Size = 28 */
    if(result) return result;

    tag_cdt_iter(db, tag_get_type(db, "Test1"), NULL, callback);
    exit(0);
}
