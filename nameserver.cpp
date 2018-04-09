#include "nameserver.h"

int main(int argc, char const *argv[]) {
    if (argc != 5)
    {
        perror("Usage: ./nameserver <log> <port> <geography_based> <servers>");
        return -1;
    }

    string log = argv[1];
    int port = atoi(argv[2]);
    int g = atoi(argv[3]);
    string servers = argv[4];

    Nameserver ns(log, port, g, servers);
    ns.dns_listen();

    cout << log << port << g << servers << endl;
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

    int sock_fd, browser_sockfd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int yes = 1;
    char request[MAXPACKETSIZE], response[MAXPACKETSIZE];

    // create socket using TCP
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1) {
        perror("setsockopt");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( listen_port );

    // attach socket to port
    int bind_result = bind(sock_fd, (struct sockaddr *) &address, sizeof(address));
    if (bind_result<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen on socket
    if (listen(sock_fd, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // get DNS request from client
    if ((browser_sockfd = accept(sock_fd, (struct sockaddr *)&address,
                                 (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    inet_ntop(address.sin_family,
              get_in_addr((struct sockaddr *)&address),
              buffer, sizeof buffer);

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