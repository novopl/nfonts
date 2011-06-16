//==============================================================================
/**
\file            FontRenderers.cpp
\author          Mateusz 'novo' Klos
\date            July 20, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//==============================================================================
#include "nFontRenderers.hpp"
#include "nFont.hpp"
#include "nFontFace.hpp"


#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>


#if defined(N_WIN32_BUILD) || defined(N_WIN32_CONSOLE_BUILD)
#  define STDCALL __stdcall
#else
#  define STDCALL
#endif
/************************************************************************
* Buffers.                                                              *
************************************************************************/
// Targets.
#define GL_ARRAY_BUFFER       0x8892
#define GL_ELEMENT_ARRAY_BUFFER   0x8893
#define GL_PIXEL_PACK_BUFFER    0x88EB
#define GL_PIXEL_UNPACK_BUFFER    0x88EC


// glBufferData
#define GL_STREAM_DRAW        0x88E0
#define GL_STREAM_READ        0x88E1
#define GL_STREAM_COPY        0x88E2
#define GL_STATIC_DRAW        0x88E4
#define GL_STATIC_READ        0x88E5
#define GL_STATIC_COPY        0x88E6
#define GL_DYNAMIC_DRAW       0x88E8
#define GL_DYNAMIC_READ       0x88E9
#define GL_DYNAMIC_COPY       0x88EA

// glMapBuffer
#define GL_READ_ONLY        0x88B8
#define GL_WRITE_ONLY       0x88B9
#define GL_READ_WRITE       0x88BA

// glPrimitiveRestart
#define GL_PRIMITIVE_RESTART_NV   0x8558

typedef void      (STDCALL * PFNGLGENBUFFERSARBPROC)        (GLsizei n, GLuint *buffers);
typedef void      (STDCALL * PFNGLDELETEBUFFERSARBPROC)     (GLsizei n, const GLuint *buffers);
typedef void      (STDCALL * PFNGLBINDBUFFERARBPROC)        (GLenum target, GLuint buffer);
typedef void      (STDCALL * PFNGLBUFFERDATAARBPROC)        (GLenum target, GLint size, const GLvoid *data, GLenum usage);
typedef void*     (STDCALL * PFNGLMAPBUFFERARBPROC)         (GLenum target, GLenum access);
typedef GLboolean (STDCALL * PFNGLUNMAPBUFFERARBPROC)       (GLenum target);
typedef GLboolean (STDCALL * PFNGLISBUFFERARBPROC)          (GLuint buffer);

PFNGLGENBUFFERSARBPROC      glGenBuffers            =0;
PFNGLISBUFFERARBPROC        glIsBuffer              =0;
PFNGLDELETEBUFFERSARBPROC   glDeleteBuffers         =0;
PFNGLBINDBUFFERARBPROC      glBindBuffer            =0;
PFNGLBUFFERDATAARBPROC      glBufferData            =0;
PFNGLMAPBUFFERARBPROC       glMapBuffer             =0;
PFNGLUNMAPBUFFERARBPROC     glUnmapBuffer           =0;

