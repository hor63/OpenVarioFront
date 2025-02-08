/*
 * GLFrameWorkPtr.h
 *
 *  Created on: Feb 8, 2025
 *      Author: hor
 */

#ifndef GLES_GLFRAMEWORKPTR_H_
#define GLES_GLFRAMEWORKPTR_H_

#include <memory>

namespace OevGLES {

class GLFramework;

using GLFrameworkWeakPtr = std::weak_ptr<GLFramework>;
using GLFrameworkSharedPtr = std::shared_ptr<GLFramework>;

} // namespace OevGLES {


#endif /* GLES_GLFRAMEWORKPTR_H_ */
