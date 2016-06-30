/**
 * @file   arg-inl.h
 * @author Fridolin Pokorny fridex.devel@gmail.com
 * @brief  Command line arguments parsing and representation.
 */

#ifndef ARG_INL_H_
#define ARG_INL_H_

/**
 * Access `begin' of port list.
 *
 * @return constant iterator if portlist_t
 */
inline portlist_t::const_iterator Arg::ports_begin() const
{
    return m_ports.begin();
}

/**
 * Access `end' of port list.
 *
 * @return constant iterator if portlist_t
 */
inline portlist_t::const_iterator Arg::ports_end() const
{
    return m_ports.end();
}

/**
 * Get filename of file specified using arguments.
 *
 * @return name of the file
 */
inline const std::string & Arg::filename() const
{
    return m_filename;
}

/**
 * Get delay described using arguments.
 *
 * @return delay
 */
inline const delay_t Arg::delay() const
{
    return m_delay;
}

/**
 * Check if user wants verbose output.
 *
 * @return true if verbose was turned on
 */
inline bool Arg::verbose() const
{
    return m_verbose;
}

#endif // ARG_INL_H_

