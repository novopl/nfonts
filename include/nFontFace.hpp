//==============================================================================
/**
\file            FontFace.hpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//==============================================================================
#if !defined(__FONTS_FONTFACE_HPP__)
#define __FONTS_FONTFACE_HPP__

#include "nFontTypes.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_GLYPH_H
#include FT_TRIGONOMETRY_H

namespace ngl{
  typedef std::vector<Glyph> Glyphs;

//==============================================================================
/** \class GlyphAtlas
\brief  Glyph atlas.
*/
//==============================================================================
  class GlyphAtlas{
      GlyphAtlas(const GlyphAtlas &obj);
      GlyphAtlas& operator=(const GlyphAtlas &obj);
    public:
      GlyphAtlas(size_t width, size_t height);
      virtual ~GlyphAtlas();


      Error add_glyph(Glyph &out, const byte *rgbData, const Size2 &size);

      TextureID textureID() const   { return m_texture; }
    private:
      Error init_atlas(size_t width, size_t height);

      TextureID     m_texture;
      Size2         m_size;
      uint2         m_freeOff;
      size_t        m_currRowHeight;
  };

//==============================================================================
/** \brief  Font face.
*/
//==============================================================================
  class FontFace{
      FontFace(const FontFace &obj);
      FontFace& operator=(const FontFace &obj);
    public:
      FontFace(const String &face, size_t sizeInPt);
      virtual ~FontFace();


      const String  &name()     const;
      size_t        size()      const;
      const uint2   &maxSize()  const;

      size_t        text_width(const String &text);
      size_t        split(const String &text,
                          size_t width,
                          StringList &lines,
                          TextWrapMode wrapMode=TextWrap::LineWrap);
      const Glyph   &get_glyph(char code);

      const GlyphAtlas  *atlas() const  { return m_atlas;   }
    private:
      void load(const String &face, size_t size);


      String        m_name;
      size_t        m_size;
      uint2         m_maxSize;
      GlyphAtlas    *m_atlas;
      FT_Face       m_ftFace;
      Glyphs        m_glyphs;
  };
  
  namespace freetype{
    extern bool init();
    extern void cleanup();
    extern bool initialized();
    extern const FT_Library& handle();
  }
}
#endif/* __FONTS_FONTFACE_HPP__ */
