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

#include <aos_shell.h>

#if (AMIROOS_CFG_SHELL_ENABLE == true)
#include <aos_debug.h>
#include <aos_time.h>
#include <aos_system.h>
#include <string.h>

/**
 * @brief   Event mask to be set on OS related events.
 */
#define AOS_SHELL_EVENTMASK_OS                  EVENT_MASK(0)

/**
 * @brief   Event mask to be set on a input event.
 */
#define AOS_SHELL_EVENTMASK_INPUT               EVENT_MASK(1)

/**
 * @brief   Implementation of the BaseAsynchronous write() method (inherited from BaseSequentialStream).
 */
static size_t _channelwrite(void *instance, const uint8_t *bp, size_t n)
{
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_OUTPUT_ENABLED) {
    return streamWrite(((AosShellChannel*)instance)->asyncchannel, bp, n);
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous read() method (inherited from BaseSequentialStream).
 */
static size_t _channelread(void *instance, uint8_t *bp, size_t n)
{
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_INPUT_ENABLED) {
    return streamRead(((AosShellChannel*)instance)->asyncchannel, bp, n);
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous put() method (inherited from BaseSequentialStream).
 */
static msg_t _channelput(void *instance, uint8_t b)
{
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_OUTPUT_ENABLED) {
    return streamPut(((AosShellChannel*)instance)->asyncchannel, b);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous get() method (inherited from BaseSequentialStream).
 */
static msg_t _channelget(void *instance)
{
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_INPUT_ENABLED) {
    return streamGet(((AosShellChannel*)instance)->asyncchannel);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous putt() method.
 */
static msg_t _channelputt(void *instance, uint8_t b, systime_t time)
{
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_OUTPUT_ENABLED) {
    return chnPutTimeout(((AosShellChannel*)instance)->asyncchannel, b, time);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous gett() method.
 */
static msg_t _channelgett(void *instance, systime_t time)
{
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_INPUT_ENABLED) {
    return chnGetTimeout(((AosShellChannel*)instance)->asyncchannel, time);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous writet() method.
 */
static size_t _channelwritet(void *instance, const uint8_t *bp, size_t n, systime_t time)
{
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_OUTPUT_ENABLED) {
    return chnWriteTimeout(((AosShellChannel*)instance)->asyncchannel, bp, n, time);
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous readt() method.
 */
static size_t _channelreadt(void *instance, uint8_t *bp, size_t n, systime_t time)
{
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_INPUT_ENABLED) {
    return chnReadTimeout(((AosShellChannel*)instance)->asyncchannel, bp, n, time);
  } else {
    return 0;
  }
}

static const struct AosShellChannelVMT _channelvmt = {
  (size_t) 0,
  _channelwrite,
  _channelread,
  _channelput,
  _channelget,
  _channelputt,
  _channelgett,
  _channelwritet,
  _channelreadt,
};

static size_t _streamwrite(void *instance, const uint8_t *bp, size_t n)
{
  aosDbgCheck(instance != NULL);

  // local variables
  AosShellChannel* channel = ((AosShellStream*)instance)->channel;
  size_t bytes;
  size_t maxbytes = 0;

  // iterate through the list of channels
  while (channel != NULL) {
    bytes = streamWrite(channel, bp, n);
    maxbytes = (bytes > maxbytes) ? bytes : maxbytes;
    channel = channel->next;
  }

  return maxbytes;
}

static size_t _stremread(void *instance, uint8_t *bp, size_t n)
{
  (void)instance;
  (void)bp;
  (void)n;

  return 0;
}

static msg_t _streamput(void *instance, uint8_t b)
{
  aosDbgCheck(instance != NULL);

  // local variables
  AosShellChannel* channel = ((AosShellStream*)instance)->channel;
  msg_t ret = MSG_OK;

  // iterate through the list of channels
  while (channel != NULL) {
    msg_t ret_ = streamPut(channel, b);
    ret = (ret_ < ret) ? ret_ : ret;
    channel = channel->next;
  }

  return ret;
}

static msg_t _streamget(void *instance)
{
  (void)instance;

  return 0;
}

static const struct AosShellStreamVMT _streamvmt = {
  (size_t) 0,
  _streamwrite,
  _stremread,
  _streamput,
  _streamget,
};

/**
 * @brief   Enumerator of special keyboard keys.
 */
typedef enum special_key {
  KEY_UNKNOWN,      /**< any/unknow key */
  KEY_AMBIGUOUS,    /**< key is ambiguous */
  KEY_TAB,          /**< tabulator key */
  KEY_ESCAPE,       /**< escape key */
  KEY_BACKSPACE,    /**< backspace key */
  KEY_INSERT,       /**< insert key */
  KEY_DELETE,       /**< delete key */
  KEY_HOME,         /**< home key */
  KEY_END,          /**< end key */
  KEY_PAGE_UP,      /**< page up key */
  KEY_PAGE_DOWN,    /**< page down key */
  KEY_ARROW_UP,     /**< arrow up key */
  KEY_ARROW_DOWN,   /**< arrow down key */
  KEY_ARROW_LEFT,   /**< arrow left key */
  KEY_ARROW_RIGHT,  /**< arrow right key */
} special_key_t;

/**
 * @brief   Enumerator for case (in)sensitive character matching.
 */
typedef enum charmatch {
  CHAR_MATCH_NOT    = 0,  /**< Characters do not match at all. */
  CHAR_MATCH_NCASE  = 1,  /**< Characters would match case insensitive. */
  CHAR_MATCH_CASE   = 2,  /**< Characters do match with case. */
} charmatch_t;

/**
 * @brief   Print the shell prompt
 * @details Depending on the configuration flags, the system uptime is printed before the prompt string.
 *
 * @param[in] shell   Pointer to the shell object.
 */
static void _printPrompt(aos_shell_t* shell)
{
  aosDbgCheck(shell != NULL);

  // print some time informattion before prompt if configured
  if (shell->config & (AOS_SHELL_CONFIG_PROMPT_UPTIME | AOS_SHELL_CONFIG_PROMPT_DATETIME)) {
    // printf the system uptime
    if ((shell->config & (AOS_SHELL_CONFIG_PROMPT_UPTIME | AOS_SHELL_CONFIG_PROMPT_DATETIME)) == AOS_SHELL_CONFIG_PROMPT_UPTIME) {
      // get current system uptime
      aos_timestamp_t uptime;
      aosSysGetUptime(&uptime);

      chprintf((BaseSequentialStream*)&shell->stream, "[%01u:%02u:%02u:%02u:%03u:%03u] ",
               (uint32_t)(uptime / MICROSECONDS_PER_DAY),
               (uint8_t)(uptime % MICROSECONDS_PER_DAY / MICROSECONDS_PER_HOUR),
               (uint8_t)(uptime % MICROSECONDS_PER_HOUR / MICROSECONDS_PER_MINUTE),
               (uint8_t)(uptime % MICROSECONDS_PER_MINUTE / MICROSECONDS_PER_SECOND),
               (uint16_t)(uptime % MICROSECONDS_PER_SECOND / MICROSECONDS_PER_MILLISECOND),
               (uint16_t)(uptime % MICROSECONDS_PER_MILLISECOND / MICROSECONDS_PER_MICROSECOND));
    }
    else if ((shell->config & (AOS_SHELL_CONFIG_PROMPT_UPTIME | AOS_SHELL_CONFIG_PROMPT_DATETIME)) == AOS_SHELL_CONFIG_PROMPT_DATETIME) {
      // get current RTC time
      struct tm dt;
      aosSysGetDateTime(&dt);
      chprintf((BaseSequentialStream*)&shell->stream, "[%02u-%02u-%04u|%02u:%02u:%02u] ",
               dt.tm_mday,
               dt.tm_mon + 1,
               dt.tm_year + 1900,
               dt.tm_hour,
               dt.tm_min,
               dt.tm_sec);
    }
    else {
      aosDbgAssert(false);
    }
  }

  // print the actual prompt string
  if (shell->prompt && !(shell->config & AOS_SHELL_CONFIG_PROMPT_MINIMAL)) {
    chprintf((BaseSequentialStream*)&shell->stream, "%s$ ", shell->prompt);
  } else {
    chprintf((BaseSequentialStream*)&shell->stream, "%>$ ");
  }

  return;
}

/**
 * @brief   Interprete a escape sequence
 *
 * @param[in] seq   Character sequence to interprete.
 *                  Must be terminated by NUL byte.
 *
 * @return          A @p special_key value.
 */
static special_key_t _interpreteEscapeSequence(const char seq[])
{
  // local variables
  bool ambiguous = false;
  int cmp = 0;

  // TAB
  /* not supported yet; use "\x09" instead */

  // BACKSPACE
  /* not supported yet; use "\x08" instead */

  // ESCAPE
  cmp = strcmp(seq, "\x1B");
  if (cmp == 0) {
    return KEY_ESCAPE;
  } else {
    ambiguous |= (cmp < 0);
  }

  // INSERT
  cmp = strcmp(seq, "\x1B\x5B\x32\x7E");
  if (cmp == 0) {
    return KEY_INSERT;
  } else {
    ambiguous |= (cmp < 0);
  }

  // DELETE
  cmp = strcmp(seq, "\x1B\x5B\x33\x7E");
  if (cmp == 0) {
    return KEY_DELETE;
  } else {
    ambiguous |= (cmp < 0);
  }

  // HOME
  cmp = strcmp(seq, "\x1B\x4F\x48");
  if (cmp == 0) {
    return KEY_HOME;
  } else {
    ambiguous |= (cmp < 0);
  }

  // END
  cmp = strcmp(seq, "\x1B\x4F\x46");
  if (cmp == 0) {
    return KEY_END;
  } else {
    ambiguous |= (cmp < 0);
  }

  // PAGE UP
  cmp = strcmp(seq, "\x1B\x5B\x35\x7E");
  if (cmp == 0) {
    return KEY_PAGE_UP;
  } else {
    ambiguous |= (cmp < 0);
  }

  // PAGE DOWN
  cmp = strcmp(seq, "\x1B\x5B\x36\x7E");
  if (cmp == 0) {
    return KEY_PAGE_DOWN;
  } else {
    ambiguous |= (cmp < 0);
  }

  // ARROW UP
  cmp = strcmp(seq, "\x1B\x5B\x41");
  if (cmp == 0) {
    return KEY_ARROW_UP;
  } else {
    ambiguous |= (cmp < 0);
  }

  // ARROW DOWN
  cmp = strcmp(seq, "\x1B\x5B\x42");
  if (cmp == 0) {
    return KEY_ARROW_DOWN;
  } else {
    ambiguous |= (cmp < 0);
  }

  // ARROW LEFT
  cmp = strcmp(seq, "\x1B\x5B\x44");
  if (cmp == 0) {
    return KEY_ARROW_LEFT;
  } else {
    ambiguous |= (cmp < 0);
  }

  // ARROW RIGHT
  cmp = strcmp(seq, "\x1B\x5B\x43");
  if (cmp == 0) {
    return KEY_ARROW_RIGHT;
  } else {
    ambiguous |= (cmp < 0);
  }

  return ambiguous ? KEY_AMBIGUOUS : KEY_UNKNOWN;
}

/**
 * @brief   Move the cursor in the terminal
 *
 * @param[in] shell   Pointer to the shell object.
 * @param[in] from    Starting position of the cursor.
 * @param[in] to      Target position to move the cursor to.
 *
 * @return            The number of positions moved.
 */
static int _moveCursor(aos_shell_t* shell, const size_t from, const size_t to)
{
  aosDbgCheck(shell != NULL);

  // local variables
  size_t pos = from;

  // move cursor left by printing backspaces
  while (pos > to) {
    streamPut(&shell->stream, '\b');
    --pos;
  }

  // move cursor right by printing line content
  while (pos < to) {
    streamPut(&shell->stream, shell->line[pos]);
    ++pos;
  }

  return (int)pos - (int)from;
}

/**
 * @brief   Print content of the shell line
 *
 * @param[in] shell   Pointer to the shell object.
 * @param[in] from    First position to start printing from.
 * @param[in] to      Position after the last character to print.
 *
 * @return            Number of characters printed.
 */
static inline size_t _printLine(aos_shell_t* shell, const size_t from, const size_t to)
{
  aosDbgCheck(shell != NULL);

  // local variables
  size_t cnt;

  for (cnt = 0; from + cnt < to; ++cnt) {
    streamPut(&shell->stream, shell->line[from + cnt]);
  }

  return cnt;
}

/**
 * @brief   Compare two characters.
 *
 * @param[in] lhs       First character to compare.
 * @param[in] rhs       Second character to compare.
 *
 * @return              How well the characters match.
 */
static inline charmatch_t _charcmp(char lhs, char rhs)
{
  // if lhs is a upper case letter and rhs is a lower case letter
  if (lhs >= 'A' && lhs <= 'Z' && rhs >= 'a' && rhs <= 'z') {
    return (lhs == (rhs - 'a' + 'A')) ? CHAR_MATCH_NCASE : CHAR_MATCH_NOT;
  }
  // if lhs is a lower case letter and rhs is a upper case letter
  else if (lhs >= 'a' && lhs <= 'z' && rhs >= 'A' && rhs <= 'Z') {
    return ((lhs - 'a' + 'A') == rhs) ? CHAR_MATCH_NCASE : CHAR_MATCH_NOT;
  }
  // default
  else {
    return (lhs == rhs) ? CHAR_MATCH_CASE : CHAR_MATCH_NOT;
  }
}

/**
 * @brief   Maps an character from ASCII to a modified custom encoding.
 * @details The custom character encoding is very similar to ASCII and has the following structure:
 *          0x00=NULL ... 0x40='@' (identically to ASCII)
 *          0x4A='a'; 0x4B='A'; 0x4C='b'; 0x4D='B' ... 0x73='z'; 0x74='Z' (custom letter order)
 *          0x75='[' ... 0x7A='`' (0x5B..0x60 is ASCII)
 *          0x7B='{' ... 0x7F=DEL (identically to ASCII)
 *
 * @param[in] c   Character to map to the custom encoding.
 *
 * @return    The customly encoded character.
 */
static inline char _mapAscii2Custom(const char c)
{
  if (c >= 'A' && c <= 'Z') {
    return ((c - 'A') * 2) + 'A' + 1;
  } else if (c > 'Z' && c < 'a') {
    return c + ('z' - 'a') + 1;
  } else if (c >= 'a' && c <= 'z') {
    return ((c - 'a') * 2) + 'A';
  } else {
    return c;
  }
}

/**
 * @brief   Compares two strings wrt letter case.
 * @details Comparisson uses a custom character encoding or mapping.
 *          See @p _mapAscii2Custom for details.
 *
 * @param[in] str1    First string to compare.
 * @param[in] str2    Second string to compare.
 * @param[in] cs      Flag indicating whether comparison shall be case sensitive.
 * @param[in,out] n   Maximum number of character to compare (in) and number of matching characters (out).
 *                    If a null pointer is specified, this parameter is ignored.
 *                    If the value pointed to is zero, comarison will not be limited.
 * @param[out] m      Optional indicator whether there was at least one case mismatch.
 *
 * @return      Integer value indicating the relationship between the strings.
 * @retval <0   The first character that does not match has a lower value in str1 than in str2.
 * @retval  0   The contents of both strings are equal.
 * @retval >0   The first character that does not match has a greater value in str1 than in str2.
 */
static int _strccmp(const char *str1, const char *str2, bool cs, size_t* n, charmatch_t* m)
{
  aosDbgCheck(str1 != NULL);
  aosDbgCheck(str2 != NULL);

  // initialize variables
  if (m) {
    *m = CHAR_MATCH_NOT;
  }
  size_t i = 0;

  // iterate through the strings
  while ((n == NULL) || (*n == 0) || (*n > 0 && i < *n)) {
    // break on NUL
    if (str1[i] == '\0' || str2[i] == '\0') {
      if (n) {
        *n = i;
      }
      break;
    }
    // compare character
    const charmatch_t match = _charcmp(str1[i], str2[i]);
    if ((match == CHAR_MATCH_CASE) || (!cs && match == CHAR_MATCH_NCASE)) {
      if (m != NULL && *m != CHAR_MATCH_NCASE) {
        *m = match;
      }
      ++i;
    } else {
      if (n) {
        *n = i;
      }
      break;
    }
  }

  return _mapAscii2Custom(str1[i]) - _mapAscii2Custom(str2[i]);
}

static aos_status_t _readChannel(aos_shell_t* shell, AosShellChannel* channel, size_t* n)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(channel != NULL);
  aosDbgCheck(n != NULL);

  // local variables
  aos_shellaction_t action = AOS_SHELL_ACTION_NONE;
  char c;
  special_key_t key;

  // initialize output variables
  *n = 0;

  // read character by character from the channel
  while (chnReadTimeout(channel, (uint8_t*)&c, 1, TIME_IMMEDIATE)) {
    key = KEY_UNKNOWN;

    // parse escape sequence
    if (shell->inputdata.escp > 0) {
      shell->inputdata.escseq[shell->inputdata.escp] = c;
      ++shell->inputdata.escp;
      key = _interpreteEscapeSequence(shell->inputdata.escseq);
      if (key == KEY_AMBIGUOUS) {
        // read next byte to resolve ambiguity
        continue;
      } else {
        /*
         * If the escape sequence could either be parsed sucessfully
         * or there is no match (KEY_UNKNOWN),
         * reset the sequence variable and interprete key/character
         */
        shell->inputdata.escp = 0;
        memset(shell->inputdata.escseq, '\0', sizeof(shell->inputdata.escseq)*sizeof(shell->inputdata.escseq[0]));
      }
    }

    /* interprete keys or character */
    {
      // default
      action = AOS_SHELL_ACTION_NONE;

      // printable character
      if (key == KEY_UNKNOWN && c >= '\x20' && c <= '\x7E') {
        action = AOS_SHELL_ACTION_READCHAR;
      }

      // tab key or character
      else if (key == KEY_TAB || c == '\x09') {
        /*
         * pressing tab once applies auto fill
         * pressing tab a second time prints suggestions
         */
        if (shell->inputdata.lastaction == AOS_SHELL_ACTION_AUTOFILL || shell->inputdata.lastaction == AOS_SHELL_ACTION_SUGGEST) {
          action = AOS_SHELL_ACTION_SUGGEST;
        } else {
          action = AOS_SHELL_ACTION_AUTOFILL;
        }
      }

      // INS key
      else if (key == KEY_INSERT) {
        action = AOS_SHELL_ACTION_INSERTTOGGLE;
      }

      // DEL key or character
      else if (key == KEY_DELETE || c == '\x7F') {
        // ignore if cursor is at very right
        if (shell->inputdata.cursorpos < shell->inputdata.lineend) {
          action = AOS_SHELL_ACTION_DELETEFORWARD;
        }
      }

      // backspace key or character
      else if (key == KEY_BACKSPACE || c == '\x08') {
        // ignore if cursor is at very left
        if (shell->inputdata.cursorpos > 0) {
          action = AOS_SHELL_ACTION_DELETEBACKWARD;
        }
      }

      // 'page up' of 'arrow up' key
      else if (key == KEY_PAGE_UP || key == KEY_ARROW_UP) {
        // ignore if there was some input
        if (shell->inputdata.noinput) {
          action = AOS_SHELL_ACTION_RECALLLAST;
        }
      }

      // 'page down' key, 'arrow done' key, 'end of test' character or 'end of transmission' character
      else if (key == KEY_PAGE_DOWN || key == KEY_ARROW_DOWN || c == '\x03' || c == '\x03') {
        // ignore if line is empty
        if (shell->inputdata.lineend > 0) {
          action = AOS_SHELL_ACTION_CLEAR;
        }
      }

      // 'home' key
      else if (key == KEY_HOME) {
        // ignore if cursor is very left
        if (shell->inputdata.cursorpos > 0) {
          action = AOS_SHELL_ACTION_CURSOR2START;
        }
      }

      // 'end' key
      else if (key == KEY_END) {
        // ignore if cursos is very right
        if (shell->inputdata.cursorpos < shell->inputdata.lineend) {
          action = AOS_SHELL_ACTION_CURSOR2END;
        }
      }

      // 'arrow left' key
      else if (key == KEY_ARROW_LEFT) {
        // ignore if cursor is very left
        if (shell->inputdata.cursorpos > 0) {
          action = AOS_SHELL_ACTION_CURSORLEFT;
        }
      }

      // 'arrow right' key
      else if (key == KEY_ARROW_RIGHT) {
        // irgnore if cursor is very right
        if (shell->inputdata.cursorpos < shell->inputdata.lineend) {
          action = AOS_SHELL_ACTION_CURSORRIGHT;
        }
      }

      // carriage return ('\r') or line feed ('\n') character
      else if (c == '\x0D' || c == '\x0A') {
        action = AOS_SHELL_ACTION_EXECUTE;
      }

      // ESC key or [ESCAPE] character
      else if (key == KEY_ESCAPE || c == '\x1B') {
        action = AOS_SHELL_ACTION_ESCSTART;
      }
    }

    /* handle function */
    switch (action) {
      case AOS_SHELL_ACTION_READCHAR:
      {
        // line is full
        if (shell->inputdata.lineend + 1 >= shell->linesize) {
          _moveCursor(shell, shell->inputdata.cursorpos, shell->inputdata.lineend);
          chprintf((BaseSequentialStream*)&shell->stream, "\n\tmaximum line width reached\n");
          _printPrompt(shell);
          _printLine(shell, 0, shell->inputdata.lineend);
          _moveCursor(shell, shell->inputdata.lineend, shell->inputdata.cursorpos);
        }
        // read character
        else {
          // clear old line content on first input
          if (shell->inputdata.noinput) {
            memset(shell->line, '\0', shell->linesize);
            shell->inputdata.noinput = false;
          }
          // overwrite content
          if (shell->config & AOS_SHELL_CONFIG_INPUT_OVERWRITE) {
            shell->line[shell->inputdata.cursorpos] = c;
            ++shell->inputdata.cursorpos;
            shell->inputdata.lineend = (shell->inputdata.cursorpos > shell->inputdata.lineend) ? shell->inputdata.cursorpos : shell->inputdata.lineend;
            streamPut(&shell->stream, (uint8_t)c);
          }
          // insert character
          else {
            memmove(&(shell->line[shell->inputdata.cursorpos+1]), &(shell->line[shell->inputdata.cursorpos]), shell->inputdata.lineend - shell->inputdata.cursorpos);
            shell->line[shell->inputdata.cursorpos] = c;
            ++shell->inputdata.lineend;
            _printLine(shell, shell->inputdata.cursorpos, shell->inputdata.lineend);
            ++shell->inputdata.cursorpos;
            _moveCursor(shell, shell->inputdata.lineend, shell->inputdata.cursorpos);
          }
        }
        break;
      }

      case AOS_SHELL_ACTION_AUTOFILL:
      {
        const char* fill = shell->line;
        size_t cmatch = shell->inputdata.cursorpos;
        charmatch_t matchlevel = CHAR_MATCH_NOT;
        size_t n;
        // iterate through command list
        for (aos_shellcommand_t* cmd = shell->commands; cmd != NULL; cmd = cmd->next) {
          // compare current match with command
          n = cmatch;
          charmatch_t mlvl = CHAR_MATCH_NOT;
          _strccmp(fill, cmd->name, shell->config & AOS_SHELL_CONFIG_MATCH_CASE, (n == 0) ? NULL : &n, &mlvl);
          const int cmp = (n < cmatch) ?
                            (n - cmatch) :
                            (cmd->name[n] != '\0') ?
                              strlen(cmd->name) - n :
                              0;
          // if an exact match was found
          if (cmatch + cmp == shell->inputdata.cursorpos) {
            cmatch = shell->inputdata.cursorpos;
            fill = cmd->name;
            // break the loop only if there are no case mismatches with the input
            n = shell->inputdata.cursorpos;
            _strccmp(fill, shell->line, false, &n, &mlvl);
            if (mlvl == CHAR_MATCH_CASE) {
              break;
            }
          }
          // if a not exact match was found
          else if (cmatch + cmp > shell->inputdata.cursorpos) {
            // if this is the first one
            if (fill == shell->line) {
              cmatch += cmp;
              fill = cmd->name;
            }
            // if this is a worse one
            else if ((cmp < 0) || (cmp == 0 && mlvl == CHAR_MATCH_CASE)) {
              cmatch += cmp;
            }
          }
          // non matching commands are ignored
          else {}
        }
        // evaluate if there are case mismatches
        n = cmatch;
        _strccmp(shell->line, fill, shell->config & AOS_SHELL_CONFIG_MATCH_CASE, &n, &matchlevel);
        // print the auto fill if any
        if (cmatch > shell->inputdata.cursorpos || (cmatch == shell->inputdata.cursorpos && matchlevel == CHAR_MATCH_NCASE)) {
          shell->inputdata.noinput = false;
          // limit auto fill so it will not overflow the line width
          if (shell->inputdata.lineend + (cmatch - shell->inputdata.cursorpos) > shell->linesize) {
            cmatch = shell->linesize - shell->inputdata.lineend + shell->inputdata.cursorpos;
          }
          // move trailing memory further in the line
          memmove(&(shell->line[cmatch]), &(shell->line[shell->inputdata.cursorpos]), shell->inputdata.lineend - shell->inputdata.cursorpos);
          shell->inputdata.lineend += cmatch - shell->inputdata.cursorpos;
          // if there was no incorrect case when matching
          if (matchlevel == CHAR_MATCH_CASE) {
            // insert fill command name to line
            memcpy(&(shell->line[shell->inputdata.cursorpos]), &(fill[shell->inputdata.cursorpos]), cmatch - shell->inputdata.cursorpos);
            // print the output
            _printLine(shell, shell->inputdata.cursorpos, shell->inputdata.lineend);
          } else {
            // overwrite line with fill command name
            memcpy(shell->line, fill, cmatch);
            // reprint the whole line
            _moveCursor(shell, shell->inputdata.cursorpos, 0);
            _printLine(shell, 0, shell->inputdata.lineend);
          }
          // move cursor to the end of the matching sequence
          shell->inputdata.cursorpos = cmatch;
          _moveCursor(shell, shell->inputdata.lineend, shell->inputdata.cursorpos);
        }
        break;
      }

      case AOS_SHELL_ACTION_SUGGEST:
      {
        unsigned int matches = 0;
        // iterate through command list
        for (aos_shellcommand_t* cmd = shell->commands; cmd != NULL; cmd = cmd->next) {
          // compare line content with command, excpet if cursorpos=0
          size_t i = shell->inputdata.cursorpos;
          if (shell->inputdata.cursorpos > 0) {
            _strccmp(shell->line, cmd->name, true, &i, NULL);
          }
          const int cmp = (i < shell->inputdata.cursorpos) ?
                            (i - shell->inputdata.cursorpos) :
                            (cmd->name[i] != '\0') ?
                              strlen(cmd->name) - i :
                              0;
          // if a match was found
          if (cmp > 0) {
            // if this is the first one
            if (matches == 0) {
              _moveCursor(shell, shell->inputdata.cursorpos, shell->inputdata.lineend);
              streamPut(&shell->stream, '\n');
            }
            // print the command
            chprintf((BaseSequentialStream*)&shell->stream, "\t%s\n", cmd->name);
            ++matches;
          }
        }
        // reprint the prompt and line if any matches have been found
        if (matches > 0) {
          _printPrompt(shell);
          _printLine(shell, 0, shell->inputdata.lineend);
          _moveCursor(shell, shell->inputdata.lineend, shell->inputdata.cursorpos);
          shell->inputdata.noinput = false;
        }
        break;
      }

      case AOS_SHELL_ACTION_INSERTTOGGLE:
      {
        if (shell->config & AOS_SHELL_CONFIG_INPUT_OVERWRITE) {
          shell->config &= ~AOS_SHELL_CONFIG_INPUT_OVERWRITE;
        } else {
          shell->config |= AOS_SHELL_CONFIG_INPUT_OVERWRITE;
        }
        break;
      }

      case AOS_SHELL_ACTION_DELETEFORWARD:
      {
        --shell->inputdata.lineend;
        memmove(&(shell->line[shell->inputdata.cursorpos]), &(shell->line[shell->inputdata.cursorpos+1]), shell->inputdata.lineend - shell->inputdata.cursorpos);
        _printLine(shell, shell->inputdata.cursorpos, shell->inputdata.lineend);
        streamPut(&shell->stream, ' ');
        _moveCursor(shell, shell->inputdata.lineend + 1, shell->inputdata.cursorpos);
        break;
      }

      case AOS_SHELL_ACTION_DELETEBACKWARD:
      {
        --shell->inputdata.cursorpos;
        memmove(&(shell->line[shell->inputdata.cursorpos]), &(shell->line[shell->inputdata.cursorpos+1]), shell->inputdata.lineend - shell->inputdata.cursorpos);
        --shell->inputdata.lineend;
        shell->line[shell->inputdata.lineend] = '\0';
        _moveCursor(shell, shell->inputdata.cursorpos + 1, shell->inputdata.cursorpos);
        _printLine(shell, shell->inputdata.cursorpos, shell->inputdata.lineend);
        streamPut(&shell->stream, ' ');
        _moveCursor(shell, shell->inputdata.lineend+1, shell->inputdata.cursorpos);
        break;
      }

      case AOS_SHELL_ACTION_RECALLLAST:
      {
        // replace any intermediate NUL bytes with spaces
        shell->inputdata.lineend = 0;
        size_t nul_start = 0;
        size_t nul_end = 0;
        // search line for a NUL byte
        while (nul_start < shell->linesize) {
          if (shell->line[nul_start] == '\0') {
            nul_end = nul_start + 1;
            // keep searcjing for a byte that is not NUL
            while (nul_end < shell->linesize) {
              if (shell->line[nul_end] != '\0') {
                // an intermediate NUL sequence was found
                memset(&(shell->line[nul_start]), ' ', nul_end - nul_start);
                shell->inputdata.lineend = nul_end + 1;
                break;
              } else {
                ++nul_end;
              }
            }
            nul_start = nul_end + 1;
          } else {
            ++shell->inputdata.lineend;
            ++nul_start;
          }
        }
        shell->inputdata.cursorpos = shell->inputdata.lineend;
        // print the line
        shell->inputdata.noinput = _printLine(shell, 0, shell->inputdata.lineend) == 0;
        break;
      }

      case AOS_SHELL_ACTION_CLEAR:
      {
        // clear output
        _moveCursor(shell, shell->inputdata.cursorpos, 0);
        for (shell->inputdata.cursorpos = 0; shell->inputdata.cursorpos < shell->inputdata.lineend; ++shell->inputdata.cursorpos) {
          streamPut(&shell->stream, ' ');
        }
        _moveCursor(shell, shell->inputdata.lineend, 0);
        shell->inputdata.cursorpos = 0;
        shell->inputdata.lineend = 0;
        shell->inputdata.noinput = true;
        break;
      }

      case AOS_SHELL_ACTION_CURSOR2START:
      {
        _moveCursor(shell, shell->inputdata.cursorpos, 0);
        shell->inputdata.cursorpos = 0;
        break;
      }

      case AOS_SHELL_ACTION_CURSOR2END:
      {
        _moveCursor(shell, shell->inputdata.cursorpos, shell->inputdata.lineend);
        shell->inputdata.cursorpos = shell->inputdata.lineend;
        break;
      }

      case AOS_SHELL_ACTION_CURSORLEFT:
      {
        _moveCursor(shell, shell->inputdata.cursorpos, shell->inputdata.cursorpos-1);
        --shell->inputdata.cursorpos;
        break;
      }

      case AOS_SHELL_ACTION_CURSORRIGHT:
      {
        _moveCursor(shell, shell->inputdata.cursorpos, shell->inputdata.cursorpos+1);
        ++shell->inputdata.cursorpos;
        break;
      }

      case AOS_SHELL_ACTION_EXECUTE:
      {
        streamPut(&shell->stream, '\n');
        // set the number of read bytes and return
        if (!shell->inputdata.noinput) {
          *n = shell->linesize - shell->inputdata.lineend;
          // fill the remainder of the line with NUL bytes
          memset(&(shell->line[shell->inputdata.lineend]), '\0', *n);
          // reset static variables
          shell->inputdata.noinput = true;
        }
        return AOS_SUCCESS;
        break;
      }

      case AOS_SHELL_ACTION_ESCSTART:
      {
        shell->inputdata.escseq[0] = c;
        ++shell->inputdata.escp;
        break;
      }

      case AOS_SHELL_ACTION_NONE:
      default:
      {
        // do nothing (ignore input) and read next byte
        continue;
        break;
      }
    } /* end of switch */

    shell->inputdata.lastaction = action;
  } /* end of while */

  // no more data could be read from the channel
  return AOS_WARNING;
}

/**
 * @brief   Parses the content of the input buffer (line) to separate arguments.
 *
 * @param[in] shell   Pointer to the shell object.
 *
 * @return            Number of arguments found.
 */
static size_t _parseArguments(aos_shell_t* shell)
{
  aosDbgCheck(shell != NULL);

  /*
   * States for a very small FSM.
   */
  typedef enum {
    START,
    SPACE,
    TEXT,
    END,
  } state_t;

  // local variables
  state_t state = START;
  size_t arg = 0;

  // iterate through the line
  for (char* c = shell->line; c < shell->line + shell->linesize; ++c) {
    // terminate at first NUL byte
    if (*c == '\0') {
      state = END;
      break;
    }
    // spaces become NUL bytes
    else if (*c == ' ') {
      *c = '\0';
      state = SPACE;
    }
    // handle non-NUL bytes
    else {
      switch (state) {
        case START:
        case SPACE:
          // ignore too many arguments
          if (arg < shell->arglistsize) {
            shell->arglist[arg] = c;
          }
          ++arg;
          break;
        case TEXT:
        case END:
        default:
          break;
      }
      state = TEXT;
    }
  }

  // set all remaining argument pointers to NULL
  for (size_t a = arg; a < shell->arglistsize; ++a) {
    shell->arglist[a] = NULL;
  }

  return arg;
}

/**
 * @brief   Initializes a shell object with the specified parameters.
 *
 * @param[in] shell         Pointer to the shell object.
 * @param[in] stream        I/O stream to use.
 * @param[in] prompt        Prompt line to print (NULL = use default prompt).
 * @param[in] line          Pointer to the input buffer.
 * @param[in] linesize      Size of the input buffer.
 * @param[in] arglist       Pointer to the argument buffer.
 * @param[in] arglistsize   Size of te argument buffer.
 */
void aosShellInit(aos_shell_t* shell, event_source_t* oseventsource,  const char* prompt, char* line, size_t linesize, char** arglist, size_t arglistsize)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(oseventsource != NULL);
  aosDbgCheck(line != NULL);
  aosDbgCheck(arglist != NULL);

  // set parameters
  shell->thread = NULL;
  chEvtObjectInit(&shell->eventSource);
  shell->os.eventSource = oseventsource;
  aosShellStreamInit(&shell->stream);
  shell->prompt = prompt;
  shell->commands = NULL;
  shell->execstatus.command = NULL;
  shell->execstatus.retval = 0;
  shell->line = line;
  shell->linesize = linesize;
  shell->inputdata.lastaction = AOS_SHELL_ACTION_NONE;
  shell->inputdata.escp = 0;
  memset(shell->inputdata.escseq, '\0', sizeof(shell->inputdata.escseq)*sizeof(shell->inputdata.escseq[0]));
  shell->inputdata.cursorpos = 0;
  shell->inputdata.lineend = 0;
  shell->inputdata.noinput = true;
  shell->arglist = arglist;
  shell->arglistsize = arglistsize;
  shell->config = 0x00;

  // initialize arrays
  memset(shell->line, '\0', shell->linesize);
  for (size_t a = 0; a < shell->arglistsize; ++a) {
    shell->arglist[a] = NULL;
  }

  return;
}

/**
 * @brief   Initialize an AosShellStream object.
 *
 * @param[in] stream  The AosShellStrem to initialize.
 */
void aosShellStreamInit(AosShellStream* stream)
{
  aosDbgCheck(stream != NULL);

  stream->vmt = &_streamvmt;
  stream->channel = NULL;

  return;
}

/**
 * @brief   Initialize an AosShellChannel object with the specified parameters.
 *
 * @param[in] channel       The AosShellChannel to initialize.
 * @param[in] asyncchannel  An BaseAsynchronousChannel this AosShellChannel is associated with.
 */
void aosShellChannelInit(AosShellChannel* channel, BaseAsynchronousChannel* asyncchannel)
{
  aosDbgCheck(channel != NULL);
  aosDbgCheck(asyncchannel != NULL);

  channel->vmt = &_channelvmt;
  channel->asyncchannel = asyncchannel;
  channel->listener.wflags = 0;
  channel->next = NULL;
  channel->flags = 0;

  return;
}

/**
 * @brief   Inserts a command to the shells list of commands.
 *
 * @param[in] shell   Pointer to the shell object.
 * @param[in] cmd     Pointer to the command to add.
 *
 * @return            A status value.
 * @retval AOS_SUCCESS  The command was added successfully.
 * @retval AOS_ERROR    Another command with identical name already exists.
 */
aos_status_t aosShellAddCommand(aos_shell_t *shell, aos_shellcommand_t *cmd)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(cmd != NULL);
  aosDbgCheck(cmd->name != NULL && strlen(cmd->name) > 0 && strchr(cmd->name, ' ') == NULL && strchr(cmd->name, '\t') == NULL);
  aosDbgCheck(cmd->callback != NULL);
  aosDbgCheck(cmd->next == NULL);

  aos_shellcommand_t* prev = NULL;
  aos_shellcommand_t** curr = &(shell->commands);

  // insert the command to the list wrt lexographical order (exception: lower case characters preceed upper their uppercase counterparts)
  while (*curr != NULL) {
    // iterate through the list as long as the command names are 'smaller'
    const int cmp = _strccmp((*curr)->name, cmd->name, true, NULL, NULL);
    if (cmp < 0) {
      prev = *curr;
      curr = &((*curr)->next);
      continue;
    }
    // error if the command already exists
    else if (cmp == 0) {
      return AOS_ERROR;
    }
    // insert the command as soon as a 'larger' name was found
    else /* if (cmpval > 0) */ {
      cmd->next = *curr;
      // special case: the first command is larger
      if (prev == NULL) {
        shell->commands = cmd;
      } else {
        prev->next = cmd;
      }
      return AOS_SUCCESS;
    }
  }
  // the end of the list has been reached

  // append the command
  *curr = cmd;
  return AOS_SUCCESS;
}

/**
 * @brief   Removes a command from the shells list of commands.
 *
 * @param[in] shell     Pointer to the shell object.
 * @param[in] cmd       Name of the command to removde.
 * @param[out] removed  Optional pointer to the command that was removed.
 *
 * @return              A status value.
 * @retval AOS_SUCCESS  The command was removed successfully.
 * @retval AOS_ERROR    The command name was not found.
 */
aos_status_t aosShellRemoveCommand(aos_shell_t *shell, char *cmd, aos_shellcommand_t **removed)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(cmd != NULL && strlen(cmd) > 0);

  aos_shellcommand_t* prev = NULL;
  aos_shellcommand_t** curr = &(shell->commands);

  // iterate through the list and seach for the specified command name
  while (curr != NULL) {
    const int cmpval = strcmp((*curr)->name, cmd);
    // iterate through the list as long as the command names are 'smaller'
    if (cmpval < 0) {
      prev = *curr;
      curr = &((*curr)->next);
      continue;
    }
    // remove the command when found
    else if (cmpval == 0) {
      // special case: the first command matches
      if (prev == NULL) {
        shell->commands = (*curr)->next;
      } else {
        prev->next = (*curr)->next;
      }
      (*curr)->next = NULL;
      // set the optional output argument
      if (removed != NULL) {
        *removed = *curr;
      }
      return AOS_SUCCESS;
    }
    // break the loop if the command names are 'larger'
    else /* if (cmpval > 0) */ {
      break;
    }
  }

  // if the command was not found, return an error
  return AOS_ERROR;
}

/**
 * @brief   Add a channel to a AosShellStream.
 *
 * @param[in] stream    The AosShellStream to extend.
 * @param[in] channel   The channel to be added to the stream.
 */
void aosShellStreamAddChannel(AosShellStream* stream, AosShellChannel* channel)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(channel != NULL && channel->asyncchannel != NULL && channel->next == NULL && (channel->flags & AOS_SHELLCHANNEL_ATTACHED) == 0);

  // prepend the new channel
  chSysLock();
  channel->flags |= AOS_SHELLCHANNEL_ATTACHED;
  channel->next = stream->channel;
  stream->channel = channel;
  chSysUnlock();

  return;
}

/**
 * @brief   Remove a channel from an AosShellStream.
 *
 * @param[in] stream    The AosShellStream to modify.
 * @param[in] channel   The channel to remove.
 * @return
 */
aos_status_t aosShellStreamRemoveChannel(AosShellStream* stream, AosShellChannel* channel)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(channel != NULL && channel->asyncchannel != NULL && channel->flags & AOS_SHELLCHANNEL_ATTACHED);

  // local varibales
  AosShellChannel* prev = NULL;
  AosShellChannel* curr = stream->channel;

  // iterate through the list and search for the specified channel
  while (curr != NULL) {
    // if the channel was found
    if (curr == channel) {
      chSysLock();
      // special case: the first channel matches (prev is NULL)
      if (prev == NULL) {
        stream->channel = curr->next;
      } else {
        prev->next = channel->next;
      }
      curr->next = NULL;
      curr->flags &= ~AOS_SHELLCHANNEL_ATTACHED;
      chSysUnlock();
      return AOS_SUCCESS;
    }
  }

  // if the channel was not found, return an error
  return AOS_ERROR;
}

