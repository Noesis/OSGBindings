/* ============================================================================
**
** Demonstration of the simplest integration of OpenSceneGraph and NoesisGUI
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

int main( int argc, char** argv )
{
    osg::ArgumentParser arguments( &argc, argv );
    osg::DisplaySettings::instance()->setMinimumNumStencilBits( 8 );
    
    std::string uiFile("Gui/Samples/CarHud.xaml");
    arguments.read( "--ui", uiFile );
    
    osg::ref_ptr<osg::Node> scene = osgDB::readNodeFiles(arguments);
    if ( !scene ) scene = osgDB::readNodeFile("cessna.osg");
    
    osg::ref_ptr<NoesisDrawable> noesis = new NoesisDrawable(uiFile);
    
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
