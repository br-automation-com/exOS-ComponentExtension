#include "termination.h"
#include <signal.h>
#include <stdlib.h>

static bool terminate_process = false;

bool is_terminated()
{
    return terminate_process;
}

static void handle_term_signal(int signum)
{
    switch (signum)
    {
    case SIGINT:
    case SIGTERM:
    case SIGQUIT:
        terminate_process = true;
        break;

    default:
        break;
    }
}

void catch_termination()
{
    struct sigaction new_action;

    // Register termination handler for signals with termination semantics
    new_action.sa_handler = handle_term_signal;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    // Sent via CTRL-C.
    sigaction(SIGINT, &new_action, NULL);

    // Generic signal used to cause program termination.
    sigaction(SIGTERM, &new_action, NULL);

    // Terminate because of abnormal condition.
    sigaction(SIGQUIT, &new_action, NULL);
}
