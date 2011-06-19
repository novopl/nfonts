//======================================================================
/**
\file            fonts.cpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//======================================================================
#include "nFont.hpp"
#include "nFontRenderers.hpp"
#include "nSDLFramework.hpp"

#include <sys/time.h>
#include <cstdio>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>



typedef int64_t   Time_t;   // usec
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
Time_t curr_time(){
  timeval tv;
  gettimeofday(&tv, 0);
  return static_cast<Time_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}
//--------------------------------------------------------------------//
/// \returns
///   Last frame duration in miliseconds.
//--------------------------------------------------------------------//
float frame_time(Time_t &timeStamp){
  Time_t    curr =curr_time();
  float     ret  =(curr - timeStamp)*0.001f;
  timeStamp      =curr;
  return ret;
}
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
template<uint32_t C>
bool do_every(){
  static uint32_t counter=1;
  if( !(--counter) )
    counter=C;
  return counter==C;
}

//======================================================================
/** \struct App
\brief  Application.
*/
//======================================================================
struct FrameStatus{
    inline FrameStatus();

    inline void update(float dtInMs, size_t verts, size_t tris);
    size_t fps()    const { return m_fps;         }
    size_t vps()    const { return m_vertsPerSec; }
    size_t tps()    const { return m_trisPerSec;  }
    size_t verts()  const { return m_verts;       }
    size_t tris()   const { return m_tris;        }
    
  private:
    size_t  m_numFrames;
    float   m_timeAcc;
    size_t  m_vertAcc;
    size_t  m_triAcc;
    size_t  m_verts;
    size_t  m_tris;
    size_t  m_fps;
    size_t  m_vertsPerSec;
    size_t  m_trisPerSec;
};
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
FrameStatus::FrameStatus()
:m_numFrames(0), m_timeAcc    (0.f),  m_vertAcc   (0), m_triAcc(0),
m_fps       (0), m_vertsPerSec(0),    m_trisPerSec(0),
m_verts     (0), m_tris       (0){
}
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
void FrameStatus::update(float dtInMs, size_t verts, size_t tris){
  m_vertAcc   +=verts;
  m_triAcc    +=tris;
  m_timeAcc +=dtInMs;
  ++m_numFrames;
  if( m_timeAcc > 1000.0f ){
    m_fps         =m_numFrames;
    m_vertsPerSec =m_vertAcc;
    m_trisPerSec  =m_triAcc;
    m_verts       =verts;
    m_tris        =tris;

    m_timeAcc-=1000.0f;
    m_numFrames=m_vertAcc=m_triAcc=0;
  }
}


//======================================================================
/** \class App
\brief  SDL framework application.
*/
//======================================================================
class App : public ngl::AppInterface{
  App(const App &obj)             {               }
  App& operator=(const App &obj)  { return *this; }

  public:
    App();
    virtual ~App();
    
    virtual int   init(int argc, char **argv);
    virtual int   cleanup();
    virtual int   tick();

  private:
    ngl::AbstractRenderer *renderer;
    ngl::Font             *font;
    FrameStatus           frameStats;
};
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
App::App(){
  
}
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
App::~App(){
  
}
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
int App::init(int argc, char **argv){
  ngl::init_extensions();
  ngl::freetype::init();

  font      =new ngl::Font("Inconsolata.otf", 11);
  //renderer  =ngl::create_renderer(ngl::Renderer::VBO);
  renderer  =ngl::create_renderer(ngl::Renderer::VA);
  //renderer  =ngl::create_renderer(ngl::Renderer::Legacy);

  font->init_position( 600 );

  return ngl::EOk;
}
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
int App::cleanup(){
  ngl::freetype::cleanup();
  delete renderer;
  delete font;
  return ngl::EOk;
}
//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
int App::tick(){
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  char cbuff[128]={0};
  snprintf(cbuff, 128,
            "^3FPS:     ^07%d\n"
            "^3verts/s: ^07%d\n^3tris/s:  ^07%d\n"
            "^3verts:   ^07%d\n^3tris:    ^07%d\n",
            frameStats.fps(),
            frameStats.vps(),
            frameStats.tps(),
            frameStats.verts(),
            frameStats.tris()
            );
  font->cprint(cbuff);
  font->cprint("^4Testing, ^5one, ^6two, ^7testing\n");

  font->print("Lorem ipsum sit dolor amet\n", ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n", 
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n", 
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Lorem ipsum sit dolor amet. Lorem ipsum dolor amet\n",
              ngl::Color32::white);
  font->print("Hello, world!!!\n", ngl::Color32::red);

  font->update_cache();
  renderer->render(*font);

  frameStats.update(ngl::app::frame_time(), 
                    font->vertex_count(), 
                    font->tri_count());

  return ngl::EOk;
}




//--------------------------------------------------------------------//
//--------------------------------------------------------------------//
int main(int argc, char **argv){
  using namespace ngl;

  Error errCode;
  if( (errCode =app::setup( new App() ))  != EOk )    return errCode;
  if( (errCode =app::init(argc, argv))    != EOk )    return errCode;

  app::run();
  app::cleanup();

  return 0;
  
//   ngl::Error errCode;
//   ngl::app::setup(new App());
//   errCode=ngl::app::init(argc, argv);
//   if( errCode != ngl::EOk )
//     return errCode;
// 
//   ngl::app::run();
//   ngl::app::cleanup();
}
