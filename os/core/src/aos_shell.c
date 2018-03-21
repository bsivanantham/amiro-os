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

#include <aos_debug.h>
#include <aos_time.h>
#include <aos_system.h>
#include <chprintf.h>
#include <string.h>
#include <aos_thread.h>

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
  aosDbgCheck(shell->stream != NULL);

  // print the system uptime before prompt is configured
  if (shell->config & AOS_SHELL_CONFIG_PROMPT_UPTIME) {
    // get current system uptime
    aos_timestamp_t uptime;
    aosSysGetUptime(&uptime);

    chprintf(shell->stream, "[%01u:%02u:%02u:%02u:%03u:%03u] ",
           (uint32_t)(uptime / MICROSECONDS_PER_DAY),
           (uint8_t)(uptime % MICROSECONDS_PER_DAY / MICROSECONDS_PER_HOUR),
           (uint8_t)(uptime % MICROSECONDS_PER_HOUR / MICROSECONDS_PER_MINUTE),
           (uint8_t)(uptime % MICROSECONDS_PER_MINUTE / MICROSECONDS_PER_SECOND),
           (uint16_t)(uptime % MICROSECONDS_PER_SECOND / MICROSECONDS_PER_MILLISECOND),
           (uint16_t)(uptime % MICROSECONDS_PER_MILLISECOND / MICROSECONDS_PER_MICROSECOND));
  }

  // print the actual prompt string
  if (shell->prompt && !(shell->config & AOS_SHELL_CONFIG_PROMPT_MINIMAL)) {
    chprintf(shell->stream, "%s$ ", shell->prompt);
  } else {
    chprintf(shell->stream, "%>$ ");
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
  aosDbgCheck(shell->stream != NULL);

  // local variables
  size_t pos = from;

  // move cursor left by printing backspaces
  while (pos > to) {
    streamPut(shell->stream, '\b');
    --pos;
  }

  // move cursor right by printing line content
  while (pos < to) {
    streamPut(shell->stream, shell->line[pos]);
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
  aosDbgCheck(shell->stream != NULL);

  // local variables
  size_t cnt;

  for (cnt = 0; from + cnt < to; ++cnt) {
    streamPut(shell->stream, shell->line[from + cnt]);
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

/**
 * @brief   Reads a line from input stream
 * @details The line is directly written to the given shell object.
 *
 * @param[in] shell   Pointer to the shell object.
 *
 * @return              A status indicator.
 * @retval AOS_SUCCESS  Input sucessfully read, line is valid.
 * @retval AOS_ERROR    An I/O error occurred.
 */
static aos_status_t _readLine(aos_shell_t* shell)
{
  aosDbgCheck(shell != NULL);

  /*
   * Enumerator to encode a function.
   */
  typedef enum {
    READ_CHAR,
    AUTOFILL,
    SUGGEST,
    INS_TOGGLE,
    DELETE_FORWARD,
    DELETE_BACKWARD,
    RECALL_LAST,
    CLEAR,
    CURSOR2START,
    CURSOR2END,
    CURSOR_LEFT,
    CURSOR_RIGHT,
    EXECUTE,
    ESC_START,
    NONE,
  } func_t;

  // local variables
  func_t func = NONE;
  func_t lastfunc = NONE;
  bool noinput = true;
  size_t lineend = 0;
  size_t cursorpos = 0;
  char c;
  uint8_t escp = 0;
  char escseq[5] = {'\0'};

  // read character by character from stream
  while (streamRead(shell->stream, (uint8_t*)&c, 1)) {
    special_key_t key = KEY_UNKNOWN;

    // parse escape sequence
    if (escp > 0) {
      escseq[escp] = c;
      ++escp;
      key = _interpreteEscapeSequence(escseq);
      if (key == KEY_AMBIGUOUS) {
        // read next byte to resolve ambiguity
        continue;
      } else {
        // if the escape sequence could either be parsed sucessfully
        // or there is no match (KEY_UNKNOWN),
        // reset the sequence variables and interprete key/character
        escp = 0;
        memset(escseq, '\0', sizeof(escseq));
      }
    }

    // interprete keys or characters
    {
      func = NONE; // default
      if (key == KEY_UNKNOWN &&
          (/* printable character */ c >= '\x20' && c <= '\x7E') ) {
        func = READ_CHAR;
      } else if (key == KEY_TAB ||
                 /* horizontal tab ('\t') */ c == '\x09') {
        // pressing tab once applies auto fill,
        // presing a second time prints suggestions
        if (lastfunc == AUTOFILL || lastfunc == SUGGEST) {
          func = SUGGEST;
        } else {
          func = AUTOFILL;
        }
      } else if (key == KEY_INSERT) {
        func = INS_TOGGLE;
      } else if (key == KEY_DELETE ||
                 /* [DEL] */ c == '\x7F') {
        // ignore of cursor is very right
        if (cursorpos < lineend) {
          func = DELETE_FORWARD;
        }
      } else if (key == KEY_BACKSPACE ||
                 /* backpace ('\b') */c == '\x08') {
        // ignore if cursor is very left
        if (cursorpos > 0) {
          func = DELETE_BACKWARD;
        }
      } else if (key == KEY_PAGE_UP ||
                 key == KEY_ARROW_UP) {
        // ignore if there was some input
        if (noinput) {
          func = RECALL_LAST;
        }
      } else if (key == KEY_PAGE_DOWN ||
                 key == KEY_ARROW_DOWN ||
                 /* end of test */ c == '\x03' ||
                 /* end of transmission */ c == '\x04') {
        // ignore if line is empty
        if (lineend > 0) {
          func = CLEAR;
        }
      } else if (key == KEY_HOME) {
        // ignore if cursor is very left
        if (cursorpos > 0) {
          func = CURSOR2START;
        }
      } else if (key == KEY_END) {
        // ignore if cursor is very right
        if (cursorpos < lineend) {
          func = CURSOR2END;
        }
      } else if (key == KEY_ARROW_LEFT) {
        // ignore if cursor is very left
        if (cursorpos > 0) {
          func = CURSOR_LEFT;
        }
      } else if (key == KEY_ARROW_RIGHT) {
        // ignore if cursor is very right
        if (cursorpos < lineend) {
          func = CURSOR_RIGHT;
        }
      } else if (/* carriage return ('\r') */c == '\x0D' ||
                 /* line feed ('\n') */ c == '\x0A') {
        func = EXECUTE;
      } else if (key == KEY_ESCAPE ||
                 /* [ESCAPE] */ c == '\x1B') {
        func = ESC_START;
      }
    }

    /* handle function */
    switch (func) {
      case READ_CHAR:
        // line is full
        if (lineend + 1 >= shell->linesize) {
          _moveCursor(shell, cursorpos, lineend);
          chprintf(shell->stream, "\n\tmaximum line width reached\n");
          _printPrompt(shell);
          _printLine(shell, 0, lineend);
          _moveCursor(shell, lineend, cursorpos);
        }
        // read character
        else {
          // clear old line content on first input
          if (noinput) {
            memset(shell->line, '\0', shell->linesize);
            noinput = false;
          }
          // overwrite content
          if (shell->config & AOS_SHELL_CONFIG_INPUT_OVERWRITE) {
            shell->line[cursorpos] = c;
            ++cursorpos;
            lineend = (cursorpos > lineend) ? cursorpos : lineend;
            streamPut(shell->stream, c);
          }
          // insert character
          else {
            memmove(&(shell->line[cursorpos+1]), &(shell->line[cursorpos]), lineend - cursorpos);
            shell->line[cursorpos] = c;
            ++lineend;
            _printLine(shell, cursorpos, lineend);
            ++cursorpos;
            _moveCursor(shell, lineend, cursorpos);
          }
        }
        break;

      case AUTOFILL:
      {
        const char* fill = shell->line;
        size_t cmatch = cursorpos;
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
          if (cmatch + cmp == cursorpos) {
            cmatch = cursorpos;
            fill = cmd->name;
            // break the loop only if there are no case mismatches with the input
            n = cursorpos;
            _strccmp(fill, shell->line, false, &n, &mlvl);
            if (mlvl == CHAR_MATCH_CASE) {
              break;
            }
          }
          // if a not exact match was found
          else if (cmatch + cmp > cursorpos) {
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
        if (cmatch > cursorpos || (cmatch == cursorpos && matchlevel == CHAR_MATCH_NCASE)) {
          noinput = false;
          // limit auto fill so it will not overflow the line width
          if (lineend + (cmatch - cursorpos) > shell->linesize) {
            cmatch = shell->linesize - lineend + cursorpos;
          }
          // move trailing memory further in the line
          memmove(&(shell->line[cmatch]), &(shell->line[cursorpos]), lineend - cursorpos);
          lineend += cmatch - cursorpos;
          // if there was no incorrect case when matching
          if (matchlevel == CHAR_MATCH_CASE) {
            // insert fill command name to line
            memcpy(&(shell->line[cursorpos]), &(fill[cursorpos]), cmatch - cursorpos);
            // print the output
            _printLine(shell, cursorpos, lineend);
          } else {
            // overwrite line with fill command name
            memcpy(shell->line, fill, cmatch);
            // reprint the whole line
            _moveCursor(shell, cursorpos, 0);
            _printLine(shell, 0, lineend);
          }
          // move cursor to the end of the matching sequence
          cursorpos = cmatch;
          _moveCursor(shell, lineend, cursorpos);
        }
        break;
      }

      case SUGGEST:
      {
        unsigned int matches = 0;
        // iterate through command list
        for (aos_shellcommand_t* cmd = shell->commands; cmd != NULL; cmd = cmd->next) {
          // compare line content with command, excpet if cursorpos=0
          size_t i = cursorpos;
          if (cursorpos > 0) {
            _strccmp(shell->line, cmd->name, true, &i, NULL);
          }
          const int cmp = (i < cursorpos) ?
                            (i - cursorpos) :
                            (cmd->name[i] != '\0') ?
                              strlen(cmd->name) - i :
                              0;
          // if a match was found
          if (cmp > 0) {
            // if this is the first one
            if (matches == 0) {
              _moveCursor(shell, cursorpos, lineend);
              streamPut(shell->stream, '\n');
            }
            // print the command
            chprintf(shell->stream, "\t%s\n", cmd->name);
            ++matches;
          }
        }
        // reprint the prompt and line if any matches have been found
        if (matches > 0) {
          _printPrompt(shell);
          _printLine(shell, 0, lineend);
          _moveCursor(shell, lineend, cursorpos);
          noinput = false;
        }
        break;
      }

      case INS_TOGGLE:
        if (shell->config & AOS_SHELL_CONFIG_INPUT_OVERWRITE) {
          shell->config &= ~AOS_SHELL_CONFIG_INPUT_OVERWRITE;
        } else {
          shell->config |= AOS_SHELL_CONFIG_INPUT_OVERWRITE;
        }
        break;

      case DELETE_FORWARD:
        --lineend;
        memmove(&(shell->line[cursorpos]), &(shell->line[cursorpos+1]), lineend - cursorpos);
        _printLine(shell, cursorpos, lineend);
        streamPut(shell->stream, ' ');
        _moveCursor(shell, lineend + 1, cursorpos);
        break;

      case DELETE_BACKWARD:
        --cursorpos;
        memmove(&(shell->line[cursorpos]), &(shell->line[cursorpos+1]), lineend - cursorpos);
        --lineend;
        shell->line[lineend] = '\0';
        _moveCursor(shell, cursorpos + 1, cursorpos);
        _printLine(shell, cursorpos, lineend);
        streamPut(shell->stream, ' ');
        _moveCursor(shell, lineend+1, cursorpos);
        break;

      case RECALL_LAST:
      {
        // replace any intermediate NUL bytes with spaces
        lineend = 0;
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
                lineend = nul_end + 1;
                break;
              } else {
                ++nul_end;
              }
            }
            nul_start = nul_end + 1;
          } else {
            ++lineend;
            ++nul_start;
          }
        }
        cursorpos = lineend;
        // print the line
        noinput = _printLine(shell, 0, lineend) == 0;
        break;
      }

      case CLEAR:
        // clear output
        _moveCursor(shell, cursorpos, 0);
        for (cursorpos = 0; cursorpos < lineend; ++cursorpos) {
          streamPut(shell->stream, ' ');
        }
        _moveCursor(shell, lineend, 0);
        cursorpos = 0;
        lineend = 0;
        noinput = true;
        break;

      case CURSOR2START:
        _moveCursor(shell, cursorpos, 0);
        cursorpos = 0;
        break;

      case CURSOR2END:
        _moveCursor(shell, cursorpos, lineend);
        cursorpos = lineend;
        break;

      case CURSOR_LEFT:
        _moveCursor(shell, cursorpos, cursorpos-1);
        --cursorpos;
        break;

      case CURSOR_RIGHT:
        _moveCursor(shell, cursorpos, cursorpos+1);
        ++cursorpos;
        break;

      case EXECUTE:
        streamPut(shell->stream, '\n');
        // return a warning if there was no input
        if (noinput) {
          return AOS_WARNING;
        } else {
          // fill the remainder of the line with NUL bytes
          memset(&(shell->line[lineend]), '\0', shell->linesize - lineend);
          return AOS_SUCCESS;
        }
        break;

      case ESC_START:
        escseq[0] = c;
        ++escp;
        break;

      case NONE:
      default:
        // do nothing (ignore input) and read next byte
        continue;
        break;
    }

    lastfunc = func;
  } /* end of while */

  /* This code is only executed when some error occurred.
   * The reason may be:
   *   - The input stream was disabled (streamRead() returned 0)
   *   - Parsing of input failed unexpectedly
   */
  return AOS_ERROR;
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
void aosShellInit(aos_shell_t* shell, BaseSequentialStream* stream, const char* prompt, char* line, size_t linesize, char** arglist, size_t arglistsize)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(stream != NULL);
  aosDbgCheck(line != NULL);
  aosDbgCheck(arglist != NULL);

  // set parameters
  shell->thread = NULL;
  chEvtObjectInit(&shell->eventSource);
  shell->stream = stream;
  shell->prompt = prompt;
  shell->commands = NULL;
  shell->execstatus.command = NULL;
  shell->execstatus.retval = 0;
  shell->line = line;
  shell->linesize = linesize;
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
  while (1) {
    // if the end of the list was reached, append the command
    if (*curr == NULL) {
      *curr = cmd;
      return AOS_SUCCESS;
    } else {
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
  }
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
 * @brief   Thread main function.
 *
 * @param[in] aosShellThread    Name of the function;
 * @param[in] shell             Pointer to the shell object.
 */
THD_FUNCTION(aosShellThread, shell)
{
  aosDbgCheck(shell != NULL);

  // local variables
  size_t nargs = 0;
  aos_status_t readlval;

  // fire start event
  chEvtBroadcastFlags(&(((aos_shell_t*)shell)->eventSource), AOS_SHELL_EVTFLAG_START);

  // enter thread loop
  while (!chThdShouldTerminateX()) {
    // print the prompt
    _printPrompt((aos_shell_t*)shell);

    // read input line
    readlval = _readLine((aos_shell_t*)shell);
    if (readlval == AOS_ERROR) {
      // emit an error event
      chEvtBroadcastFlags(&(((aos_shell_t*)shell)->eventSource), AOS_SHELL_EVTFLAG_IOERROR);
      // erither break the loop or try again afte some time
      if (chThdShouldTerminateX()) {
        break;
      } else {
        aosThdSSleep(1);
      }
    }

    // parse input line to arguments only if reading the line was successful
    nargs = (readlval == AOS_SUCCESS) ? _parseArguments((aos_shell_t*)shell) : 0;
    if (nargs > ((aos_shell_t*)shell)->arglistsize) {
      // error: too many arguments
      chprintf(((aos_shell_t*)shell)->stream, "\tERROR: too many arguments\n");
      continue;
    }

    // skip if there are no arguments
    if (nargs > 0) {
      // search command list for arg[0] and execure callback function
      aos_shellcommand_t* cmd = ((aos_shell_t*)shell)->commands;
      while (cmd != NULL) {
        if (strcmp(((aos_shell_t*)shell)->arglist[0], cmd->name) == 0) {
          ((aos_shell_t*)shell)->execstatus.command = cmd;
          chEvtBroadcastFlags(&(((aos_shell_t*)shell)->eventSource), AOS_SHELL_EVTFLAG_EXEC);
          ((aos_shell_t*)shell)->execstatus.retval = cmd->callback(((aos_shell_t*)shell)->stream, nargs, ((aos_shell_t*)shell)->arglist);
          chEvtBroadcastFlags(&(((aos_shell_t*)shell)->eventSource), AOS_SHELL_EVTFLAG_DONE);
          // append a line break so the next print will start from the very left
          // usually this should just add an empty line, which is visually appealing
          chprintf(((aos_shell_t*)shell)->stream, "\n");
          // notify if the command was not successful
          if (((aos_shell_t*)shell)->execstatus.retval != 0) {
            chprintf(((aos_shell_t*)shell)->stream, "command returned exit status %d\n", ((aos_shell_t*)shell)->execstatus.retval);
          }
          break;
        }
        cmd = cmd->next;
      }
      // if no matching command was found, print an error
      if (cmd == NULL) {
        chprintf(((aos_shell_t*)shell)->stream, "%s: command not found\n", ((aos_shell_t*)shell)->arglist[0]);
      }
    }

  } /* end of while loop */

  // fire event and exit the thread
  chSysLock();
  chEvtBroadcastFlagsI(&(((aos_shell_t*)shell)->eventSource), AOS_SHELL_EVTFLAG_EXIT);
  chThdExitS(MSG_OK);
  // no chSysUnlock() required since the thread has been terminated an all waiting threads have been woken up
}
