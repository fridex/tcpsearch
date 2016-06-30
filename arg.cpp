/**
 * @file   arg.cpp
 * @author Fridolin Pokorny fridex.devel@gmail.com
 * @brief  Command line arguments parsing and representation.
 */

#include "arg.h"
#include "arg-inl.h"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cassert>

#include "tcpsearch.h"

/**
 * Constructor.
 */
Arg::Arg()
{
    m_delay = 0;
    m_verbose = false;
}

/**
 * Destructor.
 */
Arg::~Arg()
{
    for (portlist_t::iterator it = m_ports.begin();
            it != m_ports.end();
            it = m_ports.erase(it)) {
        delete *it;
    }
}

/**
 * Access singleton instance
 *
 * @return singleton instance
 */
Arg & Arg::get_instace()
{
    static Arg instance;
    return instance;
}

/**
 * Parse command line arguments and initialize self.
 *
 * @param   argc argument count
 * @param   argv argument vector
 * @return  false on error
 */
bool Arg::parse(int argc, char * argv[])
{
    assert(argv);
    assert(argc >= 0);

    if (argc == 1) {
        std::cerr << "Err: No arguments provided\n";
        print_help(argv[0]);
        return false;
    }

    for (int i = 1; i < argc; ++i) {
        if (! strcmp(argv[i], "-v")) {
            if (m_verbose) {
                std::cerr << "Err: bad arguments\n";
                return false;
            } else
                m_verbose = true;
        } else if (! strcmp(argv[i], "-h")) {
            if (argc == 2) {
                print_help(argv[0]);
                return false;
            } else {
                std::cerr << "Err: bad arguments\n";
                return false;
            }
        } else if (! strcmp(argv[i], "-t")) {
            ++i;
            if (i == argc) {
                std::cerr << "Err: no time specified\n";
                return false;
            } else if (! parse_time(argv[i])) {
                std::cerr << "Err: bad time delay specified\n";
                return false;
            }
        } else if (! strcmp(argv[i], "-p")) {
            ++i;
            if (i == argc) {
                std::cerr << "Err: no ports specified\n";
                return false;
            } else if (! parse_ports(argv[i])) {
                std::cerr << "Err: bad port range\n";
                return false;
            }
        } else { // this should be input file
            if (m_filename.empty()) {
                m_filename = argv[i];
            } else {
                std::cerr << "Err: bad arguments\n";
                return false;
            }
        }
    }

    if (m_ports.empty()) {
        std::cerr << "Err: port range has to be specified\n";
        return false;
    } else if (m_filename.empty()) {
        std::cerr << "Err: file has to be specified (use '-' for stdin)\n";
        return false;
    }

    return true;
}

/**
 * Parse port range and initialize m_ports.
 *
 * @param   ports from command line
 * @return  false on error
 */
bool Arg::parse_ports(char * ports)
{
    assert(ports);

    char * ptr = ports;
    char * ptr2 = NULL;
    Port * tmp_port = NULL;
    long tmp;

    while (*ptr != '\0') {
        tmp_port = new Port();
        tmp = strtol(ptr, &ptr2, 10);

        // check if we have something read
        if (ptr2 == ptr) {
            delete tmp_port;
            return false;
        } else
            ptr = ptr2;

        // non-negative value
        if (tmp <= 0) {
            delete tmp_port;
            return false;
        }

        tmp_port->port_from = tmp;

        if (*ptr == '-') {
            ++ptr;
            tmp = strtol(ptr, &ptr2, 10);

            // check if we have something read
            if (ptr == ptr2) {
                delete tmp_port;
                return false;
            }

            if (tmp <= 0) {
                delete tmp_port;
                return false;
            }

            tmp_port->port_to = tmp;

            // from lower to upper port no
            if (tmp_port->port_from >= tmp_port->port_to) {
                delete tmp_port;
                return false;
            }

            ptr = ptr2;

            if (*ptr != ',' && *ptr != '\0') {
                delete tmp_port;
                return false;
            } else if (*ptr == ',') {
                ++ptr;
            }

        } else if (*ptr == ',' || *ptr == '\0') {
            // it is not allowed to end range with colon
            if (*ptr == ',' && ptr[1] == '\0') {
                delete tmp_port;
                return false;
            }

            if (*ptr != '\0')
                ++ptr;

            tmp_port->port_to = tmp_port->port_from;
        } else {
            delete tmp_port;
            return false;
        }

        m_ports.push_back(tmp_port);
    }

    return true;
}

/**
 * Parse time and initialize m_delay.
 *
 * @param   time from command line
 * @return  false on error
 */
bool Arg::parse_time(const char * time)
{
    assert(time);

    char * endptr;
    long delay;

    delay = strtol(time, &endptr, 10);

    // non-negative
    if (delay <= 0)
        return false;

    m_delay = delay;

    // was it all scanned?
    if (endptr == time + strlen(time))
        return true;
    else
        return false;
}

/**
 * Print help to stdout.
 *
 * @param  progname program name
 * @return void
 */
void Arg::print_help(const char * progname) const
{
    assert(progname);

    static const char * HELP_MSG_BEGIN =
        "tcpsearch - port discover tool\n"
        "Written by Fridolin Pokorny <fridex.devel@gmail.com>\n\n"
        "Usage:\n\t";

    static const char * HELP_MSG_END =
        " [-t TIME] [-v] -p PORT_RANGE FILE\n\n"
        "Options:\n"
        "\tFILE\t\t file whith domain names or IP addresses\n"
        "\t-t TIME\t\t specify wait time\n"
        "\t-p PORT_RANGE\t comma-separated list of ports and port ranges\n"
        "\t-v\t\t verbose info messages\n";

    std::cout << HELP_MSG_BEGIN << progname << HELP_MSG_END;
}

