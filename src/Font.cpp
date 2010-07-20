//==============================================================================
/**
\file            Font.cpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos.
*/
//==============================================================================
#include <Font.hpp>
#include <FontFace.hpp>

#include <cstdio>
#include <GL/gl.h>
#include <GL/glu.h>

namespace ngl{
  //--------------------------------------------------------------------------//
  /// \brief  Copy constructor.
  ///   \param[in]  obj   Font object to copy from.
  //--------------------------------------------------------------------------//
  Font::Font(const Font &obj){
  }
  //--------------------------------------------------------------------------//
  /// \brief  Assign operator
  ///    \param[in] obj   Font object to assign to this.
  /// \returns
  /// Reference to itself.
  //--------------------------------------------------------------------------//
  Font &Font::operator=(const Font &obj){
    return *this;
  }
  
  //--------------------------------------------------------------------------//
  /// \brief  Default constructor.
  //--------------------------------------------------------------------------//
  Font::Font(const String &face, size_t sizeInPt)
  :m_face(NULL),
  m_counter(0),
  m_vertCount(0),
  m_cacheUpdated(false),
  m_cacheTTL(1){
    m_face=new FontFace(face, sizeInPt);
  }
  //--------------------------------------------------------------------------//
  /// \brief  Destructor.
  //--------------------------------------------------------------------------//
  Font::~Font(){
    if(m_face)
      delete m_face;
    for(Cache::const_iterator it=m_cache.begin(); it!=m_cache.end();++it){
      delete[] it->verts;
    }
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  size_t Font::vertex_count() const{
    return (m_cacheUpdated ? m_vertCount : kInvalidIndex);
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  size_t Font::tri_count() const{
    return (m_cacheUpdated ? (m_vertCount >> 1) : kInvalidIndex);
    //                          vertCount / 2
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void Font::set_position(const int2 &position){
    m_requestedPosition=m_position=position;
  }
  //--------------------------------------------------------------------------//
  /// \remarks
  ///   Invalidates geometry returned by get_geometry.
  //--------------------------------------------------------------------------//
  void Font::print(const String &msg, const Color32 &color){
    if( msg.empty() )
      return;

    // Check cache.
    CacheEntry *cached=find_cached(msg);
    if( cached ){
      cached->lastUsed=m_counter;
      m_position+=cached->positionDelta;
      return;
    }

    CacheEntry *ce=cache(msg);

    int2 position=m_position;
    const char *str=msg.c_str();
    for(int i=0; i<msg.length(); ++i){
      if( msg[i] == '\n' ){
        position.x   =m_position.x;
        position.y  -=m_face->maxSize().y;
        continue;
      }

      const Glyph &glyph  =m_face->get_glyph(msg[i]);
      if( glyph == Glyph::null ){
        fprintf(stderr, "Failed to load glyph '%c'.\n", msg[i]);
        continue;
      }


      //generate(ce->verts+(i*4), i*4, glyph, position, color);
      ce->verts[i*4+0].position.set(glyph.off.x+position.x,
                                    glyph.off.y+position.y);
      ce->verts[i*4+0].texCoord=glyph.botLeft;
      ce->verts[i*4+0].color=color;

      ce->verts[i*4+1].position.set(glyph.off.x+position.x+glyph.size.width,
                                    glyph.off.y+position.y);
      ce->verts[i*4+1].texCoord.set(glyph.topRight.u, glyph.botLeft.v);
      ce->verts[i*4+1].color=color;

      ce->verts[i*4+2].position.set(glyph.off.x+position.x+glyph.size.width,
                                    glyph.off.y+position.y+glyph.size.height);
      ce->verts[i*4+2].texCoord=glyph.topRight;
      ce->verts[i*4+2].color=color;

      ce->verts[i*4+3].position.set(glyph.off.x+position.x,
                                    glyph.off.y+position.y+glyph.size.height);
      ce->verts[i*4+3].texCoord.set(glyph.botLeft.u, glyph.topRight.v);
      ce->verts[i*4+3].color=color;

      ce->tris[i*2+0].set(i*4+0, i*4+1, i*4+3);
      ce->tris[i*2+1].set(i*4+3, i*4+1, i*4+2);
      
      position.x+=glyph.advance; 
    }
    ce->positionDelta =position - m_position;
    m_position        =position;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void Font::get_geometry(Vertex *vb, Triangle16 *ib, TextureID &texID) const{
    // Copy vertices.
    size_t vOff =0;
    size_t tOff =0;
    for(Cache::const_iterator it=m_cache.begin(); it!=m_cache.end(); ++it){
      //if( it->lastUsed == m_counter - 1 ){
        memcpy( &vb[vOff], it->verts, it->vertCount*sizeof(Vertex) );
//         memcpy( &ib[tOff], it->tris, (it->vertCount/2)*sizeof(Vertex) );
        // Generate triangles.
        for(int i=vOff; i < vOff+it->vertCount; i+=4, tOff+=2){
          ib[tOff+0].set(i+0, i+1, i+3);
          ib[tOff+1].set(i+3, i+1, i+2);
        }
        vOff +=it->vertCount;
        tOff +=it->vertCount/2;
      //}
    }
    
    texID=m_face->atlas()->textureID();
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void Font::update_cache(){
    Cache::iterator tmp;
    m_vertCount=0;
    ++m_counter;
//     printf("-- Cache update: %d\n", m_counter);
    for(Cache::iterator it=m_cache.begin(); it!=m_cache.end();){
//       if( m_cacheTTL && it->lastUsed >= m_counter - m_cacheTTL ){
      if( m_cacheTTL && it->lastUsed == m_counter-1 ){
        m_vertCount +=it->vertCount;
        ++it;
      }
      else{
//         printf( "-- [%05d]Dropping cache[%#x] %d\n", m_counter, it->hash, it->lastUsed );
        delete[] it->verts;
        delete[] it->tris;
        tmp=it;
        ++it;
        m_cache.erase(tmp);
      }
    }
    m_cacheUpdated=true;
    m_position=m_requestedPosition;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  Font::CacheEntry* Font::cache(const String &msg){
    m_cache.push_back( CacheEntry() );
    CacheEntry &ce=m_cache.back();
    ce.hash         =gen_hash( (const byte*)&m_position, sizeof(m_position) );
    ce.hash         =gen_hash(msg, ce.hash);
    ce.lastUsed     =m_counter;
    ce.vertCount    =msg.length()*4;
    ce.verts        =new Vertex[ce.vertCount];
    ce.tris         =new Triangle16[ce.vertCount/2];
    m_cacheUpdated  =false;
//     printf( "-- [%05d]Caching msg[%#x]:\n%s\n", m_counter, ce.hash, msg.c_str() );
    return &ce;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  Font::CacheEntry* Font::find_cached(const String &msg){
    Hash_t hash =gen_hash( (const byte*)&m_position, sizeof(m_position) );
    hash        =gen_hash(msg, hash);

    for(Cache::iterator it=m_cache.begin(); it!=m_cache.end();++it){
      if( it->hash == hash )
        return &*it;
    }
    return NULL;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void Font::generate(Vertex *vb, size_t vOff,
                      const Glyph &glyph,
                      const int2 &pos,
                      const Color32 &color){
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void Font::generate(const Glyph &glyph, const int2 &pos, const Color32 &color){
//     Font::Vertex v;
//     size_t       off=m_verts.size();
//     
//     v.position.set(pos.x,                   glyph.off.y+pos.y);
//     v.texCoord=glyph.botLeft;
//     v.color=color;
//     m_verts.push_back(v);
// 
//     v.position.set(pos.x+glyph.size.width,  glyph.off.y+pos.y);
//     v.texCoord.set(glyph.topRight.u, glyph.botLeft.v);
//     v.color=color;
//     m_verts.push_back(v);
// 
//     v.position.set(pos.x+glyph.size.width,  glyph.off.y+pos.y+glyph.size.height);
//     v.texCoord=glyph.topRight;
//     v.color=color;
//     m_verts.push_back(v);
// 
//     v.position.set(pos.x,                   glyph.off.y+pos.y+glyph.size.height);
//     v.texCoord.set(glyph.botLeft.u, glyph.topRight.v);
//     v.color=color;
//     m_verts.push_back(v);
// 
//     m_tris.push_back( Triangle16(off+0, off+1, off+3) );
//     m_tris.push_back( Triangle16(off+3, off+1, off+2) );
  }
}
