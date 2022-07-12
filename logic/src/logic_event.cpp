
#include "logic_event.h"
;
Event::Event() {
	isJunctionEvent = false;
	newJunctionState = false;
	junctionID = ID_NONE;

	newState = UNKNOWN;
	eventTime = TIME_NONE;
	wireID = ID_NONE;
	gateID = ID_NONE;
	gateOutputID = "";

	// Tag the creation time, for sorting if there are two at the same simulation time:
	myCreationTime = globalCreationTime++;
};

TimeType Event::getCreationTime() const {
	return myCreationTime;
};
;
bool Event::operator > (const Event &other) const {

	if (eventTime == other.eventTime) {
		return (getCreationTime() > other.getCreationTime());
	}

	return (eventTime > other.eventTime);
}

TimeType Event::globalCreationTime = 0;