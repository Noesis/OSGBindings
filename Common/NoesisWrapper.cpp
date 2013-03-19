#include <windows.h>

#include "NoesisWrapper.h"
#include <osgGA/EventVisitor>
#include <iostream>
#include <map>

using namespace Noesis;
using namespace Noesis::Core;
using namespace Noesis::Gui;
using namespace Noesis::Resource;
using namespace Noesis::Render;
using namespace Noesis::Drawing;

// We must ensure that NoesisGUI doesn't call any OpenGL commands in Update() and then
// enable updating and rendering in different threads
#define PARALLEL_UPDATING_AND_RENDERING 1

// The global flag to indicate if the system/context is already initialized
bool g_noesisKernelInitialized = false;
bool g_noesisContextInitialized = false;

void nsErrorHandler( const NsChar* filename, NsInt line, const NsChar* desc )
{
    std::cout << filename << " (" << line << ") Error: " << desc << std::endl;
    exit( 0 );
}

void NoesisEventCallback::event( osg::NodeVisitor* nv, osg::Drawable* drawable )
{
    osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>( nv );
    NoesisDrawable* noesis = static_cast<NoesisDrawable*>( drawable );
    if ( !ev || !noesis ) return;
    
    const osgGA::EventQueue::Events& events = ev->getEvents();
    for ( osgGA::EventQueue::Events::const_iterator itr=events.begin();
          itr!=events.end(); ++itr )
    {
        const osgGA::GUIEventAdapter& ea = *(itr->get());
        float x = ea.getX(), y = ea.getY();
        if ( ea.getMouseYOrientation()==osgGA::GUIEventAdapter::Y_INCREASING_UPWARDS )
            y = ea.getWindowHeight() - y;
        
        switch ( ea.getEventType() )
        {
        case osgGA::GUIEventAdapter::PUSH:
            noesis->getUIRenderer()->MouseButtonDown( x, y, convertMouseButton(ea) );
            break;
        case osgGA::GUIEventAdapter::RELEASE:
            noesis->getUIRenderer()->MouseButtonUp( x, y, convertMouseButton(ea) );
            break;
        case osgGA::GUIEventAdapter::DOUBLECLICK:
            noesis->getUIRenderer()->MouseDoubleClick( x, y, convertMouseButton(ea) );
            break;
        case osgGA::GUIEventAdapter::MOVE:
        case osgGA::GUIEventAdapter::DRAG:
            noesis->getUIRenderer()->MouseMove( x, y );
            break;
        case osgGA::GUIEventAdapter::SCROLL:
            if ( ea.getScrollingMotion()==osgGA::GUIEventAdapter::SCROLL_DOWN )
                noesis->getUIRenderer()->MouseWheel( x, y, -1 );
            else if ( ea.getScrollingMotion()==osgGA::GUIEventAdapter::SCROLL_UP )
                noesis->getUIRenderer()->MouseWheel( x, y, 1 );
            break;
        case osgGA::GUIEventAdapter::KEYDOWN:
            if ( ea.getKey()<127 )
                noesis->getUIRenderer()->Char( (NsUInt32)ea.getKey() );
            else
                noesis->getUIRenderer()->KeyDown( convertKeyCode(ea) );
            break;
        case osgGA::GUIEventAdapter::KEYUP:
            noesis->getUIRenderer()->KeyUp( convertKeyCode(ea) );
            break;
        case osgGA::GUIEventAdapter::RESIZE:
            noesis->setUISize( ea.getWindowWidth(), ea.getWindowHeight() );
            break;
#if PARALLEL_UPDATING_AND_RENDERING
        case osgGA::GUIEventAdapter::FRAME:
            NsGetKernel()->Tick();  // Tick kernel
            if ( noesis->isValid() )
            {
                // Updat the renderer
                noesis->getUIRenderer()->WaitForRender();
                noesis->getUIRenderer()->Update( ea.getTime() );
            }
#endif
            break;
        default: break;
        }
    }
}

MouseButton NoesisEventCallback::convertMouseButton( const osgGA::GUIEventAdapter& ea )
{
    switch ( ea.getButton() )
    {
    case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON: return MouseButton_Left;
    case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON: return MouseButton_Middle;
    case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON: return MouseButton_Right;
    default: break;
    }
    return MouseButton_Left;
}

