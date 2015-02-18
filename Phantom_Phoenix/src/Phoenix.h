//=============================================================================
//Project Lynxmotion Phoenix
//Description: Phoenix software
//
//Programmer: Jeroen Janssen [aka Xan]
//         Kurt Eckhardt(KurtE) converted to C and Arduino
//   K�re Halvorsen aka Zenta - Makes everything work correctly!
//
// This version of the Phoenix code was ported over to the Arduino Environement
//
//
// Phoenix.h - This is the first header file that is needed to build
//			a Phoenix program for a specific Hex Robot.
//
//
// This file assumes that the main source file either directly or through include
// file has defined all of the configuration information for the specific robot.
// Each robot will also need to include:
//
//=============================================================================
//
//KNOWN BUGS:
//    - Lots ;)
//
//=============================================================================
//==============================================================================
#ifndef _PHOENIX_CORE_H_
#define _PHOENIX_CORE_H_
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <inttypes.h>

#include "ArduinoDefs.h"

#ifdef DBGSerial
#include "WrapperSerial.h"
extern WrapperSerial DBGSerialWrapper;
#endif

#ifdef USEXBEE
#include "diyxbee.h"
#endif

//=============================================================================
//[CONSTANTS]
//=============================================================================
#define BUTTON_DOWN 0
#define BUTTON_UP   1

#define c1DEC       10
#define c2DEC       100
#define c4DEC       10000
#define c6DEC       1000000

#ifdef QUADMODE
enum {
  cRR=0, cRF, cLR, cLF, CNT_LEGS};
#else
enum {
  cRR=0, cRM, cRF, cLR, cLM, cLF, CNT_LEGS};
#endif

#define WTIMERTICSPERMSMUL      64                // BAP28 is 16mhz need a multiplyer and divider to make the conversion with /8192
#define WTIMERTICSPERMSDIV      125               //
#define USEINT_TIMERAV

// BUGBUG: to make Dynamic first pass simpl make it a variable.
//#define NUM_GAITS    6
extern  const byte  NUM_GAITS;
#define SmDiv        4                            //"Smooth division" factor for the smooth control function, a value of 3 to 5 is most suitable
extern void GaitSelect(void);
extern short SmoothControl (short CtrlMoveInp, short CtrlMoveOut, byte CtrlDivider);

//-----------------------------------------------------------------------------
// Define Global variables
//-----------------------------------------------------------------------------
extern boolean          g_fDebugOutput;
extern boolean          g_fEnableServos;          // Hack to allow me to turn servo processing off...
extern boolean          g_fRobotUpsideDown;       // Is the robot upside down?

extern void MSound(byte cNotes, ...);
extern boolean CheckVoltage(void);

void AdjustLegPositions(float XZLength);
void AdjustLegPositionsToBodyHeight();
void ResetLegInitAngles(void);
void RotateLegInitAngles (int iDeltaAngle);

// debug handler...
extern boolean g_fDBGHandleError;

#ifdef c4DOF
extern const byte cTarsLength[] ;
#endif

#ifdef OPT_BACKGROUND_PROCESS
#define DoBackgroundProcess()   g_ServoDriver.BackgroundProcess()
#else
#define DoBackgroundProcess()
#endif

#ifdef DEBUG_IOPINS
#define DebugToggle(pin)  {digitalWrite(pin, !digitalRead(pin));}
#define DebugWrite(pin, state) {digitalWrite(pin, state);}
#else
#define DebugToggle(pin)  {;}
#define DebugWrite(pin, state) {;}
#endif

#ifdef __AVR__
#if not defined(UBRR1H)
#if cSSC_IN != 0
extern SoftwareSerial SSCSerial;
#endif
#endif
#endif
#if defined(__PIC32MX__)
#if defined F
#undef F
#endif
#define F(X) (X)
#endif

//=============================================================================
//=============================================================================
// Define the class(s) for our Input controllers.
//=============================================================================
//=============================================================================
class InputController {
    public:
        virtual void     Init(void);
        virtual void     ControlInput(void);
        virtual void     AllowControllerInterrupts(boolean fAllow);
#ifdef OPT_TERMINAL_MONITOR
        void            ShowTerminalCommandList(void);
        boolean         ProcessTerminalCommand(byte *psz, byte bLen);
#endif


    private:
}
;

// Define a function that allows us to define which controllers are to be used.
extern void  RegisterInputController(InputController *pic);



struct coord3D {
    float      x;
    float      y;
    float      z;
} ;

