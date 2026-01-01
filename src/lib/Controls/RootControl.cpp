/*
 * RootControl.cpp
 *
 *  Created on: Aug 23, 2025
 *      Author: hor
 */
#include "ControlBase.h"
#include "ExceptionBase.h"
#include <GLES2/gl2.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "OVFCommon.h"

#include <cstdio>

#include "RootControl.h"

namespace OevControls {

#if defined HAVE_LOG4CXX_H
	static log4cxx::LoggerPtr logger;
#endif



static OevUtil::Uuid const rootControlUUID ("4c99a482-52f0-44b3-bb3b-a5a4cc506095");

RootControlSharedPtr RootControl::makeRootControl(
	OevGLES::SDLRenderSurface &renderSurface,
	RenderContextSharedPtr const& controlsContextPtr
	) {

RootControlSharedPtr newRootControlPtr;

#if defined HAVE_LOG4CXX_H
		// Get the logger if necessary
		if (!logger) {
			logger = log4cxx::Logger::getLogger("OpenVarioFront.Controls.RootControl");
		}
#endif

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__);

	// Now things are getting tricky:
	// I need a pointer of the object before it is constructed
	// because the parent of the root control is the root control itself.
	// Bring in the allocators
	using RootControlAllocator = std::allocator<RootControl>;
	using RootControlAllocatorTraits = std::allocator_traits<RootControlAllocator>;
	RootControlAllocator rootCtlAllocator;
	
	RootControl* rawPtr = rootCtlAllocator.allocate(1);

	LOG4CXX_DEBUG(logger,"\trawPtr = " << reinterpret_cast<void const *>(rawPtr));
	
	newRootControlPtr.reset(rawPtr);
	
	RootControlAllocatorTraits::construct(rootCtlAllocator, rawPtr,
										  newRootControlPtr, controlsContextPtr,rootControlUUID,
										  renderSurface, "root");

	return newRootControlPtr;
}

RootControl::RootControl(ControlsContainerWeakPtr const &parent,
		RenderContextSharedPtr const& controlsContextPtr,
		OevUtil::Uuid const & uuid,
		OevGLES::SDLRenderSurface &renderSurface,
		char const* name)
	:ControlsContainer(parent,controlsContextPtr,uuid,name),
	 renderSurface{renderSurface}
{
	#if defined HAVE_LOG4CXX_H
		// Get the logger if necessary
		if (!logger) {
			logger = log4cxx::Logger::getLogger("OpenVarioFront.Controls.RootControl");
		}

	auto parentPtr = this->getParent().lock();
	
	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ 
		<< ": this = " << reinterpret_cast<void const*>(this)
		<< ": parent = " << reinterpret_cast<void const*>(parentPtr.get())
		);
#endif

	if (reinterpret_cast<void const*>(this) != 
		reinterpret_cast<void const*>(parentPtr.get())){
			throw ControlsFatalException(
				"Programming error: Parent of the root window must point to itself.\n"
				"Please use OevControls::RootControl::getRootWindowPtr() to obtain a pointer to the one root control."
			);
		}
}

RootControl::~RootControl() { 

	// Please note that I am in shutdown mode of the program when this
	// destructor is being called because the object is bound to a static
	// shared_ptr.
	// A lot of stuff is no longer available.

}

void RootControl::draw() {
	
	GLboolean saveDepthTest = glIsEnabled(GL_DEPTH_TEST);
	GLboolean saveDepthMask = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK,&saveDepthMask);
	
	// Write to the depth buffer, but disable the depth check, i.e. draw the controls in any case.
	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);

	ControlsContainer::draw();
	
	// Restore the depth buffer stuff
	glDepthMask(saveDepthMask);
	if (saveDepthTest == GL_TRUE) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}


} /* namespace OevControls */