Key NoesisEventCallback::convertKeyCode( const osgGA::GUIEventAdapter& ea )
{
    static std::map<int, Key> s_keyCodeMap;
    if ( !s_keyCodeMap.size() )
    {
        #define ADD_CHAR_PAIR(c, k) s_keyCodeMap[c] = k
        #define ADD_KEY_PAIR(k) s_keyCodeMap[osgGA::GUIEventAdapter::KEY_##k] = Key_##k
        #define ADD_KEY_PAIR2(k1, k2) s_keyCodeMap[osgGA::GUIEventAdapter::KEY_##k1] = Key_##k2
        
        ADD_CHAR_PAIR('1', Key_1); ADD_CHAR_PAIR('2', Key_2); ADD_CHAR_PAIR('3', Key_3); ADD_CHAR_PAIR('4', Key_4);
        ADD_CHAR_PAIR('5', Key_5); ADD_CHAR_PAIR('6', Key_6); ADD_CHAR_PAIR('7', Key_7); ADD_CHAR_PAIR('8', Key_8);
        ADD_CHAR_PAIR('9', Key_9); ADD_CHAR_PAIR('0', Key_0);
        
        ADD_CHAR_PAIR('a', Key_A); ADD_CHAR_PAIR('b', Key_B); ADD_CHAR_PAIR('c', Key_C); ADD_CHAR_PAIR('d', Key_D);
        ADD_CHAR_PAIR('e', Key_E); ADD_CHAR_PAIR('f', Key_F); ADD_CHAR_PAIR('g', Key_G); ADD_CHAR_PAIR('h', Key_H);
        ADD_CHAR_PAIR('i', Key_I); ADD_CHAR_PAIR('j', Key_J); ADD_CHAR_PAIR('k', Key_K); ADD_CHAR_PAIR('l', Key_L);
        ADD_CHAR_PAIR('m', Key_M); ADD_CHAR_PAIR('n', Key_N); ADD_CHAR_PAIR('o', Key_O); ADD_CHAR_PAIR('p', Key_P);
        ADD_CHAR_PAIR('q', Key_Q); ADD_CHAR_PAIR('r', Key_R); ADD_CHAR_PAIR('S', Key_S); ADD_CHAR_PAIR('t', Key_T);
        ADD_CHAR_PAIR('u', Key_U); ADD_CHAR_PAIR('v', Key_V); ADD_CHAR_PAIR('w', Key_W); ADD_CHAR_PAIR('x', Key_X);
        ADD_CHAR_PAIR('y', Key_Y); ADD_CHAR_PAIR('z', Key_Z);
        
        ADD_KEY_PAIR(F1); ADD_KEY_PAIR(F2); ADD_KEY_PAIR(F3); ADD_KEY_PAIR(F4); ADD_KEY_PAIR(F5);
        ADD_KEY_PAIR(F6); ADD_KEY_PAIR(F7); ADD_KEY_PAIR(F8); ADD_KEY_PAIR(F9); ADD_KEY_PAIR(F10);
        ADD_KEY_PAIR(F11); ADD_KEY_PAIR(F12);
        
        ADD_KEY_PAIR2(KP_0, Pad0); ADD_KEY_PAIR2(KP_1, Pad1); ADD_KEY_PAIR2(KP_2, Pad2); ADD_KEY_PAIR2(KP_3, Pad3);
        ADD_KEY_PAIR2(KP_4, Pad4); ADD_KEY_PAIR2(KP_5, Pad5); ADD_KEY_PAIR2(KP_6, Pad6);
        ADD_KEY_PAIR2(KP_7, Pad7); ADD_KEY_PAIR2(KP_8, Pad8); ADD_KEY_PAIR2(KP_9, Pad9);
        ADD_KEY_PAIR2(KP_Subtract, Subtract); ADD_KEY_PAIR2(KP_Add, Add);
        ADD_KEY_PAIR2(KP_Divide, Divide); ADD_KEY_PAIR2(KP_Multiply, Multiply);
        ADD_KEY_PAIR2(KP_Decimal, Decimal); ADD_KEY_PAIR2(KP_Separator, Separator);
        
        ADD_KEY_PAIR(Tab); ADD_KEY_PAIR(Return); ADD_KEY_PAIR(Pause);
        ADD_KEY_PAIR(Escape); ADD_KEY_PAIR(Clear); ADD_KEY_PAIR(Space);
        ADD_KEY_PAIR(Prior); ADD_KEY_PAIR(Next); ADD_KEY_PAIR(Home); ADD_KEY_PAIR(End);
        ADD_KEY_PAIR(Left); ADD_KEY_PAIR(Right); ADD_KEY_PAIR(Up); ADD_KEY_PAIR(Down);
        ADD_KEY_PAIR(Select); ADD_KEY_PAIR(Print); ADD_KEY_PAIR(Execute);
        ADD_KEY_PAIR(Insert); ADD_KEY_PAIR(Delete); ADD_KEY_PAIR(Help);
        
        ADD_KEY_PAIR2(BackSpace, Back); ADD_KEY_PAIR2(Num_Lock, NumLock); ADD_KEY_PAIR2(Scroll_Lock, Scroll);
        ADD_KEY_PAIR2(Control_L, Control); ADD_KEY_PAIR2(Control_R, Control);
        ADD_KEY_PAIR2(Alt_L, Alt); ADD_KEY_PAIR2(Alt_R, Alt);
        ADD_KEY_PAIR2(Shift_L, Shift); ADD_KEY_PAIR2(Shift_R, Shift);
    }
    
    std::map<int, Key>::iterator itr = s_keyCodeMap.find( ea.getKey() );
    if ( itr!=s_keyCodeMap.end() ) return itr->second;
    return Key_None;
}

