/*
 * RootControl.cpp
 *
 *  Created on: Aug 23, 2025
 *      Author: hor
 */

#include "RootControl.h"
#include "ControlBase.h"
#include "ControlsContainer.h"
#include "Uuid.h"

namespace OevControls {

static RootControl::RootControlSharedPtrT theRootControl;
static OevUtil::Uuid const rootContolUUID ("4c99a482-52f0-44b3-bb3b-a5a4cc506095");

RootControl::RootControlWeakPtrT RootControl::getRootWindowPtr() {
	
	if(!theRootControl) {
		// Now things are getting tricky:
		// I need a pointer of the object before it is constructed
		// because the parent of the root control is the root control itself.
		// Bring in the allocators
		using RootControlAllocator = std::allocator<RootControl>;
		using RootControlAllocatorTraits = std::allocator_traits<RootControlAllocator>;
		RootControlAllocator rootCtlAllocator;
		
		RootControl* rawPtr = rootCtlAllocator.allocate(1);
		
		theRootControl.reset(rawPtr);
		
		RootControlAllocatorTraits::construct(rootCtlAllocator, rawPtr, theRootControl, std::move(OevUtil::Uuid(rootContolUUID)),"root");
		
	}
	
	return theRootControl;
}


RootControl::RootControl(ControlsContainerWeakPtr && parent,
		OevUtil::Uuid && uuid,
		char const* name)
	:ControlsContainer(std::move(parent),std::move(uuid),name) 
{ }

RootControl::~RootControl() {
}

void RootControl::setupVertexBuffers () {
	
}

void RootControl::draw(
			OevGLES::Mat4 const &modelMatrix,
			OevGLES::Mat4 const &viewMatrix,
			OevGLES::Mat4 const &ProjMatrix,
			OevGLES::Mat4 const &MVMatrix,
			OevGLES::Mat4 const &MVPMatrix,
			OevGLES::Vec3 const &lightDir,
			OevGLES::Vec4 const &lightColor,
			OevGLES::Vec4 const &ambientLightColor
			) {
				
}


} /* namespace OevControls */
