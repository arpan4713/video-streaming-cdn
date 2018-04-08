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