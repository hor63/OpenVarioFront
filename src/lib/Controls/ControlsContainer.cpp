/*
 * ControlsContainer.cpp
 *
 *  Created on: Aug 14, 2025
 *      Author: hor
 */
#include <utility>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "OVFCommon.h"

#include "ControlsContainer.h"

namespace OevControls {

ControlsContainer::ControlsContainer() {

}

ControlsContainer::~ControlsContainer() {
	// TODO Auto-generated destructor stub
}

void ControlsContainer::addControl(ControlBasePtr controlPtr) {
	controlsMap.insert(std::make_pair(controlPtr->getUuid(), controlPtr));
}

} /* namespace OevControls */