//==============================================================================
// Define Gait structure/class - Hopefully allow specific robots to define their
// own gaits and/or define which of the standard ones they want.
//==============================================================================
typedef struct _PhoenixGait {
  short           NomGaitSpeed;       //Nominal speed of the gait
  byte            StepsInGait;         //Number of steps in gait
  byte            NrLiftedPos;         //Number of positions that a single leg is lifted [1-3]
  byte            FrontDownPos;        //Where the leg should be put down to ground
  byte            LiftDivFactor;       //Normaly: 2, when NrLiftedPos=5: 4
  byte            TLDivFactor;         //Number of steps that a leg is on the floor while walking
  byte            HalfLiftHeight;      // How high to lift at halfway up.

#ifdef QUADMODE
  // Extra information used in the Quad balance mode
  word            COGAngleStart1;      // COG shifting starting angle
  word            COGAngleStep1;       // COG Angle Steps in degrees
  byte            COGRadius;           // COG Radius; the amount the body shifts
  boolean         COGCCW;              // COG Gait sequence runs counter clock wise
#endif
  byte            GaitLegNr[CNT_LEGS]; //Init position of the leg
#ifdef DISPLAY_GAIT_NAMES
  const char *           pszName;             // The gait name
#endif
}
PHOENIXGAIT;

#ifdef DISPLAY_GAIT_NAMES
#define GATENAME(name)  ,name
#else
#define GATENAME(name)
#endif

//==============================================================================
// class ControlState: This is the main structure of data that the Control
//      manipulates and is used by the main Phoenix Code to make it do what is
//      requested.
//==============================================================================
typedef struct _InControlState {
    boolean       fRobotOn;                       //Switch to turn on Phoenix
    boolean       fPrev_RobotOn;                  //Previous loop state
    boolean       fControllerInUse;               // Does it look like the controller is activly being used
    //Body position
    coord3D       BodyPos;
    coord3D        BodyRotOffset;                 // Body rotation offset;

    //Body Inverse Kinematics
    coord3D       BodyRot1;                       // X -Pitch, Y-Rotation, Z-Roll

    //[gait]
    byte          GaitType;                       //Gait type
    byte          GaitStep;            //Actual current step in gait
    PHOENIXGAIT   gaitCur;             // Definition of the current gait

    short       LegLiftHeight;                    //Current Travel height
    coord3D       TravelLength;                   // X-Z or Length, Y is rotation.

#ifdef cTurretRotPin
  // Turret information
  int           TurretRotAngle;      // Rotation of turrent in 10ths of degree
  int           TurretTiltAngle;    // the tile for the turret
#endif

    //[Balance]
    boolean       BalanceMode;

    //[TIMING]
    byte          InputTimeDelay;                 //Delay that depends on the input to get the "sneaking" effect
    word          SpeedControl;                   //Adjustible Delay
    byte          ForceGaitStepCnt;               // new to allow us to force a step even when not moving

#ifdef ADJUSTABLE_LEG_ANGLES
  short         aCoxaInitAngle[CNT_LEGS];
#endif

  //

}
INCONTROLSTATE;

//==============================================================================
//==============================================================================
// Define the class(s) for Servo Drivers.
//==============================================================================
//==============================================================================
class ServoDriver {
    public:
        void Init(void);
        void Cleanup(void); // Add a cleanup function;

        word GetBatteryVoltage(void);

#ifdef OPT_GPPLAYER
  inline boolean  FIsGPEnabled(void) {
    return _fGPEnabled;
  };
        boolean         FIsGPSeqDefined(uint8_t iSeq);
  inline boolean  FIsGPSeqActive(void) {
    return _fGPActive;
  };
        void            GPStartSeq(uint8_t iSeq); // 0xff - says to abort...
        void            GPPlayer(void);
        uint8_t         GPNumSteps(void);         // How many steps does the current sequence have
        uint8_t         GPCurStep(void);          // Return which step currently on...
  void            GPSetSpeedMultiplyer(short sm) ;      // Set the Speed multiplier (100 is default)
#endif
        void            BeginServoUpdate(void);   // Start the update
#ifdef c4DOF
        void            OutputServoInfoForLeg(int LegIndex, float CoxaAngle, float FemurAngle, float TibiaAngle, float TarsAngle);
#else
        void            OutputServoInfoForLeg(int LegIndex, float CoxaAngle, float FemurAngle, float TibiaAngle);
#endif
#ifdef cTurretRotPin
        void            OutputServoInfoForTurret(float RotateAngle, float TiltAngle);
#endif
  boolean            CommitServoDriver(word wMoveTime);
        void            FreeServos(void);

  void            IdleTime(void);        // called when the main loop when the robot is not on

        // Allow for background process to happen...
#ifdef OPT_BACKGROUND_PROCESS
        void            BackgroundProcess(void);
#endif

#ifdef OPT_TERMINAL_MONITOR
        void            ShowTerminalCommandList(void);
        boolean         ProcessTerminalCommand(byte *psz, byte bLen);
#endif
        boolean       _fServosActive;             // Are the servos active?

private:

#ifdef OPT_GPPLAYER
        boolean       _fGPEnabled;                // IS GP defined for this servo driver?
        boolean       _fGPActive;                 // Is a sequence currently active - May change later when we integrate in sequence timing adjustment code
        uint8_t       _iSeq;                      // current sequence we are running
        short         _sGPSM;                     // Speed multiplier +-200
#endif

}
;

//==============================================================================
//==============================================================================
// Define global class objects
//==============================================================================
//==============================================================================
extern ServoDriver      g_ServoDriver;            // our global servo driver class
extern InputController  g_InputController;        // Our Input controller
extern INCONTROLSTATE   g_InControlState;         // State information that controller changes
#endif
