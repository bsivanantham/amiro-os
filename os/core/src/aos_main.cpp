/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2018  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <amiroos.h>
#include <module.h>

/*
 * hook to add further includes
 */
#if defined(AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER)
#include AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER
#endif

/**
 * @brief   Event mask to identify I/O events.
 */
#define IOEVENT_MASK                            EVENT_MASK(0)

/**
 * @brief   Event mask to identify OS events.
 */
#define OSEVENT_MASK                            EVENT_MASK(1)

/**
 * @brief   Event mask to idetify CAN events.
 */
#define CANEVENT_MASK                           EVENT_MASK(2)

/**
 * @brief   Event mask to idetify timeout events.
 */
#define TIMEOUTEVENT_MASK                       EVENT_MASK(3)

/**
 * @brief   Event mask to idetify signal delay events.
 */
#define DELAYEVENT_MASK                         EVENT_MASK(4)

/**
 * @brief   CAN message identifier for initialization of the SSSP stack initialization sequence.
 */
#define SSSP_STACKINIT_CANMSGID_INIT            0x003

/**
 * @brief   CAN message identifier for transmitting module IDs during the SSSP stack initialization sequence.
 */
#define SSSP_STACKINIT_CANMSGID_MODULEID        0x002

/**
 * @brief   CAN message identifier for abortion of the SSSP stack initialization sequence.
 */
#define SSSP_STACKINIT_CANMSGID_ABORT           0x001

/**
 * @brief   CAN message identifier for calender synchronization message.
 */
#define CALENDERSYNC_CANMSGID                   0x004

/**
 * @brief   Listener object for I/O events.
 */
static event_listener_t _eventListenerIO;

/**
 * @brief   Listener object for OS events.
 */
static event_listener_t _eventListenerOS;

#if defined(MODULE_HAL_PROGIF) || defined(__DOXYGEN__)
/**
 * @brief   I/O channel for the programmer interface.
 */
static AosIOChannel _stdiochannel;

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   I/O shell channel for the programmer interface.
 */
static AosShellChannel _stdshellchannel;
#endif
#endif

/*
 * hook to add further static variables
 */
#if defined(AMIROOS_CFG_MAIN_EXTRA_STATIC_VARIABLES)
AMIROOS_CFG_MAIN_EXTRA_STATIC_VARIABLES
#endif

/**
 * @brief   Prints an error message about an unexpected event.
 *
 * @param[in] mask    The event mask.
 * @param[in] flags   The event flags.
 */
static inline void _unexpectedEventError(const eventmask_t mask, const eventflags_t flags)
{
#if (AMIROOS_CFG_DBG == true)
  aosprintf("unexpected/unknown event received. mask: 0x%08X; flags: 0x%08X\n", mask, flags);
#else
  (void)(mask);
  (void)(flags);
#endif
  return;
}

/**
 * @brief   Callback function to be used during SSSP stack initialization sequence.
 *
 * @param[in] par   A pointer to an @p event_source_t to be fired.
 */
static void _ssspTimerCallback(void* par)
{
  aosDbgCheck(par != NULL);

  chSysLockFromISR();
  chEvtBroadcastI((event_source_t*)par);
  chSysUnlockFromISR();

  return;
}

/**
 * @brief   Helper function to serialize data.
 *
 * @param[out]  dst   Pointer to the output buffer.
 * @param[in]   src   Data to be serialized.
 * @param[in]   n     Number of bytes to serialize.
 */
inline void _serialize(uint8_t* dst, const uint64_t src, const uint8_t n)
{
  aosDbgCheck(dst != NULL);
  aosDbgCheck(n > 0 && n <= 8);

  for (uint8_t byte = 0; byte < n; ++byte) {
    dst[byte] = (uint8_t)((src >> (byte * 8)) & 0xFF);
  }

  return;
}

/**
 * @brief   Helper function to deserialize data.
 *
 * @param[in] src   Pointer to the buffer of data to be deserialzed.
 * @param[in] n     Number of bytes to deserialize.
 *
 * @return    The deserialized 32 bit data.
 */
inline uint64_t _deserialize(uint8_t* src, const uint8_t n)
{
  aosDbgCheck(src != NULL);
  aosDbgCheck(n > 0 && n <= 8);

  uint64_t result = 0;
  for (uint8_t byte = 0; byte < n; ++byte) {
    result |= ((uint64_t)src[byte]) << (byte * 8);
  }

  return result;
}

/**
 * @brief   Converter function to encode a TM value to a single unsigned 64 bit integer.
 *
 * @details Contents of the TM struct are mapped as follows:
 *            bits  |63     62|61      53|52    50|49         26|25     22|21     17|16     12|11      6|5       0|
 *            #bits |       2 |        9 |      3 |          24 |       4 |       5 |       5 |       6 |       6 |
 *            value |   isdst |     yday |   wday |        year |     mon |    mday |    hour |     min |     sec |
 *            range | special | [0, 365] | [0, 6] | [1900, ...] | [0, 11] | [1, 31] | [0, 23] | [0, 59] | [0, 61] |
 *          The Daylight Saving Time Flag (isdsst) is encoded as follows:
 *            DST not in effect         -> 0
 *            DST in effect             -> 1
 *            no information available  -> 2
 *
 * @param[in] src   Pointer to the TM struct to encode.
 *
 * @return  An unsigned 64 bit integer, which holds the encoded time value.
 */
