//==============================================================================
/**
\file            Font.hpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//==============================================================================
#if !defined(__FONTS_FONT_HPP__)
#define __FONTS_FONT_HPP__

#include "nFontTypes.hpp"
#include <vector>
#include <list>


namespace ngl{

  //==============================================================================
  /** \classHashTable
  \brief  Hash table implementation.
  */
  //==============================================================================
  template<typename Type>
  class HashTable{
      struct Slot{
        Hash_t  hash;
        Slot    *next;
        Slot    *prev;
        Type    data;
      };
    public:
      HashTable();
      HashTable(const HashTable &obj);
      virtual ~HashTable();

      HashTable& operator=(const HashTable &obj);

      void init(size_t capacity, size_t tableSize);

    private:
      
  };

  struct FontStdAllocPolicy{
    MemAddr_t alloc(size_t size);
    void free(MemAddr_t addr);
  };
  
  class FontFace;
  class FontCacheRenderer;
  class FontCacheBatchRenderer;
//==============================================================================
/** \class Font
\brief  Font class.
*/
//==============================================================================
  class Font{
      Font(const Font &obj)             = delete;
      Font& operator=(const Font &obj)  = delete;
    public:
      struct RenderRequest;
      struct Vertex;
      
      Font(const String &face, size_t sizeInPt);
      virtual ~Font();

      size_t  tri_count()                                                 const;
      size_t  vertex_count()                                              const;

      void init_position(const int screenHeight);
      void set_position(const int2 &position);
      void print(const String &msg, const Color32 &color=Color32::white);
      void cprint(const String &msg);
      void get_geometry(Vertex *vb, Triangle16 *ib, TextureID &texID)  const;
      void update_cache();

      FontFace *face() { return m_face; }
      
    private:
      struct CacheEntry;
      typedef std::vector<Vertex>       Vertices;
      typedef std::vector<Triangle16>   Triangles;
      typedef std::list<CacheEntry>     Cache;
      

      CacheEntry* cache(const String &msg);
      CacheEntry* find_cached(const String &msg);
      void generate(Vertex *verts, int index, const Glyph &glyph,
                    const int2 &position, Color32 color);
      
      FontFace    *m_face;
      int2        m_position;
      int2        m_requestedPosition;
      size_t      m_vertCount;
      uint32_t    m_counter;
      Cache       m_cache;
      bool        m_cacheUpdated;
      uint32_t    m_cacheTTL;

      friend class ngl::FontCacheRenderer;
      friend class ngl::FontCacheBatchRenderer;
  };
  //========================================================
  /** \class CacheEntry
  \brief  Font cache entry.
  */
  //========================================================
  struct Font::CacheEntry{
    Hash_t      hash;
    uint32_t    lastUsed;
    Vertex      *verts;
    int2        positionDelta;
    size_t      vertCount;
  };
  //========================================================
  /** \class Vertex
  \brief  Font vertex.
  */
  //========================================================
  struct Font::Vertex{
    int2      position;
    float2    texCoord;
    Color32   color;
  };

}
#endif/* __FONTS_FONT_HPP__ */
