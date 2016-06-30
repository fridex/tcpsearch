/**
 * @file   tcpsearch.cpp
 * @author Fridolin Pokorny fridex.devel@gmail.com
 * @brief  Port scanner and service discover tool.
 */

#include "tcpsearch.h"

#include <iostream>
#include <algorithm>

#include <unistd.h>

#include <sys/wait.h>
#include <signal.h>

#include "arg.h"
#include "arg-inl.h"
#include "connect.h"
#include "host.h"

/**
 * @brief Return values from main()
 */
enum ret_t {
    RET_OK,           ///<! No error occourred
    RET_E_PARAM,      ///<! Bad arguments
    RET_E_HOST_INIT,  ///<! Unable to init Host object (bad file?)
    RET_E_TCPSEARCH   ///<! There was an error during port scan
};

/**
 * Dummy for signal handler to exit sleep().
 *
 * @param sig signal num
 * @return void
 */
static void foo(int sig)
{
    /*
     * When SIGCHLD is emitted, sleep() is exited and we can continue execution.
     */
    UNUSED(sig);
}

/**
 * Main loop of the program.
 *
 * @param  port to connect to
 * @return false if an error occourred
 */
static bool tcpsearch(Port * port)
{
    std::string service;

    const std::string & host = Host::get_instace().host();

    for (unsigned i = port->port_from; i <= port->port_to; ++i) {
        int pid = fork();

        if (pid < 0) {
            std::cerr << "Err: unable to fork\n";
            return false;
        } else if (pid != 0) {
            // father will sleep before it kills childs depending on delay given
            if (Arg::get_instace().delay() != 0) {
                // if child exited, continue...
                signal(SIGCHLD, foo);
                sleep(Arg::get_instace().delay());
                kill(pid, SIGTERM);
                wait(NULL); // wait for killed child
            } else {
                wait(NULL);
            }
        } else {
            if (Connect::get_instace().examine(service, i, host.c_str())) {
                // if there was no error, print service name
                std::cout << service << std::endl;
            }

            exit(0);
        }
    }

    return true;
};

/**
 * Program's main()
 *
 * @param  argc argument count
 * @return argv argument vector
 */
int main(int argc, char * argv[])
{

    if (! Arg::get_instace().parse(argc, argv)) {
        return RET_E_PARAM;
    }

    if (! Host::get_instace().init(Arg::get_instace().filename())) {
        return RET_E_HOST_INIT;
    }

    // program's main loop
    while (Host::get_instace().next_host()) {

        Connect::pretty_host_print(std::cout, Host::get_instace().host());

        // check if server exists...
        if (! Connect::try_translate(Host::get_instace().host())) {
            continue;
        }


        for_each(Arg::get_instace().ports_begin(),
                 Arg::get_instace().ports_end(),
                 tcpsearch);
    }

    return RET_OK;
}