inline uint64_t _TM2U64(struct tm* src)
{
  aosDbgCheck(src != NULL);

  return (((uint64_t)(src->tm_sec  & 0x0000003F) << (0))               |
          ((uint64_t)(src->tm_min  & 0x0000003F) << (6))               |
          ((uint64_t)(src->tm_hour & 0x0000001F) << (12))              |
          ((uint64_t)(src->tm_mday & 0x0000001F) << (17))              |
          ((uint64_t)(src->tm_mon  & 0x0000000F) << (22))              |
          ((uint64_t)(src->tm_year & 0x00FFFFFF) << (26))              |
          ((uint64_t)(src->tm_wday & 0x00000007) << (50))              |
          ((uint64_t)(src->tm_yday & 0x000001FF) << (53))              |
          ((uint64_t)((src->tm_isdst == 0) ? 0 : (src->tm_isdst > 0) ? 1 : 2) << (62)));
}

/**
 * @brief   Converter functiomn to retrieve the encoded TM value from an unsigned 64 bit integer.
 *
 * @details For information on the encoding, please refer to @p _TM2U64 function.
 *
 * @param[out] dst  The TM struct to fill with the decoded values.
 * @param[in]  src  Unsigned 64 bit integer holding the encoded TM value.
 */
inline void _U642TM(struct tm* dst, const uint64_t src)
{
  aosDbgCheck(dst != NULL);

  dst->tm_sec  = (src >> 0)  & 0x0000003F;
  dst->tm_min  = (src >> 6)  & 0x0000003F;
  dst->tm_hour = (src >> 12) & 0x0000001F;
  dst->tm_mday = (src >> 17) & 0x0000001F;
  dst->tm_mon  = (src >> 22) & 0x0000000F;
  dst->tm_year = (src >> 26) & 0x00FFFFFF;
  dst->tm_wday = (src >> 50) & 0x00000007;
  dst->tm_yday = (src >> 53) & 0x000001FF;
  dst->tm_isdst = (((src >> 62) & 0x03) == 0) ? 0 : (((src >> 62) & 0x03) > 0) ? 1 : -1;

  return;
}

/**
 * @brief   Implementation of the SSSP module stack initialization sequence (startup phase 3).
 *
 * @return Shutdown value.
 * @retval AOS_SHUTDOWN_NONE      No shutdown signal received
 * @retval AOS_SHUTDOWN_PASSIVE   Shutdown signal received.
 */
