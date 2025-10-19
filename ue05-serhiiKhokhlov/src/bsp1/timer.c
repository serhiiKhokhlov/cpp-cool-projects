//       $Id: timer.c 47849 2023-11-22 18:35:38Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/timer.c $
// $Revision: 47849 $
//     $Date: 2023-11-22 19:35:38 +0100 (Mi., 22 Nov 2023) $
//   $Author: p20068 $
//   Creator: Michael Hava
//  Creation: October 24, 2023
// Copyright: (c) 2023 Michael Hava (michael.hava<AT>risc-software.at)
//            (c) 2023 Peter Kulczycki (peter.kulczycki<AT>fh-hagenberg.at)
//   License: This document contains proprietary information belonging to
//            University of Applied Sciences Upper Austria, Campus Hagenberg.
//            It is distributed under the Boost Software License (see
//            https://www.boost.org/users/license.html).

#include "./timer.h"

// -------------------------------------------------------------------------------------------------

static on_timer_type * g_on_timer = NULL;   // the user's event handler for events from the timer

// -------------------------------------------------------------------------------------------------

void drive_timer (void) {
   if ((glfwGetTime () >= *get_interval ()) && g_on_timer) {
      g_on_timer (glfwGetTime ()); reset_timer ();
   }
}

double * get_interval (void) {
   static double interval = 0.5; return &interval;
}

void initialize_timer (on_timer_type * const on_timer) {
   g_on_timer = on_timer; reset_timer ();
}

void reset_timer (void) {
   glfwSetTime (0);
}

void set_interval (double const interval) {
   *get_interval () = interval >= 0.1 ? (interval <= 5.0 ? interval : 5.0) : 0.1;
}
