//====================================================================
//Project Lynxmotion Phoenix
//Description: Phoenix, control file.
//The control input subroutine for the phoenix software is placed in this file.
//Can be used with V2.0 and above
//Configuration version: V1.0
//Date: 25-10-2009
//Programmer: Jeroen Janssen (aka Xan)
//             Kurt Eckhardt (aka KurtE) - converted to c ported to Arduino...
//
//Hardware setup: Arbotix Commander version - Try to emulate most of PS2, but PS2 has 16 buttons and Commander
// has 10. so some things may not be there, others may be doubled up.
//
//NEW IN V1.0
//- First Release
//
//Walk method 1:
//- Left StickWalk/Strafe
//- Right StickRotate
//
//Walk method 2:
//- Left StickDisable
//- Right StickWalk/Rotate
//
//
// Quick and Dirty description of controls... WIP
// In most cases I try to mention what button on the PS2 things coorespond to..
// On/OFF - Turning the commander 2 on and off (PS2 start button)
// R1 - options (Change walk gait, Change Leg in Single Leg, Change GP sequence) (Select on PS2)
// R2 - Toggle walk method...  Run Sequence in GP mode
// R3 - Walk method (Not done yet) - (PS2 R3)
// L4 - Ballance mode on and off
// L5 - Stand/Sit (Triangle on PS2)
// L6+Right Joy UP/DOWN - Body up/down - (PS2 Dpad Up/Down)
// L6+Right Joy Left/Right - Speed higher/lower - (PS2 DPad left/right)
// Right Top(S7) - Cycle through options of Normal walk/Double Height/Double Travel) - (PS2 R1, R2)
// Left Top(S8) - Cycle through modes (Walk, Translate, Rotate, Single Leg) (PS2: Circle, X, L1, L2)

// Note: Left some descriptions of PS2 stuff, especially parts still left to Map/Implement.
//
//[Walk Controls]
//- selectSwitch gaits
//- Left Stick(Walk mode 1) Walk/Strafe
// (Walk mode 2) Disable
//- Right Stick(Walk mode 1) Rotate,
//(Walk mode 2) Walk/Rotate
//- R1Toggle Double gait travel speed
//- R2Toggle Double gait travel length
//
//[Shift Controls]
//- Left StickShift body X/Z
//- Right StickShift body Y and rotate body Y
//
//[Rotate Controls]
//- Left StickRotate body X/Z
//- Right StickRotate body Y
//
//[Single leg Controls] - Need to check...
//- selectSwitch legs
//- Left StickMove Leg X/Z (relative)
//- Right StickMove Leg Y (absolute)
//- R2Hold/release leg position
//
//[GP Player Controls] - How to do sequences???
//- selectSwitch Sequences
//- R2Start Sequence
//
//====================================================================
#include "Hex_Cfg.h"
#include "Phoenix.h"
#include "CommanderEx.h"

#ifdef OPT_ESPEAK
#include "speak.h"
#endif

//[CONSTANTS]
#ifdef OPT_GPPLAYER
enum
{
    WALKMODE=0, TRANSLATEMODE, ROTATEMODE, GPPLAYERMODE, MODECNT
};
#else
enum
{
    WALKMODE=0, TRANSLATEMODE, ROTATEMODE, MODECNT
};
#endif
enum
{
    NORM_NORM=0, NORM_LONG, HIGH_NORM, HIGH_LONG
};

#ifdef OPT_ESPEAK
#define SpeakStr(psz) Speak.Speak((psz), true)
extern "C"
{
    // Move the Gait Names to program space...
    const char s_sGN1[] PROGMEM = "Ripple 12";
    const char s_sGN2[] PROGMEM = "Tripod 8";
    const char s_sGN3[] PROGMEM = "Tripple 12";
    const char s_sGN4[] PROGMEM = "Tripple 16";
    const char s_sGN5[] PROGMEM = "Wave 24";
    const char s_sGN6[] PROGMEM = "Tripod 6";
    PGM_P s_asGateNames[] PROGMEM =
    {
        s_sGN1, s_sGN2, s_sGN3, s_sGN4, s_sGN5, s_sGN6
    };
}


#else
#define SpeakStr(PSZ)
#endif

#define cTravelDeadZone 4                         //The deadzone for the analog input from the remote

#define ARBOTIX_TO  1000                          // if we don't get a valid message in this number of mills turn off

//=============================================================================
// Global - Local to this file only...
//=============================================================================
Commander command = Commander();
unsigned long g_ulLastMsgTime;
short  g_sGPSMController;                         // What GPSM value have we calculated. 0xff - Not used yet