aos_shutdown_t _ssspModuleStackInitialization(void)
{
  // local types
  /**
   * @brief   States for the internal state machine to implement SSSP startup stage 3.
   */
  typedef enum {
    STAGE_3_1,                  /**< Initiation of SSSP startup stage 3. */
    STAGE_3_2,                  /**< Starting the sequence and broadcasting the first ID. */
    STAGE_3_3_WAITFORFIRSTID,   /**< Waiting for first ID after initiation. */
    STAGE_3_3_WAITFORIDORSIG,   /**< Waiting for next ID or activation of neighbor signal. */
    STAGE_3_3_WAITFORID,        /**< Waiting for next ID (after the module has set its own ID). */
    STAGE_3_4_FINISH,           /**< Successful finish of stage 3. */
    STAGE_3_4_ABORT_ACTIVE,     /**< Aborting stage 3 (active). */
    STAGE_3_4_ABORT,            /**< Aborting stage 3 (passive). */
  } sssp_modulestackinitstage_t;

  typedef struct {
    bool loop     : 1;
    bool wfe      : 1;
    bool wfe_next : 1;
  } flags_t;

  // local variables
  aos_shutdown_t shutdown = AOS_SHUTDOWN_NONE;
  sssp_modulestackinitstage_t stage = STAGE_3_1;
  eventmask_t eventmask = 0;
  eventflags_t ioflags;
  event_source_t eventSourceTimeout;
  event_source_t eventSourceDelay;
  event_listener_t eventListenerTimeout;
  event_listener_t eventListenerDelay;
  event_listener_t eventListenerCan;
  virtual_timer_t timerTimeout;
  virtual_timer_t timerDelay;
  CANTxFrame canTxFrame;
  CANRxFrame canRxFrame;
#if (AMIROOS_CFG_SSSP_STACK_START != true) || (AMIROOS_CFG_DBG == true)
  aos_ssspmoduleid_t lastid = 0;
#endif
  flags_t flags;

  // initialize local varibles
  chEvtObjectInit(&eventSourceTimeout);
  chEvtObjectInit(&eventSourceDelay);
  chVTObjectInit(&timerTimeout);
  chVTObjectInit(&timerDelay);
  canTxFrame.RTR = CAN_RTR_DATA;
  canTxFrame.IDE = CAN_IDE_STD;
  flags.loop = true;
  flags.wfe = false; // do not wait for events in the initial iteration of the FSM loop
  flags.wfe_next = true;

  // initialize system variables
  aos.sssp.stage = AOS_SSSP_STARTUP_3_1;
  aos.sssp.moduleId = 0;

  // listen to events (timout, delay, CAN receive)
  chEvtRegisterMask(&eventSourceTimeout, &eventListenerTimeout, TIMEOUTEVENT_MASK);
  chEvtRegisterMask(&eventSourceDelay, &eventListenerDelay, DELAYEVENT_MASK);
  chEvtRegisterMask(&MODULE_HAL_CAN.rxfull_event, &eventListenerCan, CANEVENT_MASK);

  /*
   * FSM in a loop.
   *
   * This is a fully event-based FSM for the module stack initialization
   * sequence, defined by SSSP as startup stage 3. There are five different
   * events that can occur at this point:
   *  I/O events: The input level of an input pin has changed. Such events must
   *              be handled differently depending on the current state. Most
   *              of the time, however, such events can be ignored.
   *  OS events:  Such events are only available after this stage completed and
   *              thus should never occur. However, there is an optional hook
   *              to handle such events, nevertheless.
   *  CAN events: At least one CAN message was received. Note that this event
   *              will only fire again if all input buffers have been cleared.
   *  timeouts:   If some module does not support the sequence of there is any
   *              issue, such a case is detected via timeouts and must be
   *              handled accordingly (see abort state). In some cases, it is
   *              possible that a timeout event occurres 'simultaneously' with
   *              some other event. This can be caused by several timing issues
   *              and is a valid situation. As a result, any other events
   *              should be handled before the timeout event. If the other
   *              events are expected and valid, this implementation requires
   *              the timeout event flag to be cleared explicitely. Otherwise
   *              it is evaluated at the end of each iteration of the loop.
   *  delays:     Depending on the current state, delays are required by SSSP
   *              for timing of the sequential activation of signals.
   */
  aosDbgPrintf("SSSP stack initialization sequence:\n");
  while (flags.loop) {
#if (AMIROOS_CFG_DBG == true)
    switch (stage) {
      case STAGE_3_1:
        aosDbgPrintf(">>> 3-1\n");
        break;
      case STAGE_3_2:
        aosDbgPrintf(">>> 3-2\n");
        break;
      case STAGE_3_3_WAITFORFIRSTID:
        aosDbgPrintf(">>> 3-3 (1st ID)\n");
        break;
      case STAGE_3_3_WAITFORIDORSIG:
        aosDbgPrintf(">>> 3-3 (ID/sig)\n");
        break;
      case STAGE_3_3_WAITFORID:
        aosDbgPrintf(">>> 3-3 (ID)\n");
        break;
      case STAGE_3_4_FINISH:
        aosDbgPrintf(">>> 3-4 (finish)\n");
        break;
      case STAGE_3_4_ABORT_ACTIVE:
        aosDbgPrintf(">>> 3-4 (active abort)\n");
        break;
      case STAGE_3_4_ABORT:
        aosDbgPrintf(">>> 3-4 (abort)\n");
        break;
    }
#endif

    // reset wfe flag for the next iteration
    flags.wfe_next = true;

    // waiting for events may be skipped
    if (flags.wfe) {
      // wait for any event to occur
      aosDbgPrintf("WFE...");
      eventmask = chEvtWaitAnyTimeout(ALL_EVENTS, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT));
      aosDbgPrintf("\t0x%08X", eventmask);
    } else {
      aosDbgPrintf("WFE skipped");
    }
    aos_timestamp_t uptime;
    aosSysGetUptime(&uptime);
    aosDbgPrintf("\t%04ums\n", (uint32_t)(uptime / 1000));

    /*
     * execute some general tasks and high priority events
     */
    // no event occurred at all
    if ((flags.wfe) && (eventmask == 0)) {
      aosDbgPrintf("ERR: no evt\n");
      // enforce timeout event
      chEvtBroadcast(&eventSourceTimeout);
      continue;
    }
    // if an IO event occurred
    if (eventmask & _eventListenerIO.events) {
      ioflags = chEvtGetAndClearFlags(&_eventListenerIO);
      aosDbgPrintf("INFO: IO evt (0x%08X)\n", ioflags);
      // a power-down event occurred
      if (ioflags & MODULE_SSSP_EVENTFLAGS_PD) {
        aosDbgPrintf("PD evt\n");
        // deactivate S and UP
        apalControlGpioSet(&moduleSsspGpioSync, APAL_GPIO_OFF);
  #if (AMIROOS_CFG_SSSP_STACK_END != true)
        apalControlGpioSet(&moduleSsspGpioUp, APAL_GPIO_OFF);
  #endif
        // set shutdown flag and exit the loop
        shutdown = AOS_SHUTDOWN_PASSIVE;
        break;
      }
      // the S signal was deactivated
      if (ioflags & MODULE_SSSP_EVENTFLAGS_SYNC) {
        apalControlGpioState_t sstate;
        apalControlGpioGet(&moduleSsspGpioSync, &sstate);
        if (sstate == APAL_GPIO_OFF) {
          aosDbgPrintf("-S evt\n");
          // either finish or abort
          if ((stage == STAGE_3_3_WAITFORID) && (aos.sssp.moduleId != 0)) {
            stage = STAGE_3_4_FINISH;
          } else if (stage != STAGE_3_4_ABORT) {
            stage = STAGE_3_4_ABORT_ACTIVE;
          }
        }
      }
    }
    // an OS event occurred
    if (eventmask & _eventListenerOS.events) {
      aosDbgPrintf("WARN: OS evt\n");
      // get the flags
      eventflags_t oseventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      // there should be no OS events at this point
#ifdef MODULE_SSSP_STARTUP_3_OSEVENT_HOOK
      MODULE_SSSP_STARTUP_3_OSEVENT_HOOK(eventmask, eventflags);
#else
      _unexpectedEventError(eventmask, oseventflags);
#endif
    }
    // if a CAN event occurred
    if ((eventmask & eventListenerCan.events)) {
      // fetch message
      if (flags.wfe) {
        canReceiveTimeout(&MODULE_HAL_CAN, CAN_ANY_MAILBOX, &canRxFrame, TIME_IMMEDIATE);
        aosDbgPrintf("CAN <- 0x%03X\n", canRxFrame.SID);
      }
      // identify and handle abort messgaes
      if (canRxFrame.SID == SSSP_STACKINIT_CANMSGID_ABORT) {
        stage = STAGE_3_4_ABORT;
      }
      // warn if a unexpected message was received
      else if ((canRxFrame.SID != SSSP_STACKINIT_CANMSGID_INIT) &&
               (canRxFrame.SID != SSSP_STACKINIT_CANMSGID_MODULEID)) {
        aosDbgPrintf("WARN: unknown msg\n");
      }
      // any further pending messages are fetched at the end of the loop
    }
    // if a timeout event occurred
    if (eventmask & eventListenerTimeout.events) {
      // is handled at the end of the loop (or must be cleared by FSM)
    }
    // if a delay event occurred
    if (eventmask & eventListenerDelay.events) {
      // is handled by FSM
    }

    /*
     * this is the actual FSM
     */
    switch (stage) {
      case STAGE_3_1:
      {
        aos.sssp.stage = AOS_SSSP_STARTUP_3_1;

        // there was no event at all (skipped wfe)
        if (eventmask == 0 && flags.wfe == false) {
#if (AMIROOS_CFG_SSSP_MASTER == true)
          // initialize the stage by transmitting an according CAN message
          aosDbgPrintf("CAN -> init\n");
          canTxFrame.DLC = 0;
          canTxFrame.SID = SSSP_STACKINIT_CANMSGID_INIT;
          if (canTransmitTimeout(&MODULE_HAL_CAN, CAN_ANY_MAILBOX, &canTxFrame, TIME_IMMEDIATE) != MSG_OK) {
            chEvtBroadcast(&eventSourceTimeout);
            break;
          }
          // activate S
          aosDbgPrintf("S+\n");
          apalControlGpioSet(&moduleSsspGpioSync, APAL_GPIO_ON);
#if (AMIROOS_CFG_SSSP_STACK_START == true)
          // proceed immediately
          stage = STAGE_3_2;
          flags.wfe_next = false;
#else
          // set the timeout timer
          chVTSet(&timerTimeout, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT), _ssspTimerCallback, &eventSourceTimeout);
          // proceed
          stage = STAGE_3_3_WAITFORFIRSTID;
#endif
#else
          // set the timeout timer
          chVTSet(&timerTimeout, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT), _ssspTimerCallback, &eventSourceTimeout);
#endif
        }

#if (AMIROOS_CFG_SSSP_MASTER != true)
        // a CAN message was received
        else if (eventmask & eventListenerCan.events) {
          // if an initiation message was received
          if (canRxFrame.DLC == 0 &&
              canRxFrame.RTR == CAN_RTR_DATA &&
              canRxFrame.IDE == CAN_IDE_STD &&
              canRxFrame.SID == SSSP_STACKINIT_CANMSGID_INIT) {
            aosDbgPrintf("init msg\n");
            // reset the timeout timer and clear pending flags
            chVTReset(&timerTimeout);
            chEvtWaitAnyTimeout(eventListenerTimeout.events, TIME_IMMEDIATE);
            eventmask &= ~(eventListenerTimeout.events);
            // activate S
            aosDbgPrintf("S+\n");
            apalControlGpioSet(&moduleSsspGpioSync, APAL_GPIO_ON);
#if (AMIROOS_CFG_SSSP_STACK_START == true)
            // proceed
            stage = STAGE_3_2;
            flags.wfe_next = false;
#else
            // set the timeout timer
            chVTSet(&timerTimeout, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT), _ssspTimerCallback, &eventSourceTimeout);
            // proceed
            stage = STAGE_3_3_WAITFORFIRSTID;
#endif
          }
        }
