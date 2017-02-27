/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   klsCollisionChecker: Maintains bounding box overlaps
*****************************************************************************/

#include "klsCollisionChecker.h"

#include "../MainApp.h"
DECLARE_APP(MainApp)

// ************************* klsCollisionObject *****************************

klsCollisionObject::klsCollisionObject(klsCollisionObjectType theType) {
	setType(theType);
	cData.bboxChanged = true; // The object is new, so mark it as having changed!
};

klsCollisionObject::~klsCollisionObject() {
	deleteSubObjects();
	deleteCollisionObject();
}

klsBBox klsCollisionObject::getBBox() const {
	return cData.bbox;
};

void klsCollisionObject::resetBBox() {
	cData.bbox.reset(); setBBoxChanged();
}

void klsCollisionObject::setBBox(klsBBox newBBox) {
	cData.bbox = newBBox;
	makeValidBBox(); // Make sure that the new bbox contains its children!
					 // Also can set a flag here that marks this object
					 // as "changed" for the collision system to make sure to update it.
	setBBoxChanged();
}

void klsCollisionObject::makeValidBBox() {
	CollisionGroup::iterator theObj = cData.subObjs.begin();
	while (theObj != cData.subObjs.end()) {
		cData.bbox.addBBox((*theObj)->getBBox());
		theObj++;
	}

	// NOTE: This can't use setBBox, because setBBox calls it. Therefore,
	// it must also set the "changed" flag, just in case it is called from
	// outside of setBBox.
	setBBoxChanged();
}

CollisionGroup klsCollisionObject::getSubObjects() {
	return cData.subObjs;
}

bool klsCollisionObject::overlaps(klsCollisionObject* objB) {
	return this->getBBox().overlaps(objB->getBBox());
}

// Check this object's subobjects against another obj's bbox:
// (Returns a list of subobjects of this object involved in any collisions.)
CollisionGroup klsCollisionObject::checkSubsToObj( klsCollisionObject* objB, bool resetOverlaps ) {
	CollisionGroup theObjB;
	theObjB.insert( objB );
	return klsCollisionChecker::checkGroupCollisions( this->getSubObjects(), theObjB, resetOverlaps );
}

// Check the subs of this object against the subs of another object:
// (Returns a list of subobjects of this object involved in any collisions.)
CollisionGroup klsCollisionObject::checkSubsToSubs( klsCollisionObject* objB, bool resetOverlaps ) {
	return klsCollisionChecker::checkGroupCollisions( this->getSubObjects(), objB->getSubObjects(), resetOverlaps );
}

CollisionGroup klsCollisionObject::getOverlaps() {
	return cData.overlaps;
}

klsCollisionObjectType klsCollisionObject::getType() {
	return cData.objType;
}

void klsCollisionObject::setType(klsCollisionObjectType newType) {
	cData.objType = newType;
};

void klsCollisionObject::insertSubObject(klsCollisionObject* klsc) {
	cData.subObjs.insert( klsc );
}

void klsCollisionObject::deleteSubObjects() {
	CollisionGroup::iterator sub = cData.subObjs.begin();
	while( sub != cData.subObjs.end() ) {
		(*sub)->deleteCollisionObject();
		sub++;
	}
	cData.subObjs.clear();
}

void klsCollisionObject::deleteCollisionObject() {
	CollisionGroup badOverlaps = this->getOverlaps();
	CollisionGroup::iterator remOver = badOverlaps.begin();
	while( remOver != badOverlaps.end() ) {
		(*remOver)->removeOverlap( this );
		remOver++;
	}
	this->clearOverlaps();	

	// NOT a good idea: (Will cause recursive loop!)
	// this->deleteSubObjects()
	// (Yes, include this comment in the code!)
}

void klsCollisionObject::setBBoxChanged() {
	cData.bboxChanged = true;
}

void klsCollisionObject::setBBoxUpdated() {
	cData.bboxChanged = false;
}

bool klsCollisionObject::bboxHasChanged() {
	return cData.bboxChanged;
}

void klsCollisionObject::clearOverlaps() {
	cData.overlaps.clear();
}

void klsCollisionObject::clearSubsOverlaps() {
	CollisionGroup::iterator sub = cData.subObjs.begin();
	while (sub != cData.subObjs.end()) {
		(*sub)->clearOverlaps();
		sub++;
	}
}

void klsCollisionObject::addOverlap(klsCollisionObject* newOverlap) {
	cData.overlaps.insert(newOverlap);
}

void klsCollisionObject::removeOverlap(klsCollisionObject* oldOverlap) {
	cData.overlaps.erase(oldOverlap);
}

CollisionGroup klsCollisionObject::verifyOverlaps() {
	CollisionGroup badOverlaps; // The overlaps that will need removed.
	CollisionGroup::iterator thisOver = cData.overlaps.begin();
	while (thisOver != cData.overlaps.end()) {
		// If the bounding boxes of the collision objects overlap,
		// then we have a collision:
		if (this->overlaps(*thisOver)) {
			// This one's good, so keep checking.
		}
		else {
			// We've got a changed collision, so remember to remove it later:
			badOverlaps.insert(*thisOver);
		}
		thisOver++;
	}

	// Remove all the bad overlaps from the objects:
	thisOver = badOverlaps.begin();
	while (thisOver != badOverlaps.end()) {
		this->removeOverlap(*thisOver);
		(*thisOver)->removeOverlap(this);
		thisOver++;
	}

	return badOverlaps;
}

