#include "HT.h"
using namespace std;
using namespace HT;

#define PASSWORD_1 "key111"
#define ADDRESS_1 "D:\\HT_Space_1.ht"

#define PASSWORD_2 "key222"
#define ADDRESS_2 "D:\\HT_Space_2.ht"

int main()
{
    SetConsoleOutputCP(1251);

    cout << "С новыми:" << endl << endl;
    HTHANDLE* ht1 = nullptr;
    HTHANDLE* ht2 = nullptr;
    try
    {
        // Создание первой хеш-таблицы
        ht1 = Create(1000, 3, 10, 256, ADDRESS_1);
        if (ht1)
            cout << "Create 1: success" << endl;
        else throw "Create 1: error";

        // Создание второй хеш-таблицы
        ht2 = Create(1000, 3, 10, 256, ADDRESS_2);
        if (ht2)
            cout << "Create 2: success" << endl;
        else throw "Create 2: error";

        // Вставка в первую таблицу
        if (Insert(ht1, new Element(PASSWORD_1, 7, "payload", 8)))
            cout << "Insert 1: success" << endl;
        else throw "Insert 1: error";

        // Вставка во вторую таблицу
        if (Insert(ht2, new Element(PASSWORD_2, 7, "payload", 8)))
            cout << "Insert 2: success" << endl;
        else throw "Insert 2: error";

        // Получение из первой таблицы
        Element* hte1 = Get(ht1, new Element(PASSWORD_1, 7));
        if (hte1)
            cout << "Get 1: success" << endl;
        else throw "Get 1: error";

        // Получение из второй таблицы
        Element* hte2 = Get(ht2, new Element(PASSWORD_2, 7));
        if (hte2)
            cout << "Get 2: success" << endl;
        else throw "Get 2: error";

        // Печать элементов
        cout << "HT1: ";
        print(hte1);
        cout << "HT2: ";
        print(hte2);

        // Снимок первой таблицы
        if (Snap(ht1))
            cout << "Snap 1: success" << endl;
        else throw "Snap 1: error";

        // Снимок второй таблицы
        if (Snap(ht1))
            cout << "Snap 2: success" << endl;
        else throw "Snap 2: error";

        // Обновление элемента в первой таблице
        if (Update(ht1, hte1, "newpayload", 11))
            cout << "Update 1: success" << endl;
        else throw "Update 1: error";

        // Обновление элемента в первой таблице
        if (Update(ht2, hte2, "newpayload", 11))
            cout << "Update 2: success" << endl;
        else throw "Update 2: error";

        // Получение обновленного элемента
        Element* hte1_updated = Get(ht1, new Element(PASSWORD_1, 7));
        if (hte1_updated)
            cout << "Get updated 1: success" << endl;
        else throw "Get updated 1: error";

        Element* hte2_updated = Get(ht2, new Element(PASSWORD_2, 7));
        if (hte2_updated)
            cout << "Get updated 2: success" << endl;
        else throw "Get updated 2: error";

        cout << "Updated 1: ";
        print(hte1_updated);

        cout << "Updated 2: ";
        print(hte2_updated);

        // Закрытие таблиц
        if (Close(ht1))
            cout << "Close 1: success" << endl;
        else throw "Close 1: error";

        if (Close(ht2))
            cout << "Close 2: success" << endl;
        else throw "Close 2: error";

    }
    catch (const char* msg)
    {
        cout << msg << endl;
        if (ht1 != nullptr) cout << "HT1 Error: " << GetLastError(ht1) << endl;
        if (ht2 != nullptr) cout << "HT2 Error: " << GetLastError(ht2) << endl;
    }

    cout << endl << "С уже существующим:" << endl << endl;
    HTHANDLE* ht_1_old = nullptr;
    HTHANDLE* ht_2_old = nullptr;
    try
    {
        ht_1_old = Open(ADDRESS_1);
        if (ht_1_old) 
            cout << "Open 1: success" << endl;
        else throw " Open 1: error";

        ht_2_old = Open(ADDRESS_2);
        if (ht_2_old)
            cout << "Open 2: success" << endl;
        else throw " Open 2: error";

        Element* hte_1_old = Get(ht_1_old, new Element(PASSWORD_1, 7));
        if (hte_1_old)
            cout << "Get 1: success" << endl;
        else throw "Get 1: error";

        Element* hte_2_old = Get(ht_2_old, new Element(PASSWORD_2, 7));
        if (hte_2_old)
            cout << "Get 2: success" << endl;
        else throw "Get 2: error";

        cout << "1:" << endl;
        print(hte_1_old);

        cout << "2:" << endl;
        print(hte_2_old);

        if (Delete(ht_1_old, hte_1_old))
            cout << "Delete 1: success" << endl;
        else throw "Delete 1: error";

        if (Delete(ht_2_old, hte_2_old))
            cout << "Delete 2: success" << endl;
        else throw "Delete 2: error";

        if (Close(ht_1_old)) 
            cout << "Close 1: success" << endl;
        else throw "Close 1: error";

        if (Close(ht_2_old))
            cout << "Close 2: success" << endl;
        else throw "Close 2: error";
    }
    catch (const char* msg)
    {
        cout << msg << endl;
        if (ht_1_old != nullptr) cout << "HT1 Error: " << GetLastError(ht_1_old) << endl;
        if (ht_2_old != nullptr) cout << "HT2 Error: " << GetLastError(ht_2_old) << endl;
    }

    return 0;
}