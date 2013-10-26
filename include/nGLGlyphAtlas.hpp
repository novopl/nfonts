#ifndef __NOVO_GLGLYPHATLAS_HPP__
#define __NOVO_GLGLYPHATLAS_HPP__
#include "nFontTypes.hpp"

namespace ngl{
  //============================================================================
  //{{{ GLGLGlyphAtlas
  /** Glyph Atlas 
  */
  //============================================================================
  class GLGlyphAtlas: public IGlyphAtlas{
      GLGlyphAtlas(const GLGlyphAtlas &obj);
      GLGlyphAtlas& operator=(const GLGlyphAtlas &obj);
    public:
      GLGlyphAtlas(size_t width, size_t height);
      virtual ~GLGlyphAtlas();

      Error add(Glyph &out, const byte *data, const Size2 &size);

      TextureID texid() const   { return m_texture; }
    private:
      Error init_atlas(size_t width, size_t height);

      TextureID     m_texture;
      Size2         m_size;
      uint2         m_freeOff;
      size_t        m_currRowHeight;
  };//}}}
}

#endif/* __NOVO_GLGLYPHATLAS_HPP__ */
