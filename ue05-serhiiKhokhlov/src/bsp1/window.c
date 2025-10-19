//       $Id: window.c 47849 2023-11-22 18:35:38Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/window.c $
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

#include "./window.h"

// -------------------------------------------------------------------------------------------------

static GLint         g_location_color    = 0;      //
static GLint         g_location_position = 0;      //
static on_key_type * g_on_key            = NULL;   // the user's event handler for events from the keyboard
static GLFWwindow *  g_p_window          = NULL;   //
static GLuint        g_program           = 0;      //
static GLuint        g_vertex_array      = 0;      //

static char const * const g_fragment_shader =
   "#version 330 core\n"
   "\n"
   "uniform vec3 color;\n"
   "\n"
   "out vec4 FragColor;\n"
   "\n"
   "void main () {\n"
   "   FragColor = vec4 (color, 1);\n"
   "}\n";

static char const * const g_vertex_shader =
   "#version 330 core\n"
   "\n"
   "uniform vec2 scaling;\n"
   "uniform vec2 position_type;\n"
   "\n"
   "const vec2 offsets [] = vec2 [] (vec2 (0, 0), vec2 (1, 0), vec2 (0, 1), vec2 (1, 1));\n"
   "\n"
   "void main () {\n"
   "   gl_Position = vec4 ((position_type + offsets[gl_VertexID]) * scaling - vec2 (1, 1), 0, 1);\n"
   "}\n";

// -------------------------------------------------------------------------------------------------

static void   callback_key (GLFWwindow * g_p_window, int glfw_key, int scancode, int glfw_action, int mods);
static GLuint make_shader  (GLenum type, const char * src);

// -------------------------------------------------------------------------------------------------

void initialize_renderer (int const rows, int const cols, on_key_type * const on_key) {
   if (!glfwInit ())
      signal_last_glfw_error (true);

   glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   if ((g_p_window = glfwCreateWindow (400, 800, "Tetris", NULL, NULL)) == false)
      signal_last_glfw_error (true);

   int width  = 0;
   int height = 0;

   glfwGetWindowSize (g_p_window, &width, &height);
   glfwSetWindowAspectRatio (g_p_window, width, height);   // enforce correct aspect ratio

   glfwMakeContextCurrent (g_p_window);

   if(!gladLoadGLLoader ((GLADloadproc) glfwGetProcAddress))
      signal_last_glad_error (true);

   GLuint const fragment_shader = make_shader (GL_FRAGMENT_SHADER, g_fragment_shader);
   GLuint const vertex_shader   = make_shader (GL_VERTEX_SHADER, g_vertex_shader);

   if ((g_program = glCreateProgram ()) == false)
      signal_last_gl_error (true);

   glAttachShader (g_program, fragment_shader);
   glAttachShader (g_program, vertex_shader);
   glLinkProgram  (g_program);
   glDeleteShader (vertex_shader);
   glDeleteShader (fragment_shader);

   int success = 0; glGetProgramiv (g_program, GL_LINK_STATUS, &success);

   if (!success)
      signal_last_gl_error (true);

   glUseProgram (g_program);

   if ((g_location_position = glGetUniformLocation (g_program, "position_type")) == -1)
      signal_last_gl_error (true);

   if ((g_location_color = glGetUniformLocation (g_program, "color")) == -1)
      signal_last_gl_error (true);

   GLint const location_scaling = glGetUniformLocation (g_program, "scaling");

   if (location_scaling == -1)
      signal_last_gl_error (true);

   glUniform2f (location_scaling, 2.0f / cols, 2.0f / rows);

   glGenVertexArrays (1, &g_vertex_array);

   if (!g_vertex_array)
      signal_last_gl_error (true);

   glBindVertexArray (g_vertex_array);

   g_on_key = on_key; glfwSetKeyCallback (g_p_window, callback_key);
}

bool renderer_is_open (void) {
   return !glfwWindowShouldClose (g_p_window);
}

void shut_down_renderer (void) {
   glUseProgram (0);
   glDeleteProgram (g_program);
   glBindVertexArray (0);
   glDeleteVertexArrays (1, &g_vertex_array);
   glfwDestroyWindow (g_p_window);
   glfwTerminate ();
}

void render_block (position_type const position, color_type const color) {
   glUniform2f (g_location_position, (GLfloat) position.x, (GLfloat) position.y);
   glUniform3f (g_location_color, normalize_red (color), normalize_green (color), normalize_blue (color));

   glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
}

void begin_frame (void) {
   int width  = 0;
   int height = 0;

   glfwGetFramebufferSize (g_p_window, &width, &height);

   glViewport (0, 0, width, height);
   glClear (GL_COLOR_BUFFER_BIT);
}

void end_frame (void) {
   glfwSwapBuffers (g_p_window);
   glfwWaitEventsTimeout (0.25);
}

// -------------------------------------------------------------------------------------------------

static void callback_key (GLFWwindow * const p_window, int const glfw_key, int const scancode, int const glfw_action, int const mods) {
   UNUSED (p_window);
   UNUSED (scancode);
   UNUSED (mods);

   if (g_on_key)
      g_on_key (key_type_from_glfw_key (glfw_key), action_type_from_glfw_action (glfw_action));
}

static GLuint make_shader (GLenum const type, const char * const src) {
   GLuint const shader = glCreateShader (type);

   if (!shader)
      signal_last_gl_error (true);

   glShaderSource  (shader, 1, &src, NULL);
   glCompileShader (shader);

   int success = 0; glGetShaderiv (shader, GL_COMPILE_STATUS, &success);

   if (!success)
      signal_last_ogl_error (shader, true);

   return shader;
}
