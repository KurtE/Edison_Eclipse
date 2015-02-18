// Warning - This version is hacked up to use Kurt's extended USB2AX
//
//====================================================================
//Project Lynxmotion Phoenix
//
// Servo Driver - This version is setup to use the SSC-32 to control
// the servos.
//====================================================================
#include "Hex_Cfg.h"
#include "Phoenix.h"
#include "WrapperSerial.h"

#ifdef c4DOF
#define NUMSERVOSPERLEG 4
#else
#define NUMSERVOSPERLEG 3
#endif

#ifdef cTurretRotPin
#define NUMSERVOS (NUMSERVOSPERLEG*CNT_LEGS +2)
#else
#define NUMSERVOS (NUMSERVOSPERLEG*CNT_LEGS)
#endif

#define cPwmMult      128
#define cPwmDiv       375
#define cPFConst      512    // half of our 1024 range


#include <ax12.h>

#define USE_BIOLOIDEX            // Use the Bioloid code to control the AX12 servos...
#define USE_AX12_SPEED_CONTROL   // Experiment to see if the speed control works well enough...
boolean g_fAXSpeedControl;      // flag to know which way we are doing output...
#include <BioloidEX.h>


#ifdef USE_AX12_SPEED_CONTROL
// Current positions in AX coordinates
word      g_awCurAXPos[NUMSERVOS];
word      g_awGoalAXPos[NUMSERVOS];
#endif

#ifdef DBGSerial
//#define DEBUG
// Only allow debug stuff to be turned on if we have a debug serial port to output to...
//#define DEBUG_SERVOS
#endif

#ifdef DEBUG_SERVOS
#define ServosEnabled   (g_fEnableServos)
#else
#define ServosEnabled  (true)      // always true compiler should remove if...
#endif

//=============================================================================
// Global - Local to this file only...
//=============================================================================
#ifdef QUADMODE
static const byte cPinTable[]  = {
  cRRCoxaPin,  cRFCoxaPin,  cLRCoxaPin,  cLFCoxaPin,
  cRRFemurPin, cRFFemurPin, cLRFemurPin, cLFFemurPin,
  cRRTibiaPin, cRFTibiaPin, cLRTibiaPin, cLFTibiaPin
#ifdef c4DOF
 ,cRRTarsPin,  cRFTarsPin,  cLRTarsPin,  cLFTarsPin
#endif
#ifdef cTurretRotPin
  , cTurretRotPin, cTurretTiltPin
#endif
};
#else
static const byte cPinTable[]  = {
  cRRCoxaPin,  cRMCoxaPin,  cRFCoxaPin,  cLRCoxaPin,  cLMCoxaPin,  cLFCoxaPin,
  cRRFemurPin, cRMFemurPin, cRFFemurPin, cLRFemurPin, cLMFemurPin, cLFFemurPin,
  cRRTibiaPin, cRMTibiaPin, cRFTibiaPin, cLRTibiaPin, cLMTibiaPin, cLFTibiaPin
#ifdef c4DOF
   ,cRRTarsPin, cRMTarsPin, cRFTarsPin, cLRTarsPin, cLMTarsPin, cLFTarsPin
#endif
#ifdef cTurretRotPin
  , cTurretRotPin, cTurretTiltPin
#endif
};
#endif
#define FIRSTCOXAPIN     0
#define FIRSTFEMURPIN    (CNT_LEGS)
#define FIRSTTIBIAPIN    (CNT_LEGS*2)
#ifdef c4DOF
#define FIRSTTARSPIN     (CNT_LEGS*3)
#define FIRSTTURRETPIN   (CNT_LEGS*4)
#else
#define FIRSTTURRETPIN   (CNT_LEGS*3)
#endif
// Not sure yet if I will use the controller class or not, but...
BioloidControllerEx bioloid = BioloidControllerEx(1000000);
boolean g_fServosFree;    // Are the servos in a free state?


