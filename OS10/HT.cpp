#include "HT.h"
#include <cstring>
#include <ctime>

using namespace std;

namespace HT
{
    HTHANDLE* Create(
        int Capacity,                   // �������
        int SecSnapshotInterval,        // �������� �������� ������� � ��������
        int MaxKeyLength,               // ������������ ����� �����
        int MaxPayloadLength,           // ������������ ����� �������� ��������
        const char FileName[SIZE])      // ��� �����
    {
        HTHANDLE* ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);

        // �������� �����
        ht->File = CreateFileA(
            FileName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (ht->File == INVALID_HANDLE_VALUE) {
            strncpy_s(ht->LastErrorMessage, "�� ������� ������� ����", SIZE);
            delete ht;
            return nullptr;
        }

        // ���������� ������ ������������ �������
        DWORD fileSize = sizeof(HTHANDLE) + (Capacity * (sizeof(Element) + MaxKeyLength + MaxPayloadLength));

        // ��������� ������� �����
        SetFilePointer(ht->File, fileSize, NULL, FILE_BEGIN);
        SetEndOfFile(ht->File);

        // �������� �������� ����� (file mapping)
        ht->FileMapping = CreateFileMappingA(
            ht->File,
            NULL,
            PAGE_READWRITE,
            0,
            fileSize,
            NULL
        );

        if (ht->FileMapping == NULL) {
            strncpy_s(ht->LastErrorMessage, "�� ������� ������� �������� �����", SIZE);
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        // ����������� ������������� ����� � ������
        ht->Addr = MapViewOfFile(
            ht->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            fileSize
        );

        if (ht->Addr == NULL) {
            strncpy_s(ht->LastErrorMessage, "�� ������� ���������� ������������� �����", SIZE);
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        // ����������� ��������� HTHANDLE � ������������ ������
        memcpy(ht->Addr, ht, sizeof(HTHANDLE));

        ht->lastsnaptime = time(nullptr); // ��������� ������� ���������� ������
        return ht;
    }

    HTHANDLE* Open(const char FileName[SIZE]) // �������� ������������ ���-�������
    {
        HTHANDLE* ht = new HTHANDLE();
        strncpy_s(ht->FileName, FileName, SIZE);

        // �������� �����
        ht->File = CreateFileA(
            FileName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (ht->File == INVALID_HANDLE_VALUE) {
            strncpy_s(ht->LastErrorMessage, "�� ������� ������� ����", SIZE);
            delete ht;
            return nullptr;
        }

        // ��������� ������� �����
        DWORD fileSize = GetFileSize(ht->File, NULL);

        // �������� �������� �����
        ht->FileMapping = CreateFileMappingA(
            ht->File,
            NULL,
            PAGE_READWRITE,
            0,
            fileSize,
            NULL
        );

        if (ht->FileMapping == NULL) {
            strncpy_s(ht->LastErrorMessage, "�� ������� ������� �������� �����", SIZE);
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        // ����������� ������������� ����� � ������
        ht->Addr = MapViewOfFile(
            ht->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            fileSize
        );

        if (ht->Addr == NULL) {
            strncpy_s(ht->LastErrorMessage, "�� ������� ���������� ������������� �����", SIZE);
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        // ����������� ��������� HTHANDLE �� ������������ ������
        memcpy(ht, ht->Addr, sizeof(HTHANDLE));

        return ht;
    }

    BOOL Snap(const HTHANDLE* hthandle) // �������� ������ (���������� ���������)
    {
        if (hthandle == nullptr || hthandle->Addr == nullptr) {
            return FALSE;
        }

        // ����� ������������� ��� �������� ������ ������ �� ����
        if (!FlushViewOfFile(hthandle->Addr, 0)) {
            return FALSE;
        }

        // ���������� ������� ���������� ������
        HTHANDLE* nonConstHt = const_cast<HTHANDLE*>(hthandle);
        nonConstHt->lastsnaptime = time(nullptr);
        memcpy(nonConstHt->Addr, nonConstHt, sizeof(HTHANDLE));

        return TRUE;
    }

    BOOL Close(const HTHANDLE* hthandle) // �������� ���-�������
    {
        if (hthandle == nullptr) {
            return FALSE;
        }

        BOOL result = TRUE;

        // ���������� ������
        if (!Snap(hthandle)) {
            result = FALSE;
        }

        // ������������ ��������
        if (hthandle->Addr != nullptr) {
            UnmapViewOfFile(hthandle->Addr);
        }

        if (hthandle->FileMapping != NULL) {
            CloseHandle(hthandle->FileMapping);
        }

        if (hthandle->File != INVALID_HANDLE_VALUE) {
            CloseHandle(hthandle->File);
        }

        // �������� ������� HTHANDLE
        delete hthandle;

        return result;
    }

    BOOL Insert(const HTHANDLE* hthandle, const Element* element) // ������� ��������
    {
        if (hthandle == nullptr || element == nullptr || element->key == nullptr) {
            return FALSE;
        }

        // �������� ������������� �����
        Element* existing = Get(hthandle, element);
        if (existing != nullptr) {
            delete existing;
            strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
                "���� ��� ����������", SIZE);
            return FALSE;
        }

        // ����� ������� ����� � �������
        char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);

        // �������� �������
        memcpy(dataStart, element, sizeof(Element));

        // �������� ������������� �������� ������
        time_t currentTime = time(nullptr);
        if (difftime(currentTime, hthandle->lastsnaptime) >= hthandle->SecSnapshotInterval) {
            Snap(hthandle);
        }

        return TRUE;
    }

    BOOL Delete(const HTHANDLE* hthandle, const Element* element) // �������� ��������
    {
        if (hthandle == nullptr || element == nullptr || element->key == nullptr) {
            return FALSE;
        }

        // ����� ��������
        Element* found = Get(hthandle, element);
        if (found == nullptr) {
            strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
                "���� �� ������", SIZE);
            return FALSE;
        }

        // �������� ��� ���������
        char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);
        memset(dataStart, 0, sizeof(Element));

        delete found;

        // �������� ������������� �������� ������
        time_t currentTime = time(nullptr);
        if (difftime(currentTime, hthandle->lastsnaptime) >= hthandle->SecSnapshotInterval) {
            Snap(hthandle);
        }

        return TRUE;
    }

    Element* Get(const HTHANDLE* hthandle, const Element* element) // ��������� ��������
    {
        if (hthandle == nullptr || element == nullptr || element->key == nullptr) {
            return nullptr;
        }

        // ����� � ���-�������
        char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);
        Element* storedElement = reinterpret_cast<Element*>(dataStart);

        // ��������, �������� �� ��� ������� �������
        if (storedElement->key != nullptr &&
            storedElement->keylength == element->keylength &&
            memcmp(storedElement->key, element->key, element->keylength) == 0) {

            Element* result = new Element();
            result->key = storedElement->key;
            result->keylength = storedElement->keylength;
            result->payload = storedElement->payload;
            result->payloadlength = storedElement->payloadlength;

            return result;
        }

        return nullptr;
    }

