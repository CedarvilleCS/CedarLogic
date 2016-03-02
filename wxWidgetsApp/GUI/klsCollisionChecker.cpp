/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   klsCollisionChecker: Maintains bounding box overlaps
*****************************************************************************/

#include "klsCollisionChecker.h"

#include "MainApp.h"
DECLARE_APP(MainApp)

// ************************* klsCollisionObject *****************************

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

void klsCollisionChecker::removeObject( klsCollisionObject* oldObj ) {
	collisionObjects.erase( oldObj );
	oldObj->deleteSubObjects();
	oldObj->deleteCollisionObject();
}

