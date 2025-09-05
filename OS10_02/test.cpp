#include "HT.h"
using namespace std;
using namespace HT;

#define PASSWORD "key111"
#define ADDRESS "D:\\HT_Space.ht"

int main()
{
    SetConsoleOutputCP(1251);

    cout << "С новым:" << endl << endl;
    HTHANDLE* ht = nullptr;
    try
    {
        ht = Create(1000, 3, 10, 256, ADDRESS);
        if (ht) 
            cout << "Create: success" << endl;
        else throw "Create: error";

        if (Insert(ht, new Element(PASSWORD, 7, "payload", 8))) 
            cout << "Insert:success" << endl;
        else throw "Insert:error";

        Element* hte = Get(ht, new Element(PASSWORD, 7));
        if (hte) 
            cout << "Get: success" << endl;
        else throw "Get: error";

        print(hte);

        if (Snap(ht)) 
            cout << "Snap: success" << endl;
        else throw "Snap: error";

        if (Update(ht, hte, "newpayload", 11)) 
            cout << "Update:success" << endl;
        else throw "Update:error";

        Element* htel = Get(ht, new Element(PASSWORD, 7));
        if (htel) 
            cout << "Get: success" << endl;
        else throw "Get: error";

        print(htel);

        if (Close(ht)) 
            cout << "Close: success" << endl;
        else throw "Close: error";

    }
    catch (const char* msg)
    {
        cout << msg << endl;
        if (ht != nullptr) cout << GetLastError(ht) << endl;
    }

    cout << endl << "С уже существующим:" << endl << endl;
    HTHANDLE* ht2 = nullptr;
    try
    {
        ht2 = Open(ADDRESS);
        if (ht2) 
            cout << "Open: success" << endl;
        else throw " Open: error";

        Element* hte = Get(ht2, new Element(PASSWORD, 7));
        if (hte)
            cout << "Get: success" << endl;
        else throw "Get: error";

        print(hte);

        if (Delete(ht, hte))
            cout << "Delete: success" << endl;
        else throw "Delete: error";

        if (Close(ht2)) 
            cout << "Close: success" << endl;
        else throw "Close: error";

    }
    catch (const char* msg)
    {
        cout << msg << endl;
        if (ht2 != nullptr) 
            cout << GetLastError(ht2) << endl;
    }

    return 0;
}