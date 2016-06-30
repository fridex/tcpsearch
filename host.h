/**
 * @file   host.h
 * @author Fridolin Pokorny fridex.devel@gmail.com
 * @brief  Implementation of host getter and host parser.
 */

#ifndef HOST_H_
#define HOST_H_

#include "tcpsearch.h"

#include <string>
#include <fstream>

/**
 * @brief Host getter and parser
 */
class Host {
  public:
    bool init(const std::string & filename);
    static Host & get_instace() { static Host instance; return instance; }

    bool next_host();
    const std::string & host() const;

    const std::string & filename() { return m_filename; }

  private:
    Host();
    ~Host();

    std::string   m_filename;
    std::string   m_host;
    std::istream  * m_file;

    void remove_blank(std::string & s);

    // dissallow copy and assign
    DISABLE_COPY_AND_ASSIGN(Host);
}; // class Host

#endif // HOST_H_

