//==============================================================================
/**
\file            SDLFramework.hpp
\author          Mateusz 'novo' Klos
\date            July 20, 2010



Copyright (c) 2010 Mateusz 'novo' Klos
*/
//==============================================================================
#if !defined(__NOVO_SDLFRAMEWORK_HPP__)
#define __NOVO_SDLFRAMEWORK_HPP__

#include <stdint.h>

#define RetOnError( FUNC )                  \
  if( ( err =(FUNC) ) < 0 )    return err;
  
union SDL_Event;
namespace ngl{
  //==============================================================================
  /** \class AppInterface
  \brief  Application interface.
  */
  //==============================================================================
  class AppInterface {
    AppInterface(const AppInterface &obj)             {               }
    AppInterface& operator=(const AppInterface &obj)  { return *this; }

    public:
      AppInterface(){}
      virtual ~AppInterface(){}

      virtual int   init(int argc, char **argv)   =0;
      virtual int   cleanup()                     =0;
      virtual int   tick()                        =0;
      
      virtual int   on_event(::SDL_Event*)              { return 0;   }
      virtual int   on_resize(uint32_t w, uint32_t h)   { return 0;   }
      
    private:
      
  };
  namespace app{
    namespace Window{
      enum WindowType{
        Unknown   =0x00,
        Normal    =0x01,
        Fullscreen=0x02
      };
    }
    using Window::WindowType;

    int setup(ngl::AppInterface *app);
    int init(int argc, char **argv);
    int cleanup();
    int run();
    float frame_time();

    int       set_window_geometry(uint32_t width,
                                  uint32_t height,
                                  uint32_t bpp,
                                  WindowType type);

    template<typename Type>
    inline int execute(int argc, char **argv){
      int err;
      RetOnError( ngl::app::setup( new Type() ) );
      RetOnError( ngl::app::init( argc, argv ) );
      RetOnError( ngl::app::run()              );
      RetOnError( ngl::app::cleanup()          );
      return 0;
    }
  }
}

#endif/* __NOVO_SDLFRAMEWORK_HPP__ */
