#ifndef H_NOESISGUI_WRAPPER
#define H_NOESISGUI_WRAPPER

#include <osg/Camera>
#include <osg/Drawable>
#include <osgGA/GUIEventHandler>

#include <Noesis.h>
#include <NsCore/Kernel.h>
#include <NsCore/Error.h>
#include <NsCore/NsSystem.h>
#include <NsCore/NsConfig.h>
#include <NsRender/IRenderTarget2D.h>
#include <NsRender/IGLRenderSystem.h>
#include <NsResource/IResourceSystem.h>
#include <NsDrawing/IVGLSurface.h>
#include <NsDrawing/IVGLSystem.h>
#include <NsGui/IUIResource.h>
#include <NsGui/Visual.h>
#include <NsGui/IRenderer.h>
#include <NsGui/Slider.h>
#include <NsGui/ComboBox.h>
#include <NsGui/RoutedEvent.h>
#include <NsGui/VisualTreeHelper.h>

class NoesisEventCallback : public osg::Drawable::EventCallback
{
public:
    virtual void event( osg::NodeVisitor* nv, osg::Drawable* drawable );
    
protected:
    Noesis::Gui::MouseButton convertMouseButton( const osgGA::GUIEventAdapter& ea );
    Noesis::Gui::Key convertKeyCode( const osgGA::GUIEventAdapter& ea );
};

class NoesisDrawable : public osg::Drawable
{
public:
    NoesisDrawable( const std::string& uiFile=std::string() );
    NoesisDrawable( const NoesisDrawable& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    META_Object( osgNoesisGUI, NoesisDrawable )
    
    Noesis::Gui::IRenderer* getUIRenderer() { return _uiRenderer.GetPtr(); }
    const Noesis::Gui::IRenderer* getUIRenderer() const { return _uiRenderer.GetPtr(); }
    
    Noesis::Gui::UIElement* getUIRoot() { return _uiRoot.GetPtr(); }
    const Noesis::Gui::UIElement* getUIRoot() const { return _uiRoot.GetPtr(); }
    
    bool isValid() const { return _initialized; }
    const std::string& getUIFileName() const { return _uiFileName; }
    
    void setUISize( unsigned int w, unsigned int h ) { _width = w; _height = h; _dirty = true; }
    void getUISize( unsigned int& w, unsigned int& h ) { w = _width; h = _height; }
    
    virtual void drawImplementation( osg::RenderInfo& renderInfo ) const;
    virtual void releaseGLObjects( osg::State* state=0 ) const;
    
protected:
    virtual ~NoesisDrawable() {}
    
    virtual void initializeUI( osg::RenderInfo& renderInfo );
    virtual void resetUI( osg::RenderInfo& renderInfo );
    virtual void registerEventHandlers() {}
    
    Noesis::Core::Ptr<Noesis::Gui::IRenderer> _uiRenderer;
    Noesis::Core::Ptr<Noesis::Gui::UIElement> _uiRoot;
    std::string _uiFileName;
    unsigned int _width, _height;
    mutable unsigned int _activeContextID;
    mutable bool _initialized;
    mutable bool _dirty;
};

#endif
