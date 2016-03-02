/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   klsCollisionChecker: Maintains bounding box overlaps
*****************************************************************************/

#ifndef KLSCOLLISIONCHECKER_H_
#define KLSCOLLISIONCHECKER_H_

#include <map>
#include <vector>
#include <set>
using namespace std;

#include "klsBBox.h"


enum klsCollisionObjectType {
	COLL_GATE = 0,
	COLL_GATE_HOTSPOT,
	COLL_GATE_CLICKABLE,

	COLL_WIRE,
	COLL_WIRE_SEG,

	// PLACE OBJECT TYPES THAT ARE ALWAYS CHECKED AFTER THIS POINT
	COLL_SELBOX,
	COLL_VIEWPORT,
	
	COLL_MOUSEBOX,
	
	COLL_NONE
};

class klsCollisionObject;
class klsCollisionChecker;

// An arbitrary-ordered group of collision objects:
typedef set< klsCollisionObject* > CollisionGroup;

class klsCollisionObject {
friend class klsCollisionChecker;
public:

	// All collision objects must set their type by default:
	klsCollisionObject( klsCollisionObjectType theType ) {
		setType( theType );
		cData.bboxChanged = true; // The object is new, so mark it as having changed!
	};
	
	~klsCollisionObject() {
		deleteSubObjects();
		deleteCollisionObject();
	}

	// Get and set the bounding box for this collision object:
	// Always use setBBox() in order to flag the object as changed whenever it's
	// bounding box is changed!
	klsBBox getBBox( void ) const { return cData.bbox; };

	void resetBBox( void ) { cData.bbox.reset(); setBBoxChanged(); };

	void setBBox( klsBBox newBBox ) {
		cData.bbox = newBBox;
		makeValidBBox(); // Make sure that the new bbox contains its children!
		// Also can set a flag here that marks this object
		// as "changed" for the collision system to make sure to update it.
		setBBoxChanged();
	};

	// Make sure that the bbox surrounds its sub bboxes:
	// NOTE: It does not reset the bbox first. It just expands the box if needed.
	void makeValidBBox( void ) {
		CollisionGroup::iterator theObj = cData.subObjs.begin();
		while( theObj != cData.subObjs.end() ) {
			cData.bbox.addBBox( (*theObj)->getBBox() );
			theObj++;
		}

		// NOTE: This can't use setBBox, because setBBox calls it. Therefore,
		// it must also set the "changed" flag, just in case it is called from
		// outside of setBBox.
		setBBoxChanged();
	};


	// Return this object's sub objects as a set:
	CollisionGroup getSubObjects( void ) {
		return cData.subObjs;
	};


	// Overlap checking methods:

	// Check this object's bbox against another obj's, but don't
	// update the "overlaps" data structure:
	bool overlaps( klsCollisionObject* objB ) {
		return this->getBBox().overlaps( objB->getBBox() );
	};

	// Check this object's subobjects against another obj's bbox:
	// (Returns a list of subobjects of this object involved in any collisions.)
	CollisionGroup checkSubsToObj( klsCollisionObject* objB, bool resetOverlaps = true );

	// Check the subs of this object against the subs of another object:
	// (Returns a list of subobjects of this object involved in any collisions.)
	CollisionGroup checkSubsToSubs( klsCollisionObject* objB, bool resetOverlaps = true );

	// Return the overlaps of this object:
	CollisionGroup getOverlaps( void ) { return cData.overlaps; };

	// Mutators included for convenience:
	klsCollisionObjectType getType( void ) { return cData.objType; };
private: // Only accessed by constructor:
	void setType( klsCollisionObjectType newType ) { cData.objType = newType; };

protected:
	void insertSubObject( klsCollisionObject* klsc );
	void deleteSubObjects();
	void deleteCollisionObject();
	
private:
	// Functions only to be used by klsCollisionChecker or the object itself:
	// Flag the bbox as having changed:
	void setBBoxChanged( void ) {
		cData.bboxChanged = true;
	}

