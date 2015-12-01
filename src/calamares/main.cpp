/* === This file is part of Calamares - <http://github.com/calamares> ===
 *
 *   Copyright 2014, Teo Mrnjavac <teo@kde.org>
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 */


#include "CalamaresApplication.h"

#include "kdsingleapplicationguard/kdsingleapplicationguard.h"
#include "utils/CalamaresUtils.h"
#include "utils/Logger.h"

#include <QCommandLineParser>
#include <QDebug>
#include <QDir>


/* get REG_EIP from ucontext.h */
#define __USE_GNU
#include <ucontext.h>

//#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
//#include <stdlib.h>
#include <unistd.h>


void handler(int sig) {
  void *array[100];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 100);

  // print out all the frames to stderr
//  fprintf(stderr, "Error: signal %d:\n", sig);
  fprintf(stdout, "Error: signal %d:\n", sig);
//  backtrace_symbols_fd(array, size, STDERR_FILENO);
  backtrace_symbols_fd(array, size, STDOUT_FILENO);
  exit(1);
}



void bt_sighandler(int sig, siginfo_t *info,
                   void *secret) {

  void *trace[16];
  char **messages = (char **)NULL;
  int i, trace_size = 0;
  ucontext_t *uc = (ucontext_t *)secret;

  /* Do something useful with siginfo_t */
  if (sig == SIGSEGV)
    printf("Got signal %d, faulty address is %p, "
           "from %p\n", sig, info->si_addr,
           uc->uc_mcontext.gregs[REG_EIP]);
  else
    printf("Got signal %d#92;\n", sig);

  trace_size = backtrace(trace, 16);
  /* overwrite sigaction with caller's address */
  trace[1] = (void *) uc->uc_mcontext.gregs[REG_EIP];

  messages = backtrace_symbols(trace, trace_size);
  /* skip first stack frame (points here) */
  printf("[bt] Execution path:#92;\n");
  for (i=1; i<trace_size; ++i)
  {
    printf("[bt] %s#92;\n", messages[i]);

    /* find first occurence of '(' or ' ' in message[i] and assume
     * everything before that is the file name. (Don't go beyond 0 though
     * (string terminator)*/
    size_t p = 0;
    while(messages[i][p] != '(' && messages[i][p] != ' '
            && messages[i][p] != 0)
        ++p;

    char syscom[256];
    sprintf(syscom,"addr2line %p -e %.*s", trace[i] , p, messages[i] );
           //last parameter is the filename of the symbol
    system(syscom);

  }
  exit(0);
}


int
main( int argc, char* argv[] )
{
    CalamaresApplication a( argc, argv );
//    signal(SIGSEGV, handler);   // install our handler
//    signal(SIGABRT, handler);   // install our handler
//    signal(SIGBUS, handler);   // install our handler
    struct sigaction sa;

    sa.sa_sigaction = (void *)bt_sighandler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;


    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);

    QCommandLineParser parser;
    parser.setApplicationDescription( "Distribution-independent installer framework" );
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption debugOption( QStringList() << "d" << "debug",
                                      "Verbose output for debugging purposes." );
    parser.addOption( debugOption );

    QCommandLineOption configOption( QStringList() << "c" << "config",
                                     "Configuration directory to use, for testing purposes.", "config" );
    parser.addOption( configOption );

    parser.process( a );

    a.setDebug( parser.isSet( debugOption ) );

    if ( parser.isSet( configOption ) )
        CalamaresUtils::setAppDataDir( QDir( parser.value( configOption ) ) );

    KDSingleApplicationGuard guard( KDSingleApplicationGuard::AutoKillOtherInstances );

    int returnCode = 0;
    if ( guard.isPrimaryInstance() )
    {
        a.init();
        returnCode = a.exec();
    }
    else
        qDebug() << "Calamares is already running, shutting down.";

    return returnCode;
}