//============================================================================================
// Lets try rolling our own GPSequence code here...
#define GPSEQ_EEPROM_START 0x40       // Reserve the first 64 bytes of EEPROM for other stuff...
#define GPSEQ_EEPROM_START_DATA  0x50 // Reserved room for up to 8 in header...
#define GPSEQ_EEPROM_SIZE 0x800       // I think we have 2K
#define GPSEQ_EEPROM_MAX_SEQ 5        // For now this is probably the the max we can probably hold...


// Not sure if pragma needed or not...
//#pragma pack(1)
typedef struct {
  byte  bSeqNum;       // the sequence number, used to verify
  byte  bCntServos;    // count of servos
  byte  bCntSteps;     // How many steps there are
  byte  bCntPoses;     // How many poses
}
EEPromPoseHeader;

typedef struct {
  byte bPoseNum;        // which pose to use
  word wTime;        // Time to do pose
}
EEPROMPoseSeq;      // This is a sequence entry

// Pose is just an array of words...


// A sequence is stored as:
//<header> <sequences><poses>



// Some forward references
extern void MakeSureServosAreOn(void);
extern void SetRegOnAllServos(uint8_t bReg, uint8_t bVal);
extern void DoPyPose(byte *psz);

//--------------------------------------------------------------------
//Init
//--------------------------------------------------------------------
void ServoDriver::Init(void) {
  // First lets get the actual servo positions for all of our servos...
  g_fServosFree = true;
  bioloid.poseSize(NUMSERVOS);  // Method in this version

  bioloid.readPose();

  ax12SetRegister(AX_ID_DEVICE, USB2AX_REG_VOLTAGE, cPinTable[FIRSTFEMURPIN]);

  g_fAXSpeedControl = false;

#ifdef OPT_GPPLAYER
  _fGPEnabled = true;    // assume we support it.
#endif

  // Currently have Turret pins not necessarily same as numerical order so
  // Maybe should do for all pins and then set the positions by index instead
  // of having it do a simple search on each pin...
#ifdef cTurretRotPin
  bioloid.setId(FIRSTTURRETPIN, cTurretRotPin);
  bioloid.setId(FIRSTTURRETPIN+1, cTurretTiltPin);
#endif
}

//--------------------------------------------------------------------
// Cleanup
//--------------------------------------------------------------------
void ServoDriver::Cleanup(void) {
    // Do any cleanup that the driver may need.
    printf("ServoDriver::Cleanup\n\r");
    ax12SetRegister(AX_ID_DEVICE, USB2AX_REG_VOLTAGE, 0);   // Turn off the voltage testing...

    // Turn off all of the servo LEDS...  Maybe use broadcast?
    for (int iServo=0; iServo < NUMSERVOS; iServo++) {
		dxl_write_byte((cPinTable[iServo]), AX_LED, 0);
		dxl_get_result();   // don't care for now
    }

}


//--------------------------------------------------------------------
//GetBatteryVoltage - Maybe should try to minimize when this is called
// as it uses the serial port... Maybe only when we are not interpolating
// or if maybe some minimum time has elapsed...
//--------------------------------------------------------------------

#define VOLTAGE_MIN_TIME_BETWEEN_CALLS 250      // Max 4 times per second
#define VOLTAGE_MAX_TIME_BETWEEN_CALLS 1000    // call at least once per second...
#define VOLTAGE_TIME_TO_ERROR          3000    // Error out if no valid item is returned in 3 seconds...
word g_wLastVoltage = 0xffff;    // save the last voltage we retrieved...
byte g_bLegVoltage = 0;		// what leg did we last check?
unsigned long g_ulTimeLastBatteryVoltage;

word ServoDriver::GetBatteryVoltage(void) {
  // The USB2AX is caching informatation for us, so simply ask it for the data.  If this is still
  // too much overhead, then we may want to only do this on timer...

  // Lets cycle through the Tibia servos asking for voltages as they may be the ones doing the most work...

  word wVoltage = (word)ax12GetRegister(AX_ID_DEVICE, USB2AX_REG_VOLTAGE, 1);
  if (wVoltage != g_ulTimeLastBatteryVoltage) {
    printf("Voltage: %d\n\r", wVoltage);
    g_ulTimeLastBatteryVoltage = wVoltage;
  }
  return ((wVoltage != (word)-1)? wVoltage*10 : (word)-1);

}

