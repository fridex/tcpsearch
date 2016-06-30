/**
 * @file   connect.cpp
 * @author Fridolin Pokorny fridex.devel@gmail.com
 * @brief  Connection implementation and service discover.
 */

#include "connect.h"

#include "arg.h"
#include "arg-inl.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * Constructor.
 */
Connect::Connect() : kNoSocket(-1)
{
    // no socket is opened
    m_socket = kNoSocket;
}

/**
 * Destructor.
 */
Connect::~Connect()
{
}

/**
 * Access singleton
 *
 * @return singleton instance
 */
Connect & Connect::get_instace()
{
    static Connect instance;
    return instance;
}

/**
 * Close sockets on signal
 *
 * @param signum signal number
 * @return void
 */
static void connect_socket_close(int signum)
{
    UNUSED(signum);

    Connect::get_instace().close_socket();

    if (Arg::get_instace().verbose())
        std::cerr << "Warn: Connection timeout!\n";
    else if (Connect::get_instace().established()) // leave blank line if connection was estambished
        std::cout << std::endl;

    exit(0);
}

/**
 * Close socket if opened
 *
 * @return void
 */
void Connect::close_socket()
{
    if (m_socket != kNoSocket)
        close(m_socket);

    m_socket = kNoSocket;
}

/**
 * Try to connect using ipv4 connection.
 *
 * @param service service to determinate
 * @param sockaddr initialized sockaddr_in for connect
 * @param port a port to connect to
 */
bool Connect::examine_ipv4(std::string & service,
                           struct sockaddr_in * sockaddr,
                           port_t port)
{
    // create socket
    signal(SIGTERM, connect_socket_close);
    m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    // estamblish connection...
    if (connect(m_socket, reinterpret_cast<struct sockaddr *>(sockaddr),
                sizeof(struct sockaddr_in)) < 0) {
        // unable connect to given port
        if (Arg::get_instace().verbose())
            std::cerr << "Warn: Cannot connect to given port: "
                      << std::strerror(errno)
                      << std::endl;

        return false;
    }

    // we are connected, print port
    if (! Arg::get_instace().verbose())
        std::cout << port << std::endl;
    m_established = true;

    bool ret = read_service(service);

    // now we can close the socket
    close_socket();

    return ret;

}

/**
 * Try to connect using ipv6 connection.
 *
 * @param service service to determinate
 * @param sockaddr initialized sockaddr_in for connect
 * @param port a port to connect to
 */
bool Connect::examine_ipv6(std::string & service,
                           struct sockaddr_in6 * sockaddr,
                           port_t port)
{
    // create socket
    signal(SIGTERM, connect_socket_close);
    m_socket = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);

    // estamblish connection...
    if (connect(m_socket, reinterpret_cast<struct sockaddr *>(sockaddr),
                sizeof(struct sockaddr_in6)) < 0) {
        // unable connect to given port
        if (Arg::get_instace().verbose())
            std::cerr << "Warn: Cannot connect to given port: "
                      << std::strerror(errno)
                      << std::endl;

        return false;
    }

    // we are connected, print port
    if (! Arg::get_instace().verbose())
        std::cout << port << std::endl;
    m_established = true;

    bool ret = read_service(service);

    // now we can close the socket
    close_socket();

    return ret;
}

/**
 * Check for connection on port `port' on host `host' and return service name
 * (if any).
 *
 * @param   service service name for callee
 * @param   port port number to connect to
 * @param   host host name to connect to (IPv4, IPv6 or domain)
 * @return  true if connection was estambished and service contains service name
 */
bool Connect::examine(std::string & service, port_t port, const char * host)
{
    service.erase(); // clear the output
    m_established = false;

    struct addrinfo     * res = NULL;
    struct sockaddr_in  * addr;
    struct sockaddr_in6 * addr6;

    int ret = getaddrinfo(host, NULL, NULL, &res);

    // if verbose, always print port
    if (Arg::get_instace().verbose())
        std::cout << port << std::endl;

    // use first family with ipv6 or ipv4
    if (! ret) {
        for (struct addrinfo * result = res; result != NULL; result = result->ai_next) {
            switch (result->ai_family) {
                case AF_INET:
                    addr = reinterpret_cast<struct sockaddr_in *>(result->ai_addr);
                    addr->sin_port = htons(port);
                    freeaddrinfo(res);
                    return examine_ipv4(service, addr, port);
                    break;

                case AF_INET6:
                    addr6 = reinterpret_cast<struct sockaddr_in6 *>(result->ai_addr);
                    addr6->sin6_port = htons(port);
                    freeaddrinfo(res);
                    return examine_ipv6(service, addr6, port);
                    break;

               default:
                    break;
            }
        }
    }

    // this should never fail, try_translate() was called to be sure it can be
    // translated
    if (! ret)
        std::cerr << "Err: failed to get info\n";

    return false;
}

