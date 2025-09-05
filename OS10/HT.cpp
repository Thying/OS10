#include "HT.h"
#include <cstring>
#include <ctime>

using namespace std;

namespace HT
{
    HTHANDLE* Create(
        int Capacity,                   // Емкость
        int SecSnapshotInterval,        // Интервал создания снимков в секундах
        int MaxKeyLength,               // Максимальная длина ключа
        int MaxPayloadLength,           // Максимальная длина полезной нагрузки
        const char FileName[SIZE])      // Имя файла
    {
        HTHANDLE* ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);

        // Создание файла
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
            strncpy_s(ht->LastErrorMessage, "Не удалось создать файл", SIZE);
            delete ht;
            return nullptr;
        }

        // Вычисление общего необходимого размера
        DWORD fileSize = sizeof(HTHANDLE) + (Capacity * (sizeof(Element) + MaxKeyLength + MaxPayloadLength));

        // Установка размера файла
        SetFilePointer(ht->File, fileSize, NULL, FILE_BEGIN);
        SetEndOfFile(ht->File);

        // Создание проекции файла (file mapping)
        ht->FileMapping = CreateFileMappingA(
            ht->File,
            NULL,
            PAGE_READWRITE,
            0,
            fileSize,
            NULL
        );

        if (ht->FileMapping == NULL) {
            strncpy_s(ht->LastErrorMessage, "Не удалось создать проекцию файла", SIZE);
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        // Отображение представления файла в память
        ht->Addr = MapViewOfFile(
            ht->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            fileSize
        );

        if (ht->Addr == NULL) {
            strncpy_s(ht->LastErrorMessage, "Не удалось отобразить представление файла", SIZE);
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        // Копирование структуры HTHANDLE в отображенную память
        memcpy(ht->Addr, ht, sizeof(HTHANDLE));

        ht->lastsnaptime = time(nullptr); // Установка времени последнего снимка
        return ht;
    }

    HTHANDLE* Open(const char FileName[SIZE]) // Открытие существующей хеш-таблицы
    {
        HTHANDLE* ht = new HTHANDLE();
        strncpy_s(ht->FileName, FileName, SIZE);

        // Открытие файла
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
            strncpy_s(ht->LastErrorMessage, "Не удалось открыть файл", SIZE);
            delete ht;
            return nullptr;
        }

        // Получение размера файла
        DWORD fileSize = GetFileSize(ht->File, NULL);

        // Создание проекции файла
        ht->FileMapping = CreateFileMappingA(
            ht->File,
            NULL,
            PAGE_READWRITE,
            0,
            fileSize,
            NULL
        );

        if (ht->FileMapping == NULL) {
            strncpy_s(ht->LastErrorMessage, "Не удалось создать проекцию файла", SIZE);
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        // Отображение представления файла в память
        ht->Addr = MapViewOfFile(
            ht->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            fileSize
        );

        if (ht->Addr == NULL) {
            strncpy_s(ht->LastErrorMessage, "Не удалось отобразить представление файла", SIZE);
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        // Копирование структуры HTHANDLE из отображенной памяти
        memcpy(ht, ht->Addr, sizeof(HTHANDLE));

        return ht;
    }

    BOOL Snap(const HTHANDLE* hthandle) // Создание снимка (сохранение состояния)
    {
        if (hthandle == nullptr || hthandle->Addr == nullptr) {
            return FALSE;
        }

        // Сброс представления для гарантии записи данных на диск
        if (!FlushViewOfFile(hthandle->Addr, 0)) {
            return FALSE;
        }

        // Обновление времени последнего снимка
        HTHANDLE* nonConstHt = const_cast<HTHANDLE*>(hthandle);
        nonConstHt->lastsnaptime = time(nullptr);
        memcpy(nonConstHt->Addr, nonConstHt, sizeof(HTHANDLE));

        return TRUE;
    }

    BOOL Close(const HTHANDLE* hthandle) // Закрытие хеш-таблицы
    {
        if (hthandle == nullptr) {
            return FALSE;
        }

        BOOL result = TRUE;

        // Выполнение снимка
        if (!Snap(hthandle)) {
            result = FALSE;
        }

        // Освобождение ресурсов
        if (hthandle->Addr != nullptr) {
            UnmapViewOfFile(hthandle->Addr);
        }

        if (hthandle->FileMapping != NULL) {
            CloseHandle(hthandle->FileMapping);
        }

        if (hthandle->File != INVALID_HANDLE_VALUE) {
            CloseHandle(hthandle->File);
        }

        // Удаление объекта HTHANDLE
        delete hthandle;

        return result;
    }

    BOOL Insert(const HTHANDLE* hthandle, const Element* element) // Вставка элемента
    {
        if (hthandle == nullptr || element == nullptr || element->key == nullptr) {
            return FALSE;
        }

        // Проверка существования ключа
        Element* existing = Get(hthandle, element);
        if (existing != nullptr) {
            delete existing;
            strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
                "Ключ уже существует", SIZE);
            return FALSE;
        }

        // Поиск пустого слота и вставка
        char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);

        // Копируем элемент
        memcpy(dataStart, element, sizeof(Element));

        // Проверка необходимости создания снимка
        time_t currentTime = time(nullptr);
        if (difftime(currentTime, hthandle->lastsnaptime) >= hthandle->SecSnapshotInterval) {
            Snap(hthandle);
        }

        return TRUE;
    }

    BOOL Delete(const HTHANDLE* hthandle, const Element* element) // Удаление элемента
    {
        if (hthandle == nullptr || element == nullptr || element->key == nullptr) {
            return FALSE;
        }

        // Поиск элемента
        Element* found = Get(hthandle, element);
        if (found == nullptr) {
            strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
                "Ключ не найден", SIZE);
            return FALSE;
        }

        // Пометить как удаленный
        char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);
        memset(dataStart, 0, sizeof(Element));

        delete found;

        // Проверка необходимости создания снимка
        time_t currentTime = time(nullptr);
        if (difftime(currentTime, hthandle->lastsnaptime) >= hthandle->SecSnapshotInterval) {
            Snap(hthandle);
        }

        return TRUE;
    }

    Element* Get(const HTHANDLE* hthandle, const Element* element) // Получение элемента
    {
        if (hthandle == nullptr || element == nullptr || element->key == nullptr) {
            return nullptr;
        }

        // Поиск в хеш-таблице
        char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);
        Element* storedElement = reinterpret_cast<Element*>(dataStart);

        // Проверка, является ли это искомый элемент
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
        const void* newpayload, int newpayloadlength) // Обновление элемента
    {
        if (hthandle == nullptr || oldelement == nullptr || newpayload == nullptr) {
            return FALSE;
        }

        // Поиск элемента
        Element* found = Get(hthandle, oldelement);
        if (found == nullptr) {
            strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
                "Ключ не найден", SIZE);
            return FALSE;
        }

        // Обновление полезной нагрузки
        char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);
        Element* storedElement = reinterpret_cast<Element*>(dataStart);

        // Выделение памяти
        storedElement->payload = newpayload;
        storedElement->payloadlength = newpayloadlength;

        delete found;

        // Проверка необходимости создания снимка
        time_t currentTime = time(nullptr);
        if (difftime(currentTime, hthandle->lastsnaptime) >= hthandle->SecSnapshotInterval) {
            Snap(hthandle);
        }

        return TRUE;
    }

    char* GetLastError(HTHANDLE* ht) // Получение последнего сообщения об ошибке
    {
        if (ht == nullptr) {
            char buffer[] = "Неверный HTHANDLE";
            return buffer;
        }
        return ht->LastErrorMessage;
    }

    void print(const Element* element) // Вывод элемента
    {
        if (element == nullptr) {
            cout << "Элемент равен null" << endl;
            return;
        }

        cout << "Ключ: ";
        if (element->key != nullptr) {
            for (int i = 0; i < element->keylength; i++) {
                cout << static_cast<const char*>(element->key)[i];
            }
        }
        cout << endl;

        cout << "Полезная нагрузка: ";
        if (element->payload != nullptr) {
            for (int i = 0; i < element->payloadlength; i++) {
                cout << static_cast<const char*>(element->payload)[i];
            }
        }
        cout << endl;
    }
}