//--------------------------------------------------------------------
//[GP PLAYER]
//--------------------------------------------------------------------
#ifdef OPT_GPPLAYER
EEPromPoseHeader g_eepph;  // current header
byte g_bSeqStepNum;
word g_wSeqHeaderStart;
boolean g_fSeqProgmem;
transition_t *g_ptransCur;    // pointer to our current transisiton...

boolean fRobotUpsideDownGPStart;  // state when we start sequence
#ifdef USE_PYPOSE_HEADER
#define CNT_PYPOSE_SEGS (sizeof(PoseList)/sizeof(PoseList[0]))
#else
#define CNT_PYPOSE_SEGS 0
#endif
//--------------------------------------------------------------------
//[FIsGPSeqDefined]
//--------------------------------------------------------------------
boolean ServoDriver::FIsGPSeqDefined(uint8_t iSeq)
{
    return false;
}


//--------------------------------------------------------------------
// Setup to start sequence number...
//--------------------------------------------------------------------
void ServoDriver::GPStartSeq(uint8_t iSeq)
{
}

//--------------------------------------------------------------------
//[GP PLAYER]
//--------------------------------------------------------------------

void ServoDriver::GPPlayer(void)
{
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
uint8_t ServoDriver::GPNumSteps(void)          // How many steps does the current sequence have
{
  return  0;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
uint8_t ServoDriver::GPCurStep(void)           // Return which step currently on...
{
  return _fGPActive ? g_bSeqStepNum + 1 : 0xff;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void ServoDriver::GPSetSpeedMultiplyer(short sm)      // Set the Speed multiplier (100 is default)
{
  _sGPSM = sm;
}



#endif // OPT_GPPLAYER

//------------------------------------------------------------------------------------------
//[BeginServoUpdate] Does whatever preperation that is needed to starrt a move of our servos
//------------------------------------------------------------------------------------------
void ServoDriver::BeginServoUpdate(void)    // Start the update
{
  MakeSureServosAreOn();
}

//------------------------------------------------------------------------------------------
//[OutputServoInfoForLeg] Do the output to the SSC-32 for the servos associated with
//         the Leg number passed in.
//------------------------------------------------------------------------------------------
#ifdef c4DOF
void ServoDriver::OutputServoInfoForLeg(byte LegIndex, short sCoxaAngle1, short sFemurAngle1, short sTibiaAngle1, short sTarsAngle1)
#else
void ServoDriver::OutputServoInfoForLeg(byte LegIndex, short sCoxaAngle1, short sFemurAngle1, short sTibiaAngle1)
#endif
{
  word    wCoxaSDV;        // Coxa value in servo driver units
  word    wFemurSDV;        //
  word    wTibiaSDV;        //
#ifdef c4DOF
  word    wTarsSDV;        //
#endif
  // The Main code now takes care of the inversion before calling.
  wCoxaSDV = (((long)(sCoxaAngle1))* cPwmMult) / cPwmDiv +cPFConst;
  wFemurSDV = (((long)((long)(sFemurAngle1))* cPwmMult) / cPwmDiv +cPFConst);
  wTibiaSDV = (((long)(sTibiaAngle1))* cPwmMult) / cPwmDiv +cPFConst;
#ifdef c4DOF
  wTarsSDV = (((long)(sTarsAngle1))* cPwmMult) / cPwmDiv +cPFConst;
#endif
  if (ServosEnabled) {
    if (g_fAXSpeedControl) {
#ifdef USE_AX12_SPEED_CONTROL
      // Save away the new positions...
      g_awGoalAXPos[FIRSTCOXAPIN+LegIndex] = wCoxaSDV;    // What order should we store these values?
      g_awGoalAXPos[FIRSTFEMURPIN+LegIndex] = wFemurSDV;
      g_awGoalAXPos[FIRSTTIBIAPIN+LegIndex] = wTibiaSDV;
#ifdef c4DOF
      g_awGoalAXTarsPos[FIRSTTARSPIN+LegIndex] = wTarsSDV;
#endif
#endif
    }
    else {
      // With new library we set by Index.  Note Index defaults to servoID - 1
      bioloid.setNextPoseByIndex((cPinTable[FIRSTCOXAPIN+LegIndex])-1, wCoxaSDV);
      bioloid.setNextPoseByIndex((cPinTable[FIRSTFEMURPIN+LegIndex])-1, wFemurSDV);
      bioloid.setNextPoseByIndex((cPinTable[FIRSTTIBIAPIN+LegIndex])-1, wTibiaSDV);
#ifdef c4DOF
      if ((byte)(cTarsLength[LegIndex]))   // We allow mix of 3 and 4 DOF legs...
        bioloid.setNextPoseByIndex((cPinTable[FIRSTTARSPIN+LegIndex])-1, wTarsSDV);
#endif
    }
  }
#ifdef DEBUG_SERVOS
  if (g_fDebugOutput) {
    DBGSerial.print(LegIndex, DEC);
    DBGSerial.print("(");
    DBGSerial.print(sCoxaAngle1, DEC);
    DBGSerial.print("=");
    DBGSerial.print(wCoxaSDV, DEC);
    DBGSerial.print("),(");
    DBGSerial.print(sFemurAngle1, DEC);
    DBGSerial.print("=");
    DBGSerial.print(wFemurSDV, DEC);
    DBGSerial.print("),(");
    DBGSerial.print("(");
    DBGSerial.print(sTibiaAngle1, DEC);
    DBGSerial.print("=");
    DBGSerial.print(wTibiaSDV, DEC);
    DBGSerial.print(") :");
  }
#endif
  g_InputController.AllowControllerInterrupts(true);    // Ok for hserial again...
}


//==============================================================================
// Calculate servo speeds to achieve desired pose timing
// We make the following assumptions:
// AX-12 speed is 59rpm @ 12V which corresponds to 0.170s/60deg
// The AX-12 manual states this as the 'no load speed' at 12V
// The Moving Speed control table entry states that 0x3FF = 114rpm
// and according to Robotis this means 0x212 = 59rpm and anything greater 0x212 is also 59rpm
#ifdef USE_AX12_SPEED_CONTROL
word CalculateAX12MoveSpeed(word wCurPos, word wGoalPos, word wTime)
{
  word wTravel;
  uint32_t factor;
  word wSpeed;
  // find the amount of travel for each servo
  if( wGoalPos > wCurPos) {
    wTravel = wGoalPos - wCurPos;
  }
  else {
    wTravel = wCurPos - wGoalPos;
  }

  // now we can calculate the desired moving speed
  // for 59pm the factor is 847.46 which we round to 848
  // we need to use a temporary 32bit integer to prevent overflow
  factor = (uint32_t) 848 * wTravel;

  wSpeed = (uint16_t) ( factor / wTime );
  // if the desired speed exceeds the maximum, we need to adjust
  if (wSpeed > 1023) wSpeed = 1023;
  // we also use a minimum speed of 26 (5% of 530 the max value for 59RPM)
  if (wSpeed < 26) wSpeed = 26;

  return wSpeed;
}
#endif

//------------------------------------------------------------------------------------------
//[OutputServoInfoForTurret] Set up the outputse servos associated with an optional turret
//         the Leg number passed in.  FIRSTTURRETPIN
//------------------------------------------------------------------------------------------
#ifdef cTurretRotPin
void ServoDriver::OutputServoInfoForTurret(short sRotateAngle1, short sTiltAngle1)
{
  word    wRotateSDV;
  word    wTiltSDV;        //

  // The Main code now takes care of the inversion before calling.
  wRotateSDV = (((long)(sRotateAngle1))* cPwmMult) / cPwmDiv +cPFConst;
  wTiltSDV = (((long)((long)(sTiltAngle1))* cPwmMult) / cPwmDiv +cPFConst);

  if (ServosEnabled) {
    if (g_fAXSpeedControl) {
#ifdef USE_AX12_SPEED_CONTROL
      // Save away the new positions...
      g_awGoalAXPos[FIRSTTURRETPIN] = wRotateSDV;    // What order should we store these values?
      g_awGoalAXPos[FIRSTTURRETPIN+1] = wTiltSDV;
#endif
    }
    else {
      bioloid.setNextPoseByIndex(FIRSTTURRETPIN, wRotateSDV);
      bioloid.setNextPoseByIndex(FIRSTTURRETPIN, wRotateSDV);
      bioloid.setNextPoseByIndex(FIRSTTURRETPIN+1, wTiltSDV);
    }
  }
#ifdef DEBUG_SERVOS
  if (g_fDebugOutput) {
    DBGSerial.print("(");
    DBGSerial.print(sRotateAngle1, DEC);
    DBGSerial.print("=");
    DBGSerial.print(wRotateSDV, DEC);
    DBGSerial.print("),(");
    DBGSerial.print(sTiltAngle1, DEC);
    DBGSerial.print("=");
    DBGSerial.print(wTiltSDV, DEC);
    DBGSerial.print(") :");
  }
#endif
}
#endif
//--------------------------------------------------------------------
//[CommitServoDriver Updates the positions of the servos - This outputs
//         as much of the command as we can without committing it.  This
//         allows us to once the previous update was completed to quickly
//        get the next command to start
//--------------------------------------------------------------------
bool ServoDriver::CommitServoDriver(word wMoveTime)
{
#ifdef cSSC_BINARYMODE
  byte    abOut[3];
#endif

  g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...
  if (ServosEnabled) {
      bioloid.interpolateSetup(wMoveTime);
  }
#ifdef DEBUG_SERVOS
  if (g_fDebugOutput)
    DBGSerial.println(wMoveTime, DEC);
#endif
  g_InputController.AllowControllerInterrupts(true);
  return true;
}

//--------------------------------------------------------------------
//[FREE SERVOS] Frees all the servos
//--------------------------------------------------------------------
void ServoDriver::FreeServos(void)
{
  if (ServosEnabled) {
    g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...
    SetRegOnAllServos(AX_TORQUE_ENABLE, 0);
#if 0
    for (byte i = 0; i < NUMSERVOS; i++) {
      Relax((cPinTable[i]));
    }
#endif
    g_InputController.AllowControllerInterrupts(true);
    g_fServosFree = true;
  }
}

//--------------------------------------------------------------------
//Function that gets called from the main loop if the robot is not logically
//     on.  Gives us a chance to play some...
//--------------------------------------------------------------------
static uint8_t g_iIdleServoNum  = (uint8_t)-1;
static uint8_t g_iIdleLedState = 1;  // what state to we wish to set...
void ServoDriver::IdleTime(void)
{
  // Each time we call this set servos LED on or off...
  g_iIdleServoNum++;
  if (g_iIdleServoNum >= NUMSERVOS) {
    g_iIdleServoNum = 0;
    g_iIdleLedState = 1 - g_iIdleLedState;
  }
  dxl_write_byte((cPinTable[g_iIdleServoNum]), AX_LED, g_iIdleLedState);
  dxl_get_result();   // don't care for now
}

//--------------------------------------------------------------------
//[SetRegOnAllServos] Function that is called to set the state of one
//  register in all of the servos, like Torque on...
//--------------------------------------------------------------------
void SetRegOnAllServos(uint8_t bReg, uint8_t bVal)
{
    dxl_set_txpacket_id(AX_ID_BROADCAST);
    dxl_set_txpacket_instruction(AX_CMD_SYNC_WRITE);

    dxl_set_txpacket_parameter(0, bReg);    // which register
    dxl_set_txpacket_parameter(1, 1);       // 1 byte
    dxl_set_txpacket_length(2*NUMSERVOS+3);

    for (byte i = 0; i < NUMSERVOS; i++) {
        dxl_set_txpacket_parameter(2+i*2, (cPinTable[i]));       // 1 byte
        dxl_set_txpacket_parameter(3+i*2, bVal);       // 1 byte
    }
    dxl_txrx_packet();
    //return dxl_get_result();   // don't care for now
}

//--------------------------------------------------------------------
//[MakeSureServosAreOn] Function that is called to handle when you are
//  transistioning from servos all off to being on.  May need to read
//  in the current pose...
//--------------------------------------------------------------------
void MakeSureServosAreOn(void)
{
  if (ServosEnabled) {
    if (!g_fServosFree)
      return;    // we are not free

    g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...
    bioloid.readPose();

    SetRegOnAllServos(AX_TORQUE_ENABLE, 1);
#if 0
    for (byte i = 0; i < NUMSERVOS; i++) {
      TorqueOn((cPinTable[i]));
    }
#endif
    g_InputController.AllowControllerInterrupts(true);
    g_fServosFree = false;
  }
}


#ifdef OPT_TERMINAL_MONITOR
//==============================================================================
// ShowTerminalCommandList: Allow the Terminal monitor to call the servo driver
//      to allow it to display any additional commands it may have.
//==============================================================================
void ServoDriver::ShowTerminalCommandList(void)
{
  DBGSerial.println("M - Toggle Motors on or off");
  DBGSerial.println("F<frame length> - FL in ms");    // BUGBUG::
  DBGSerial.println("A - Toggle AX12 speed control");
  DBGSerial.println("T - Test Servos");
#ifdef OPT_PYPOSE
  DBGSerial.println("P<DL PC> - Pypose");
#endif
#ifdef OPT_FIND_SERVO_OFFSETS
  DBGSerial.println("O - Enter Servo offset mode");
#endif
}

//==============================================================================
// ProcessTerminalCommand: The terminal monitor will call this to see if the
//     command the user entered was one added by the servo driver.
//==============================================================================
boolean ServoDriver::ProcessTerminalCommand(byte *psz, byte bLen)
{
  if ((bLen == 1) && ((*psz == 'm') || (*psz == 'M'))) {
    g_fEnableServos = !g_fEnableServos;
    if (g_fEnableServos)
      DBGSerial.println("Motors are on");
    else
      DBGSerial.println("Motors are off");

    return true;
  }

  if ((bLen == 1) && ((*psz == 't') || (*psz == 'T'))) {
    // Test to see if all servos are responding...
    for(int i=1;i<=NUMSERVOS;i++){
      word w;
      w = ax12GetRegister(i,AX_PRESENT_POSITION_L, 2);
      DBGSerial.print(i,DEC);
      DBGSerial.print("=");
      DBGSerial.println(w, DEC);
      delay(25);
    }
  }

  if ((bLen == 1) && ((*psz == 'a') || (*psz == 'A'))) {
    g_fAXSpeedControl = !g_fAXSpeedControl;
    if (g_fAXSpeedControl)
      DBGSerial.println("AX12 Speed Control");
    else
      DBGSerial.println("Bioloid Speed");
  }
  if ((bLen >= 1) && ((*psz == 'f') || (*psz == 'F'))) {
    psz++;  // need to get beyond the first character
    while (*psz == ' ')
      psz++;  // ignore leading blanks...
    byte bFrame = 0;
    while ((*psz >= '0') && (*psz <= '9')) {  // Get the frame count...
      bFrame = bFrame*10 + *psz++ - '0';
    }
    if (bFrame != 0) {
      DBGSerial.print("New Servo Cycles per second: ");
      DBGSerial.println(1000/bFrame, DEC);
      extern BioloidControllerEx bioloid;
      bioloid.frameLength = bFrame;
    }
  }

#ifdef OPT_FIND_SERVO_OFFSETS
  else if ((bLen == 1) && ((*psz == 'o') || (*psz == 'O'))) {
    FindServoOffsets();
  }
#endif
#ifdef OPT_PYPOSE
  else if ((*psz == 'p') || (*psz == 'P')) {
    DoPyPose(++psz);
  }
#endif
   return false;

}
#endif



//==============================================================================
//	FindServoOffsets - Find the zero points for each of our servos...
// 		Will use the new servo function to set the actual pwm rate and see
//		how well that works...
//==============================================================================
#ifdef OPT_FIND_SERVO_OFFSETS

void FindServoOffsets()
{

}
#endif  // OPT_FIND_SERVO_OFFSETS



