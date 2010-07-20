//==============================================================================
/**
\file            fonts.cpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//==============================================================================
#include "Font.hpp"
#include "FontFace.hpp"

#include <sys/time.h>
#include <cstdio>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>



typedef int64_t   Time_t;   // usec
Time_t curr_time(){
  timeval tv;
  gettimeofday(&tv, 0);
  return static_cast<Time_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}
//--------------------------------------------------------------------------//
/// \returns
///   Last frame duration in miliseconds.
//--------------------------------------------------------------------------//
float frame_time(Time_t &timeStamp){
  Time_t    curr =curr_time();
  float     ret  =(curr - timeStamp)*0.001f;
  timeStamp      =curr;
  return ret;
}

struct FrameStatus{
    FrameStatus()
    :m_numFrames(0), m_timeAcc(0.f), m_vertAcc(0), m_triAcc(0),
    m_fps(0), m_vertsPerSec(0), m_trisPerSec(0),
    m_verts(0), m_tris(0){
    }

    void update(float dtInMs, size_t verts, size_t tris){
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
    size_t fps() const{
      return m_fps;
    }()
    size_t vps() const{
      return m_vertsPerSec;
    }
    size_t tps() const{
      return m_trisPerSec;
    }
    size_t verts() const{
      return m_verts;
    }
    size_t tris() const{
      return m_tris;
    }
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

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
int set_window_geometry(uint32_t width,
                        uint32_t height,
                        uint32_t bpp){
  if( !SDL_SetVideoMode(width, height, bpp, SDL_OPENGL | SDL_RESIZABLE) ){
    fprintf(stderr, "SDL_SetVideoMode failed\n");
    return 0;
  }
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)width/height, .1f, 1024.0f);

  glViewport(0, 0, width, height);

  return 1;
}





struct Renderer{
  Renderer(){
  }
  ~Renderer(){
  }
  int state_setup();
  int state_cleanup();
  int legacy(const ngl::Font &font);
  int vertex_array(const ngl::Font &font);

  static const int    kNumVerts=4096;
  ngl::Font::Vertex   vb[kNumVerts];
  ngl::Triangle16     ib[kNumVerts/2];
};
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
int Renderer::state_setup(){
  using namespace ngl;
  float view[4];
  Error err;
  uint32_t attr =GL_TEXTURE_BIT
                 | GL_TRANSFORM_BIT
                 | GL_COLOR_BUFFER_BIT
                 | GL_CURRENT_BIT;
  GL_DBG( glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT)            );
  GL_DBG( glPushAttrib(attr)                                        );
  GL_DBG( glGetFloatv(GL_VIEWPORT, view)                            );
  GL_DBG( glMatrixMode(GL_PROJECTION)                               );
  GL_DBG( glPushMatrix()                                            );
  GL_DBG( glLoadIdentity()                                          );
  GL_DBG( glOrtho(view[0], view[2], view[1], view[3], -10.f, 10.f)  );
  GL_DBG( glMatrixMode(GL_MODELVIEW)                                );
  GL_DBG( glLoadIdentity()                                          );
  GL_DBG( glEnable(GL_BLEND)                                        );
  GL_DBG( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)         );
  return EOk;
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
int Renderer::state_cleanup(){
  using namespace ngl;
  Error err;
  GL_DBG( glMatrixMode(GL_PROJECTION);      );
  GL_DBG( glPopMatrix();                    );
  GL_DBG( glPopAttrib();                    );
  GL_DBG( glPopClientAttrib();              );
  return EOk;
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
int Renderer::legacy(const ngl::Font &font){
  using namespace ngl;
  TextureID     texID;
  ngl::Font::Vertex   *vb=new ngl::Font::Vertex[font.vertex_count()];
  ngl::Triangle16     *ib=new ngl::Triangle16[font.tri_count()];
//   memset(vb, 0, sizeof(Font::Vertex)*kNumVerts);
//   memset(ib, 0, sizeof(Triangle16)*kNumVerts/2);
  font.get_geometry(vb, ib, texID);

  const int kPrintInterval=1000;
  static int counter=kPrintInterval-1;
  bool printInfo=false;
  if( printInfo && !(++counter % kPrintInterval) ){
    printf("verts: %d\ntris:  %d\n", font.vertex_count(), font.tri_count() );
  }

//   glDisable(GL_TEXTURE_2D);
  state_setup();
  const Font::Vertex *v;
  glBindTexture(GL_TEXTURE_2D, texID);
  glBegin(GL_TRIANGLES);
  for(size_t i=0;  i < font.tri_count(); ++i){
      v =&vb[ ib[i].a ];
      glColor4ubv((byte*)&v->color);
      glTexCoord2f(v->texCoord.u, v->texCoord.v);
      glVertex2i(v->position.x, v->position.y);
      if( !(counter % kPrintInterval) && printInfo)
        printf("(%d, %d)[%.3f, %.3f] ; ",
               v->position.x, v->position.y,
               v->texCoord.u, v->texCoord.v);

      v =&vb[ ib[i].b ];
      glColor4ubv((byte*)&v->color);
      glTexCoord2f(v->texCoord.u, v->texCoord.v);
      glVertex2i(v->position.x, v->position.y);
      if( !(counter % kPrintInterval) && printInfo)
        printf("(%d, %d)[%.3f, %.3f] ; ",
               v->position.x, v->position.y,
               v->texCoord.u, v->texCoord.v);

      v =&vb[ ib[i].c ];
      glColor4ubv((byte*)&v->color);
      glTexCoord2f(v->texCoord.u, v->texCoord.v);
      glVertex2i(v->position.x, v->position.y);
      if( !(counter % kPrintInterval) && printInfo)
        printf("(%d, %d)[%.3f, %.3f]\n",
               v->position.x, v->position.y,
               v->texCoord.u, v->texCoord.v);
  }
  glEnd();

  state_cleanup();

  delete[] vb;
  delete[] ib;

  return EOk;
}
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
int Renderer::vertex_array(const ngl::Font &font){
  using namespace ngl;;
  TextureID     texID;
  float         view[4];
  font.get_geometry(vb, ib, texID);

  bool        printInfo       =false;
  const int   kPrintInterval  =500;
  static int  counter         =kPrintInterval-1;
  if( printInfo && !(++counter % kPrintInterval) ){
    printf("----------------------------\n");
    printf("verts: %d\ntris:  %d\n", font.vertex_count(), font.tri_count() );
//     printf("view: (%.0f, %.0f)x(%.0f, %.0f)\n", view[0], view[1], view[2], view[3]);
  }

  state_setup();
  Error err;
  GL_DBG( glEnableClientState(GL_VERTEX_ARRAY)                      );
  GL_DBG( glEnableClientState(GL_COLOR_ARRAY)                       );
  GL_DBG( glEnableClientState(GL_TEXTURE_COORD_ARRAY)               );

  GL_DBG( glVertexPointer(2, GL_INT,
                          sizeof(Font::Vertex),
                          (byte*)vb+OFFSET(Font::Vertex, position) ) );
  GL_DBG( glTexCoordPointer(2, GL_FLOAT,
                          sizeof(Font::Vertex),
                          (byte*)vb+OFFSET(Font::Vertex, texCoord) ) );
  GL_DBG( glColorPointer(4, GL_UNSIGNED_BYTE,
                          sizeof(Font::Vertex),
                          (byte*)vb+OFFSET(Font::Vertex, color) ) );

  glDrawElements(GL_TRIANGLES, font.tri_count()*3, GL_UNSIGNED_SHORT, ib);
  

  state_cleanup();

  return EOk;
}

struct App{
  int init();
  void cleanup();
  int run();
  void tick(float dt);


  Renderer    renderer;
  ngl::Font   *font;
  FrameStatus frameStats;
};
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
int App::init(){
  /*
   * Init
   */
  {
    if( SDL_Init(SDL_INIT_VIDEO) ){
      return -1;
    }
    set_window_geometry(800, 600, 32);

    SDL_EnableUNICODE(0);
    SDL_WM_SetCaption("fonts", NULL);
  }
  {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
//     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//     glDisable(GL_TEXTURE_2D);
  }

  ngl::freetype::init();
  font=new ngl::Font("Inconsolata.otf", 11);
  font->set_position( ngl::int2(5., 600-font->face()->maxSize().y) );

  return ngl::EOk;
}
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
void App::cleanup(){
  /*
   * Cleanup
   */
  SDL_Quit();
  ngl::freetype::cleanup();
}
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
int App::run(){
  int ret=init();
  if( ret != ngl::EOk )
    return ret;
  init();
  
  Time_t      ts      =curr_time()-3000;
  bool        running =true;
  SDL_Event   event;
  while(running){
    while(SDL_PollEvent(&event)){
      if( event.type==SDL_QUIT )
        running =false;
      else if( event.type==SDL_VIDEORESIZE )
        set_window_geometry(event.resize.w, event.resize.h, 32);
      else if( event.type==SDL_KEYDOWN ){
        if( event.key.keysym.sym == SDLK_ESCAPE )
          running =false;
      }
    }
    tick( frame_time(ts) );
  }
  cleanup();
  return ngl::EOk;
}
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
void App::tick(float dt){
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -2.0f);

  glBindTexture(GL_TEXTURE_2D, font->face()->atlas()->textureID());
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);   glVertex2f(-0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f);   glVertex2f( 0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f);   glVertex2f( 0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f);   glVertex2f(-0.5f,  0.5f);
  glEnd();

  char buff[128]={0};
  snprintf(buff, 128,
            "FPS:     %d\n"
            //"verts/s: %d\ntris/s:  %d\n"
            "verts:   %d\ntris:    %d\n",
            frameStats.fps(),
            //frameStats.vps(),
            //frameStats.tps(),
            frameStats.verts(),
            frameStats.tris()
            );//45+numbers
//     font.print(buff, ngl::Color32::lightGreen);
  font->print("Testing, one, two, testing\n", ngl::Color32::yellow);
  font->print("Lorem ipsum sit dolor amet\n", ngl::Color32::lightGreen);
  font->print("Lorem ipsum sit dolor amet\n", ngl::Color32::lightRed);
  font->print("Hello, world!!!\n", ngl::Color32::lightBlue);
  // 93 + stats
  // 45+93+numbers=138+numbers
  font->print(buff, ngl::Color32::lightGreen);
  font->print(buff, ngl::Color32::lightGreen);
  font->print(buff, ngl::Color32::lightGreen);

  font->update_cache();
  frameStats.update(dt, font->vertex_count(), font->tri_count());

  renderer.vertex_array(*font);
//     renderer.legacy(font);

  glFlush();
  SDL_GL_SwapBuffers();
}




//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
int main(int argc, char **argv){

//   TypeAA    a;
//   TypeABB   b;

  
  App app;
  return app.run();
}
