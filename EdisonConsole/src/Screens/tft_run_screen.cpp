//=============================================================================
//Project Linux Eclipse projects
//Description: Phoenix software
//
//  This code is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//=============================================================================

//=============================================================================
// Run screen
//=============================================================================

#include "globals.h"
#include "tft_screen.h"
#include "tft_text_box.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>

//=============================================================================
// Class definition
//=============================================================================
#define TRYPOPEN2

class TFTRunScreen : public TFTScreen {
public:
	TFTRunScreen(uint16_t color_background, TFTDisplayObject **ppdisp, uint8_t cdisp, TFTScreen* pscreenNext, TFTScreen* pscreenPrev) :
		TFTScreen(color_background, ppdisp, cdisp, pscreenNext, pscreenPrev) {
#ifdef TRYPOPEN2
			_toProcessfp=0; _fromProcessfp = 0; _tid = 0;
#else
			_pfile = NULL;
#endif
	};

	  virtual uint16_t processTouch(uint16_t x, uint16_t y);

	  // Stuff to talk to other process
#ifdef TRYPOPEN2
	  int 	_toProcessfp;						// handle for input
	  int	_fromProcessfp;					    // handle for output
#else
	  FILE	*_pfile;							// pipe file pointer
#endif
	  pthread_t _tid;                	// Thread Id of our reader thread...
      pid_t		_pid;					// process handle
      pthread_barrier_t _barrier;		// barrier.
      volatile uint8_t _fCancel;					// should we cancel out
      volatile uint8_t _fSigChild;		// Child has signaled.

      static void *ProcessThreadProc(void *);
      void cancelRunningApp();
      void sendCommandToRunningApp(std::string command_string);

      // Define our local commands here
      static const int CMD_CLOSE = 103;
	  static const int CMD_DBG	=  104;
	  static const int CMD_SRVO = 105;


};

//=============================================================================
// Define graphic objects specific to this screen
//=============================================================================

static TFTButton _btnClose(10, 50, 100, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_RED, ILI9341_BLACK, "Close", TFTRunScreen::CMD_CLOSE);
static TFTButton _btnDBG(210, 10, 48, 34, DCLR_BUTTON_GREY, DCLR_BUTTON_YELLOW, ILI9341_BLACK, "Dbg", TFTRunScreen::CMD_DBG);
static TFTButton _btnServos(210, 55, 48, 34, DCLR_BUTTON_GREY, DCLR_BUTTON_YELLOW, ILI9341_BLACK, "Srv", TFTRunScreen::CMD_SRVO);


static TFTTextBox txtMsgs(10, 100, 300, 132, ILI9341_BLACK, ILI9341_WHITE, ILI9341_GREEN);

TFTDisplayObject *_runscreenobjs[] = {&g_btnup,&g_btndn,
		&_btnClose, &_btnDBG, &_btnServos, &txtMsgs, &g_txtTitle};




//=============================================================================
// Create our global Object.
//=============================================================================
TFTRunScreen g_run_screen(ILI9341_BLACK, _runscreenobjs, sizeof(_runscreenobjs)/sizeof(_runscreenobjs[0]),
		(TFTScreen*)&g_hex_screen, (TFTScreen*)&g_main_screen);



//=============================================================================
// ProcessTouch
//=============================================================================
uint16_t TFTRunScreen::processTouch(uint16_t x, uint16_t y)
{
	uint16_t wTouch = TFTScreen::processTouch(x, y);
	if (wTouch != 0xffff) {
		switch (wTouch) {
		case CMD_CLOSE:
			cancelRunningApp();
				break;

		case CMD_DBG:
			sendCommandToRunningApp("d\n\r");
				break;

		case CMD_SRVO:
			sendCommandToRunningApp("t\n\r");
				break;
		}
	}
	return wTouch;
}

//=============================================================================
// cancelRunningApp
//=============================================================================
void TFTRunScreen::cancelRunningApp(void) {
#ifdef TRYPOPEN2
	if (_pid > 1 ) {
		// First ask nicely
		kill(_pid, SIGQUIT);

		delay(500); // wait 1/2 second
		if (_fromProcessfp) {
			// The secondary thread is still going assume it was not closed off.
			_fCancel = true;	// tell thread to exit...
			delay(250);
			kill(_pid, SIGTERM);
		}

		if(_toProcessfp) {
			close(_toProcessfp);
			_toProcessfp = 0;
		}
		if(_fromProcessfp) {
			close(_fromProcessfp);
			_fromProcessfp = 0;
		}
		_pid = 0;
	}
#else
	if (_pfile) {
		_fCancel = true;	// simple tell child process to exit...
	}
#endif
}

//=============================================================================
// sendCommandToRunningApp - send command string to running app
//=============================================================================
void TFTRunScreen::sendCommandToRunningApp(std::string command_string)
{
	if (_toProcessfp) {
		const char *psz = command_string.c_str();
		write(_toProcessfp, (void*)psz, strlen(psz));
	}

}