#ifdef USEMULTI
//==============================================================================
//
// Lets define our Sub-class of the InputControllerClass
//
//==============================================================================
class CommanderInputController :
public InputController
{
    public:
        CommanderInputController();               // A reall simple constructor...

        virtual void     Init(void);
        virtual void     ControlInput(void);
        virtual void     AllowControllerInterrupts(boolean fAllow);

};

CommanderInputController g_CommanderController;

//==============================================================================
// Constructor. See if there is a simple way to have one or more Input
//     controllers. Maybe register at construction time
//==============================================================================
CommanderInputController::CommanderInputController()
{
    RegisterInputController(this);
}


#else
#define CommanderInputController InputController
// Define an instance of the Input Controller...
InputController  g_InputController;               // Our Input controller
#endif

static short   g_BodyYOffset;
static short   g_BodyYShift;
static byte    ControlMode;
static byte    HeightSpeedMode;
//static bool  DoubleHeightOn;
static bool    DoubleTravelOn;
static bool    WalkMethod;
byte           GPSeq;                             //Number of the sequence

static byte    buttonsPrev;
static byte    extPrev;

// some external or forward function references.
extern void CommanderTurnRobotOff(void);

//==============================================================================
// This is The function that is called by the Main program to initialize
//the input controller, which in this case is the PS2 controller
//process any commands.
//==============================================================================
//char szDevice[] = "/dev/ttyXBEE";
char szDevice[] = "/dev/ttyMFD1";
// If both PS2 and XBee are defined then we will become secondary to the xbee
void CommanderInputController::Init(void)
{
    //  DBGSerial.println("Init Commander Start");
    g_BodyYOffset = 0;
    g_BodyYShift = 0;
    command.begin(szDevice, B38400);
    GPSeq = 0;                                    // init to something...

    ControlMode = WALKMODE;
    HeightSpeedMode = NORM_NORM;
    //    DoubleHeightOn = false;
    DoubleTravelOn = false;
    WalkMethod = false;
    //  DBGSerial.println("Init Commander End");

    SpeakStr("Init");

}


//==============================================================================
// This function is called by the main code to tell us when it is about to
// do a lot of bit-bang outputs and it would like us to minimize any interrupts
// that we do while it is active...
//==============================================================================
void CommanderInputController::AllowControllerInterrupts(boolean fAllow)
{
    // We don't need to do anything...
}


