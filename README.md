OpenSceneGraph-NoesisGUI Integration Tutorial
=============

In this simple and quick tutorial, we will show you how to integrate NoesisGUI with OpenSceneGraph (OSG), which is one of the most famous 3D rendering engine and widely used in scientific visualization and virtual reality applications.
You may download and compile NoesisGUI from:

http://www.noesisengine.com/

You may download and compile OSG from:

http://www.openscenegraph.com

Or you may directly download and use the prebuilt binaries from AlphaPixel:

http://www.alphapixel.com/osg/downloads/free-openscenegraph-binary-downloads

Build introduction and tutorials about OpenSceneGraph itself can be found in the book OpenSceneGraph 3.0 Beginners Guide, written by Rui Wang and Xuelei Qian and published by the Packt Publishing:

http://www.packtpub.com/openscenegraph-3-0-beginners-guide/book

Before you start to work on the integration, don¡¯t forget to add the files, NoesisWrapper.h and NoesisWrapper.cpp, to your project. A NoesisDrawable class is defined and implemented in these files, which will perform GUI operations and drawings in a single drawable of the scene graph, which is totally controlled by the OSG scene management system and rendering backend.

There is no need to worry about handling user events like mouse and keyboard inputs. All of them are already done in the NoesisDrawable. The complete source code can be found in the SimpleScene.cpp example in the source code.

If you want to get feedback from the UI elements, for instance, to get to know if a button is clicked, you may refer to the InteractiveUI.cpp example in the source code.