#endif

        break;
      } /* end of STAGE_3_1 */

      case STAGE_3_2:
      {
#if (AMIROOS_CFG_SSSP_STACK_START == true)
        aos.sssp.stage = AOS_SSSP_STARTUP_3_2;

        // if this stage was just entered
        if (flags.wfe == false) {
          // set the module ID
          aos.sssp.moduleId = 1;
          // broadcast module ID
          aosDbgPrintf("CAN -> ID (%u)\n", aos.sssp.moduleId);
          canTxFrame.DLC = 4;
          canTxFrame.SID = SSSP_STACKINIT_CANMSGID_MODULEID;
          _serialize(canTxFrame.data8, aos.sssp.moduleId, 4);
          if (canTransmitTimeout(&MODULE_HAL_CAN, CAN_ANY_MAILBOX, &canTxFrame, TIME_IMMEDIATE) != MSG_OK) {
            chEvtBroadcast(&eventSourceTimeout);
            break;
          }
#if (AMIROOS_CFG_SSSP_STACK_START != true) || (AMIROOS_CFG_DBG == true)
          lastid = aos.sssp.moduleId;
#endif
#if (AMIROOS_CFG_SSSP_STACK_END == true)
          // sequence is already over
          // deactivate S
          aosDbgPrintf("S-\n");
          apalControlGpioSet(&moduleSsspGpioSync, APAL_GPIO_OFF);
          // proceed
          stage = STAGE_3_3_WAITFORID;
#else
          // set the delay timer so the UP signal is activated later
          chVTSet(&timerDelay, LL_US2ST(AMIROOS_CFG_SSSP_SIGNALDELAY), _ssspTimerCallback, &eventSourceDelay);
#endif
        }

        // if a delay event occurred
        if (eventmask & eventListenerDelay.events) {
          // activate UP
          aosDbgPrintf("UP+\n");
          apalControlGpioSet(&moduleSsspGpioUp, APAL_GPIO_ON);
          // deactivate S
          aosDbgPrintf("S-\n");
          apalControlGpioSet(&moduleSsspGpioSync, APAL_GPIO_OFF);
          // explicitely clear timeout event flag
          chEvtWaitAnyTimeout(eventListenerTimeout.events, TIME_IMMEDIATE);
          eventmask &= ~(eventListenerTimeout.events);
          // proceed
          stage = STAGE_3_3_WAITFORID;
        }
#endif

        break;
      } /* end of STAGE_3_2 */

      case STAGE_3_3_WAITFORFIRSTID:
      {
#if (AMIROOS_CFG_SSSP_STACK_START != true)
        aos.sssp.stage = AOS_SSSP_STARTUP_3_3;

        // a CAN message was received
        if (eventmask & eventListenerCan.events) {
          // if an ID message was received
          if (canRxFrame.DLC == 4 &&
              canRxFrame.RTR == CAN_RTR_DATA &&
              canRxFrame.IDE == CAN_IDE_STD &&
              canRxFrame.SID == SSSP_STACKINIT_CANMSGID_MODULEID) {
            aosDbgPrintf("ID (%u)\n", (uint32_t)_deserialize(canRxFrame.data8, 4));
            // validate received ID
            if (lastid < _deserialize(canRxFrame.data8, 4)) {
              // store received ID
              lastid = _deserialize(canRxFrame.data8, 4);
              // restart timeout timer
              chVTSet(&timerTimeout, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT), _ssspTimerCallback, &eventSourceTimeout);
              // proceed
              stage = STAGE_3_3_WAITFORIDORSIG;
            } else {
              aosDbgPrintf("ERR: invalid ID\n");
              // abort
              stage = STAGE_3_4_ABORT_ACTIVE;
              flags.wfe_next = false;
            }
            // explicitely clear timeout event flag
            chEvtWaitAnyTimeout(eventListenerTimeout.events, TIME_IMMEDIATE);
            eventmask &= ~(eventListenerTimeout.events);
          }
        }
