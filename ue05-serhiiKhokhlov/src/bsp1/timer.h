//       $Id: timer.h 47849 2023-11-22 18:35:38Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/timer.h $
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

#pragma once

#include "./primitives.h"

// -------------------------------------------------------------------------------------------------

void     drive_timer      (void);
double * get_interval     (void);
void     initialize_timer (on_timer_type * on_timer);
void     reset_timer      (void);
void     set_interval     (double interval);
