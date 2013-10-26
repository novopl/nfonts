//======================================================================
/**
\file            FontFace.cpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//======================================================================
#include "nFontFace.hpp"
#include "nGLGlyphAtlas.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_GLYPH_H
#include FT_TRIGONOMETRY_H

namespace ngl{
  namespace freetype{
    const FT_Library& handle();
  }

  struct FontFace::Pimpl{
    FT_Face       ftFace;
    Glyphs        glyphs;
  };



  //--------------------------------------------------------------------------//
  //{{{ FontFace(const String &face, size_t size)
  /// \brief  Default constructor.
  //--------------------------------------------------------------------------//
  FontFace::FontFace(const String &face, size_t size)
  :d(new Pimpl){

    d->ftFace = nullptr;
    m_atlas=new GLGlyphAtlas(128,128);
    load(face, size);
  }
  //}}}-----------------------------------------------------------------------//
  //{{{ FontFace(const FontFace &obj)
  /// \brief  Copy constructor.
  ///   \param[in]  obj   FontFace to copy from.
  //--------------------------------------------------------------------------//
  FontFace::FontFace(const FontFace &obj){
  }
  //}}}-----------------------------------------------------------------------//
  //{{{ ~FontFace()
  FontFace::~FontFace(){
    FT_Done_Face(d->ftFace);
    delete d;
    delete m_atlas;
  }
  //}}}-----------------------------------------------------------------------//
  //{{{ FontFace &operator=(const FontFace &obj)
  /// \brief  Assign operator
  ///    \param[in] obj   FontFace to assign to this.
  /// \returns
  /// Reference to itself.
  //--------------------------------------------------------------------------//
  FontFace &FontFace::operator=(const FontFace &obj){
    return *this;
  }
  //}}}-----------------------------------------------------------------------//
  const String& FontFace::name() const{ //{{{
    return m_name;
  }
  //}}}-----------------------------------------------------------------------//
  size_t FontFace::size() const{ //{{{
    return m_size;
  }
  //}}}-----------------------------------------------------------------------//
  const uint2& FontFace::maxSize() const{ //{{{
    return m_maxSize;
  }
  //}}}-----------------------------------------------------------------------//
  size_t FontFace::text_width(const String &text){ //{{{
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
  //}}}-----------------------------------------------------------------------//
  //{{{ size_t split( text, width,lines, wrapMode )
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
  //}}}-----------------------------------------------------------------------//
  const Glyph& FontFace::get_glyph(char code){ //{{{
    if( !d->ftFace )
      return Glyph::null;

    bool isTab = false;
    if( code == '\t' ){
      code  =' ';
      isTab =true;
    }

    // Check if the glyph is already loaded.
    for(Glyphs::iterator it=d->glyphs.begin(); it!=d->glyphs.end(); ++it){
      if(it->code==code){
        return *it;
      }
    }

    //  Glyph not found, load it.
    if( FT_Load_Char( d->ftFace, code,
                      FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT) ){
      fprintf(stderr, "FT_Load_Char failed.\n");
      return Glyph::null;
    }
    // shortcut
    FT_Bitmap &bitmap = d->ftFace->glyph->bitmap;

    byte *pixels = new byte[bitmap.width * bitmap.rows];
    for(int y = 0; y < bitmap.rows; ++y){
      memcpy( pixels + y * bitmap.width,
              bitmap.buffer + (bitmap.rows - y - 1)* bitmap.pitch,
              bitmap.width);
    }

    Glyph glyph;
    glyph.size.set(bitmap.width, bitmap.rows);
    glyph.code    = (isTab ? '\t' : code );
    glyph.advance = (d->ftFace->glyph->advance.x >> 6);
    glyph.off.x   = 0;
    glyph.off.x   = ( d->ftFace->glyph->metrics.horiBearingX >> 6 )
                    - ( d->ftFace->glyph->metrics.width >> 6 );
    glyph.off.y   = ( d->ftFace->glyph->metrics.horiBearingY >> 6 )
                    - ( d->ftFace->glyph->metrics.height >> 6 );
    m_atlas->add(glyph, pixels, glyph.size);
    d->glyphs.push_back(glyph);

    delete[] pixels;

    return d->glyphs.back();
  }
  //}}}-----------------------------------------------------------------------//
  //{{{ void load(const String &face, size_t size)
  /// Load the given font face.
  ///   \param[in]  face    Font face name.
  ///   \param[in]  size    Size in pt.
  //--------------------------------------------------------------------------//
  void FontFace::load(const String &face, size_t size){
    m_name=face;
    m_size=size;

    const FT_Library &handle = freetype::handle();
    if( FT_New_Face(handle, face.c_str(), 0, &d->ftFace) ){
      fprintf(stderr, "Failed to load font face.\n");
      return;
    }

    if( FT_Set_Char_Size(d->ftFace,
                         static_cast<uint32_t>(m_size)<<6,
                         static_cast<uint32_t>(m_size)<<6,
                         96, 96) ){
      fprintf(stderr, "Failed to set font size.\n");
      return;
    }

    // TODO: try changing to float.
    float k     = (float)d->ftFace->size->metrics.x_ppem
                  / d->ftFace->units_per_EM;
    m_maxSize.x = (d->ftFace->bbox.xMax - d->ftFace->bbox.xMin) * k;
    m_maxSize.y = (d->ftFace->bbox.yMax - d->ftFace->bbox.yMin) * k;
    m_maxSize.y*= 0.9f;
  }
  //}}}




  namespace freetype{
    static FT_Library   g_library;
    static bool         g_initialized=false;
    //------------------------------------------------------------------------//
    bool init(){ //{{{
      if( g_initialized )
        fprintf(stderr, "FreeType library already initialized\n");
      else if( FT_Init_FreeType(&g_library) )
        fprintf(stderr, "Failed to initialize freetype library.\n");
      else
        g_initialized=true;

      return g_initialized;
    }
    //}}}---------------------------------------------------------------------//
    void cleanup(){ //{{{
      if( !g_initialized )
        fprintf(stderr, "FreeType library not initialized\n");
      else
        FT_Done_FreeType(g_library);
    }
    //}}}---------------------------------------------------------------------//
    bool initialized(){ //{{{
      return g_initialized;
    }
    //}}}---------------------------------------------------------------------//
    const FT_Library& handle(){ //{{{
      if( !g_initialized )
        fprintf(stderr, "FreeType library not initialized\n");

      return g_library;
    }
    //}}}
  }

}