//==============================================================================
// This is The main code to input function to read inputs from the Commander and then
//process any commands.
//==============================================================================
void CommanderInputController::ControlInput(void)
{
    // See if we have a new command available...
    if(command.ReadMsgs() > 0)
    {
        // If we receive a valid message than turn robot on...
        g_InControlState.fRobotOn = true;

        // Experimenting with trying to detect when IDLE.  maybe set a state of
        // of no button pressed and all joysticks are in the DEADBAND area...
        g_InControlState.fControllerInUse = command.buttons
            || (abs(command.leftH) >= cTravelDeadZone)
            || (abs(command.leftV) >= cTravelDeadZone)
            || (abs(command.rightH) >= cTravelDeadZone)
            || (abs(command.rightV) >= cTravelDeadZone);
        // [SWITCH MODES]

        // Cycle through modes...
        if ((command.buttons & BUT_LT) && !(buttonsPrev & BUT_LT))
        {
            if (++ControlMode >= MODECNT)
            {
                ControlMode = WALKMODE;           // cycled back around...
                MSound( 2, 50, 2000, 50, 3000);
            }
            else
            {
                MSound( 1, 50, 2000);
            }

        }

        //[Common functions]
        //Switch Balance mode on/off
        if ((command.buttons & BUT_L4) && !(buttonsPrev & BUT_L4))
        {
            g_InControlState.BalanceMode = !g_InControlState.BalanceMode;
            if (g_InControlState.BalanceMode)
            {
                MSound( 1, 250, 1500);
            }
            else
            {
                MSound( 2, 100, 2000, 50, 4000);
            }
        }

        //Stand up, sit down
        if ((command.buttons & BUT_L5) && !(buttonsPrev & BUT_L5))
        {
            if (g_BodyYOffset>0)
                g_BodyYOffset = 0;
            else
                g_BodyYOffset = 35;
        }

        // We will use L6 with the Right joystick to control both body offset as well as Speed...
        // We move each pass through this by a percentage of how far we are from center in each direction
        // We get feedback with height by seeing the robot move up and down.  For Speed, I put in sounds
        // which give an idea, but only for those whoes robot has a speaker
        if (command.buttons & BUT_L6 )
        {
        	printf("L6 down\n\r");
            // raise or lower the robot on the joystick up /down
            // Maybe should have Min/Max
            g_BodyYOffset += command.rightV/25;
            if (abs(command.rightV)>= 25)
            	printf("%d %d", g_BodyYOffset, command.rightV);

            // Likewise for Speed control
            int dspeed = command.rightH / 16;      //
            if ((dspeed < 0) && g_InControlState.SpeedControl)
            {
                if ((word)(-dspeed) <  g_InControlState.SpeedControl)
                    g_InControlState.SpeedControl += dspeed;
                else
                    g_InControlState.SpeedControl = 0;
                MSound( 1, 50, 1000+g_InControlState.SpeedControl);
            }
            if ((dspeed > 0) && (g_InControlState.SpeedControl < 2000))
            {
                g_InControlState.SpeedControl += dspeed;
                if (g_InControlState.SpeedControl > 2000)
                    g_InControlState.SpeedControl = 2000;
                MSound( 1, 50, 1000+g_InControlState.SpeedControl);
            }

            command.rightH = 0;                    // don't walk when adjusting the speed here...
        }

        //[Walk functions]
        if (ControlMode == WALKMODE)
        {
            //Switch gates
            if (((command.buttons & BUT_R1) && !(buttonsPrev & BUT_R1))
                                                  //No movement
                && abs(g_InControlState.TravelLength.x)<cTravelDeadZone
                && abs(g_InControlState.TravelLength.z)<cTravelDeadZone
                && abs(g_InControlState.TravelLength.y*2)<cTravelDeadZone  )
            {
                                                  // Go to the next gait...
                g_InControlState.GaitType = g_InControlState.GaitType+1;
                                                  // Make sure we did not exceed number of gaits...
                if (g_InControlState.GaitType<NUM_GAITS)
                {
#ifndef OPT_ESPEAK
                    MSound( 1, 50, 2000);
#endif
                }
                else
                {
#ifdef OPT_ESPEAK
                    MSound (2, 50, 2000, 50, 2250);
#endif
                    g_InControlState.GaitType = 0;
                }
#ifdef OPT_ESPEAK
                SpeakStr(s_asGateNames[g_InControlState.GaitType]);
#endif
                GaitSelect();
            }

            //Double leg lift height
            if ((command.buttons & BUT_RT) && !(buttonsPrev & BUT_RT))
            {
                MSound( 1, 50, 2000);
                                                  // wrap around mode
                HeightSpeedMode = (HeightSpeedMode + 1) & 0x3;
                DoubleTravelOn = HeightSpeedMode & 0x1;
                if ( HeightSpeedMode & 0x2)
                    g_InControlState.LegLiftHeight = 80;
                else
                    g_InControlState.LegLiftHeight = 50;
            }

            // Switch between Walk method 1 && Walk method 2
            if ((command.buttons & BUT_R2) && !(buttonsPrev & BUT_R2))
            {
                MSound (1, 50, 2000);
                WalkMethod = !WalkMethod;
            }

            //Walking
            if (WalkMethod)                       //(Walk Methode)
                                                  //Right Stick Up/Down
                g_InControlState.TravelLength.z = (command.rightV);

            else
            {
                g_InControlState.TravelLength.x = -command.leftH;
                g_InControlState.TravelLength.z = command.leftV;
            }

            if (!DoubleTravelOn)                  //(Double travel length)
            {
                g_InControlState.TravelLength.x = g_InControlState.TravelLength.x/2;
                g_InControlState.TravelLength.z = g_InControlState.TravelLength.z/2;
            }

                                                  //Right Stick Left/Right
            g_InControlState.TravelLength.y = -(command.rightH)/4;
        }

        //[Translate functions]
        g_BodyYShift = 0;
        if (ControlMode == TRANSLATEMODE)
        {
            g_InControlState.BodyPos.x =  SmoothControl(((command.leftH)*2/3), g_InControlState.BodyPos.x, SmDiv);
            g_InControlState.BodyPos.z =  SmoothControl(((command.leftV)*2/3), g_InControlState.BodyPos.z, SmDiv);
            g_InControlState.BodyRot1.y = SmoothControl(((command.rightH)*2), g_InControlState.BodyRot1.y, SmDiv);

            //      g_InControlState.BodyPos.x = (command.leftH)/2;
            //      g_InControlState.BodyPos.z = -(command.leftV)/3;
            //      g_InControlState.BodyRot1.y = (command.rightH)*2;
            g_BodyYShift = (-(command.rightV)/2);
        }

        //[Rotate functions]
        if (ControlMode == ROTATEMODE)
        {
            g_InControlState.BodyRot1.x = (command.leftV);
            g_InControlState.BodyRot1.y = (command.rightH)*2;
            g_InControlState.BodyRot1.z = (command.leftH);
            g_BodyYShift = (-(command.rightV)/2);
        }
#ifdef OPT_GPPLAYER
        //[GPPlayer functions]
        if (ControlMode == GPPLAYERMODE)
        {
            // Lets try some speed control... Map all values if we have mapped some before
            // or start mapping if we exceed some minimum delta from center
            // Have to keep reminding myself that commander library already subtracted 128...
            if (g_ServoDriver.FIsGPSeqActive() )
            {
                if ((g_sGPSMController != 32767)
                    || (command.rightV > 16) || (command.rightV < -16))
                {
                    // We are in speed modify mode...
                    if (command.rightV >= 0)
                        g_sGPSMController = map(command.rightV, 0, 127, 0, 200);
                    else
                        g_sGPSMController = map(command.rightV, -127, 0, -200, 0);
                    g_ServoDriver.GPSetSpeedMultiplyer(g_sGPSMController);
                }
            }

            //Switch between sequences
            if ((command.buttons & BUT_R1) && !(buttonsPrev & BUT_R1))
            {
                if (!g_ServoDriver.FIsGPSeqActive() )
                {
                    if (GPSeq < 5)                //Max sequence
                    {
                        MSound (1, 50, 1500);
                        GPSeq = GPSeq+1;
                    }
                    else
                    {
                        MSound (2, 50, 2000, 50, 2250);
                        GPSeq=0;
                    }
                }
            }
            //Start Sequence
            if ((command.buttons & BUT_R2) && !(buttonsPrev & BUT_R2))
            {
                if (!g_ServoDriver.FIsGPSeqActive() )
                {
                    g_ServoDriver.GPStartSeq(GPSeq);
                    g_sGPSMController = 32767;    // Say that we are not in Speed modify mode yet... valid ranges are 50-200 (both postive and negative...
                }
                else
                {
                                                  // tell the GP system to abort if possible...
                    g_ServoDriver.GPStartSeq(0xff);
                    MSound (2, 50, 2000, 50, 2000);
                }
            }

        }
#endif                                    // OPT_GPPLAYER

        //Calculate walking time delay
        g_InControlState.InputTimeDelay = 128 - max(max(abs(command.leftH), abs(command.leftV)), abs(command.rightH));

        //Calculate g_InControlState.BodyPos.y
        g_InControlState.BodyPos.y = max(g_BodyYOffset + g_BodyYShift,  0);

        // Save away the buttons state as to not process the same press twice.
        buttonsPrev = command.buttons;
        extPrev = command.ext;
        g_ulLastMsgTime = millis();
    }
    else
    {
        // We did not receive a valid packet.  check for a timeout to see if we should turn robot off...
        if (g_InControlState.fRobotOn)
        {
            if ((millis() - g_ulLastMsgTime) > ARBOTIX_TO)
            {
	        g_InControlState.fControllerInUse = true;	// make sure bypass is not used.
                CommanderTurnRobotOff();
            }
        }
    }
}