#endif
        break;
      } /* end of STAGE_3_3_WAITFORFIRSTID */

      case STAGE_3_3_WAITFORIDORSIG:
      {
#if (AMIROOS_CFG_SSSP_STACK_START != true)
        aos.sssp.stage = AOS_SSSP_STARTUP_3_3;

        // a CAN message was received
        if (eventmask & eventListenerCan.events) {
          // if an ID message was received
          if (canRxFrame.DLC == 4 &&
              canRxFrame.RTR == CAN_RTR_DATA &&
              canRxFrame.IDE == CAN_IDE_STD &&
              canRxFrame.SID == SSSP_STACKINIT_CANMSGID_MODULEID) {
            aosDbgPrintf("ID (%u)\n", (uint32_t)_deserialize(canRxFrame.data8, 4));
            // validate received ID
            if (lastid < _deserialize(canRxFrame.data8, 4)) {
              // store received ID
              lastid = _deserialize(canRxFrame.data8, 4);
              // restart timeout timer
              chVTSet(&timerTimeout, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT), _ssspTimerCallback, &eventSourceTimeout);
            } else {
              aosDbgPrintf("ERR: invalid ID\n");
              // abort
              stage = STAGE_3_4_ABORT_ACTIVE;
              flags.wfe_next = false;
            }
            // explicitely clear timeout event flag
            chEvtWaitAnyTimeout(eventListenerTimeout.events, TIME_IMMEDIATE);
            eventmask &= ~(eventListenerTimeout.events);
          }
        }

        // if an IO event was received (DN signal)
        if ((eventmask & _eventListenerIO.events) && (ioflags & MODULE_SSSP_EVENTFLAGS_DN)) {
          aosDbgPrintf("DN <-\n");
          // reset timeout timer
          chVTReset(&timerTimeout);
          chEvtWaitAnyTimeout(eventListenerTimeout.events, TIME_IMMEDIATE);
          eventmask &= ~(eventListenerTimeout.events);
          // increment and broadcast ID
          aos.sssp.moduleId = lastid + 1;
          aosDbgPrintf("CAN -> ID (%u)\n", aos.sssp.moduleId);
          canTxFrame.DLC = 4;
          canTxFrame.SID = SSSP_STACKINIT_CANMSGID_MODULEID;
          _serialize(canTxFrame.data8, aos.sssp.moduleId, 4);
          if (canTransmitTimeout(&MODULE_HAL_CAN, CAN_ANY_MAILBOX, &canTxFrame, TIME_IMMEDIATE) != MSG_OK) {
            chEvtBroadcast(&eventSourceTimeout);
            break;
          }
          // set delay timer
          chVTSet(&timerDelay, LL_US2ST(AMIROOS_CFG_SSSP_SIGNALDELAY), _ssspTimerCallback, &eventSourceDelay);
        }

        // if a delay event occurred
        if (eventmask & eventListenerDelay.events) {
#if (AMIROOS_CFG_SSSP_STACK_END != true)
          // activate UP
          aosDbgPrintf("UP+\n");
          apalControlGpioSet(&moduleSsspGpioUp, APAL_GPIO_ON);
#endif
          // deactivate S
          aosDbgPrintf("S-\n");
          apalControlGpioSet(&moduleSsspGpioSync, APAL_GPIO_OFF);
          // reset the timeout timer
          chVTSet(&timerTimeout, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT), _ssspTimerCallback, &eventSourceTimeout);
          chEvtWaitAnyTimeout(eventListenerTimeout.events, TIME_IMMEDIATE);
          eventmask &= ~(eventListenerTimeout.events);
          // proceed
          stage = STAGE_3_3_WAITFORID;
        }
#endif

        break;
      } /* end of STAGE_3_3_WAITFORIDORSIG */

      case STAGE_3_3_WAITFORID:
      {
        aos.sssp.stage = AOS_SSSP_STARTUP_3_3;

#if (AMIROOS_CFG_SSSP_STACK_END != true)
        // a CAN message was received
        if (eventmask & eventListenerCan.events) {
          // if an ID message was received
          if (canRxFrame.DLC == 4 &&
              canRxFrame.RTR == CAN_RTR_DATA &&
              canRxFrame.IDE == CAN_IDE_STD &&
              canRxFrame.SID == SSSP_STACKINIT_CANMSGID_MODULEID) {
#if (AMIROOS_CFG_SSSP_STACK_START != true) || (AMIROOS_CFG_DBG == true)
            // Plausibility of the received ID is not checked at this point but is done by other modules still in a previous stage.
            lastid = _deserialize(canRxFrame.data8, 4);
            aosDbgPrintf("ID (%u)\n", lastid);
#endif
            // restart timeout timer
            chVTSet(&timerTimeout, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT), _ssspTimerCallback, &eventSourceTimeout);
            chEvtWaitAnyTimeout(eventListenerTimeout.events, TIME_IMMEDIATE);
            eventmask &= ~(eventListenerTimeout.events);
          }
        }
