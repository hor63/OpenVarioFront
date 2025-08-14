/*
 * ControlsContainer.h
 *
 *  Created on: Aug 14, 2025
 *      Author: hor
 */

#ifndef LIB_CONTROLS_CONTROLSCONTAINER_H_
#define LIB_CONTROLS_CONTROLSCONTAINER_H_

#include "ControlBase.h"

namespace OevControls {

class ControlsContainer: public ControlBase {
public:
	ControlsContainer();
	virtual ~ControlsContainer();
	ControlsContainer(const ControlsContainer &other) = delete;
	ControlsContainer(ControlsContainer &&other) = delete;
	ControlsContainer& operator=(const ControlsContainer &other) = delete;
	ControlsContainer& operator=(ControlsContainer &&other) = delete;
};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_CONTROLSCONTAINER_H_ */
