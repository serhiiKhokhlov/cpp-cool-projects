//       $Id: primitives.c 47850 2023-11-22 18:48:28Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/primitives.c $
// $Revision: 47850 $
//     $Date: 2023-11-22 19:48:28 +0100 (Mi., 22 Nov 2023) $
//   $Author: p20068 $
//   Creator: Michael Hava
//  Creation: October 24, 2023
// Copyright: (c) 2023 Michael Hava (michael.hava<AT>risc-software.at)
//            (c) 2023 Peter Kulczycki (peter.kulczycki<AT>fh-hagenberg.at)
//   License: This document contains proprietary information belonging to
//            University of Applied Sciences Upper Austria, Campus Hagenberg.
//            It is distributed under the Boost Software License (see
//            https://www.boost.org/users/license.html).

#include "./primitives.h"

// -------------------------------------------------------------------------------------------------

position_type make_position (int const x, int const y) {
   position_type const position = {x, y}; return position;
}

position_type move_position (position_type position, int const dx, int const dy) {   // ToDo
    position.x += dx;
    position.y += dy;

   return position;
}

position_type relative_pos_to_absolute(position_type rel_pos, position_type ref_point) {
    return make_position(rel_pos.x + ref_point.x, rel_pos.y + ref_point.y);
}

position_type bottom_edge(position_type pos1, position_type pos2) {
    return pos1.y < pos2.y ? pos1 : pos2;
}

position_type right_edge(position_type pos1, position_type pos2) {
    return pos1.x > pos2.x ? pos1 : pos2;
}

position_type left_edge(position_type pos1, position_type pos2) {
    return pos1.x < pos2.x ? pos1 : pos2;
}

position_type top_edge(position_type pos1, position_type pos2) {
    return pos1.y > pos2.y ? pos1 : pos2;
}

position_type get_rel_edge(position_type * obj_rel_positions, int arr_size, position_type side_edge_func(position_type pos1, position_type pos2)) {
    position_type rel_edge_pos = obj_rel_positions[0];
    for (int i = 1; i < arr_size; ++i) {
        rel_edge_pos = side_edge_func(rel_edge_pos, obj_rel_positions[i]);
    }
    return rel_edge_pos;
}
// -------------------------------------------------------------------------------------------------------------------------------------------------------


EdgeHitbox new_edge_hitbox(position_type * obj_rel_positions, int arr_size, position_type * pivot_pt) {
    EdgeHitbox e_hitbox = malloc(sizeof(edge_hitbox));
    if (e_hitbox) {
        e_hitbox->pivot_pt = pivot_pt;
        e_hitbox->rel_bottom_edge = get_rel_edge(obj_rel_positions, arr_size, &bottom_edge);
        e_hitbox->rel_top_edge = get_rel_edge(obj_rel_positions, arr_size, &top_edge);
        e_hitbox->rel_right_edge = get_rel_edge(obj_rel_positions, arr_size, &right_edge);
        e_hitbox->rel_left_edge = get_rel_edge(obj_rel_positions, arr_size, &left_edge);
        e_hitbox->rel_pts_array = obj_rel_positions;
        e_hitbox->array_size = arr_size;
    }
    return e_hitbox;
}

void free_edge_hitbox(EdgeHitbox * e_hitbox_ptr) {
    free(*e_hitbox_ptr);
    *e_hitbox_ptr = NULL;
}

NormalLineHitbox new_normal_line_hitbox(position_type pos1, position_type pos2, position_type * pivot_pt) {
    assert(pos1.y == pos2.y || pos1.x == pos2.x); // check normality
    NormalLineHitbox nl_hitbox = malloc(sizeof(normal_line_hitbox));
    if (nl_hitbox) {
        nl_hitbox->pt1 = pos1.y < pos2.y || pos1.x < pos2.x ? pos1 : pos2;
        nl_hitbox->pt2 = pos1.y > pos2.y || pos1.x > pos2.x ? pos1 : pos2;
        nl_hitbox->pivot_pt = pivot_pt;
    }
    return nl_hitbox;
}

void free_normal_line_hitbox(NormalLineHitbox* nl_hitbox_ptr) {
    free(*nl_hitbox_ptr);
    *nl_hitbox_ptr = NULL;
}

ComplexHitbox new_complex_hitbox(bool ** pt, int x_size, int y_size, position_type* pivot_pt) {
    ComplexHitbox c_hitbox = malloc(sizeof(complex_hitbox));
    if (c_hitbox) {
        c_hitbox->pt = pt;
        c_hitbox->x_size = x_size;
        c_hitbox->y_size = y_size;
        c_hitbox->pivot_pt = pivot_pt;
    }
    return c_hitbox;
}

void free_complex_hitbox(ComplexHitbox* c_hitbox_ptr) {
    for (int i = 0; i < (*c_hitbox_ptr)->x_size; ++i) {
        free((*c_hitbox_ptr)->pt[i]);
    }
    free((*c_hitbox_ptr)->pt);
    free(*c_hitbox_ptr);
    *c_hitbox_ptr = NULL;
}

// -------------------------------------------------------------------------------------------------

bool collides_with_complex_hitbox(void * hitbox, position_type hitbox_pt) {
    ComplexHitbox c_hitbox = (ComplexHitbox)hitbox;
    return (hitbox_pt.x < c_hitbox->x_size + c_hitbox->pivot_pt->x &&
        hitbox_pt.x >= c_hitbox->pivot_pt->x &&
        hitbox_pt.y < c_hitbox->y_size + c_hitbox->pivot_pt->y &&
        hitbox_pt.y >= c_hitbox->pivot_pt->y) &&
        !c_hitbox->pt[hitbox_pt.x][hitbox_pt.y];
}