// ************************* klsCollisionChecker *****************************

// Check the overlaps of all of the collision objects stored in this checker,
// and update their status:
void klsCollisionChecker::update( void ) {
	//TODO: Put a more efficient algorithm in to this that uses a sort-sweep algorithm
	// or something to that effect to do an all-to-all comparison, that keeps the data
	// structure between calls to update(). For now, it simply slices the problem up into
	// an N by N comparison that is order N^2.
	//NOTE: I made the algorithm more efficient by only updating bboxes that have changed
	// since the last call to update. So, it is now order S*N, where S is the number
	// of things that have changed.  Also, moving items are now only checked against
	// stationary ones, and if S < N/2, then the groups are switched.

	// Clear out the old collisions:
	overlaps.clear();

	// Loop through all collision objects, and identify those that have changed:
	CollisionGroup changedObjs, stationaryObjs;
	CollisionGroup::iterator thisObj = collisionObjects.begin();
	while( thisObj != collisionObjects.end() ) {
		// Changed objects and special-type objects (view box, sel box, mouse, etc)
		if( (*thisObj)->bboxHasChanged() || (*thisObj)->getType() > COLL_WIRE_SEG) {
			// Add it to the update list.
			changedObjs.insert( *thisObj );

			// Verify and remove invalid collisions with all "colliding" objects,
			// to remove overlaps that are no longer current:
			(*thisObj)->verifyOverlaps();

			// Tell it that we've fixed the problem:
			(*thisObj)->setBBoxUpdated();
		} else {
			stationaryObjs.insert( *thisObj );
		}
		
		// Add all of the overlaps of this object into the main overlaps object:
		CollisionGroup hits = (*thisObj)->getOverlaps();
		CollisionGroup::iterator thisHit = hits.begin();
		while( thisHit != hits.end() ) {
			overlaps[(*thisHit)->getType()].insert(*thisHit);
			thisHit++;
		}
		
		thisObj++;
	}

	// Now, the question is, which group has more objects?
	CollisionGroup* relChanged = &(changedObjs.size() < stationaryObjs.size() ? changedObjs : stationaryObjs);
	CollisionGroup* relStatic = &(changedObjs.size() >= stationaryObjs.size() ? changedObjs : stationaryObjs);

	// With the objects that have changed their bounding boxes, update their
	// collision information:
	unsigned long long numCompares = 0;
	CollisionGroup::iterator changedObj = relChanged->begin();
	while( changedObj != relChanged->end() ) {
		// Check this object against all of the other objects, to add potential
		// new overlaps:
//* Slow collision checking system:
		CollisionGroup groupA, groupB;
		groupA.insert( *changedObj );
		groupB.insert( relStatic->begin(), relStatic->end() );
		groupB.erase( *changedObj );

		// Check the collisions of object A with the rest of the group,
		// while not resetting the other object's overlap information:
		CollisionGroup hits;
		hits = checkGroupCollisions( groupA, groupB, false );
		numCompares += groupB.size();
/**/

		// Sort the hits into the main map object:
		CollisionGroup::iterator thisHit = hits.begin();
		while( thisHit != hits.end() ) {
			overlaps[(*thisHit)->getType()].insert(*thisHit);
			thisHit++;
		}

		// Check the next changed object:
		changedObj++;
	}
}

// Check a specific overlap group against another:
// (NOTE: These groups cannot have a same collision object in both. Their
// intersection must be the null set.)
// If resetOverlaps is true, then it resets the overlaps of all the gates before
// adding new collisions. Otherwise, it will simply add them to the current group.
// (Returns a list of all objects involved in any collisions.)
CollisionGroup klsCollisionChecker::checkGroupCollisions( CollisionGroup groupA, CollisionGroup groupB, bool resetOverlaps ) {
	CollisionGroup collidedObjects;
	CollisionGroup::iterator iterA, iterB;
	
	// Clear out old overlap information if requested:
	if( resetOverlaps ) {
		iterA = groupA.begin();
		while( iterA != groupA.end() ) {
			(*iterA)->clearOverlaps();
			iterA++;
		}
		
		iterB = groupB.begin();
		while( iterB != groupB.end() ) {
			(*iterB)->clearOverlaps();
			iterB++;
		}
	}
	
	// Check each bbox of groupA against all of groupB:
	iterA = groupA.begin();
	while( iterA != groupA.end() ) {
		iterB = groupB.begin();
		while( iterB != groupB.end() ) {

			// If the bounding boxes of the collision objects overlap,
			// then we have a collision:
			if( (*iterA)->overlaps(*iterB) ) {

				// Register the collision in both object's data structures:
				(*iterA)->addOverlap( *iterB );
				(*iterB)->addOverlap( *iterA );

				// Save the collided objects for returning them to the caller:
				collidedObjects.insert( *iterA );
//				collidedObjects.insert( *iterB );
			}

			iterB++;
		}
		iterA++;
	}

	return collidedObjects;
}

void klsCollisionChecker::addObject(klsCollisionObject* newObj) {
	collisionObjects.insert(newObj);
	newObj->bboxHasChanged();
	newObj->clearOverlaps();
	newObj->clearSubsOverlaps();
};

void klsCollisionChecker::removeObject( klsCollisionObject* oldObj ) {
	collisionObjects.erase( oldObj );
	oldObj->deleteSubObjects();
	oldObj->deleteCollisionObject();
}

void klsCollisionChecker::clear() {
	collisionObjects.clear(); update();
};