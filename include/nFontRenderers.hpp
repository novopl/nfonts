//======================================================================
/**
\file            FontRenderers.hpp
\author          Mateusz 'novo' Klos
\date            July 20, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//======================================================================
#if !defined(__NGL_FONTRENDERERS_HPP__)
#define __NGL_FONTRENDERERS_HPP__

#include "nFont.hpp"

namespace ngl{
//======================================================================
/** \class AbstractRenderer
\brief  Base class for all font renderers.
*/
//======================================================================
class AbstractRenderer{
  AbstractRenderer(const AbstractRenderer &obj){}
  AbstractRenderer& operator=(const AbstractRenderer &obj){ 
    return *this; 
  }

  public:
    AbstractRenderer();
    virtual ~AbstractRenderer();

    int state_setup();
    int state_cleanup();
    
    void print_info(const Font &font);
    void print_vertex(const Font::Vertex &v);

    virtual int render(const Font &font)=0;

  private:

};
//======================================================================
/** \class LegacyRenderer
\brief  Legacy renderer(glVertex).
*/
//======================================================================
  class LegacyRenderer : public AbstractRenderer{
    LegacyRenderer(const LegacyRenderer &obj){}
    LegacyRenderer& operator=(const LegacyRenderer &obj){ 
      return *this; 
    }

    public:
      LegacyRenderer();
      virtual ~LegacyRenderer();

      virtual int render(const Font &font);

    private:
      int extend_buffers(uint32_t vertCount);
      uint32_t      m_vertCount;
      Font::Vertex  *m_vb;
      Triangle16    *m_ib;
  };
//======================================================================
/** \class VARenderer
\brief  Vertex array renderer.
*/
//======================================================================
  class VARenderer : public AbstractRenderer{
    VARenderer(const VARenderer &obj)             {               }
    VARenderer& operator=(const VARenderer &obj)  { return *this; }

    public:
      VARenderer();
      virtual ~VARenderer();

      virtual int render(const Font &font);

    private:
      static const int  kNumVerts=4096;
      Font::Vertex      vb[kNumVerts];
      Triangle16        ib[kNumVerts/2];
  };
  
//======================================================================
/** \class VBORenderer
\brief  Vertex buffer object renderer.
*/
//======================================================================
  class VBORenderer : public AbstractRenderer{
    VBORenderer(const VBORenderer &obj)             {               }
    VBORenderer& operator=(const VBORenderer &obj)  { return *this; }

    public:
      VBORenderer();
      virtual ~VBORenderer();

      virtual int render(const Font &font);

    private:
      int extend_buffers(uint32_t vertCount);
      
      uint32_t    m_vb;
      uint32_t    m_ib;
      uint32_t    m_vertCount;
  };

  
  namespace Renderer{
    enum RendererType{
      Legacy,
      VA,
      VBO
    };
  }
  using Renderer::RendererType;
//   enum RendererType{
//     Legacy,
//     VA,
//     VBO
//   };
  extern AbstractRenderer *create_renderer(RendererType type);
  extern void init_extensions();
}

#endif/* __NGL_FONTRENDERERS_HPP__ */