//=============================================================================
// Thread process
//=============================================================================
void *TFTRunScreen::ProcessThreadProc(void *pv)
{
	TFTRunScreen *prun = (TFTRunScreen*)pv;

	char pszLine[320/12];
	uint8_t iChar=0;
	uint8_t iMax = (txtMsgs.width() - 4) / 12;				// Think the max number of characters per line

    fd_set fdset;                                // file descriptor set to wait on.
    timeval tv;                                   // how long to wait.

    int fdFromProc;
#ifdef TRYPOPEN2
    fdFromProc = prun->_fromProcessfp;
#else
    fdFromProc = fileno(prun->_pfile);
#endif

    pthread_barrier_wait(&prun->_barrier);

    //    printf("Thread Init\n");

    // May want to add end code... But for now don't have any defined...
    sprintf((char*)pszLine, "start: %d", prun->_pid);
    txtMsgs.add(pszLine);
    unsigned int ulLastChar = millis();	// know when we last saw something...
    int ret;
    unsigned char ch;
    while(!prun->_fCancel)
    {
        FD_ZERO(&fdset);
        FD_SET(fdFromProc, &fdset);                   // Make sure we are set to wait for our descriptor
        tv.tv_sec = 0;
        tv.tv_usec = 250000;                          // 1/4 of a second...
                                                      // wait until some input is available...
        int iSelectRet = select(fdFromProc + 1, &fdset, NULL, NULL, &tv);

        if(FD_ISSET(fdFromProc, &fdset)) {
            int cBytes;

            // BUGBUG:: handle standard terminal better
            ioctl(fdFromProc, FIONREAD, &cBytes);

           while((cBytes-- > 0) && (ret = read(fdFromProc, &ch, 1)) > 0)
//           if((!prun->_fCancel) && (ret = read(fdFromProc, &ch, 1)) > 0)
        	{
        		if (ch >= ' ') {
        			pszLine[iChar++] = ch;
        		}
				if ((iChar == iMax) || (ch == '\n')) {
        			pszLine[iChar] = '\0';
        			txtMsgs.add(pszLine);
        			iChar = 0;
				}
				ulLastChar = millis();
        	}
        }
        if ((millis()-ulLastChar) > 1000) {
        	// Been more than a second since we saw something check to see if the process is still
        	// running...
        	if ((ret = kill(prun->_pid, 0)) == -1)
				prun->_fCancel = true;	// got an error return, probably process has terminated...
			ulLastChar = millis();
        }
    	if (iSelectRet == -1) {
    		// error
    		printf("Thread select error: %x\n", errno);
    		break;
    	}
    	if (prun->_fSigChild) {
    		// lets release the zombie state process.  - Not sure if we should cancel the loop yet
    		// or wait for timeout?
    		int iStatus;
    		wait(&iStatus);
    	}
    };
    sprintf((char*)pszLine, "run thread exit");
    txtMsgs.add(pszLine);
#ifdef TRYPOPEN2
    close (prun->_fromProcessfp);
    close (prun->_toProcessfp);
    prun->_fromProcessfp = 0;
    prun->_toProcessfp = 0;
#else
    pclose(prun->_pfile);
    prun->_pfile = NULL;

#endif
    _btnClose.enable(false); 	// disable the close button

    printf("Process - thread exit\n");
    prun->_fCancel = true;
    return 0;
}


#define READ 0
#define WRITE 1


#ifdef TRYPOPEN2
//=============================================================================
// Helper function to try to implement popen like features with two pipes.
//=============================================================================
// Lets copy our own implementaion of popen, where
// we can get handles for both reading and writing as well as process ID...
pid_t
popen2(const char *command, int *stdinfp, int *stdoutfp)
{
    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0)
    {
        close(p_stdin[WRITE]);
        dup2(p_stdin[READ], READ);
        close(p_stdout[READ]);
        dup2(p_stdout[WRITE], WRITE);
        // Try exec directly not use shell...
#ifdef EXEC_SHELL
        execl("/bin/sh", "sh", "-c", command, NULL);
#else
        char szCommandName[80]= "";
        char *psz = strrchr((char*)command, '/');
        if (psz)
        	strcpy(szCommandName, psz+1);
        execl(command, szCommandName, NULL);
#endif
        perror("execl");
        exit(1);
    }

    if (stdinfp == NULL)
        close(p_stdin[WRITE]);
    else
        *stdinfp = p_stdin[WRITE];

    if (stdoutfp == NULL)
        close(p_stdout[READ]);
    else
        *stdoutfp = p_stdout[READ];

    return pid;
}
#endif

void sigChildHandler(int sig) {
	// Let the handler know that we have been canceled.
	g_run_screen._fSigChild = true;
}


//=============================================================================
// LaunchProgram - handle the launching of a program.
//=============================================================================
void LaunchProgram(std::string strCmd) {
	// Switch to the run screen
	TFTScreen::setCurrentScreen(&g_run_screen);

	struct sigaction act;
	act.sa_handler=sigChildHandler;
	sigaction(SIGCHLD,&act,NULL);

	// Try to start the command and set the pipe in it.
#ifdef TRYPOPEN2
	g_run_screen._pid  = popen2(strCmd.c_str(), &g_run_screen._toProcessfp, &g_run_screen._fromProcessfp);
#else
	g_run_screen._pfile = popen((char*)strCmd.c_str(), "r");
#endif
	// Then Launch the secondary thread

    // Now we need to create our thread for doing the reading from the Xbee
	g_run_screen._fCancel = false;
	g_run_screen._fSigChild = false;
	pthread_barrier_init(&g_run_screen._barrier, 0, 2);
    int err = pthread_create(&g_run_screen._tid, NULL, &TFTRunScreen::ProcessThreadProc, TFTScreen::curScreen());
    if (err != 0)
        return;
    _btnClose.enable(true); 	// enable the close button
  	// sync startup
	pthread_barrier_wait(&g_run_screen._barrier);

}