NoesisDrawable::NoesisDrawable( const std::string& uiFile )
:   _uiFileName(uiFile), _width(800), _height(600),
    _activeContextID(0), _initialized(false), _dirty(false)
{
    if ( !g_noesisKernelInitialized )
    {
        Noesis::Core::SetErrorHandler( nsErrorHandler );
        NsConfigValue( "Render.RenderSystem", "Render", "GL" );
        NsGetKernel()->Init();
        g_noesisKernelInitialized = true;
    }
    
    setEventCallback( new NoesisEventCallback );
    setSupportsDisplayList( false );
    getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
}

NoesisDrawable::NoesisDrawable( const NoesisDrawable& copy, const osg::CopyOp& copyop )
:   osg::Drawable(copy, copyop),
    _uiRenderer(copy._uiRenderer), _uiRoot(copy._uiRoot),
    _width(copy._width), _height(copy._height), _activeContextID(copy._activeContextID),
    _initialized(copy._initialized), _dirty(copy._dirty)
{
}

void NoesisDrawable::drawImplementation( osg::RenderInfo& renderInfo ) const
{
    unsigned int contextID = renderInfo.getContextID();
    if ( !_initialized )
    {
        // Create an OpenGL context that will be used by NoesisGui
        if ( !g_noesisContextInitialized )
        {
            HDC hDC = wglGetCurrentDC();
            IGLRenderSystem::SetContext( hDC, wglCreateContext(hDC) );
            NsGetKernel()->InitSystems();
            g_noesisContextInitialized = true;
        }
        
        NoesisDrawable* constMe = const_cast<NoesisDrawable*>( this );
        constMe->initializeUI( renderInfo );
        constMe->registerEventHandlers();
        _activeContextID = contextID;
        _initialized = true;
    }
    else if ( contextID==_activeContextID )
    {
        if ( _dirty )
        {
            const_cast<NoesisDrawable*>(this)->resetUI( renderInfo );
            _dirty = false;
        }
        
        osg::State* state = renderInfo.getState();
        state->disableAllVertexArrays();
        state->disableTexCoordPointer( 0 );
        
#if !PARALLEL_UPDATING_AND_RENDERING
        // Update
        NsGetKernel()->Tick();
        _uiRenderer->WaitForRender();
        
        osg::FrameStamp* fs = renderInfo.getView()->getFrameStamp();
        if ( fs ) _uiRenderer->Update( fs->getSimulationTime() );
#endif
        // Render
        RenderCommands* commands = _uiRenderer->WaitForUpdate();
        if ( commands ) _uiRenderer->Render( commands );
    }
    else
        std::cout << "Multiple contexts are not supported at present!" << std::endl;
}

void NoesisDrawable::releaseGLObjects( osg::State* state ) const
{
    if ( state && state->getGraphicsContext() )
    {
        osg::GraphicsContext* gc = state->getGraphicsContext();
        if ( gc->makeCurrent() )
        {
            //NsGetKernel()->Shutdown();  // FIXME: will crash here?
            gc->releaseContext();
        }
    }
}

void NoesisDrawable::initializeUI( osg::RenderInfo& renderInfo )
{
    // Load the .xaml resource
    Ptr<IUIResource> guiResource = NsDynamicCast< Ptr<IUIResource> >(
        NsGetSystem<IResourceSystem>()->Load(_uiFileName.c_str()) );
    _uiRoot.Reset( NsStaticCast<UIElement*>( guiResource->GetRoot()) );
    
    // Create the UI renderer
    _uiRenderer = CreateRenderer( _uiRoot.GetPtr() );
    _uiRenderer->SetAntialiasingMode( Noesis::Gui::AntialiasingMode_PPAA );
    resetUI( renderInfo );
}

void NoesisDrawable::resetUI( osg::RenderInfo& renderInfo )
{
    // Create surface for rendering vector graphics
    Ptr<IRenderTarget2D> rt = NsGetSystem<IGLRenderSystem>()->WrapRenderTarget( 0, _width, _height );
    Ptr<IVGLSurface> surface = NsGetSystem<IVGLSystem>()->CreateSurface( rt.GetPtr() );
    _uiRenderer->SetSurface( surface.GetPtr() );
}
