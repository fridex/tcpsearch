/**
 * @file   host.cpp
 * @author Fridolin Pokorny fridex.devel@gmail.com
 * @brief  Implementation of host getter and host parser.
 */

#include "host.h"

#include <iostream>
#include <cstring>
#include <cerrno>

/**
 * Constructor.
 */
Host::Host()
{
    m_file = NULL;
}

/**
 * Destructor.
 */
Host::~Host()
{
    // close file if opened
    if (std::ifstream * file = dynamic_cast<std::ifstream *>(m_file)) {
        file->close();
        delete m_file;
        m_file = NULL;
    }
}

/**
 * Initialize host getter.
 *
 * @param  filename name of the file with hosts
 * @return false if an error occourred (e.g. file does not exist)
 */
bool Host::init(const std::string & filename)
{
    std::ifstream * file;
    if (filename != "-") {
        file = new std::ifstream();

        file->open(filename.c_str(), std::ifstream::in);
        if (! file->is_open()) {
            std::cerr << "Err: "
                      << filename << ": "
                      << std::strerror(errno) << std::endl;
            return false;
        }

        m_file = file;
    } else
        m_file = &std::cin;

    m_filename = filename;

    return true;
}

/**
 * Remove blank space before and after the word.
 *
 * @param  s string to be trucated
 * @return void
 */
void Host::remove_blank(std::string & s)
{
    // remove white space at the beginning of string
    int start;
    for (start = 0; isspace(s[start]); ++start)
        continue;

    // now find the end
    int len = 0;
    for (int stop = start;
         ! isspace(s[stop]) && s[stop] != '\0';
         ++stop, ++len)
        continue;

    s = s.substr(start, len);
}


/**
 * Get next host from file with hosts.
 *
 * @return string containing host
 */
bool Host::next_host()
{
    m_host.clear();

    do {
        std::getline(*m_file, m_host);
        // skip blank lines or lines which are commented out
    } while ((m_host.empty() || m_host[0] == '#') && ! m_file->eof());

    if (m_file->eof()) {
        return false;
    }

    remove_blank(m_host);

    return true;
}

/**
 * Access host string.
 *
 * @return host string
 */
const std::string & Host::host() const
{
    return m_host;
}

