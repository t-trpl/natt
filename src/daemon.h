#ifndef DAEMON_H
#define DAEMON_H

#include "types.h"

void* flux(void* arg);
gboolean on_clock_daemon(gpointer user_data);

#endif // DAEMON_H