#endif

        break;
      } /* end of STAGE_3_3_WAITFORID */

      case STAGE_3_4_FINISH:
      {
        aos.sssp.stage = AOS_SSSP_STARTUP_3_4;

        // if an IO event was received (S signal)
        if ((eventmask & _eventListenerIO.events) && (ioflags & MODULE_SSSP_EVENTFLAGS_SYNC)) {
          // reset the timeout timer
          chVTReset(&timerTimeout);
          chEvtWaitAnyTimeout(eventListenerTimeout.events, TIME_IMMEDIATE);
          eventmask &= ~(eventListenerTimeout.events);
          //set the delay timer
          chVTSet(&timerDelay, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT), _ssspTimerCallback, &eventSourceDelay);
        }

        // if a CAN event was received
        if (eventmask & eventListenerCan.events) {
          // if an abort message was received
          if (canRxFrame.SID == SSSP_STACKINIT_CANMSGID_ABORT) {
            aosDbgPrintf("abort msg\n");
            // reset the delay timer
            chVTReset(&timerDelay);
            chEvtWaitAnyTimeout(eventListenerDelay.events, TIME_IMMEDIATE);
            eventmask &= ~(eventListenerDelay.events);
            // proceed
            stage = STAGE_3_4_ABORT;
          }
        }

        // if a delay timer event occurred
        if (eventmask & eventListenerDelay.events) {
          aosDbgPrintf("sequence sucessful\n");
          // sequence finished sucessfully
          flags.loop = false;
        }

        break;
      } /* end of STAGE_3_4_FINISH */

      case STAGE_3_4_ABORT_ACTIVE:
      {
        aos.sssp.stage = AOS_SSSP_STARTUP_3_4;

        // emit abort message
        canTxFrame.DLC = 0;
        canTxFrame.SID = SSSP_STACKINIT_CANMSGID_ABORT;
        canTransmitTimeout(&MODULE_HAL_CAN, CAN_ANY_MAILBOX, &canTxFrame, TIME_INFINITE);
        aosDbgPrintf("CAN -> abort\n");
        // clear timeout flag
        eventmask &= ~(eventListenerTimeout.events);
        // proceed immediately
        stage = STAGE_3_4_ABORT;
        flags.wfe_next = false;
        break;
      } /* end of STAGE_3_4_ABORT_ACTIVE */

      case STAGE_3_4_ABORT:
      {
        aos.sssp.stage = AOS_SSSP_STARTUP_3_4;

        // deactivate S
        aosDbgPrintf("S-\n");
        apalControlGpioSet(&moduleSsspGpioSync, APAL_GPIO_OFF);
        // invalidate module ID
        aos.sssp.moduleId = 0;

        // if an IO event was received (S signal)
        if ((eventmask & _eventListenerIO.events) && (ioflags & MODULE_SSSP_EVENTFLAGS_SYNC)) {
          aosDbgPrintf("sequence aborted\n");
          // exit the sequence
          flags.loop = false;
        }

        break;
      } /* end of STAGE_3_4_ABORT */
    }

    // fetch pending CAN message (if any)
    if ((eventmask & eventListenerCan.events) && (canReceiveTimeout(&MODULE_HAL_CAN, CAN_ANY_MAILBOX, &canRxFrame, TIME_IMMEDIATE) == MSG_OK)) {
      aosDbgPrintf("CAN <- 0x%03X\n", canRxFrame.SID);
      flags.wfe_next = false;
    }

    // handle unhandled timeout events
    if (eventmask & eventListenerTimeout.events) {
      aosDbgPrintf("ERR: timeout evt\n");
      // abort
      flags.wfe_next = false;
      stage = STAGE_3_4_ABORT_ACTIVE;
    }

    // apply wfe value for next iteration
    flags.wfe = flags.wfe_next;
  }
  aosDbgPrintf("\n");

  // unregister all events (timeout, delay, CAN receive)
  chEvtUnregister(&eventSourceTimeout, &eventListenerTimeout);
  chEvtUnregister(&eventSourceDelay, &eventListenerDelay);
  chEvtUnregister(&MODULE_HAL_CAN.rxfull_event, &eventListenerCan);
  // clear any pending events (timeout, delay, CAN receive)
  chEvtWaitAllTimeout(TIMEOUTEVENT_MASK | DELAYEVENT_MASK | CANEVENT_MASK, TIME_IMMEDIATE);

  // reset all control signals
#if (AMIROOS_CFG_SSSP_STACK_END != true)
  apalControlGpioSet(&moduleSsspGpioUp, APAL_GPIO_OFF);
#endif
  apalControlGpioSet(&moduleSsspGpioSync, APAL_GPIO_OFF);

  return shutdown;
}

/**
 * @brief   Application entry point.
 */
