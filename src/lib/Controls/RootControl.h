/*
 * RootControl.h
 *
 *  Created on: Aug 23, 2025
 *      Author: hor
 */

#ifndef LIB_CONTROLS_ROOTCONTROL_H_
#define LIB_CONTROLS_ROOTCONTROL_H_

#include "ControlsContainer.h"
#include <memory>

namespace OevControls {

class RootControl;

using RootControlSharedPtr = std::shared_ptr<RootControl>;
using RootControlWeakPtr = std::weak_ptr<RootControl>;

} // namespace OevControls {

#include "GLES/SDL/SDLRenderSurface.h"

namespace OevControls {

class RootControl: public ControlsContainer {
	
	friend class OevGLES::SDLRenderSurface;
	
public:

	/** End users must not directly create an object
	 * 
	 * I made it really hard to do because where parent points must be identical
	 * to \p this. This can only be done by \ref makeRootControl
	 *
	 * \see makeRootControl
	 */
	RootControl(ControlsContainerWeakPtr  const &parent,
		OevUtil::Uuid const & uuid,
		OevGLES::SDLRenderSurface &renderSurface,
		char const* name = "root");

	virtual ~RootControl();
	RootControl(const RootControl &other) = delete;
	RootControl(RootControl &&other) = delete;
	RootControl& operator=(const RootControl &other) = delete;
	RootControl& operator=(RootControl &&other) = delete;

	OevGLES::SDLRenderSurface& getRenderSurface() {
		return renderSurface;
	}

	virtual void setupVertexBuffers () override;
	virtual void draw(OevGLES::RenderStandardUniforms const &stdUniformData) override;

	
private:

	/// \ref OevGLES::SDLRenderSurface owns \p this. Therefore it is safe
	/// to store the reference.
	OevGLES::SDLRenderSurface &renderSurface;

	/// \brief Only to be called by good friends
	static RootControlSharedPtr
	makeRootControl(OevGLES::SDLRenderSurface &renderSurface);

};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_ROOTCONTROL_H_ */
