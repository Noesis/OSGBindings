/* ============================================================================
**
** Demonstration of the event handling in osgNoesisGUI
** Copyright (C) 2012 Wang Rui
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
** ========================================================================= */

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include "Common/NoesisWrapper.h"

class CustomNoesisDrawable : public NoesisDrawable
{
public:
    CustomNoesisDrawable( const std::string& uiFile )
    : NoesisDrawable(uiFile) {}
    
protected:
    virtual void registerEventHandlers()
    {
        Noesis::Gui::FrameworkElement* framework = NsStaticCast<Noesis::Gui::FrameworkElement*>( getUIRoot() );
        if ( !framework ) return;
        
        Noesis::Gui::ComboBox* combo0 = NsStaticCast<Noesis::Gui::ComboBox*>( framework->FindName("Background") );
        if ( combo0 ) combo0->SelectionChanged() += Noesis::Core::MakeDelegate( this, &CustomNoesisDrawable::backgroundChanged );
        
        Noesis::Gui::ComboBox* combo1 = NsStaticCast<Noesis::Gui::ComboBox*>( framework->FindName("Lights") );
        if ( combo1 ) combo1->SelectionChanged() += Noesis::Core::MakeDelegate( this, &CustomNoesisDrawable::lightsChanged );
        
        Noesis::Gui::ComboBox* combo2 = NsStaticCast<Noesis::Gui::ComboBox*>( framework->FindName("Visualization") );
        if ( combo2 ) combo2->SelectionChanged() += Noesis::Core::MakeDelegate( this, &CustomNoesisDrawable::visualizationChanged );
        
        Noesis::Gui::Slider* slider = NsStaticCast<Noesis::Gui::Slider*>( framework->FindName("Luminance") );
        if ( slider ) slider->ValueChanged() += Noesis::Core::MakeDelegate( this, &CustomNoesisDrawable::luminanceChanged );
    }
    
    void backgroundChanged( Noesis::Core::BaseComponent* sender, const Noesis::Gui::SelectionChangedEventArgs& e )
    {
        Noesis::Gui::ComboBox* combo = NsStaticCast<Noesis::Gui::ComboBox*>( sender );
        std::cout << "Select background index: " << combo->GetSelectedIndex() << std::endl;
    }
    
    void lightsChanged( Noesis::Core::BaseComponent* sender, const Noesis::Gui::SelectionChangedEventArgs& e )
    {
        Noesis::Gui::ComboBox* combo = NsStaticCast<Noesis::Gui::ComboBox*>( sender );
        std::cout << "Select light index: " << combo->GetSelectedIndex() << std::endl;
    }
    
    void visualizationChanged( Noesis::Core::BaseComponent* sender, const Noesis::Gui::SelectionChangedEventArgs& e )
    {
        Noesis::Gui::ComboBox* combo = NsStaticCast<Noesis::Gui::ComboBox*>( sender );
        std::cout << "Select visualization index: " << combo->GetSelectedIndex() << std::endl;
    }
    
    void luminanceChanged( Noesis::Core::BaseComponent* sender, const Noesis::Gui::RoutedPropertyChangedEventArgs<NsFloat32>& e )
    {
        std::cout << "Set luminance value: " << e.newValue << std::endl;
    }
};

int main( int argc, char** argv )
{
    osg::ArgumentParser arguments( &argc, argv );
    
    std::string uiFile("Gui/Samples/SDKTutorial/UI.xaml");
    arguments.read( "--ui", uiFile );
    
    osg::ref_ptr<osg::Node> scene = osgDB::readNodeFiles(arguments);
    if ( !scene ) scene = osgDB::readNodeFile("cessna.osg");
    
    osg::ref_ptr<CustomNoesisDrawable> noesis = new CustomNoesisDrawable(uiFile);
    
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->setCullingActive( false );
    geode->addDrawable( noesis.get() );
    geode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    geode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setClearMask( GL_STENCIL_BUFFER_BIT );
    camera->setClearStencil( 1 );
    camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    camera->setRenderOrder( osg::Camera::POST_RENDER );
    camera->setProjectionMatrix( osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0) );
    camera->addChild( geode.get() );
    
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild( scene.get() );
    root->addChild( camera.get() );
    
    osgViewer::Viewer viewer;
    viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );
    viewer.addEventHandler( new osgViewer::StatsHandler );
    viewer.addEventHandler( new osgViewer::WindowSizeHandler );
    viewer.setSceneData( root.get() );
    viewer.setUpViewInWindow( 50, 50, 800, 600 );
    //viewer.setThreadingModel( osgViewer::Viewer::SingleThreaded );
    viewer.realize();
    
    osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>( viewer.getCamera()->getGraphicsContext() );
    if ( gw )
    {
        // Send a window size event for resizing the UI
        int x, y, w, h; gw->getWindowRectangle( x, y, w, h );
        viewer.getEventQueue()->windowResize( x, y, w, h );
    }
    return viewer.run();
}
