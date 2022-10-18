//
// Created by xiaolei on 2022/4/13.
//

#ifndef MYPROJECT_CLUSTER_CLIENT_H
#define MYPROJECT_CLUSTER_CLIENT_H

#include <map>
#include <string>
#include <set>
#include <vector>
#include "../util/str.h"
#include "../server/cmd/Response.h"
#include "../raft/raftDataStructures.h"
#include "lightdb_client.h"

namespace lightdb {
/*
 * this is a client for a cluster*/
class ClusterClient {
private:
    LightdbClient readClient;
    LightdbClient writeClient;

    std::string remoteAddress;
    int port;

public:
    ClusterClient();

    int Connect(const std::string& address, int port);

    int Execute(Request request, Response& response);

    int Close();

    std::string ReadRemoteAddress();

    std::string WriteRemoteAddress();

    int ReadRemotePort();

    int WriteRemotePort();

};

}
#endif //MYPROJECT_CLUSTER_CLIENT_H
