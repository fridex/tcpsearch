/**
 * @file   tcpsearch.h
 * @author Fridolin Pokorny fridex.devel@gmail.com
 * @brief  Port scanner and service discover tool.
 */

#ifndef TCPSEARCH_H_
#define TCPSEARCH_H_

typedef unsigned port_t;
typedef unsigned domain_t;
typedef unsigned delay_t;

// dissallow copy and assign to classes
#define DISABLE_COPY_AND_ASSIGN(Class)  \
  Class(const Class &);                 \
  Class &operator=(const Class &);

#define UNUSED(X)   ((void) X)

#endif // TCPSEARCH_H_

