//==============================================================================
/**
\file            SDLFramework.cpp
\author          Mateusz 'novo' Klos
\date            July 20, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//==============================================================================
#include "nSDLFramework.hpp"
#include "nLogger.hpp"

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <sys/time.h>

namespace ngl{
  static const int EOk=0;
  namespace app{
    typedef int64_t   Time_t;

#if defined(COMPO_WIN32_BUILD)
    struct SystemClock{
      SystemClock(){
        Time_t freq;
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
        m_conversionConstant=1000000.0/freq;
      }

      Time_t curr_time() const{
        Time_t curr;
        QueryPerformanceCounter( (LARGE_INTEGER *)&curr );
        return curr * m_conversionConstant;
      }

      double  m_conversionConstant;
    };
#else
    struct SystemClock{
      Time_t curr_time() const{
        timeval tv;
        gettimeofday(&tv, 0);
        return static_cast<Time_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
      }
    };
#endif

    struct FrameTimer{
      float frame_time(){
        return lastFrameTime;
      }
      void lap(){
        Time_t    curr =sc.curr_time();
        if( timeStamp==0 ){
          timeStamp=curr-3000;
        }
        lastFrameTime  =(curr - timeStamp)*0.001f;
        timeStamp      =curr;
      }

      SystemClock   sc;
      Time_t        timeStamp;
      float         lastFrameTime;
    };

    struct FrameworkContext{
      AppInterface      *app;
      FrameTimer        frameTimer;
    } g_context;

    //------------------------------------------------------------------------//
    //------------------------------------------------------------------------//
    float frame_time(){
      return g_context.frameTimer.frame_time();
    }
    //------------------------------------------------------------------------//
    //------------------------------------------------------------------------//
    int setup(ngl::AppInterface *app){
      g_context.app   =app;
      return ngl::EOk;
    }
    //------------------------------------------------------------------------//
    //------------------------------------------------------------------------//
    int init(int argc, char **argv){
      //  Init SDL
      {
        if( SDL_Init(SDL_INIT_VIDEO) ){
          return 0;
        }
        set_window_geometry(800, 600, 32, Window::Normal);

        SDL_EnableUNICODE(0);
        SDL_WM_SetCaption( (argc > 0 ? argv[0] : "simple framework"), NULL );
      }
      {
        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glFrontFace(GL_CCW);
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
      }

      return g_context.app->init(argc, argv);
    }
    //------------------------------------------------------------------------//
    //------------------------------------------------------------------------//
    int cleanup(){
      int ret=g_context.app->cleanup();
      if( g_context.app )
        delete g_context.app;
      
      SDL_Quit();
      return ret;
    }
    //------------------------------------------------------------------------//
    //------------------------------------------------------------------------//
    int run(){
      static SystemClock sc;

      // first frame gets dt=3ms.
      bool        running =true;
      ::SDL_Event event;
      int         errCode=EOk;
      while(running){
        while(SDL_PollEvent(&event)){
          if(event.type==SDL_QUIT){
            running=false;
          }
          else if(event.type==SDL_VIDEORESIZE){
            set_window_geometry(event.resize.w, event.resize.h, 32, Window::Normal);
          }
          
          if( (errCode=g_context.app->on_event(&event)) != ngl::EOk ){
            if( errCode > 0 )
              errCode = ngl::EOk;
            running=false;
          }
        }

        g_context.frameTimer.lap();
        if( errCode == EOk && (errCode =g_context.app->tick()) != ngl::EOk )
          running=false;

        glFlush();
        SDL_GL_SwapBuffers();
      }
      return errCode;
    }
    //------------------------------------------------------------------------//
    //------------------------------------------------------------------------//
    int set_window_geometry(uint32_t width,
                            uint32_t height,
                            uint32_t bpp,
                            WindowType type){
      if( !SDL_SetVideoMode(width, height, bpp,
                            SDL_OPENGL | SDL_RESIZABLE |
                            (type==Window::Fullscreen ? SDL_FULLSCREEN : 0)
                            )){
        novo::logerr("SDL_SetVideoMode failed\n");
        return 0;
      }
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(45.0f, (float)width/height, 1.0f, 4024.0f);

      glViewport(0, 0, width, height);

      g_context.app->on_resize(width, height);
      
      return EOk;
    }
  }
}