    BOOL Update(const HTHANDLE* hthandle, const Element* oldelement,
        const void* newpayload, int newpayloadlength) // ���������� ��������
    {
        if (hthandle == nullptr || oldelement == nullptr || newpayload == nullptr) {
            return FALSE;
        }

        // ����� ��������
        Element* found = Get(hthandle, oldelement);
        if (found == nullptr) {
            strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
                "���� �� ������", SIZE);
            return FALSE;
        }

        // ���������� �������� ��������
        char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);
        Element* storedElement = reinterpret_cast<Element*>(dataStart);

        // ��������� ������
        storedElement->payload = newpayload;
        storedElement->payloadlength = newpayloadlength;

        delete found;

        // �������� ������������� �������� ������
        time_t currentTime = time(nullptr);
        if (difftime(currentTime, hthandle->lastsnaptime) >= hthandle->SecSnapshotInterval) {
            Snap(hthandle);
        }

        return TRUE;
    }

    char* GetLastError(HTHANDLE* ht) // ��������� ���������� ��������� �� ������
    {
        if (ht == nullptr) {
            char buffer[] = "�������� HTHANDLE";
            return buffer;
        }
        return ht->LastErrorMessage;
    }

    void print(const Element* element) // ����� ��������
    {
        if (element == nullptr) {
            cout << "������� ����� null" << endl;
            return;
        }

        cout << "����: ";
        if (element->key != nullptr) {
            for (int i = 0; i < element->keylength; i++) {
                cout << static_cast<const char*>(element->key)[i];
            }
        }
        cout << endl;

        cout << "�������� ��������: ";
        if (element->payload != nullptr) {
            for (int i = 0; i < element->payloadlength; i++) {
                cout << static_cast<const char*>(element->payload)[i];
            }
        }
        cout << endl;
    }
}