namespace ngl{
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  AbstractRenderer::AbstractRenderer(){
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  AbstractRenderer::~AbstractRenderer(){
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  int AbstractRenderer::state_setup(){
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
  int AbstractRenderer::state_cleanup(){
    Error err;
    GL_DBG( glMatrixMode(GL_PROJECTION);      );
    GL_DBG( glPopMatrix();                    );
    GL_DBG( glPopAttrib();                    );
    GL_DBG( glPopClientAttrib();              );
    return EOk;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void AbstractRenderer::print_info(const Font &font){
    bool        printInfo       =false;
    const int   kPrintInterval  =500;
    static int  counter         =kPrintInterval-1;
    if( printInfo && !(++counter % kPrintInterval) ){
      printf("----------------------------\n");
      printf("verts: %d\ntris:  %d\n", font.vertex_count(), font.tri_count() );
    }
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void AbstractRenderer::print_vertex(const Font::Vertex &v){
    printf("(%d, %d)[%.3f, %.3f] ; ",
          v.position.x, v.position.y,
          v.texCoord.u, v.texCoord.v);
  }




    
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  LegacyRenderer::LegacyRenderer()
  :m_vertCount(0), m_vb(0), m_ib(0){
    printf("Using legacy renderer\n");
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  LegacyRenderer::~LegacyRenderer(){
    delete[] m_vb;
    delete[] m_ib;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  int LegacyRenderer::render(const Font &font){
    TextureID     texID;
    if( font.vertex_count() > m_vertCount )
      extend_buffers( font.vertex_count() );
    font.get_geometry(m_vb, m_ib, texID);

    const Font::Vertex *v;

    state_setup();
    glBindTexture (GL_TEXTURE_2D, texID);
    glBegin       (GL_TRIANGLES);
    for(size_t i=0;  i < font.tri_count(); ++i){
      v =&m_vb[ m_ib[i].a ];
      glColor4ubv ( (byte*)&v->color );
      glTexCoord2f( v->texCoord.u, v->texCoord.v );
      glVertex2i  ( v->position.x, v->position.y );

      v =&m_vb[ m_ib[i].b ];
      glColor4ubv ( (byte*)&v->color );
      glTexCoord2f( v->texCoord.u, v->texCoord.v );
      glVertex2i  ( v->position.x, v->position.y );

      v =&m_vb[ m_ib[i].c ];
      glColor4ubv ( (byte*)&v->color );
      glTexCoord2f( v->texCoord.u, v->texCoord.v );
      glVertex2i  ( v->position.x, v->position.y );
    }
    glEnd();
    glFlush();

    state_cleanup();

    return EOk;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  int LegacyRenderer::extend_buffers(uint32_t vertCount){
    if( m_vb )
      delete[] m_vb;
    if( m_ib )
      delete[] m_ib;

    m_vertCount=vertCount;
    m_vb =new Font::Vertex [m_vertCount];
    m_ib =new Triangle16   [m_vertCount/2];
  }




  
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  VARenderer::VARenderer(){
    printf("Using VA renderer\n");
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  VARenderer::~VARenderer(){
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  int VARenderer::render(const Font &font){
    TextureID     texID;
    Font::Vertex  *vb =new Font::Vertex [font.vertex_count()];
    Triangle16    *ib =new Triangle16   [font.tri_count()];
    font.get_geometry(vb, ib, texID);

    state_setup();
    GL_DBG( glBindTexture(GL_TEXTURE_2D, texID)                       );
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

    GL_DBG(
      glDrawElements(GL_TRIANGLES, font.tri_count()*3, GL_UNSIGNED_SHORT, ib)
    );
    glFlush();


    state_cleanup();

    delete[] vb;
    delete[] ib;
    return EOk;
  }




  
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  VBORenderer::VBORenderer()
  :m_vb(0), m_ib(0), m_vertCount(0){
    Error err;
    glGenBuffers(1, &m_vb);
    glGenBuffers(1, &m_ib);
    printf("Using VBO renderer\n");
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  VBORenderer::~VBORenderer(){
    glDeleteBuffers(1, &m_vb);
    glDeleteBuffers(1, &m_ib);
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  int VBORenderer::render(const Font &font){
    TextureID     texID;
    if( font.vertex_count() > m_vertCount )
      extend_buffers( font.vertex_count() );
    
    Font::Vertex  *vb=(Font::Vertex*) glMapBuffer(GL_ARRAY_BUFFER,
                                                  GL_WRITE_ONLY);
    Triangle16    *ib=(Triangle16*)   glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,
                                                  GL_WRITE_ONLY);
    font.get_geometry(vb, ib, texID);

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    state_setup();
    GL_DBG( glBindTexture(GL_TEXTURE_2D, texID)                       );
    GL_DBG( glEnableClientState(GL_VERTEX_ARRAY)                      );
    GL_DBG( glEnableClientState(GL_COLOR_ARRAY)                       );
    GL_DBG( glEnableClientState(GL_TEXTURE_COORD_ARRAY)               );

    GL_DBG( glVertexPointer(2, GL_INT,
                            sizeof(Font::Vertex),
                            OFFSET(Font::Vertex, position) ) );
    GL_DBG( glTexCoordPointer(2, GL_FLOAT,
                            sizeof(Font::Vertex),
                            (void*)OFFSET(Font::Vertex, texCoord) ) );
    GL_DBG( glColorPointer(4, GL_UNSIGNED_BYTE,
                            sizeof(Font::Vertex),
                            (void*)OFFSET(Font::Vertex, color) ) );

    GL_DBG(
      glDrawElements(GL_TRIANGLES, font.tri_count()*3, GL_UNSIGNED_SHORT, 0)
    );
    glFlush();
    state_cleanup();

    return EOk;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  int VBORenderer::extend_buffers(uint32_t vertCount){
    m_vertCount =vertCount;
    Error   err;
    GL_DBG( glBindBuffer(GL_ARRAY_BUFFER, m_vb)               );
    GL_DBG( glBufferData(GL_ARRAY_BUFFER,
                         m_vertCount*sizeof(Font::Vertex),
                         0,
                         GL_STREAM_DRAW)   );
    
    GL_DBG( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib)       );
    GL_DBG( glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         m_vertCount/2*sizeof(Triangle16),
                         0,
                         GL_STREAM_DRAW)   );
  }







  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  AbstractRenderer *create_renderer(RendererType type){
    switch(type){
      case Renderer::Legacy:          return new LegacyRenderer();
      case Renderer::VA:              return new VARenderer();
      case Renderer::VBO:             return new VBORenderer();
      default:                        return 0;
    }
  }



  
  //-----------------------------------------------------------------------------------------------//
  //
  //------------------------------------------------------------------//
  template<typename FunType>
  static FunType load_proc(const char *extName){
    #if defined(IONIC_WIN32_BUILD) || defined(IONIC_WIN32_CONSOLE_BUILD)
      return reinterpret_cast<FunType>( wglGetProcAddress(extName) );
    #else
      return reinterpret_cast<FunType>( 
          glXGetProcAddress( reinterpret_cast<const GLubyte*>(extName))
      );
    #endif
  }
  void init_extensions(){
    glGenBuffers    =load_proc<PFNGLGENBUFFERSARBPROC>   ("glGenBuffersARB");
    glIsBuffer      =load_proc<PFNGLISBUFFERARBPROC>     ("glIsBufferARB");
    glBindBuffer    =load_proc<PFNGLBINDBUFFERARBPROC>   ("glBindBufferARB");
    glBufferData    =load_proc<PFNGLBUFFERDATAARBPROC>   ("glBufferDataARB");
    glDeleteBuffers =load_proc<PFNGLDELETEBUFFERSARBPROC>("glDeleteBuffersARB");
    glMapBuffer     =load_proc<PFNGLMAPBUFFERARBPROC>    ("glMapBufferARB");
    glUnmapBuffer   =load_proc<PFNGLUNMAPBUFFERARBPROC>  ("glUnmapBufferARB");
  }
}
