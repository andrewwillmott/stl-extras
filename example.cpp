#include "vector_set.h"
#include "vector_map.h"
#include <string>

int main() {
    typedef std::vector_map<std::string, int> NameToInt;
    NameToInt map;

    map["bob"] = 88;
    map["mary"] = 99;
    map["cake"] = 11;

    auto it_and_add = map.insert({"wibble", 20});

    for (const NameToInt::KeyValue& key_value : map) {
        printf("%s -> %d\n", key_value.first.c_str(), key_value.second);
    }

    map.erase("wibble");

    printf("----\n");
    for (const NameToInt::KeyValue& key_value : map) {
        printf("%s -> %d\n", key_value.first.c_str(), key_value.second);
    }

    ////////////////////////////////////////////////////////////////////////////

    typedef std::vector_set<std::string> Names;
    Names names;

    names.insert("bob" );
    names.insert("mary");
    names.insert("cake");

    auto it = names.insert("wibble");

    printf("----\n");
    for (const std::string& key : names) {
        printf("%s\n", key.c_str());
    }

    names.erase("wibble");

    printf("----\n");
    for (const std::string& key : names) {
        printf("%s\n", key.c_str());
    }

    return 0;
}