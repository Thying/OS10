#include "HT.h"
using namespace HT;

Element::Element() :
    key(nullptr),
    keylength(0),
    payload(nullptr),
    payloadlength(0)
{}

Element::Element(const void* key, int keylength) :
    key(key),
    keylength(keylength),
    payload(nullptr),
    payloadlength(0)
{}

Element::Element(const void* key, int keylength, const void* payload, int payloadlength) :
    key(key),
    keylength(keylength),
    payload(payload),
    payloadlength(payloadlength)
{}

Element::Element(Element* oldelement, const void* newpayload, int newpayloadlength) :
    key(oldelement->key),
    keylength(oldelement->keylength),
    payload(newpayload),
    payloadlength(newpayloadlength)
{}