int main(void)
{
  // local variables
  eventmask_t eventmask = 0;
  eventflags_t eventflags = 0;
  aos_shutdown_t shutdown = AOS_SHUTDOWN_NONE;
#if defined(AMIROOS_CFG_MAIN_EXTRA_THREAD_VARIABLES)
  AMIROOS_CFG_MAIN_EXTRA_THREAD_VARIABLES
#endif

  /*
   * ##########################################################################
   * # system initialization                                                  #
   * ##########################################################################
   */

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_0)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_0_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_0(AMIROOS_CFG_MAIN_INIT_HOOK_0_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_0();
#endif
#endif

  /* hardware, kernel, and operating system initialization */
  // ChibiOS/HAL and custom hal additions (if any)
  halInit();
#ifdef MODULE_INIT_HAL_EXTRA
  MODULE_INIT_HAL_EXTRA();
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_1)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_1_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_1(AMIROOS_CFG_MAIN_INIT_HOOK_1_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_1();
#endif
#endif

  // ChibiOS/RT kernel and custom kernel additions (if any)
  chSysInit();
#ifdef MODULE_INIT_KERNEL_EXTRA
  MODULE_INIT_KERNEL_EXTRA();
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_2)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_2_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_2(AMIROOS_CFG_MAIN_INIT_HOOK_2_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_2();
#endif
#endif

  // AMiRo-OS and custom OS additions (if any)
#if (AMIROOS_CFG_SHELL_ENABLE == true)
  aosSysInit(moduleShellPrompt);
#else
  aosSysInit();
#endif
#ifdef MODULE_INIT_OS_EXTRA
  MODULE_INIT_OS_EXTRA();
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_3)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_3_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_3(AMIROOS_CFG_MAIN_INIT_HOOK_3_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_3();
#endif
#endif

#if (AMIROOS_CFG_TESTS_ENABLE == true)
#if defined(MODULE_INIT_TESTS)
  MODULE_INIT_TESTS();
#else
  #warning "MODULE_INIT_TESTS not defined"
#endif
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_4)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_4_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_4(AMIROOS_CFG_MAIN_INIT_HOOK_4_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_4();
#endif
#endif

  /* event associations */
  chEvtRegisterMask(&aos.events.io, &_eventListenerIO, IOEVENT_MASK);
  chEvtRegisterMask(&aos.events.os, &_eventListenerOS, OSEVENT_MASK);

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_5)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_5_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_5(AMIROOS_CFG_MAIN_INIT_HOOK_5_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_5();
#endif
#endif

  /* periphery communication initialization */
  // CAN (mandatory)
  canStart(&MODULE_HAL_CAN, &moduleHalCanConfig);
  // module specific initialization (if any)
#ifdef MODULE_INIT_PERIPHERY_COMM
  MODULE_INIT_PERIPHERY_COMM();
#endif
  // user interface (if any)
#ifdef MODULE_HAL_PROGIF
  aosIOChannelInit(&_stdiochannel, (BaseAsynchronousChannel*)&MODULE_HAL_PROGIF);
  aosIOChannelOutputEnable(&_stdiochannel);
  aosIOStreamAddChannel(&aos.iostream, &_stdiochannel);
#if (AMIROOS_CFG_SHELL_ENABLE == true)
  aosShellChannelInit(&_stdshellchannel, (BaseAsynchronousChannel*)&MODULE_HAL_PROGIF);
  aosShellChannelInputEnable(&_stdshellchannel);
  aosShellChannelOutputEnable(&_stdshellchannel);
  aosShellStreamAddChannel(&aos.shell.stream, &_stdshellchannel);
#endif
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_6)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_6_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_6(AMIROOS_CFG_MAIN_INIT_HOOK_6_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_6();
#endif
#endif

  /* module is ready -> print welcome prompt */
  aosprintf("\n");
  aosprintf("######################################################################\n");
  aosprintf("# AMiRo-OS is an operating system designed for the Autonomous Mini   #\n");
  aosprintf("# Robot (AMiRo) platform.                                            #\n");
  aosprintf("# Copyright (C) 2016..2018  Thomas Schöpping et al.                  #\n");
  aosprintf("#                                                                    #\n");
  aosprintf("# This is free software; see the source for copying conditions.      #\n");
  aosprintf("# There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR  #\n");
  aosprintf("# A PARTICULAR PURPOSE.                                              #\n");
  aosprintf("# The development of this software was supported by the Excellence   #\n");
  aosprintf("# Cluster EXC 227 Cognitive Interaction Technology. The Excellence   #\n");
  aosprintf("# Cluster EXC 227 is a grant of the Deutsche Forschungsgemeinschaft  #\n");
  aosprintf("# (DFG) in the context of the German Excellence Initiative.          #\n");
  aosprintf("######################################################################\n");
  aosprintf("\n");

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_7)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_7_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_7(AMIROOS_CFG_MAIN_INIT_HOOK_7_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_7();
#endif
#endif

  /* SSSP startup OS synchronization phase (end of startup stage 2) */
  while ((shutdown == AOS_SHUTDOWN_NONE) && (eventmask = aosSysSsspStartupOsInitSyncCheck(&_eventListenerIO)) != 0) {
    /*
     * This code is executed if the received event was not about the SYS_SYNC control signal.
     * The returned event could be caused by any listener (not only the argument).
     */
    // IO event
    if (eventmask & _eventListenerIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerIO);
      // PD event
      if (eventflags & MODULE_SSSP_EVENTFLAGS_PD) {
        shutdown = AOS_SHUTDOWN_PASSIVE;
      } else {
#ifdef MODULE_SSSP_STARTUP_2_2_IOEVENT_HOOK
        MODULE_SSSP_STARTUP_2_2_IOEVENT_HOOK(eventmask, eventflags);
#else
        // ignore any other IO events
#endif
      }
    }
    // OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _unexpectedEventError(eventmask, eventflags);
    }
    // unknown event
    else {
      _unexpectedEventError(eventmask, 0);
    }
  }

  /*
   * There must be no delays at this point, thus no hook is allowed.
   */

  /* SSSP startup stage 3 (module stack initialization) */
  if (shutdown == AOS_SHUTDOWN_NONE) {
    shutdown = _ssspModuleStackInitialization();
  }

  /*
   * There must be no delays at this point, thus no hook is allowed.
   */

  /* snychronize calendars */
  if (shutdown == AOS_SHUTDOWN_NONE) {
#if (AMIROOS_CFG_SSSP_MASTER == true)
    CANTxFrame frame;
    struct tm t;
    uint64_t encoded;

    frame.DLC = 8;
    frame.RTR = CAN_RTR_DATA;
    frame.IDE = CAN_IDE_STD;
    frame.SID = CALENDERSYNC_CANMSGID;

    aosDbgPrintf("transmitting current date/time...\t");
    // get current date & time
    aosSysGetDateTime(&t);
    // encode
    encoded = _TM2U64(&t);
    // serialize
    _serialize(frame.data8, encoded, 8);
    // transmit
    canTransmitTimeout(&MODULE_HAL_CAN, CAN_ANY_MAILBOX, &frame, TIME_IMMEDIATE);

    aosDbgPrintf("done\n");
#else
    CANRxFrame frame;
    uint64_t encoded;
    struct tm t;

    aosDbgPrintf("receiving current date/time...\t");
    // receive message
    if (canReceiveTimeout(&MODULE_HAL_CAN, CAN_ANY_MAILBOX, &frame, LL_US2ST(AOS_SYSTEM_SSSP_TIMEOUT)) == MSG_OK) {
      // validate message
      if (frame.DLC == 8 &&
          frame.RTR == CAN_RTR_DATA &&
          frame.IDE == CAN_IDE_STD &&
          frame.SID == CALENDERSYNC_CANMSGID) {
        // deserialize
        encoded = _deserialize(frame.data8, 8);
        // decode
        _U642TM(&t, encoded);
        // set current date & time
        aosSysSetDateTime(&t);
        aosDbgPrintf("success\n");
      } else {
        aosDbgPrintf("fail (invalid message)\n");
      }
    } else {
      aosDbgPrintf("fail (timeout)\n");
    }
#endif
    aosDbgPrintf("\n");
  }

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_8)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_8_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_8(AMIROOS_CFG_MAIN_INIT_HOOK_8_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_8();
#endif
#endif

  /* completely start AMiRo-OS */
  if (shutdown == AOS_SHUTDOWN_NONE) {
    aosSysStart();
  }

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_9)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_9_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_9(AMIROOS_CFG_MAIN_INIT_HOOK_9_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_9();
#endif
#endif

  /*
   * ##########################################################################
   * # infinite loop                                                          #
   * ##########################################################################
   */

  // sleep until a shutdown event is received
  while (shutdown == AOS_SHUTDOWN_NONE) {
    // wait for an event
#if (AMIROOS_CFG_MAIN_LOOP_TIMEOUT != 0)
    eventmask = chEvtWaitOneTimeout(ALL_EVENTS, LL_US2ST(AMIROOS_CFG_MAIN_LOOP_TIMEOUT));
#else
    eventmask = chEvtWaitOne(ALL_EVENTS);
#endif

#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_0)
#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_0_ARGS)
    AMIROOS_CFG_MAIN_LOOP_HOOK_0(AMIROOS_CFG_MAIN_LOOP_HOOK_0_ARGS);
#else
    AMIROOS_CFG_MAIN_LOOP_HOOK_0();
#endif
#endif

    switch (eventmask) {
      // if this was an I/O event
      case IOEVENT_MASK:
        // evaluate flags
        eventflags = chEvtGetAndClearFlags(&_eventListenerIO);
        // PD event
        if (eventflags & MODULE_SSSP_EVENTFLAGS_PD) {
          shutdown = AOS_SHUTDOWN_PASSIVE;
        }
        // all other events
#ifdef MODULE_MAIN_LOOP_IO_EVENT
        else {
          MODULE_MAIN_LOOP_IO_EVENT(eventmask, eventflags);
        }
#endif
        break;

      // if this was an OS event
      case OSEVENT_MASK:
        // evaluate flags
        eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
        switch (eventflags) {
          case AOS_SYSTEM_EVENTFLAGS_HIBERNATE:
            shutdown = AOS_SHUTDOWN_HIBERNATE;
            break;
          case AOS_SYSTEM_EVENTFLAGS_DEEPSLEEP:
            shutdown = AOS_SHUTDOWN_DEEPSLEEP;
            break;
          case AOS_SYSTEM_EVENTFLAGS_TRANSPORTATION:
            shutdown = AOS_SHUTDOWN_TRANSPORTATION;
            break;
          case AOS_SYSTEM_EVENTFLAGS_RESTART:
            shutdown = AOS_SHUTDOWN_RESTART;
            break;
          default:
            _unexpectedEventError(eventmask, eventflags);
            break;
        }
        break;

      // if this was any other event (should be impossible to occur)
      default:
        eventflags = 0;
        _unexpectedEventError(eventmask, eventflags);
        break;
    }

#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_1)
#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_1_ARGS)
    AMIROOS_CFG_MAIN_LOOP_HOOK_1(AMIROOS_CFG_MAIN_LOOP_HOOK_1_ARGS);
#else
    AMIROOS_CFG_MAIN_LOOP_HOOK_1();
#endif
#endif
  }

  /*
   * ##########################################################################
   * # system shutdown                                                        #
   * ##########################################################################
   */

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0();
#endif
#endif

  // initialize/acknowledge shutdown
  aosSysShutdownInit(shutdown);

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1();
#endif
#endif

  // stop system threads
  aosSysStop();

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2();
#endif
#endif

  // deinitialize system
  aosSysDeinit();

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3();
#endif
#endif

  /* stop all periphery communication */
  // CAN (mandatory)
  canStop(&MODULE_HAL_CAN);
#ifdef MODULE_SHUTDOWN_PERIPHERY_COMM
  MODULE_SHUTDOWN_PERIPHERY_COMM();
#endif

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4();
#endif
#endif

  // finally hand over to bootloader
  aosSysShutdownFinal(&MODULE_HAL_EXT, shutdown);

  /*
   * ##########################################################################
   * # after shutdown/restart                                                 #
   * ##########################################################################
   *
   * NOTE: This code will not be executed, since the bootloader callbacks will stop/restart the MCU.
   *       It is included nevertheless for the sake of completeness and to explicitely indicate,
   *       which subsystems should NOT be shut down.
   */

  // return an error, since this code should not be executed
  return -1;
}