bool collides_with_edge_hitbox(void * hitbox, position_type hitbox_pt) {
    EdgeHitbox e_hitbox = (EdgeHitbox)hitbox;
    return hitbox_pt.x <= e_hitbox->pivot_pt->x + e_hitbox->rel_right_edge.x &&
           hitbox_pt.x >= e_hitbox->pivot_pt->x + e_hitbox->rel_left_edge.x &&
           hitbox_pt.y <= e_hitbox->pivot_pt->y + e_hitbox->rel_top_edge.y &&
           hitbox_pt.y >= e_hitbox->pivot_pt->y + e_hitbox->rel_bottom_edge.y;
}

bool collides_with_nl_hitbox(void * hitbox, position_type hitbox_pt1, position_type hitbox_pt2) {
    NormalLineHitbox nl_hitbox = (NormalLineHitbox)hitbox;
    return 
            (   nl_hitbox->pt1.x == nl_hitbox->pt2.x &&
                (hitbox_pt1.x - nl_hitbox->pt1.x) * (hitbox_pt2.x - nl_hitbox->pt1.x) <= 0) ||
            (nl_hitbox->pt1.y == nl_hitbox->pt2.y &&
                (hitbox_pt1.y - nl_hitbox->pt1.y) * (hitbox_pt2.y - nl_hitbox->pt1.y) <= 0)
        ;
}

// -------------------------------------------------------------------------------------------------

action_type action_type_from_glfw_action (int const glfw_action) {
   action_type action = action_unknown;

   switch (glfw_action) {
      case GLFW_PRESS:   action = action_press;   break;
      case GLFW_RELEASE: action = action_release; break;
      case GLFW_REPEAT:  action = action_repeat;  break;
   }

   return action;
}

key_type key_type_from_glfw_key (int const glfw_key) {
   key_type key = key_unknown;

   switch (glfw_key) {
      case GLFW_KEY_DOWN:   key = key_down;  break;
      case GLFW_KEY_LEFT:   key = key_left;  break;
      case GLFW_KEY_RIGHT:  key = key_right; break;
      case GLFW_KEY_UP:     key = key_up;    break;
      case GLFW_KEY_R:      key = key_r;     break;
      case GLFW_KEY_ESCAPE: key = key_esc;   break;
   }

   return key;
}

// -------------------------------------------------------------------------------------------------

char const * string_from_duration (double const duration) {
   static char buffer [32] = {0};

   sprintf_s (buffer, sizeof (buffer), "%1.2f s", duration);

   return buffer;
}

char const * string_from_key (key_type const key, action_type const action) {
   static char buffer [32] = {0};

   switch (key) {
      case key_unknown: strcpy_s (buffer, sizeof (buffer), "<unknown>"); break;
      case key_down:    strcpy_s (buffer, sizeof (buffer), "down");      break;
      case key_left:    strcpy_s (buffer, sizeof (buffer), "left");      break;
      case key_right:   strcpy_s (buffer, sizeof (buffer), "right");     break;
      case key_up:      strcpy_s (buffer, sizeof (buffer), "up");        break;
      case key_esc:     strcpy_s (buffer, sizeof (buffer), "escape");    break;
      case key_r:       strcpy_s(buffer, sizeof(buffer), "rotate");      break;
   }

   switch (action) {
      case action_unknown: strcat_s (buffer, sizeof (buffer), " <unknown>"); break;
      case action_press:   strcat_s (buffer, sizeof (buffer), " pressed");   break;
      case action_release: strcat_s (buffer, sizeof (buffer), " released");  break;
      case action_repeat:  strcat_s (buffer, sizeof (buffer), " repeated");  break;
   }

   return buffer;
}

// -------------------------------------------------------------------------------------------------

void signal_last_gl_error (bool const force_abort) {   // see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetError.xhtml
   GLenum const error = glGetError ();

   if (error != GL_NO_ERROR) {
      char const * p_message = "<unknown>";

      switch (GL_NO_ERROR) {
         case GL_INVALID_ENUM:
            p_message = "An unacceptable value is specified for an enumerated argument"; break;

         case GL_INVALID_FRAMEBUFFER_OPERATION:
            p_message = "The frame buffer object is not complete"; break;

         case GL_INVALID_OPERATION:
            p_message = "The specified operation is not allowed in the current state"; break;

         case GL_INVALID_VALUE:
            p_message = "A numeric argument is out of range"; break;

         case GL_OUT_OF_MEMORY:
            p_message = "There is not enough memory left to execute the command"; break;

         case GL_STACK_OVERFLOW:
            p_message = "An attempt has been made to perform an operation that would cause an internal stack to overflow"; break;

         case GL_STACK_UNDERFLOW:
            p_message = "An attempt has been made to perform an operation that would cause an internal stack to underflow"; break;
      }

      fprintf (stderr, "GL-ERROR #%d: '%s'\n", error, p_message); abort ();
   }

   if (force_abort)
      abort ();
}

void signal_last_glad_error (bool const force_abort) {
   UNUSED (force_abort); fprintf (stderr, "GLAD-ERROR #%d: '%s'\n", -1, "<unknown>"); abort ();
}

void signal_last_glfw_error (bool const force_abort) {
   char const * p_message = NULL;
   int const    error     = glfwGetError (&p_message);

   if ((error != GLFW_NO_ERROR) && p_message) {
      fprintf (stderr, "GLFW-ERROR #%d: '%s'\n", error, p_message); abort ();
   }

   if (force_abort)
      abort ();
}

void signal_last_ogl_error (GLuint const shader, bool const force_abort) {
   UNUSED (force_abort);

   static char buffer [512] = {0};

   glGetShaderInfoLog (shader, sizeof (buffer), NULL, buffer);

   fprintf (stderr, "OGL-ERROR: '%s'\n", buffer); abort ();
}