//==============================================================================
// CommanderTurnRobotOff - code used couple of places so save a little room...
//==============================================================================
void CommanderTurnRobotOff(void)
{
    //Turn off
    g_InControlState.BodyPos.x = 0;
    g_InControlState.BodyPos.y = 0;
    g_InControlState.BodyPos.z = 0;
    g_InControlState.BodyRot1.x = 0;
    g_InControlState.BodyRot1.y = 0;
    g_InControlState.BodyRot1.z = 0;
    g_InControlState.TravelLength.x = 0;
    g_InControlState.TravelLength.z = 0;
    g_InControlState.TravelLength.y = 0;
    g_BodyYOffset = 0;
    g_BodyYShift = 0;
    g_InControlState.fRobotOn = 0;
}
// Optional stuff to allow me to have Input device debug support
#ifdef OPT_TERMINAL_MONITOR
//==============================================================================
// ShowTerminalCommandList: Allow the Terminal monitor to call the servo driver
//      to allow it to display any additional commands it may have.
//==============================================================================
void CommanderInputController::ShowTerminalCommandList(void)
{
  DBGSerial.println(F("Xn - Debug level XBee"));
}


boolean CommanderInputController::ProcessTerminalCommand(byte *psz, byte bLen)
{
  if ((*psz == 'x') || (*psz == 'X')) {
	  uint8_t bLevel = 0;
	  if (bLen > 1) {
		  psz++;
		  if ((*psz >= '0') && (*psz <= '9'))
			  bLevel = *psz-'0';
	  }
	  command.setDebugLevel(bLevel);
	  DBGSerial.print("XBee Debug Level: ");
	  DBGSerial.println(bLevel, DEC);
      return true;
  }
  return false;

}
#endif
