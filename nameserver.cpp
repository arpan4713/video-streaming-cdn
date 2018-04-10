#include "nameserver.h"
#include "dns_lib.h"

int main(int argc, char const *argv[]) {
    if (argc != 5)
    {
        perror("Usage: ./nameserver <log> <port> <geography_based> <servers>");
        return -1;
    }

    string log = argv[1];
    string port = argv[2];
    int g = atoi(argv[3]);
    string servers = argv[4];

    Nameserver ns(log, port, g, servers);
    ns.dns_listen();

    return 0;
}


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void Nameserver::dns_listen() {
    std::ofstream log;
    log.open(logfile);

    /* Start UDP Server */
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXPACKETSIZE];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, listen_port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    DNSMessage message; // deserialization assumes both hosts use same platform/architecture

    addr_len = sizeof(their_addr);
    if ((numbytes = recvfrom(sockfd, reinterpret_cast<char*>(&message), MAXPACKETSIZE-1 , 0,
                             (struct sockaddr *)&their_addr, (socklen_t *) addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    printf("listener: got packet from %s\n",
           inet_ntop(their_addr.ss_family,
                     get_in_addr((struct sockaddr *)&their_addr),
                     s, sizeof s));
    printf("listener: packet is %d bytes long\n", numbytes);

    cout << message.question.QNAME << endl;

    strcpy(message.answer.RDATA, get_next_addr().c_str());

    // send to client the DNS response
//    if ((numbytes = sendto(sockfd, reinterpret_cast<const char*>(&message), sizeof(message), 0,
//                           (struct sockaddr *)&their_addr, (socklen_t *) addr_len)) == -1) {
//        exit(1);
//    }

    close(sockfd);
    /* End UDP Server */

    log.close();
}

const char* get_value(string str, string key) {
    int key_index = str.find(key);
    int space_index = str.find(' ', key_index);
    int end = str.find('\n', key_index);
    return str.substr(space_index + 1, end - space_index - 1).c_str();
}

void Nameserver::parse_rr_addrs(string fname) {
    std::ifstream file(fname);
    std::string str;
    while (std::getline(file, str))
    {
        ip_addrs.push_back(str);
    }
}

// populate network topo using file
void Nameserver::parse_network_topo(string fname) {
    using std::string;
    using std::getline;
    std::ifstream file(fname);
    string line;
    getline(file, line);
    int num_nodes = atoi(get_value(line, "NUM_NODES"));
    V = num_nodes;

    // initialize data structs
    for (int i = 0; i < num_nodes; i++) {
        vector<int> v;
        node_types.push_back("");
        node_ips.push_back("");
        dist.push_back(INT_MAX);
        for (int j = 0; j < num_nodes; j++) {
            v.push_back(0);
        }
        network_topo.push_back(v);
    }

    while (num_nodes > 0) {
        string node_id, node_type, node_ip;
        getline(file, node_id, ' ');
        getline(file, node_type, ' ');
        getline(file, node_ip);
        int id = atoi(node_id.c_str());
        node_types[id] = node_type;
        node_ips[id] = node_ip;
        num_nodes--;
    }

    getline(file, line);
    int num_links = atoi(get_value(line, "NUM_LINKS"));
    while (num_links > 0) {
        string source, dest, cost;
        getline(file, source, ' ');
        getline(file, dest, ' ');
        getline(file, cost);

        int s, d, c;
        s = atoi(source.c_str());
        d = atoi(dest.c_str());
        c = atoi(cost.c_str());
        network_topo[s][d] = c;
        network_topo[d][s] = c;

        num_links--;
    }
}

int Nameserver::min_dist(bool visited[]) {
    int min = INT_MAX, min_index;

    for (int v = 0; v < V; v++)
        if (!visited[v] && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}

string Nameserver::get_next_addr() {
    if (geography_based == 0) {
        string result = ip_addrs[rr_index];
        rr_index = (rr_index+1)%ip_addrs.size();
        return result;
    } else {
        // get shortest path from src to a server node
        int src = -1;
        for (int i = 0; i < node_ips.size(); i++) {
            if (node_ips[i] == proxy_ip && node_types[i] == "CLIENT") {
                src = i;
            }
        }
        if (src == -1) {
            return nullptr; // TODO: handle error, proxy IP does not correspond to a node
        }

        bool visited[V];

        for (int i = 0; i < V; i++)
            visited[i] = false;
        dist[src] = 0;

        // Find shortest path for all vertices
        for (int count = 0; count < V-1; count++)
        {
            int u = min_dist(visited);

            // Mark the picked vertex as processed
            visited[u] = true;
            // Update dist value of the adjacent vertices of the picked vertex.
            for (int v = 0; v < V; v++)
                if (!visited[v] && network_topo[u][v] && dist[u] != INT_MAX
                    && dist[u]+network_topo[u][v] < dist[v])
                    dist[v] = dist[u] + network_topo[u][v];
        }

        // print the constructed distance array
        int min = INT_MAX;
        int min_node_id = -1;
        for (int i = 0; i < dist.size(); i++) {
            if (dist[i] <= min && node_types[i] == "SERVER") {
                min = dist[i];
                min_node_id = i;
            }
        }

        return node_ips[min_node_id];
    }
}