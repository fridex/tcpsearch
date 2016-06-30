/**
 * @file   arg.h
 * @author Fridolin Pokorny fridex.devel@gmail.com
 * @brief  Command line arguments parsing and representation.
 */

#ifndef ARG_H_
#define ARG_H_

#include <istream>
#include <list>
#include <string>

#include "tcpsearch.h"

/**
 * @brief Port range specification.
 */
class Port {
  public:
    port_t port_from;
    port_t port_to;
};

typedef std::list<Port *> portlist_t;

/**
 * @brief Command-line arguments.
 */
class Arg {
  public:
    static Arg & get_instace();

    bool parse(int argc, char * argv[]);

    const std::string & filename() const;
    const delay_t       delay() const;
    bool                verbose() const;

    portlist_t::const_iterator ports_begin() const;
    portlist_t::const_iterator ports_end() const;

  private:
    Arg();
    ~Arg();

    void print_help(const char * progname) const;
    bool parse_ports(char * ports);
    bool parse_time(const char * ports);

    std::string m_filename;
    portlist_t  m_ports;
    delay_t     m_delay;
    bool        m_verbose;

    // dissallow copy and assign
    DISABLE_COPY_AND_ASSIGN(Arg);
}; // class Arg

#endif // ARG_H_

