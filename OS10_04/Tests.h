#pragma once
#include "HT.h"
#include <cassert>

using namespace std;
using namespace HT;

void TestCreate
(
	int   capacity,                   // емкость хранилища в количестве элементов 
	int   secSnapshotInterval,        // периодичность сохранения в секундах
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename              // имя файла для хранения данных
);

void TestOpen
(
	int   capacity,                   // ожидаемая емкость хранилища
	int   secSnapshotInterval,        // ожидаемая периодичность сохранения
	int   maxKeyLength,               // ожидаемая максимальная длина ключа
	int   maxPayloadLength,           // ожидаемая максимальная длина данных
	const char* filename              // имя файла для открытия
);

void TestInsert
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key,                  // ключ для вставки
	const char* value                 // значение для вставки
);

void TestGet
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key,                  // ключ для поиска
	const char* value                 // ожидаемое значение
);

void TestGetNonExistent
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key                   // несуществующий ключ
);

void TestUpdate
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key,                  // ключ элемента для обновления
	const char* initialValue,         // начальное значение
	const char* newValue              // новое значение
);

void TestDelete
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key,                  // ключ элемента для удаления
	const char* value                 // значение элемента
);

void TestDeleteNonExistent
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key                   // несуществующий ключ
);

void TestDuplicateInsert
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key,                  // ключ для дублирования
	const char* value1,               // первое значение
	const char* value2                // второе значение
);

void TestSnap
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key,                  // ключ тестового элемента
	const char* value                 // значение тестового элемента
);

void TestClose
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename              // имя файла
);

void TestGetLastError
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key                   // ключ для создания ошибки
);

void TestPrint
(
	int   capacity,                   // емкость хранилища
	int   secSnapshotInterval,        // периодичность сохранения
	int   maxKeyLength,               // максимальная длина ключа
	int   maxPayloadLength,           // максимальная длина данных
	const char* filename,             // имя файла
	const char* key,                  // ключ элемента для печати
	const char* value                 // значение элемента для печати
);