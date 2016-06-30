/**
 * @file   connect.h
 * @author Fridolin Pokorny fridex.devel@gmail.com
 * @brief  Connection implementation and service discover.
 */

#ifndef CONNECT_H_
#define CONNECT_H_

#include "tcpsearch.h"

#include <iostream>
#include <string>

/**
 * @brief Singleton to estamblish connection and receive banner info
 */
class Connect {
  public:
    static Connect & get_instace();

    bool examine(std::string & service, port_t port, const char * host);
    void close_socket();
    static void pretty_host_print(std::ostream & out, const std::string & host);
    static bool try_translate(const std::string & host);
    bool established() { return m_established; }

  private:
    bool examine_ipv4(std::string & service, struct sockaddr_in * sockaddr, port_t port);
    bool examine_ipv6(std::string & service, struct sockaddr_in6 * sockaddr, port_t port);
    bool read_service(std::string & service);

    Connect();
    ~Connect();

    const int kNoSocket;    ///<! no socket was opened
    int m_socket;           ///<! opened socket to read from
    bool m_established;     ///<! true if connection was established

    // dissallow copy and assign
    DISABLE_COPY_AND_ASSIGN(Connect);
}; // class Connect

#endif // CONNECT_H_

