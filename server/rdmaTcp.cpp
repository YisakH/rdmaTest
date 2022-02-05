#include <rdmaTcp.hpp>

rdmaTcp::rdmaTcp(string ip):manyToMany(ip){}

map<string, string> rdmaTcp::readRDMAInfo()
{
    map<string, string> info;
    string info_name[6] = {"addr", "len", "lkey", "rkey", "lid", "qp_num"};

    vector<int> sockList = getValidSock();
    if(sockList.size() <= 0){
        printf("유효한 소켓이 없습니다.\n");
        return {};
    }

    for(int i = 0; i < 6; i++){

        string result="";

        while(result.back() != '\n'){

            string result = recv_msg(sockList.back(), "111.111.111.111");
        }

        info.insert({info_name[i], result});
    }

    return info;
}

vector<int> rdmaTcp::getValidSock()
{
    return this->connect_clnt_sock;
}