/**
 * @brief   Enable a AosSheööChannel as input.
 *
 * @param[in] channel   The channel to enable as input.
 */
void aosShellChannelInputEnable(AosShellChannel* channel)
{
  aosDbgCheck(channel != NULL && channel->asyncchannel != NULL);

  chSysLock();
  channel->listener.wflags |= CHN_INPUT_AVAILABLE;
  channel->flags |= AOS_SHELLCHANNEL_INPUT_ENABLED;
  chSysUnlock();

  return;
}

/**
 * @brief   Disable a AosSheööChannel as input.
 *
 * @param[in] channel   The channel to disable as input.
 */
void aosShellChannelInputDisable( AosShellChannel* channel)
{
  aosDbgCheck(channel != NULL && channel->asyncchannel != NULL);

  chSysLock();
  channel->listener.wflags &= ~CHN_INPUT_AVAILABLE;
  channel->flags &= ~AOS_SHELLCHANNEL_INPUT_ENABLED;
  chSysUnlock();

  return;
}

/**
 * @brief   Enable a AosSheööChannel as output.
 *
 * @param[in] channel   The channel to enable as output.
 */
void aosShellChannelOutputEnable(AosShellChannel* channel)
{
  aosDbgCheck(channel != NULL && channel->asyncchannel != NULL);

  channel->flags |= AOS_SHELLCHANNEL_OUTPUT_ENABLED;

  return;
}

