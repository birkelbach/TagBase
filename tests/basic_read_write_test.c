#include <testing.h>

/* This is a very basic test of reading and writing to the database */

tag_db_t *db;


static void
_bool_test(void) {
    int result;
    type_byte test[2];

    test[0] = 0x55;
    test[1] = 0xAA;
    result = tag_write(db, "test_bool", 16, test);
    xassert(result == ERR_OK);

    test[0] = test[1] = 0x00;
    xassert(test[0] == 0x00);
    xassert(test[1] == 0x00);

    result =- tag_read(db, "test_bool", 16, test);
    xassert(result == ERR_OK);
    xassert(test[0] == 0x55);
    xassert(test[1] == 0xAA);

    test[0] = test[1] = 0x00;
    xassert(test[0] == 0x00);
    xassert(test[1] == 0x00);

    result =- tag_read(db, "test_bool[8]", 8, test);
    xassert(result == ERR_OK);
    xassert(test[0] == 0xAA);
    xassert(test[1] == 0x00);
}


static void
_byte_test(void) {
    int result;
    type_byte test[16];

    for(type_byte n=0; n<16;n++) {
        test[n] = n;
    }
    result = tag_write(db, "test_byte", 16, test);
    xassert(result == ERR_OK);
    for(type_byte n=0;n<16;n++) {
        test[n] = 0;
    }

    result = tag_read(db, "test_byte", 16, test);
    xassert(result == ERR_OK);
    for(type_byte n=0;n<16;n++) {
        xassert(test[n] == n);
    }
}

static void
_sint_test(void) {
    int result;
    type_sint test[16];

    for(type_sint n=0; n<16;n++) {
        test[n] = n+4;
    }
    result = tag_write(db, "test_sint", 16, test);
    xassert(result == ERR_OK);
    for(type_byte n=0;n<16;n++) {
        test[n] = 0;
    }

    result = tag_read(db, "test_sint", 16, test);
    xassert(result == ERR_OK);
    for(type_byte n=0;n<16;n++) {
        xassert(test[n] == n+4);
    }
}


static void
_char_test(void) {
    int result;
    type_char test[16];
    char *test_string = "Character Test1";

    result = tag_write(db, "test_char", 16, test_string);
    xassert(result == ERR_OK);
    for(int n=0;n<16;n++) {
        test[n] = 0;
    }

    result = tag_read(db, "test_char", 16, test);
    xassert(result == ERR_OK);
    for(type_byte n=0;n<16;n++) {
        xassert(test[n] == test_string[n]);
    }
}

static void
_int_test(void) {
    int result;
    type_int test[16];

    for(int n=0; n<16;n++) {
        test[n] = n+1000;
    }
    result = tag_write(db, "test_int", 16, test);
    xassert(result == ERR_OK);
    for(int n=0;n<16;n++) {
        test[n] = 0;
    }

    result = tag_read(db, "test_int", 16, test);
    xassert(result == ERR_OK);
    for(int n=0;n<16;n++) {
        xassert(test[n] == n+1000);
    }
}



int
main(int argc, char *argv[]) {

    db = new_tagbase();
    xassert(tag_add(db, "test_bool", TYPE_BOOL, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_byte", TYPE_BYTE, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_sint", TYPE_SINT, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_char", TYPE_CHAR, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_word", TYPE_WORD, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_int", TYPE_INT, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_uint", TYPE_UINT, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_dword", TYPE_DWORD, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_dint", TYPE_DINT, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_udint", TYPE_UDINT, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_real", TYPE_REAL, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_lword", TYPE_LWORD, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_lint", TYPE_LINT, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_ulint", TYPE_ULINT, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_time", TYPE_TIME, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_lreal", TYPE_LREAL, 16, 0x0000, NULL) != NULL);

    _bool_test();
    _byte_test();
    _sint_test();
    _char_test();
    _int_test();
    //TODO: finish these raw tests...
    exit(0);
}