/**
 * Create string for user output.
 *
 * @param out output stream used to output the string
 * @param host host to print
 * @return void
 */
void Connect::pretty_host_print(std::ostream & out, const std::string & host)
{
    const int STR_SIZE = 80;

    struct addrinfo * result;
    struct addrinfo * res;
    char str[STR_SIZE];

    // have we a record for ipv4 or ipv6?
    bool ip6_printed = false;
    bool ip4_printed = false;
    bool ip_print = true;

    // remembered adresses
    std::string ip6;
    std::string ip4;

    // translate host!
    if (! getaddrinfo(host.c_str() , NULL, NULL, &result)) {
        for (res = result; res != NULL; res = res->ai_next) {
            switch (res->ai_family) {
                case AF_INET:
                    struct sockaddr_in  *sockaddr_ipv4;
                    sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
                    inet_ntop(AF_INET, static_cast<void *>(&sockaddr_ipv4->sin_addr),
                              str, STR_SIZE);

                    if (! strcmp(str, host.c_str()))
                        ip_print = false;
                    else if (! ip4_printed) {
                        ip4_printed = true;
                        ip4 = str;
                    }

                case AF_INET6:
                    struct sockaddr_in6  *sockaddr_ipv6;
                    sockaddr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(res->ai_addr);
                    inet_ntop(AF_INET6, static_cast<void *>(&sockaddr_ipv6->sin6_addr),
                              str, STR_SIZE);

                    if (! strcmp(str, host.c_str()))
                        ip_print = false;
                    else if (! ip6_printed && strncmp(str, "::", 2)) {
                        ip6 = str;
                        ip6_printed = true;
                    }
                    break;

                default:
                    break;
            }
        }

        freeaddrinfo(result);
    }

    // prefer ipv6 when available
    if (ip6_printed && ip_print)
        out << ip6;
    else if (ip4_printed && ip_print)
        out << ip4;

    if ((ip4_printed || ip6_printed) && ip_print)
        out << " (" << host << ")\n";
    else
        out << host << std::endl;
}

/**
 * Try to translate address to ipv4 or ipv6.
 *
 * @param host host to be translated
 * @return false if translation failed
 */
bool Connect::try_translate(const std::string & host)
{
    struct addrinfo * res;

    int ret = getaddrinfo(host.c_str() , NULL, NULL, &res);

    if (ret || res == NULL) {
        switch(ret) {
// on some systems (e.g. BSD) macros EAI_NODATA and EAI_ADDRFAMILY are marked as
// obsolete
#if defined(EAI_NODATA)
#if defined(EAI_ADDRFAMILY)
            case EAI_NODATA:
            case EAI_ADDRFAMILY:
                std::cerr << "Err: Server not found\n";
                return false;
                break;
#endif
#endif

            case EAI_NONAME:
                std::cerr << "Err: unknown service or name\n";
                return false;
                break;

            case NO_RECOVERY:
            case EAI_AGAIN:
            case EAI_FAIL:
            default:
                std::cerr << "Err: DNS error\n";
                return false;
                break;
        }

        std::cerr << "Err: error while retrieving host info\n";

        return false;
    }

    if (! ret)
        freeaddrinfo(res);

    return ret == 0;
}

/**
 * Read data from opened socket.
 *
 * @param service service which was read from socket
 * @return false on error
 */
bool Connect::read_service(std::string & service)
{
    char buf;

    for (;;) {
        if (read(m_socket, &buf, 1) < 0) {
            std::cerr << "Err: read: " << std::strerror(errno) << std::endl;
            return false;
        }

        // do not put \r to service name
        if (buf == '\r')
            continue;

        // read till end-of-line
        if (buf == '\n')
            break;

        service.append(1, buf);
    }

    return true;
}

