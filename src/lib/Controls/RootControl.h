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

class RootControl: public ControlsContainer {
public:

	using RootControlSharedPtrT = std::shared_ptr<RootControl>;
	using RootControlWeakPtrT = std::weak_ptr<RootControl>;

	/// End users cannot directly create an object
	RootControl(ControlsContainerWeakPtr && parent,
		OevUtil::Uuid && uuid,
		char const* name = "root");

	virtual ~RootControl();
	RootControl(const RootControl &other) = delete;
	RootControl(RootControl &&other) = delete;
	RootControl& operator=(const RootControl &other) = delete;
	RootControl& operator=(RootControl &&other) = delete;
	
	static RootControlWeakPtrT getRootWindowPtr();

	virtual void setupVertexBuffers () override;
	virtual void draw(
			OevGLES::Mat4 const &modelMatrix,
			OevGLES::Mat4 const &viewMatrix,
			OevGLES::Mat4 const &ProjMatrix,
			OevGLES::Mat4 const &MVMatrix,
			OevGLES::Mat4 const &MVPMatrix,
			OevGLES::Vec3 const &lightDir,
			OevGLES::Vec4 const &lightColor,
			OevGLES::Vec4 const &ambientLightColor
			) override;

	
private:

};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_ROOTCONTROL_H_ */
