#include "myTcp.hpp"

class rdmaTcp : public manyToMany
{
public:
    rdmaTcp(string ip);

    map<string, string> readRDMAInfo(int sock);
    void send_msg(int index, string msg);

    vector<int> getValidSock();

};