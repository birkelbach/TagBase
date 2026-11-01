/*
This test checks that handles are returned properly from the server.  It starts by setting up
an environment of datatypes and tags.  Then it tries every combination that we can think of
to get handles.  The parameters of what the returned handle should be are given and the
called function tests these and if the PASS/FAIL test doesn't match then an error is returned
and this test fails.  The CDT creation and tag addition functions should all pass.  If they
fail then the error is allowed to raise to the caller and the test fail.

TODO: Need to test the handle that is returned when tags are added
*/

#include <tagbase.h>
#include <stdlib.h>
#include "testing.h"

#define PASS 0
#define FAIL 1

tag_db_t *db;


static void
handle_test(char *name, int rcount, int byte, int bit, int count, int size, tag_type type, int pass) {
    int result=0;
    tag_handle_t *h;

    TP("Testing %s count = %d", name, rcount);
    h = tag_gethandle(db, name, rcount, &result);
    if(pass == PASS) {
        xassert(result == 0);
        xassert(h->byte == byte);
        xassert(h->bit == bit);
        xassert(h->count == count);
        xassert(h->size == size);
        xassert(h->type == type);
    } else {
        xassert(result !=0);
    }

    return;
}

int
main(int argc, char *argv[]) {
    db = new_tagbase();
    int result;
    tag_t *tag;

    tag_type test1, test2, test3, test4, test5;

    test1 = cdt_create(db, "Test1:Int5,INT,5:Bool10,BOOL,10:Dint1,DINT,1:Dint3,DINT,3", NULL);
    xassert(test1 >= 0);

    test2 = cdt_create(db, "Test2:Int3,INT,3:Test1,Test1,5", NULL);
    xassert(test2 >= 0);

    test3 = cdt_create(db, "Test3:Int7,INT,7:Bool30,BOOL,30:Bool32,BOOL,32:Test1,Test1,1:Test2,Test2,2", NULL);
    xassert(test3 >= 0);

    test4 = cdt_create(db, "Test4:Int,INT,1:Bool,BOOL,1:reBool,BOOL,1:triBool,BOOL,1:Dint,DINT,1", NULL);
    xassert(test4 >= 0);

    test5 = cdt_create(db, "Test5:Bool1,BOOL,10:Bool2,BOOL,1:Bool3,BOOL,3", NULL);
    xassert(test5 >= 0);

    tag = tag_add(db,"HandleBool1", TYPE_BOOL, 1, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleBool7", TYPE_BOOL, 7, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleBool8", TYPE_BOOL, 8, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleBool9", TYPE_BOOL, 9, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleBool16", TYPE_BOOL, 16, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleBool33", TYPE_BOOL, 33, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleInt", TYPE_INT, 2, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleInt32", TYPE_INT, 32, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleTest1", test1, 1, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleTest2", test2, 5, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleTest3", test3, 1, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleTest4", test3, 10, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleTestIV", test4, 1, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleTest5", test5, 1, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    tag = tag_add(db,"HandleTest5_10", test5, 10, 0x0000, &result);
    TP("result = %d", result);
    xassert(tag != NULL);
    /*
    Test that the handles returned are correct
    Each tag entry contains...
        the tagname string to parse
        item count  (N) - requested by test
    Returned Values
        byte offset (B)
        bit offset  (b)
        item count  (c) - returned by opendax
        byte size   (s)
        data type
        PASS/FAIL
    If FAIL then all arguments are ignored and the string/count is expected to fail.
    Otherwise the returned handle values must match those given
    or the test will fail.
                NAME                          N   B   b  c   s   TYPE      TEST  --
    */
    handle_test("HandleBool1",                 0,  0,  0, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool7",                 0,  0,  0, 7,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool7[2]",              4,  0,  2, 4,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool7[6]",              1,  0,  6, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool7[5]",              2,  0,  5, 2,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool7[6]",              2,  0,  6, 2,  1,  TYPE_BOOL,   FAIL);
    handle_test("HandleBool7[7]",              1,  0,  2, 4,  1,  TYPE_BOOL,   FAIL);
    handle_test("HandleBool8",                 0,  0,  0, 8,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool9",                 0,  0,  0, 9,  2,  TYPE_BOOL,   PASS);
    handle_test("HandleBool16",                0,  0,  0, 16, 2,  TYPE_BOOL,   PASS);
    handle_test("HandleBool16[2]",             8,  0,  2, 8,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool16[8]",             8,  1,  0, 8,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool9[8]",              0,  1,  0, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool33",                0,  0,  0, 33, 5,  TYPE_BOOL,   PASS);
    handle_test("HandleBool33[7]",             1,  0,  7, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool33[3]",             8,  0,  3, 8,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleBool33[3]",             9,  0,  3, 9,  2,  TYPE_BOOL,   PASS);
    handle_test("HandleBool33[3a]",            1,  0,  0, 0,  0,  TYPE_BOOL,   FAIL);
    handle_test("HandleInt[1]",                1,  2,  0, 1,  2,  TYPE_INT,    PASS);
    handle_test("HandleInt",                   0,  0,  0, 2,  4,  TYPE_INT,    PASS);
    handle_test("HandleInt",                   1,  0,  0, 1,  2,  TYPE_INT,    PASS);
    handle_test("HandleInt32",                 1,  0,  0, 1,  2,  TYPE_INT,    PASS);
    handle_test("HandleInt[2]",                1,  0,  0, 0,  0,  TYPE_INT,    FAIL);
    handle_test("HandleInt[2]",                5,  0,  0, 0,  0,  TYPE_INT,    FAIL);
    handle_test("HandleTest1",                 0,  0,  0, 1,  28, test1,  PASS);
    handle_test("HandleTest1.Dint3",           0,  16, 0, 3,  12, TYPE_DINT,   PASS);
    handle_test("HandleTest1.Dint3[0]",        2,  16, 0, 2,  8,  TYPE_DINT,   PASS);
    handle_test("HandleTest1.Dint3[1]",        2,  20, 0, 2,  8,  TYPE_DINT,   PASS);
    handle_test("HandleTest1.Dint3[2]",        1,  24, 0, 1,  4,  TYPE_DINT,   PASS);
    handle_test("HandleTest1.Dint3[2]",        2,  24, 0, 1,  4,  TYPE_DINT,   FAIL);
    handle_test("HandleTest1.Dint1",           1,  12, 0, 1,  4,  TYPE_DINT,   PASS);
    handle_test("HandleTest2[0].Test1[2]",     1,  62, 0, 1,  28, test1,  PASS);
    handle_test("HandleTest2[0].Test1[1]",     2,  34, 0, 2,  56, test1,  PASS);
    handle_test("HandleTest2[0].Test1[4]",     1,  118,0, 1,  28, test1,  PASS);
    handle_test("HandleTest2[0].Test1[1]",     5,  32, 0, 1,  28, test1,  FAIL);
    handle_test("HandleTest2[1].Test1",        0,  152,0, 5,  140,test1,  PASS);
    handle_test("HandleTest2[0].Test1[0].Bool10[4]",
                                            1,  16, 4, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleTest2[0].Test1[0].Bool10",
                                            0,  16, 0, 10, 2,  TYPE_BOOL,   PASS);
    handle_test("HandleTest2[0].Test1[1].Bool10",
                                            0,  44, 0, 10, 2,  TYPE_BOOL,   PASS);
    handle_test("HandleTest2[4].Test1[0].Bool10",
                                            0,  600,0, 10, 2,  TYPE_BOOL,   PASS);
    handle_test("HandleTest2[0].NotAMember",   0,  0,  0, 0,  0,  TYPE_BOOL,    FAIL);
    handle_test("NoTagName",                   0,  0,  0, 0,  0,  TYPE_BOOL,    FAIL);
    handle_test("",                            0,  0,  0, 0,  0,  TYPE_BOOL,    FAIL);
    handle_test("HandleTestIV.Bool",           0,  2,  0, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleTestIV.reBool",         0,  2,  1, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleTestIV.triBool",        0,  2,  2, 1,  1,  TYPE_BOOL,   PASS);

    handle_test("HandleTest5",                 0,  0,  0, 1,  2,  test5,  PASS);
    handle_test("HandleTest5.Bool1",           0,  0,  0, 10, 2,  TYPE_BOOL,   PASS);
    handle_test("HandleTest5.Bool1[1]",        0,  0,  1, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleTest5.Bool1[9]",        0,  1,  1, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleTest5.Bool2",           0,  1,  2, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleTest5.Bool3",           0,  1,  3, 3,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleTest5.Bool3[0]",        0,  1,  3, 1,  1,  TYPE_BOOL,   PASS);
    handle_test("HandleTest5.Bool3[2]",        0,  1,  5, 1,  1,  TYPE_BOOL,   PASS);

    exit(0);
}
