#include "myTcp.hpp"

class rdmaTcp : public manyToMany
{
public:
    rdmaTcp(string ip);

    map<string, string> readRDMAInfo(int sock);

    vector<int> getValidSock();

};