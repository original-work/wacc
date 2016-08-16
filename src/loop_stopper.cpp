/*
* =========================================================
*
*       Filename:  loop_stopper.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013.6.27
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangxx
*   Organization:  lj
*
* ==========================================================
* ================== CHANGE REPORT HISTORY ==================
*|VERSION|UPDATE DATE|UPDATED BY|DESCRIPTION OF CHANGE|*

* ================== END OF CHANGE REPORT HISTORY ===========
*/
#include "loop_stopper.h"
#include <signal.h>


bool LoopStopper::stopped_ = false;

LoopStopper::LoopStopper(void) {
	register_signal(SIGINT);
	register_signal(SIGTERM);
	register_signal(SIGHUP);
	register_signal(SIGPIPE);
}

LoopStopper::~LoopStopper(void) {

}

void LoopStopper::handle_signal(int t) {
	if (t == SIGINT || t == SIGTERM) {
		stopped_ = true;
	}
}

int LoopStopper::register_signal(int sig) {
	switch (sig) {
	case SIGINT:
	case SIGTERM:
		signal(sig, LoopStopper::handle_signal);
		break;
	case SIGHUP:
	case SIGPIPE:
		signal(sig, SIG_IGN);
		break;
	}
	return 0;
}
