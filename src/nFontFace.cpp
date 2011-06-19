//======================================================================
/**
\file            FontFace.cpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//======================================================================
#include "nFontFace.hpp"

#include <GL/gl.h>
#include <GL/glu.h>

namespace ngl{
  //------------------------------------------------------------------//
  /// \brief  Default constructor.
  GlyphAtlas::GlyphAtlas(size_t width, size_t height)
  :m_texture(0),
  m_size(Size2::null),
  m_freeOff(Size2::null),
  m_currRowHeight(0){
    init_atlas(width, height);
  }
  //------------------------------------------------------------------//
  /// \brief  Copy constructor.
  ///   \param[in]  obj   GlyphAtlas to copy from.
  GlyphAtlas::GlyphAtlas(const GlyphAtlas &obj){
  }
  //------------------------------------------------------------------//
  /// \brief  Destructor.
  GlyphAtlas::~GlyphAtlas(){
    if(m_texture){
      glDeleteTextures(1, &m_texture);
      m_texture=0;
    }
  }
  //------------------------------------------------------------------//
  /// \brief  Assign operator
  ///    \param[in] obj   GlyphAtlas to assign to this.
  /// \returns
  /// Reference to itself.
  GlyphAtlas &GlyphAtlas::operator=(const GlyphAtlas &obj){
    return *this;
  }
  //------------------------------------------------------------------//
  Error GlyphAtlas::add_glyph(Glyph &out, const byte *rgbData, 
                              const Size2 &size){
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
  //------------------------------------------------------------------//
  Error GlyphAtlas::init_atlas(size_t width, size_t height){
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









  //------------------------------------------------------------------//
  /// \brief  Default constructor.
  FontFace::FontFace(const String &face, size_t size)
  :m_ftFace(0){
    load(face, size);
  }
  //------------------------------------------------------------------//
  /// \brief  Copy constructor.
  ///   \param[in]  obj   FontFace to copy from.
  FontFace::FontFace(const FontFace &obj){
  }
  //------------------------------------------------------------------//
  /// \brief  Destructor.
  FontFace::~FontFace(){
    FT_Done_Face(m_ftFace);
    delete m_atlas;
  }
  //------------------------------------------------------------------//
  /// \brief  Assign operator
  ///    \param[in] obj   FontFace to assign to this.
  /// \returns
  /// Reference to itself.
  FontFace &FontFace::operator=(const FontFace &obj){
    return *this;
  }
  //------------------------------------------------------------------//
  const String& FontFace::name() const{
    return m_name;
  }
  //------------------------------------------------------------------//
  size_t FontFace::size() const{
    return m_size;
  }
  //------------------------------------------------------------------//
  const uint2& FontFace::maxSize() const{
    return m_maxSize;
  }
  //------------------------------------------------------------------//
  size_t FontFace::text_width(const String &text){
    size_t      maxWidth=0;
    size_t      width=0;

    for(String::size_type i=0; i < text.length(); ++i){
      if(text[i]=='\n'){
        if(width > maxWidth)
          maxWidth=width;
        width=0;
        continue;
      }
      const Glyph &glyph =get_glyph(text[i]);
      width+=glyph.advance;
    }
    if(width > maxWidth)
      maxWidth=width;

    return maxWidth;
  }
  //------------------------------------------------------------------//
  size_t FontFace::split(const String &text,
                         size_t width,
                         StringList &lines,
                         TextWrapMode wrapMode){
    size_t            off=0;
    String::size_type lineStart=0;
    String::size_type lastSpace=0;
    size_t            lastWordWidth=0;

    for(String::size_type i=0; i<text.length(); ++i){
      if( text[i]=='\n' ){
        if( i==lineStart )
          lines.push_back(String("\n"));
        else
          lines.push_back(text.substr(lineStart, i-lineStart+1));

        lineStart=i+1;
        off=0;
        continue;
      }

      const Glyph &glyph =get_glyph(text[i]);
      if( wrapMode==TextWrap::LineWrap ){
        if( off + glyph.advance > width ){
          lines.push_back(text.substr(lineStart, i-lineStart)+'\n');
          lineStart=i;
          off=0;
        }
      }
      else if( wrapMode==TextWrap::WordWrap ){
        if( text[i]==' ' || text[i]=='\t' ){
          lastSpace=i;
          lastWordWidth=0;
        }
        if(off + glyph.advance > width){
          if( lastSpace > lineStart ){
            lines.push_back( text.substr( lineStart, 
                                          lastSpace-lineStart)+'\n' );
            lineStart=lastSpace+1;
            off=lastWordWidth;
            lastWordWidth=0;
          }
          else{
            lines.push_back(text.substr(lineStart, i-lineStart)+'\n');
            lineStart=i;
            off=0;
          }
        }
        lastWordWidth+=glyph.advance;
      }
      off+=glyph.advance;
    }
    if( off > 0 )
      lines.push_back(text.substr(lineStart)+ '\n');

    return off;
  }
  //------------------------------------------------------------------//
  const Glyph& FontFace::get_glyph(char code){
    if( !m_ftFace )
      return Glyph::null;
    
    bool isTab = false;
    if( code == '\t' ){
      code  =' ';
      isTab =true;
    }

    // Check if the glyph is already loaded.
    for(Glyphs::iterator it=m_glyphs.begin(); it!=m_glyphs.end(); ++it){
      if(it->code==code){
        return *it;
      }
    }

    //  Glyph not found, load it.
    if( FT_Load_Char( m_ftFace, code, 
                      FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT) ){
      fprintf(stderr, "FT_Load_Char failed.\n");
      return Glyph::null;
    }
    // shortcut
    FT_Bitmap &bitmap = m_ftFace->glyph->bitmap;

    byte *pixels = new byte[bitmap.width * bitmap.rows];
    for(int y = 0; y < bitmap.rows; ++y){
      memcpy( pixels + y * bitmap.width, 
              bitmap.buffer + (bitmap.rows - y - 1)* bitmap.pitch, 
              bitmap.width);
    }

    Glyph glyph;
    glyph.size.set(bitmap.width, bitmap.rows);
    glyph.code    = (isTab ? '\t' : code );
    glyph.advance = (m_ftFace->glyph->advance.x >> 6);
    glyph.off.x   = 0;
    glyph.off.x   = ( m_ftFace->glyph->metrics.horiBearingX >> 6 ) 
                    - ( m_ftFace->glyph->metrics.width >> 6 );
    glyph.off.y   = ( m_ftFace->glyph->metrics.horiBearingY >> 6 ) 
                    - ( m_ftFace->glyph->metrics.height >> 6 );
    m_atlas->add_glyph(glyph, pixels, glyph.size);
    m_glyphs.push_back(glyph);

    delete[] pixels;

    return m_glyphs.back();
  }
  //------------------------------------------------------------------//
  /// Load the given font face.
  ///   \param[in]  face    Font face name.
  ///   \param[in]  size    Size in pt.
  void FontFace::load(const String &face, size_t size){
    m_name=face;
    m_size=size;
    m_atlas=new GlyphAtlas(128,128);

    const FT_Library &handle = freetype::handle();
    if( FT_New_Face(handle, face.c_str(), 0, &m_ftFace) ){
      fprintf(stderr, "Failed to load font face.\n");
      return;
    }

    if( FT_Set_Char_Size(m_ftFace,
                         static_cast<uint32_t>(m_size)<<6,
                         static_cast<uint32_t>(m_size)<<6,
                         96, 96) ){
      fprintf(stderr, "Failed to set font size.\n");
      return;
    }

    // TODO: try changing to float.
    float k     = (float)m_ftFace->size->metrics.x_ppem 
                  / m_ftFace->units_per_EM;
    m_maxSize.x = (m_ftFace->bbox.xMax - m_ftFace->bbox.xMin) * k;
    m_maxSize.y = (m_ftFace->bbox.yMax - m_ftFace->bbox.yMin) * k;
    m_maxSize.y*= 0.9f;
  }





  
  namespace freetype{
    static FT_Library   g_library;
    static bool         g_initialized=false;
    //----------------------------------------------------------------//
    bool init(){
      if( g_initialized )
        fprintf(stderr, "FreeType library already initialized\n");
      else if( FT_Init_FreeType(&g_library) )
        fprintf(stderr, "Failed to initialize freetype library.\n");
      else
        g_initialized=true;

      return g_initialized;
    }
    //----------------------------------------------------------------//
    void cleanup(){
      if( !g_initialized )
        fprintf(stderr, "FreeType library not initialized\n");
      else
        FT_Done_FreeType(g_library);
    }
    //----------------------------------------------------------------//
    bool initialized(){
      return g_initialized;
    }
    //----------------------------------------------------------------//
    const FT_Library& handle(){
      if( !g_initialized )
        fprintf(stderr, "FreeType library not initialized\n");

      return g_library;
    }
  }
  
}
