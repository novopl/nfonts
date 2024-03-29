//==============================================================================
/**
\file            Types.hpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//==============================================================================
#if !defined(__FONTS_TYPES_HPP__)
#define __FONTS_TYPES_HPP__

#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <stdint.h>

#ifdef N_DEBUG_GL
#  define GL_DBG(FUNC)                    \
      FUNC;                               \
      if( err=gl_error_check(#FUNC) )     \
        return err;
#else
#  define GL_DBG(FUNC) FUNC;
#endif
//----------------------------------------------------------------------------//
/// Returns the offset of the given member variable.
///   \param[in]  CLASS class/struct in which the member resides.
///   \param[in]  MEMBER  the member for which the offset is returned.
//----------------------------------------------------------------------------//
#if !defined(offsetof)
  #define OFFSET(CLASS,MEMBER) ((ptrdiff_t)(&((CLASS *)0)->MEMBER))
#else
  #define OFFSET(CLASS,MEMBER) offsetof(CLASS,MEMBER)
#endif

namespace ngl{
  typedef uint32_t              TextureID;
  typedef uint32_t              Hash_t;
  typedef uint8_t               byte;
  typedef std::string           String;
  typedef int32_t               Error;
  typedef std::vector<String>   StringVector;
  typedef std::list<String>     StringList;
  typedef uint32_t              Hash_t;

  const size_t  kInvalidIndex     = 0xffffffff;
  const Error   EOk               = 0;
  const Error   ENotEnoughMemory  =-1;

  Hash_t gen_hash(const byte* data, size_t size, Hash_t initial=0);
  Hash_t gen_hash(const String &string, Hash_t initial=0);
  extern Error gl_error_check(const char *msg);


  namespace TextWrap{
    enum Mode{
      None,     // No wrapping.
      LineWrap, // Wrap lines, breaks words.
      WordWrap  // Wrap lines, keep the words intact.
    };
  }
  typedef TextWrap::Mode TextWrapMode;

  //============================================================================
  //{{{ Color32
  /** 32bit color.
   */
  //============================================================================
  struct Color32{
    union{
      struct{
        byte r,g,b,a;
      };
      uint32_t  value;
    };

    Color32()
    :value(0x00000000){
    }
    Color32(const Color32 &c)
    :value(c.value){
    }
    Color32(byte red, byte green, byte blue, byte alpha)
    :r(red), g(green), b(blue), a(alpha){
    }
    explicit Color32(uint32_t val)
    :value(val){
    }
    Color32& operator=(const Color32 &c){
      value=c.value;
      return *this;
    }

    static const Color32 black;
    static const Color32 white;
    static const Color32 grey;
    static const Color32 lightGrey;
    static const Color32 darkGrey;
    static const Color32 red;
    static const Color32 lightRed;
    static const Color32 darkRed;
    static const Color32 green;
    static const Color32 lightGreen;
    static const Color32 darkGreen;
    static const Color32 blue;
    static const Color32 lightBlue;
    static const Color32 darkBlue;
    static const Color32 yellow;
    static const Color32 orange;
  };//}}}


  //============================================================================
  //{{{ Vec2
  /** 2D vector.
   */
  //============================================================================
  template<typename T>
  struct Vec2{
    typedef Vec2<T>    This;

    union{
      // Aliases for readability.
      struct{ T   x, y;            };
      struct{ T   u, v;            };
      struct{ T   width, height;   };
      struct{ T   min, max;        };
      struct{ T   off, size;       };
    };

    inline Vec2();
    inline Vec2(T _x, T _y);
    inline Vec2(const This &obj);
    template<typename U>
    inline Vec2(const Vec2<U> &obj);

    inline This& set(T _x, T _y);

    This operator=(const This &obj){
      x=obj.x;
      y=obj.y;
      return *this;
    }
    template<typename U>
    This operator=(const Vec2<U> &obj){
      x=static_cast<T>(obj.x);
      y=static_cast<T>(obj.y);
      return *this;
    }

    static const This null;
  };
  template<typename T>  const Vec2<T> Vec2<T>::null(0, 0);
  //--------------------------------------------------------------------------//
  template<typename T>
  Vec2<T>::Vec2(){
  }
  //--------------------------------------------------------------------------//
  template<typename T>
  Vec2<T>::Vec2(T _x, T _y)
  :x(_x), y(_y){
  }
  //--------------------------------------------------------------------------//
  template<typename T>
  Vec2<T>::Vec2(const This &obj)
  :x(obj.x), y(obj.y){
  }
  //--------------------------------------------------------------------------//
  template<typename T>
  template<typename U>
  Vec2<T>::Vec2(const Vec2<U> &obj)
  :x( static_cast<T>(obj.x) ),
  y( static_cast<T>(obj.y) ){
  }
  //--------------------------------------------------------------------------//
  template<typename T>
  Vec2<T>& Vec2<T>::set(T _x, T _y){
    x=_x;
    y=_y;
    return *this;
  }

  //--------------------------------------------------------------------------//
  template<typename T>
  Vec2<T>& operator+=(Vec2<T> &ls, const Vec2<T> &rs){
    ls.x+=rs.x;
    ls.y+=rs.y;
    return ls;
  }
  //--------------------------------------------------------------------------//
  template<typename T>
  Vec2<T> operator+(const Vec2<T> &ls, const Vec2<T> &rs){
    return Vec2<T>(ls.x+rs.x, ls.y+rs.y);
  }
  //--------------------------------------------------------------------------//
  template<typename T>
  Vec2<T> operator+(const Vec2<T> &ls, const T &rs){
    return Vec2<T>(ls.x+rs, ls.y+rs);
  }
  //--------------------------------------------------------------------------//
  template<typename T>
  Vec2<T> operator-(const Vec2<T> &ls, const Vec2<T> &rs){
    return Vec2<T>(ls.x-rs.x, ls.y-rs.y);
  }
  //--------------------------------------------------------------------------//
  template<typename T>
  Vec2<T> operator-(const Vec2<T> &ls, const T &rs){
    return Vec2<T>(ls.x-rs, ls.y-rs);
  }
  //}}}



  //============================================================================
  //{{{ Vec3
  /** 3D vector.
   */
  //============================================================================
  template<typename T>
  struct Vec3{
    typedef Vec3<T>    This;

    union{
      // Aliases for readability.
      struct{ T x, y, z;               };
      struct{ T u, v, w;               };
      struct{ T width, height, depth;  };
      struct{ T a, b, c;               };
    };

    inline Vec3();
    inline Vec3(T _x, T _y, T _z);
    inline Vec3(const This &obj);

    inline This& set(T _x, T _y, T _z);

    This operator=(const This &obj){
      x=obj.x;
      y=obj.y;
      z=obj.z;
      return *this;
    }

    static const This null;
  };
  template<typename T>  const Vec3<T> Vec3<T>::null(0, 0, 0);
  //--------------------------------------------------------------------------//
  template<typename T>  Vec3<T>::Vec3(){ //{{{
  }
  //}}}-----------------------------------------------------------------------//
  template<typename T>  Vec3<T>::Vec3(T _x, T _y, T _z) //{{{
  :x(_x), y(_y), z(_z){
  }
  //}}}-----------------------------------------------------------------------//
  template<typename T>  Vec3<T>::Vec3(const This &obj) //{{{
  :x(obj.x), y(obj.y), z(obj.z){
  }
  //}}}-----------------------------------------------------------------------//
  template<typename T>  Vec3<T>& Vec3<T>::set(T _x, T _y, T _z){ //{{{
    x=_x;
    y=_y;
    z=_z;
    return *this;
  }//}}}
  //}}}



  typedef void*             MemAddr_t;
  typedef Vec2<MemAddr_t>   MemAddr2;     ///< Pair of memory addresses.
  typedef Vec2<size_t>      Chunk;
  typedef Vec2<float>       float2;
  typedef Vec3<uint16_t>    Triangle16;
  typedef Vec3<uint32_t>    Triangle32;
  typedef Vec2<int32_t>     int2;
  typedef Vec2<uint32_t>    uint2;
  typedef Vec2<size_t>      Size2;
  typedef Vec2<float>       TexCoords;



  class IGlyphAtlas;
  //============================================================================
  //{{{ Glyph
  /** Encapsulation of a single glyph, including metadata.
  */
  //============================================================================
  struct Glyph{
    char          code;     // character.
    TexCoords     botLeft;
    TexCoords     topRight;
    Size2         size;
    int2          off;
    float         advance;  // glyph x advance.
    IGlyphAtlas   *owner;

    bool operator!=(const Glyph &obj) const;
    bool operator==(const Glyph &obj) const;

    static const Glyph null;
  };//}}}

  //============================================================================
  //{{{ MemPool
  /** Memory pool (freelist).
   */
  //============================================================================
  class MemPool{
      MemPool(const MemPool &obj);
      MemPool& operator=(const MemPool &obj);
      struct MemBlock{
        size_t    prevFree;
        size_t    prev;
        size_t    next;
      };
    public:
      MemPool();
      virtual ~MemPool();

      void init(size_t size);
      void clear();
      void defragment(std::list<MemAddr2> &transfers);

      MemAddr_t alloc(size_t size);
      void free(MemAddr_t addr);

      const byte*   data()      const;
      size_t        size()      const;
      size_t        available() const;

    private:
      typedef std::list<Chunk>    FreeList;

      FreeList::iterator prev_free(MemAddr_t addr);

      byte      *m_data;
      size_t    m_size;
      size_t    m_available;
      FreeList  m_free;
  };//}}}


  //============================================================================
  //{{{ IGlyphAtlas
  /** Interface for custom texture atlas implementation.
   */
  //============================================================================
  class IGlyphAtlas{
    IGlyphAtlas(const IGlyphAtlas&)             = delete;
    IGlyphAtlas(const IGlyphAtlas&&)            = delete;
    IGlyphAtlas& operator=(const IGlyphAtlas&)  = delete;
    IGlyphAtlas& operator=(const IGlyphAtlas&&) = delete;
  public:
    IGlyphAtlas(){}
    virtual ~IGlyphAtlas(){}

    virtual TextureID texid() const = 0;
    virtual Error add(Glyph &out, const byte *data, const Size2 &size)=0;
  };//}}}


  namespace freetype{
    extern bool init();
    extern void cleanup();
  }
}
#endif/* __FONTS_TYPES_HPP__ */
