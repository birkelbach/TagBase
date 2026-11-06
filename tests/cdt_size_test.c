#include <tagbase.h>
#include <testing.h>

static int
_create_types(tag_db_t *db) {
    cdt_t *cdt;
    int result = 0;

    cdt = tag_cdt_new("Test1", NULL);
    if(cdt == NULL) return 1;
    result += tag_cdt_add_member(db, cdt, "Int5", TYPE_INT, 5);     // 10
    result += tag_cdt_add_member(db, cdt, "Bool10", TYPE_BOOL, 10); // 2
    result += tag_cdt_add_member(db, cdt, "Dint1", TYPE_DINT, 1);   // 4
    result += tag_cdt_add_member(db, cdt, "Dint3", TYPE_DINT, 3);   // 12
    result += tag_cdt_finalize(db, cdt); /* Size = 28 */
    if(result) return result;

    cdt = tag_cdt_new("Test2", NULL);
    if(cdt == NULL) return 1;
    result += tag_cdt_add_member(db, cdt, "Int3", TYPE_INT, 3);
    result += tag_cdt_add_member(db, cdt, "Test1", tag_get_type(db, "test1"), 5);
    result += tag_cdt_finalize(db, cdt); /* Size = 146 */
    if(result) return result;

    cdt = tag_cdt_new("Test3", NULL);
    if(cdt == NULL) return 1;
    result += tag_cdt_add_member(db, cdt, "Int7", TYPE_INT, 5);     // 10
    result += tag_cdt_add_member(db, cdt, "Bool30", TYPE_BOOL, 30); // 4
    result += tag_cdt_add_member(db, cdt, "Bool32", TYPE_BOOL, 32); // 4
    result += tag_cdt_add_member(db, cdt, "Test1", tag_get_type(db, "test1"), 1);      // 28
    result += tag_cdt_add_member(db, cdt, "Test2", tag_get_type(db, "test2"), 5);      // 146
    result += tag_cdt_finalize(db, cdt); // size = 776
    if(result) return result;

    cdt = tag_cdt_new("Test4", NULL);
    if(cdt == NULL) return 1;
    result += tag_cdt_add_member(db, cdt, "Int", TYPE_INT, 1);      // 2
    result += tag_cdt_add_member(db, cdt, "Bool", TYPE_BOOL, 1);    // 1
    result += tag_cdt_add_member(db, cdt, "reBool", TYPE_BOOL, 1);  // 0
    result += tag_cdt_add_member(db, cdt, "triBool", TYPE_BOOL, 1); // 0
    result += tag_cdt_add_member(db, cdt, "Dint", TYPE_DINT, 1);    // 4
    result += tag_cdt_finalize(db, cdt); // size = 7
    if(result) return result;

    cdt = tag_cdt_new("Test5", NULL);
    if(cdt == NULL) return 1;
    result += tag_cdt_add_member(db, cdt, "Bool1", TYPE_BOOL, 10); // 2
    result += tag_cdt_add_member(db, cdt, "Bool2", TYPE_BOOL, 1);  // 0
    result += tag_cdt_add_member(db, cdt, "Bool3", TYPE_BOOL, 3);  // 0
    result += tag_cdt_finalize(db, cdt); // size = 2
    if(result) return result;

    cdt = tag_cdt_new("Test6", NULL);
    if(cdt == NULL) return 1;
    result += tag_cdt_add_member(db, cdt, "Bool5", TYPE_BOOL, 5);  // 1
    result += tag_cdt_add_member(db, cdt, "Bool3", TYPE_BOOL, 3);  // 0
    result += tag_cdt_add_member(db, cdt, "Dint1", TYPE_DINT, 1);  // 4
    result += tag_cdt_finalize(db, cdt); // size = 5
    if(result) return result;
    return result;
}


int
main(int argc, char *argv[]) {
    tag_db_t *db;
    int result = 0;

    db = new_tagbase();

    result = _create_types(db);
    if(result) return result;

    // result = tag_type_size(db, TYPE_BOOL);
    // if(result != 1) { TP("bool returned %d", result); return -1; }
    /* 8 Bits */
    result = tag_type_size(db, TYPE_BYTE);
    xassert(result == 1);
    result = tag_type_size(db, TYPE_SINT);
    xassert(result == 1);
    result = tag_type_size(db, TYPE_CHAR);
    xassert(result == 1);
    /* 16 Bits */
    result = tag_type_size(db, TYPE_WORD);
    xassert(result == 2);
    result = tag_type_size(db, TYPE_INT);
    xassert(result == 2);
    result = tag_type_size(db, TYPE_UINT);
    xassert(result == 2);

    /* 32 Bits */
    result = tag_type_size(db, TYPE_DWORD);
    xassert(result == 4);
    result = tag_type_size(db, TYPE_DINT);
    xassert(result == 4);
    result = tag_type_size(db, TYPE_UDINT);
    xassert(result == 4);
    result = tag_type_size(db, TYPE_REAL);
    xassert(result == 4);
    /* 64 Bits */
    result = tag_type_size(db, TYPE_LWORD);
    xassert(result == 8);
    result = tag_type_size(db, TYPE_LINT);
    xassert(result == 8);
    result = tag_type_size(db, TYPE_ULINT);
    xassert(result == 8);
    result = tag_type_size(db, TYPE_TIME);
    xassert(result == 8);
    result = tag_type_size(db, TYPE_LREAL);
    xassert(result == 8);

    result = tag_type_size(db, tag_get_type(db, "test1"));
    xassert(result == 28);
    result = tag_type_size(db, tag_get_type(db, "test2"));
    xassert(result == 146);
    result = tag_type_size(db, tag_get_type(db, "test3"));
    xassert(result == 776);
    result = tag_type_size(db, tag_get_type(db, "test4"));
    xassert(result == 7);
    result = tag_type_size(db, tag_get_type(db, "test5"));
    xassert(result == 2);

    return 0;
}