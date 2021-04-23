/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
 * Copyright 2020 Liberty Global B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifdef HAVE_EPOXY
#  include <epoxy/egl.h>
#else
#  include <EGL/egl.h>
#  include <GLES2/gl2.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "essos-app.h" 
#include <signal.h>

static EssCtx *ctx= 0;
static bool gRunning;
static int gDisplayWidth;
static int gDisplayHeight;
static EGLint swap_interval = 1;
static int32_t width = 1920;
static int32_t height = 1080;

static EGLDisplay egl_display;
static char running = 1;

struct window {
	EGLContext egl_context;
	EGLSurface egl_surface;
	int color;
};

static void show_fps() {
	struct timeval curTime;
	time_t nowMs;
	static time_t lastPrintTime = 0;
	static time_t lastPrintFrame = 0;
	static unsigned long frame = 0;

	gettimeofday(&curTime, NULL);
	nowMs =  curTime.tv_usec / 1000;
	nowMs += curTime.tv_sec  * 1000;

	frame++;

	if (nowMs - lastPrintTime >= 5000 || lastPrintFrame == 0) {
		if (nowMs - lastPrintTime != 0 && lastPrintTime != 0) {
			const float fps = (float) (frame - lastPrintFrame) / ((nowMs - lastPrintTime) / 1000.0f);
			printf("FPS: %.2f\n", fps);
		}

		lastPrintFrame = frame;
		lastPrintTime  = nowMs;
	}
}

static void draw_window (struct window *window) {
	window->color = (window->color + 1) % 256;
	float c = window->color / 255.0;
	glClearColor (0.0, c, 0.0, 1.0);
	glClear (GL_COLOR_BUFFER_BIT);

	show_fps();
}

void load_env() {
	const char *swap_str = getenv("SWAP_INTERVAL");
	const char *width_str = getenv("WIDTH");
	const char *height_str = getenv("HEIGHT");

	if (swap_str) {
		swap_interval = atoi(swap_str);
	}

	if (width_str) {
		width = atoi(width_str);
	}

	if (height_str) {
		height = atoi(height_str);
	}
}

static void signalHandler(int signum)
{
   printf("signalHandler: signum %d\n", signum);
   gRunning= false;
}

static void terminated( void * )
{
   printf("terminated event\n");
   gRunning= false;
}

static EssTerminateListener terminateListener=
{
   terminated
};

static void keyPressed( void *, unsigned int key )
{
   switch( key )
   {
       default:
          break;
   }
}

static void keyReleased( void *, unsigned int )
{
}

static EssKeyListener keyListener=
{
   keyPressed,
   keyReleased
};

void displaySize( void *userData, int width, int height )
{
   EssCtx *ctx= (EssCtx*)userData;
   if ( (gDisplayWidth != width) || (gDisplayHeight != height) )
   {
      printf("display size changed: %dx%d\n", width, height);
      gDisplayWidth= width;
      gDisplayHeight= height;
      EssContextResizeWindow( ctx, width, height );
   }
}

static EssSettingsListener settingsListener=
{
   displaySize
};

int main( int argc, char **argv )
{
   int nRC= 0;
   struct window window;
   load_env();

   ctx= EssContextCreate();

   if ( ctx )
   {
      int len;
      bool error= false;

      if ( !EssContextSetTerminateListener( ctx, 0, &terminateListener ) )
      {
         error= true;
      }

      if ( !EssContextSetKeyListener( ctx, 0, &keyListener ) )
      {
         error= true;
      }

      if ( !EssContextSetSettingsListener( ctx, ctx, &settingsListener ) )
      {
         error= true;
      }

      if ( !EssContextInit( ctx ) )
      {
         error= true;
      }

      if ( !EssContextGetDisplaySize( ctx, &gDisplayWidth, &gDisplayHeight ) )
      {
         error= true;
      }

      if ( !error )
      {
         struct sigaction sigint;
         sigint.sa_handler= signalHandler;
         sigemptyset(&sigint.sa_mask);
         sigint.sa_flags= SA_RESETHAND;
         sigaction(SIGINT, &sigint, NULL);

         if ( !EssContextStart( ctx ) )
         {
            error= true;
         }

         if ( !error )
         {
            gRunning= true;
            while( gRunning )
            {
               draw_window (&window);
               EssContextUpdateDisplay( ctx );
               EssContextRunEventLoopOnce( ctx );
            }
         }
      }

      if ( error )
      {
         const char *detail= EssContextGetLastErrorDetail( ctx );
         printf("Essos error: (%s)\n", detail );
      }
      EssContextDestroy( ctx );
   }
   return nRC;
}
