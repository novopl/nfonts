//======================================================================
/**
\file            FontFace.hpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//======================================================================
#if !defined(__FONTS_FONTFACE_HPP__)
#define __FONTS_FONTFACE_HPP__

#include "nFontTypes.hpp"

namespace ngl{
  typedef std::vector<Glyph> Glyphs;


  //===========================================================================
  //{{{ FontFace
  /** \brief  Font face.
  */
  //============================================================================
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

      const IGlyphAtlas  *atlas() const  { return m_atlas;   }
    private:
      void load(const String &face, size_t size);
      struct Pimpl;


      Pimpl         *d;
      String        m_name;
      size_t        m_size;
      uint2         m_maxSize;
      IGlyphAtlas    *m_atlas;
  };
  //}}}

  namespace freetype{
    extern bool init();
    extern void cleanup();
    extern bool initialized();
    //extern const FT_Library& handle();
  }
}
#endif/* __FONTS_FONTFACE_HPP__ */
