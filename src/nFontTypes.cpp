//==============================================================================
/**
\file            Types.cpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//==============================================================================
#include "nFontTypes.hpp"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdio>

namespace ngl{
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  Hash_t gen_hash(const byte* data, size_t size, Hash_t initial){
    if( !data || size==0 )
      return initial;

    Hash_t hash=initial;
    for(int i=0; i < size; ++i)
      hash = data[i] + (hash << 6) + (hash << 16) - hash;

    return hash;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  Hash_t gen_hash(const String &string, Hash_t initial){
    if( string.empty() )
      return initial;

    Hash_t hash=initial;
    const char *str=const_cast<char*>(string.c_str());

    while( *str != 0 )
      hash = *str++ + (hash << 6) + (hash << 16) - hash;

    return hash;
  }
  const Color32  Color32::black     (0xff000000);
  const Color32  Color32::white     (0xffffffff);
  const Color32  Color32::grey      (0xff808080);
  const Color32  Color32::lightGrey (0xffc0c0c0);
  const Color32  Color32::darkGrey  (0xff404040);
  const Color32  Color32::red       (0xff0000ff);
  const Color32  Color32::lightRed  (0xff6717ff);
  const Color32  Color32::darkRed   (0xff4040c0);
  const Color32  Color32::green     (0xff00ff00);
  const Color32  Color32::lightGreen(0xff60ffa0);
  const Color32  Color32::darkGreen (0xff008000);
  const Color32  Color32::blue      (0xffff0000);
  const Color32  Color32::lightBlue (0xffffa060);
  const Color32  Color32::darkBlue  (0xff800000);
  const Color32  Color32::yellow    (0xff00ffff);
  const Color32  Color32::orange    (0xff3a96c4);


  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  Error gl_error_check(const char *msg){
    Error err;
    if( (err=glGetError()) != GL_NO_ERROR ){
      std::fprintf(stderr, "%s: %s.", msg, gluErrorString(err));
      return err;
    }
    return EOk;
  }

  const Glyph Glyph::null={
    0x00,
    TexCoords( 0.0f, 0.0f ),
    TexCoords( 0.0f, 0.0f ),
    Size2( 0, 0 ),
    int2( 0, 0 ),
    0.0f,
    NULL
  };
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  bool Glyph::operator!=(const Glyph &obj) const{
    return !( code        == obj.code        ||
              owner       == obj.owner       ||
              size.width  == obj.size.width
            );
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  bool Glyph::operator==(const Glyph &obj) const{
    return code       == obj.code   &&
           owner      == obj.owner  &&
           size.width == obj.size.width;
  }




  struct MemHeader{
    uint32_t    magic;
    size_t      prevFree;
    size_t      nextFree;
    
  };
  //--------------------------------------------------------------------------//
  /// \brief  Copy constructor.
  ///   \param[in]  obj   MemPool object to copy from.
  //--------------------------------------------------------------------------//
  MemPool::MemPool(const MemPool &obj){
  }
  //--------------------------------------------------------------------------//
  /// \brief  Assign operator
  ///    \param[in] obj   MemPool object to assign to this.
  /// \returns
  /// Reference to itself.
  //--------------------------------------------------------------------------//
  MemPool &MemPool::operator=(const MemPool &obj){
    return *this;
  }

  //--------------------------------------------------------------------------//
  /// \brief  Default constructor.
  //--------------------------------------------------------------------------//
  MemPool::MemPool()
  :m_data(NULL),
  m_size(0),
  m_available(0){
  }
  //--------------------------------------------------------------------------//
  /// \brief  Destructor.
  //--------------------------------------------------------------------------//
  MemPool::~MemPool(){
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void MemPool::init(size_t size){
    m_data=new byte[m_size=m_available=size];
    m_free.push_back( Chunk(0, m_size) );
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void MemPool::clear(){
    if(m_data){
      delete[] m_data;
      m_data      =NULL;
      m_size      =0;
      m_available =0;
      m_free.clear();
    }
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void MemPool::defragment(std::list<MemAddr2> &transfers){
    transfers.clear();
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  MemAddr_t MemPool::alloc(size_t size){
    if( !m_data ){
      fprintf(stderr, "alloc(): Memory pool not initialized.\n");
      return NULL;
    }
    
    FreeList::iterator mem=m_free.begin();
    for( ; (mem != m_free.end()) && (mem->size < size); ++mem){
    }

    if( mem == m_free.end() ){
      fprintf(stderr, "alloc(): No free space left.\n");
      return NULL;
    }
    
    // found free.
    MemAddr_t ret=m_data+mem->off;
    if( mem->size > size ){
      mem->size -= size;
      mem->off  += size;
    }
    else{
      m_free.erase(mem);
    }
    return ret;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void MemPool::free(MemAddr_t addr){
    if( !m_data ){
      fprintf(stderr, "alloc(): Memory pool not initialized.\n");
      return;
    }

    FreeList::iterator prev=prev_free(addr);
    if( m_data+prev->off == addr ){
      // The previous free touches the current block - merge.
      
    }
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  const byte* MemPool::data() const{
    return m_data;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  size_t MemPool::size() const{
    return m_size;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  size_t MemPool::available() const{
    return 0;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  MemPool::FreeList::iterator MemPool::prev_free(MemAddr_t addr){
    for(FreeList::iterator it=++m_free.begin(); it!=m_free.end(); ++it){
      if( addr < &m_data[it->off] )
        return --it;
    }
    return m_free.end();
  }

  
}
