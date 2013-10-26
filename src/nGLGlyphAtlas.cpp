#include <nGLGlyphAtlas.hpp>

#include <GL/gl.h>
#include <GL/glu.h>

namespace ngl{
  //--------------------------------------------------------------------------//
  // {{{ GLGlyphAtlas::GLGlyphAtlas(size_t width, size_t height)
  /// \brief  Default constructor.
  //--------------------------------------------------------------------------//
  GLGlyphAtlas::GLGlyphAtlas(size_t width, size_t height)
  :   m_texture       ( 0 ),
      m_size          ( Size2::null ),
      m_freeOff       ( Size2::null ),
      m_currRowHeight ( 0 )
  {
    init_atlas(width, height);
  }
  //}}}-----------------------------------------------------------------------//
  //{{{ GLGlyphAtlas(const GLGlyphAtlas &obj)
  /// Copy constructor.
  ///   \param[in]  obj   GLGlyphAtlas to copy from.
  ///
  //--------------------------------------------------------------------------//
  GLGlyphAtlas::GLGlyphAtlas(const GLGlyphAtlas &obj){
  }
  //}}}-----------------------------------------------------------------------//
  //{{{ ~GLGlyphAtlas()
  GLGlyphAtlas::~GLGlyphAtlas(){
    if(m_texture){
      glDeleteTextures(1, &m_texture);
      m_texture=0;
    }
  }
  //}}}-----------------------------------------------------------------------//
  //{{{ GLGlyphAtlas& operator=(const GLGlyphAtlas &obj)
  /// Assign operator
  ///    \param[in] obj   GLGlyphAtlas to assign to this.
  /// \returns
  /// Reference to itself.
  //--------------------------------------------------------------------------//
  GLGlyphAtlas &GLGlyphAtlas::operator=(const GLGlyphAtlas &obj){
    return *this;
  }
  //}}}-----------------------------------------------------------------------//
  //{{{ Error add(Glyph &out, const byte *rgbData, const Size2 &size)
  Error GLGlyphAtlas::add(Glyph &out, const byte *rgbData, const Size2 &size){
    // If the glyph is too wide, go to next row.
    if( m_freeOff.x + size.width > m_size.width ){
      m_freeOff.y +=m_currRowHeight;
      m_freeOff.x  =0;
    }

    if( m_freeOff.y+size.height > m_size.height ||
        m_freeOff.x+size.width  > m_size.width   )
      return ENotEnoughMemory;

    Error err;
    GL_DBG( glPushAttrib(GL_TEXTURE_BIT)                             );
    GL_DBG( glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT)            );
    GL_DBG( glBindTexture(GL_TEXTURE_2D, m_texture)                  );
    GL_DBG( glPixelStorei(GL_UNPACK_ALIGNMENT,    1)                 );
    GL_DBG( glPixelStorei(GL_UNPACK_SWAP_BYTES,   GL_FALSE)          );
    GL_DBG( glPixelStorei(GL_UNPACK_SKIP_ROWS,    GL_FALSE)          );
    GL_DBG( glPixelStorei(GL_UNPACK_SKIP_PIXELS,  GL_FALSE)          );
    GL_DBG( glPixelStorei(GL_UNPACK_ROW_LENGTH,   (GLint)size.width) );

    GL_DBG( glTexSubImage2D(GL_TEXTURE_2D,
                            0,
                            static_cast<GLint>(m_freeOff.x),
                            static_cast<GLint>(m_freeOff.y),
                            static_cast<GLsizei>(size.width),
                            static_cast<GLsizei>(size.height),
                            GL_ALPHA,
                            GL_UNSIGNED_BYTE,
                            rgbData)
          );

    GL_DBG( glPopClientAttrib()   );
    GL_DBG( glPopAttrib()         );
    out.owner       = this;
    out.botLeft.u   = (float)m_freeOff.x / (float)m_size.width;
    out.botLeft.v   = (float)m_freeOff.y / (float)m_size.height;
    out.topRight.u  = (m_freeOff.x+size.width)  / (float)m_size.width;
    out.topRight.v  = (m_freeOff.y+size.height) / (float)m_size.height;

    m_freeOff.x+=size.width;
    if(size.height > m_currRowHeight)
      m_currRowHeight=size.height;

    return EOk;
  }
  //}}}-----------------------------------------------------------------------//
  //{{{ Error init_atlas(size_t width, size_t height)
  Error GLGlyphAtlas::init_atlas(size_t width, size_t height){
    Error err;
    GL_DBG( glPushAttrib    ( GL_TEXTURE_BIT) );
    GL_DBG( glGenTextures   ( 1, &m_texture) );
    GL_DBG( glBindTexture   ( GL_TEXTURE_2D, m_texture) );

    GL_DBG( glTexParameteri ( GL_TEXTURE_2D,
                              GL_TEXTURE_MIN_FILTER,
                              GL_NEAREST) );

    GL_DBG( glTexParameteri ( GL_TEXTURE_2D,
                              GL_TEXTURE_MAG_FILTER,
                              GL_NEAREST) );

    GL_DBG( glTexImage2D    ( GL_TEXTURE_2D,
                              0,
                              GL_ALPHA,
                              width,
                              height,
                              0,
                              GL_ALPHA,
                              GL_UNSIGNED_BYTE,
                              0) );
    GL_DBG( glPopAttrib     () );

    m_size.set(width, height);
    return EOk;
  }
  //}}}
}
