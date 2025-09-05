#include "Tests.h"

int main() {
    SetConsoleOutputCP(1251);

    cout << "Starting HT API Tests..." << endl << endl;

    try {
        // Тестовые данные
        const int capacity = 10;
        const int secSnapshotInterval = 5;
        const int maxKeyLength = 20;
        const int maxPayloadLength = 50;

        const char* testKey = "test_key";
        const char* testValue = "test_value";
        const char* testValue2 = "test_value2";
        const char* nonExistentKey = "non_existent_key";

        TestCreate(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_create.ht");
        TestOpen(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_open.ht");
        TestInsert(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_insert.ht", testKey, testValue);
        TestGet(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_get.ht", testKey, testValue);
        TestUpdate(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_update.ht", testKey, "initial_value", "updated_value");
        TestDelete(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_delete.ht", testKey, testValue);
        TestSnap(capacity, 1, maxKeyLength, maxPayloadLength, "test_snap.ht", "snap_key", "snap_value");
        TestClose(5, 2, 15, 30, "test_close.ht");
        TestDuplicateInsert(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_duplicate.ht", testKey, testValue, testValue2);
        TestDeleteNonExistent(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_delete_nonexistent.ht", nonExistentKey);
        TestGetNonExistent(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_get_nonexistent.ht", nonExistentKey);
        TestGetLastError(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_error.ht", nonExistentKey);
        TestPrint(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, "test_print.ht", "print_key", "print_value");

        cout << "=== ALL UNIT TESTS PASSED ===" << endl;
        return 0;
    }
    catch (const exception& e) {
        cerr << "Test failed: " << e.what() << endl;
        return 1;
    }
}