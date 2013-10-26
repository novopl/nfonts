//==============================================================================
/**
\file            Font.cpp
\author          Mateusz 'novo' Klos
\date            April 14, 2010



Copyright (c) 2010 Mateusz 'novo' Klos.
*/
//==============================================================================
#include "nFont.hpp"
#include "nFontFace.hpp"

#include <cstdio>
#include <GL/gl.h>
#include <GL/glu.h>

namespace ngl{
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
  // vertCount/2
  //--------------------------------------------------------------------------//
  size_t Font::tri_count() const{
    return (m_cacheUpdated ? (m_vertCount >> 1) : kInvalidIndex);
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void Font::init_position(const int screenHeight){
    set_position( int2(5, screenHeight-m_face->maxSize().height) );
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

      const Glyph &glyph  =m_face->get_glyph( msg[i]=='\n' ? ' ' : msg[i] );
      if( glyph == Glyph::null ){
        fprintf(stderr, "Failed to load glyph '%c'.\n", msg[i]);
        continue;
      }

      generate(ce->verts, i, glyph, position, color);
      
      position.x+=glyph.advance;
      if( msg[i] == '\n' ){
        position.x   =m_position.x;
        position.y  -=m_face->maxSize().y;
      }
    }
    ce->positionDelta =position - m_position;
    m_position        =position;
  }
  //--------------------------------------------------------------------------//
  /// \remarks
  ///   Invalidates geometry returned by get_geometry.
  //--------------------------------------------------------------------------//
  void Font::cprint(const String &msg){
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
    Color32 color(Color32::white);
    Color32 colors[]={
      Color32::white,         // 0
      Color32::red,           // 1
      Color32::green,         // 2
      Color32::lightGreen,    // 3
      Color32::darkGreen,     // 4
      Color32::blue,          // 5
      Color32::lightBlue,     // 6
      Color32::yellow,        // 7
      Color32::grey,          // 8
      Color32::black,         // 9
      Color32::lightGrey,     // 10
      Color32::darkGrey,      // 11
      Color32::lightRed,      // 12
      Color32::darkRed,       // 13
      Color32::darkBlue,      // 14
      Color32::orange         // 15
    };
    int vi=0;
    for(int i=0; i<msg.length(); ++i){
      if( msg[i] == '\n' ){
        position.x   =m_position.x;
        position.y  -=m_face->maxSize().y;
        continue;
      }
      else if(msg[i] == '^'){
        if( msg[++i]!='^' ){
          if( msg[i] >= '0' && msg[i] <= '9' ){
            if( msg[i+1] >= '0' && msg[i+1] <= '9' ){
              color=colors[ (msg[i]-'0') * 10 + (msg[i+1]-'0') ];
              ++i;
            }
            else
              color=colors[msg[i]-'0'];
            continue;
          }
        }
      }

      const Glyph &glyph  =m_face->get_glyph(msg[i]);
      if( glyph == Glyph::null ){
        fprintf(stderr, "Failed to load glyph '%c'.\n", msg[i]);
        continue;
      }
      
      generate(ce->verts, vi, glyph, position, color);
      position.x+=glyph.advance;
      ++vi;
    }
    ce->vertCount     =vi*4;
    ce->positionDelta =position - m_position;
    m_position        =position;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void Font::generate(Vertex *verts, int index, const Glyph &glyph,
                      const int2 &position, Color32 color){
    verts[index*4+0].position.set(glyph.off.x+position.x,
                              glyph.off.y+position.y);
    verts[index*4+0].texCoord =glyph.botLeft;
    verts[index*4+0].color    =color;

    verts[index*4+1].position.set(glyph.off.x+position.x+glyph.size.width,
                              glyph.off.y+position.y);
    verts[index*4+1].texCoord.set(glyph.topRight.u, glyph.botLeft.v);
    verts[index*4+1].color    =color;

    verts[index*4+2].position.set(glyph.off.x+position.x+glyph.size.width,
                              glyph.off.y+position.y+glyph.size.height);
    verts[index*4+2].texCoord =glyph.topRight;
    verts[index*4+2].color    =color;

    verts[index*4+3].position.set(glyph.off.x+position.x,
                              glyph.off.y+position.y+glyph.size.height);
    verts[index*4+3].texCoord.set(glyph.botLeft.u, glyph.topRight.v);
    verts[index*4+3].color    =color;
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void Font::get_geometry(Vertex *vb, Triangle16 *ib, TextureID &texID) const{
    size_t vOff =0;
    for(Cache::const_iterator i=m_cache.begin(); i != m_cache.end(); ++i){
      memcpy( &vb[vOff], i->verts,  (i->vertCount)  *sizeof(Font::Vertex) );
      vOff+=i->vertCount;
    }
    size_t tOff =0;
    for(int i=0; i < vOff; i+=4, tOff+=2){
      ib[tOff+0].set(i+0, i+1, i+3);
      ib[tOff+1].set(i+3, i+1, i+2);
    }
    texID=m_face->atlas()->texid();
  }
  //--------------------------------------------------------------------------//
  //--------------------------------------------------------------------------//
  void Font::update_cache(){
    Cache::iterator tmp;
    m_vertCount=0;
    ++m_counter;
    for(Cache::iterator it=m_cache.begin(); it!=m_cache.end();){
      if( m_cacheTTL && it->lastUsed == m_counter-1 ){
        m_vertCount +=it->vertCount;
        ++it;
      }
      else{
        delete[] it->verts;
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
    m_cacheUpdated  =false;
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
}