	// Flag the bbox as having been updated to reflect the change:
	void setBBoxUpdated( void ) {
		cData.bboxChanged = false;
	}

	// Return the "changed" status of the bbox:
	bool bboxHasChanged( void ) {
		return cData.bboxChanged;
	}

	// Reset the overlaps list for this object:
	void clearOverlaps( void ) { cData.overlaps.clear(); };

	// Reset the overlaps list for this object:
	void clearSubsOverlaps( void ) {
		CollisionGroup::iterator sub = cData.subObjs.begin();
		while( sub != cData.subObjs.end() ) {
			(*sub)->clearOverlaps();
			sub++;
		}
	};

	// Add an overlap to this collision object:
	void addOverlap( klsCollisionObject* newOverlap ) { cData.overlaps.insert( newOverlap ); };

	// Remove an overlap from this collision object:
	void removeOverlap( klsCollisionObject* oldOverlap ) { cData.overlaps.erase( oldOverlap ); };
	
	// Verify all current overlaps, and remove those that aren't overlapping anymore:
	// It returns those overlaps that needed to be removed:
	CollisionGroup verifyOverlaps( void ) {
		CollisionGroup badOverlaps; // The overlaps that will need removed.
		CollisionGroup::iterator thisOver = cData.overlaps.begin();
		while( thisOver != cData.overlaps.end() ) {
			// If the bounding boxes of the collision objects overlap,
			// then we have a collision:
			if( this->overlaps(*thisOver) ) {
				// This one's good, so keep checking.
			} else {
				// We've got a changed collision, so remember to remove it later:
				badOverlaps.insert( *thisOver );
			}
			thisOver++;
		}
		
		// Remove all the bad overlaps from the objects:
		thisOver = badOverlaps.begin();
		while( thisOver != badOverlaps.end() ) {
			this->removeOverlap( *thisOver );
			(*thisOver)->removeOverlap( this );
			thisOver++;
		}

		return badOverlaps;
	};

private:
	// Data for the collision object:
	struct {
		klsBBox bbox; // The object's world-space bounding box.

		klsCollisionObjectType objType; // The object's class type

		CollisionGroup subObjs; // Sub objects of this collision object.

		// A flag to tell if the bounding box of the object has changed:
		bool bboxChanged;

		// Temporary data:
		// (This is filled out by a call to the collision checker.)
		CollisionGroup overlaps; // Other objects that overlap this one.
	} cData;
};

class klsCollisionChecker {
public:
	klsCollisionChecker() {
	};

	~klsCollisionChecker() {
	};
	
	// Check the overlaps of all of the collision objects stored in this checker,
	// and update their status:
	void update( void );

	// Check a specific overlap group against another:
	// (NOTE: These groups cannot have a same collision object in both. Their
	// intersection must be the null set.)
	// If resetOverlaps is true, then it resets the overlaps of all the gates before
	// adding new collisions. Otherwise, it will simply add them to the current group.
	// (Returns a list of all objects involved in any collisions.)
	static CollisionGroup checkGroupCollisions( CollisionGroup groupA, CollisionGroup groupB, bool resetOverlaps = true );

	// List maintaining:
	// (NOTE: klsCollisionChecker maintains a LIST of POINTERS to the objects,
	// but it is not responsible for maintaining or deleting the objects, even
	// if you tell it to remove it. "Removing" just takes it out of this checker's list.)
	void addObject( klsCollisionObject* newObj ) {
		collisionObjects.insert( newObj );
		newObj->bboxHasChanged();
		newObj->clearOverlaps();
		newObj->clearSubsOverlaps();
	};

	void removeObject( klsCollisionObject* oldObj );

	// The overlapped objects from the last call to update(), mapped by klsCollisionObjectType:
	map< klsCollisionObjectType, CollisionGroup > overlaps;

	void clear( void ) { collisionObjects.clear(); update(); };
	
private:
	CollisionGroup collisionObjects;
};

#endif /*KLSCOLLISIONCHECKER_H_*/
