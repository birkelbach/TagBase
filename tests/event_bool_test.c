#include <tagbase.h>
#include <testing.h>

static int check_value;
tag_db_t *db;


static void
_event_callback(void *data) {
    TP("Event called - %ld", (long int)data);
    check_value = (long int)data;
}

static void
_bool_set_test(void) {
    uint8_t data;
    int id[2];
    tag_t *tag;


    check_value = 0;
    id[0] = tag_event_add(db, "test_bool[0]", 1, EVENT_SET, NULL, _event_callback, (void *)1);
    xassert(id[0] >= 0);
    id[1] = tag_event_add(db, "test_bool[1]", 1, EVENT_SET, NULL, _event_callback, (void *)2);
    xassert(id[1] >= 0);

    data = 0x01;
    tag_write(db, "test_bool", 8, &data);
    xassert(check_value == 1);
    check_value = 0;
     /* Setting the bit a second time should not fire the event again */
    tag_write(db, "test_bool", 8, &data);
    xassert(check_value == 0);

    data = 0x02;
    tag_write(db, "test_bool", 8, &data);
    xassert(check_value == 2);

    data = 0x00;
    tag_write(db, "test_bool", 8, &data);

    /* Delete the events and make sure they don't fire */
    tag = tag_get_by_name(db, "test_bool");
    tag_event_del(tag, id[0]);
    tag_event_del(tag, id[1]);

    check_value = 0;
    data = 0x03;
    tag_write(db, "test_bool", 8, &data);
    xassert(check_value == 0);

    /* Put everything back how we found it */
    data = 0x00;
    tag_write(db, "test_bool", 8, &data);
}


static void
_bool_reset_test(void) {
    uint8_t data;
    int id[2];
    tag_t *tag;

    check_value = 0;
    id[0] = tag_event_add(db, "test_rebool[0]", 1, EVENT_RESET, NULL, _event_callback, (void *)3);
    xassert(id[0] >= 0);
    id[1] = tag_event_add(db, "test_rebool[1]", 1, EVENT_RESET, NULL, _event_callback, (void *)4);
    xassert(id[1] >= 0);

    tag_read(db, "test_rebool", 8, &data);
    xassert(data == 0x00);

    data = 0x01;
    tag_write(db, "test_rebool", 8, &data);
    xassert(check_value == 0);

    data = 0x00;
    tag_write(db, "test_rebool", 8, &data);
    xassert(check_value == 3);

    /* Doing it again should not fire the event */
    check_value = 0;
    tag_write(db, "test_rebool", 8, &data);
    xassert(check_value == 0);

    data = 0x03;
    tag_write(db, "test_rebool", 8, &data);
    xassert(check_value == 0);

    data = 0x01;
    tag_write(db, "test_rebool", 8, &data);
    xassert(check_value == 4);

    /* Turn both bits on, delete the events and make sure they don't fire. */
    check_value = 0;
    data = 0x03;
    tag_write(db, "test_rebool", 8, &data);
    xassert(check_value == 0);

    tag = tag_get_by_name(db, "test_rebool");
    tag_event_del(tag, id[0]);
    tag_event_del(tag, id[1]);

    data = 0x00;
    tag_write(db, "test_rebool", 8, &data);
    xassert(check_value == 0);
}

static void
_bool_reset2_test(void) {
    uint8_t data[2];
    int id;

    xassert(tag_add(db, "test_debool", TYPE_BOOL, 16, 0x0000, NULL) != NULL);
    data[0] = data[1] = 0xFF; /* Set all the bits before we add the event */
    tag_write(db, "test_debool", 16, &data);

    check_value = 0;
    id = tag_event_add(db, "test_debool[4]", 8, EVENT_RESET, NULL, _event_callback, (void *)7);
    xassert(id >= 0);

    data[0] = 0xF0; /* First four bits should not fire */
    tag_write(db, "test_debool", 16, &data);
    xassert(check_value == 0);

    data[0] = 0xE0; /* fifth bit should fire */
    tag_write(db, "test_debool", 16, &data);
    xassert(check_value == 7);

    check_value = 0;
    data[0] = 0xC0; /* sixth bit should set(TB_LUA_SCRIPT_Td fire */
    tag_write(db, "test_debool", 16, &data);
    xassert(check_value == 7);

    check_value = 0;
    data[1] = 0x0F; /* these bits should not fire */
    tag_write(db, "test_debool", 16, &data);
    xassert(check_value == 0);

    check_value = 0;
    data[1] = 0x0D; /* This should be the last bit to fire */
    tag_write(db, "test_debool", 16, &data);
    xassert(check_value == 7);

}


int
main(int argc, char *argv[]) {

    db = new_tagbase();

    xassert(tag_add(db, "test_bool", TYPE_BOOL, 16, 0x0000, NULL) != NULL);
    xassert(tag_add(db, "test_rebool", TYPE_BOOL, 16, 0x0000, NULL) != NULL);

    _bool_set_test();
    _bool_reset_test();
    _bool_reset2_test();

    exit(0);
}

