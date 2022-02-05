#include "myTcp.hpp"

class rdmaTcp : public manyToMany
{
public:
    rdmaTcp(string ip);

    map<string, string> readRDMAInfo();

    vector<int> getValidSock();

};