/**
 * @brief   Disable a AosSheööChannel as output.
 *
 * @param[in] channel   The channel to disable as output.
 */
void aosShellChannelOutputDisable(AosShellChannel* channel)
{
  aosDbgCheck(channel != NULL && channel->asyncchannel != NULL);

  channel->flags &= ~AOS_SHELLCHANNEL_OUTPUT_ENABLED;

  return;
}

/**
 * @brief   Thread main function.
 *
 * @param[in] aosShellThread    Name of the function;
 * @param[in] shell             Pointer to the shell object.
 */
THD_FUNCTION(aosShellThread, shell)
{
  aosDbgCheck(shell != NULL);

  // local variables
  eventmask_t eventmask;
  eventflags_t eventflags;
  AosShellChannel* channel;
  aos_status_t readeval;
  size_t nchars = 0;
  size_t nargs = 0;
  aos_shellcommand_t* cmd;


  // register OS related events
  chEvtRegisterMask(((aos_shell_t*)shell)->os.eventSource, &(((aos_shell_t*)shell)->os.eventListener), AOS_SHELL_EVENTMASK_OS);
  // register events to all input channels
  for (channel = ((aos_shell_t*)shell)->stream.channel; channel != NULL; channel = channel->next) {
    chEvtRegisterMaskWithFlags(&(channel->asyncchannel->event), &(channel->listener), AOS_SHELL_EVENTMASK_INPUT, channel->listener.wflags);
  }

  // fire start event
  chEvtBroadcastFlags(&(((aos_shell_t*)shell)->eventSource), AOS_SHELL_EVTFLAG_START);

  // print the prompt for the first time
  _printPrompt((aos_shell_t*)shell);

  // enter thread loop
  while (!chThdShouldTerminateX()) {
    // wait for event and handle it accordingly
    eventmask = chEvtWaitOne(ALL_EVENTS);

    // handle event
    switch (eventmask) {

      // OS related events
      case AOS_SHELL_EVENTMASK_OS:
      {
        eventflags = chEvtGetAndClearFlags(&((aos_shell_t*)shell)->os.eventListener);
        // handle shutdown/restart events
        if (eventflags & AOS_SYSTEM_EVENTFLAGS_SHUTDOWN) {
          chThdTerminate(((aos_shell_t*)shell)->thread);
        } else {
          // print an error message
          chprintf((BaseSequentialStream*)&((aos_shell_t*)shell)->stream, "\nERROR: unknown OS event received (0x%08X)\n", eventflags);
        }
        break;
      }

      // input events
      case AOS_SHELL_EVENTMASK_INPUT:
      {
        // check and handle all channels
        channel = ((aos_shell_t*)shell)->stream.channel;
        while (channel != NULL) {
          eventflags = chEvtGetAndClearFlags(&channel->listener);
          // if there is new input
          if (eventflags & CHN_INPUT_AVAILABLE) {
            // read input from channel
            readeval = _readChannel((aos_shell_t*)shell, channel, &nchars);
            // parse input line to argument list only if the input shall be executed
            nargs = (readeval == AOS_SUCCESS && nchars > 0) ? _parseArguments((aos_shell_t*)shell) : 0;
            // check number of arguments
            if (nargs > ((aos_shell_t*)shell)->arglistsize) {
              // error too many arguments
              chprintf((BaseSequentialStream*)&((aos_shell_t*)shell)->stream, "\ttoo many arguments\n");
            } else if (nargs > 0) {
              // search command list for arg[0] and execute callback
              cmd = ((aos_shell_t*)shell)->commands;
              while (cmd != NULL) {
                if (strcmp(((aos_shell_t*)shell)->arglist[0], cmd->name) == 0) {
                  ((aos_shell_t*)shell)->execstatus.command = cmd;
                  chEvtBroadcastFlags(&(((aos_shell_t*)shell)->eventSource), AOS_SHELL_EVTFLAG_EXEC);
                  ((aos_shell_t*)shell)->execstatus.retval = cmd->callback((BaseSequentialStream*)&((aos_shell_t*)shell)->stream, nargs, ((aos_shell_t*)shell)->arglist);
                  chEvtBroadcastFlags(&(((aos_shell_t*)shell)->eventSource), AOS_SHELL_EVTFLAG_DONE);
                  // notify if the command was not successful
                  if (((aos_shell_t*)shell)->execstatus.retval != 0) {
                    chprintf((BaseSequentialStream*)&((aos_shell_t*)shell)->stream, "command returned exit status %d\n", ((aos_shell_t*)shell)->execstatus.retval);
                  }
                  break;
                }
                cmd = cmd->next;
              } /* end of while */

              // if no matching command was found, print an error
              if (cmd == NULL) {
                chprintf((BaseSequentialStream*)&((aos_shell_t*)shell)->stream, "%s: command not found\n", ((aos_shell_t*)shell)->arglist[0]);
              }
            }

            // reset some internal variables and eprint a new prompt
            if (readeval == AOS_SUCCESS && !chThdShouldTerminateX()) {
              ((aos_shell_t*)shell)->inputdata.cursorpos = 0;
              ((aos_shell_t*)shell)->inputdata.lineend = 0;
              _printPrompt((aos_shell_t*)shell);
            }
          }

          // iterate to next channel
          channel = channel->next;
        }
        break;
      }

      // other events
      default:
      {
        // print an error message
        chprintf((BaseSequentialStream*)&((aos_shell_t*)shell)->stream, "\nERROR: unknown event received (0x%08X)\n", eventmask);
        break;
      }

    } /* end of switch */

  } /* end of while */

  // fire event and exit the thread
  chSysLock();
  chEvtBroadcastFlagsI(&(((aos_shell_t*)shell)->eventSource), AOS_SHELL_EVTFLAG_EXIT);
  chThdExitS(MSG_OK);
  // no chSysUnlock() required since the thread has been terminated an all waiting threads have been woken up
}

#endif /* AMIROOS_CFG_SHELL_ENABLE == true */
