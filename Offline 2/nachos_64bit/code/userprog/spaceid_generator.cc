//
// Created by rafid on 12/31/17.
//

#include "spaceid_generator.h"

SpaceIdGenerator::SpaceIdGenerator(int maxId) {
    lock = new Lock("Space ID Generator Lock");
    freeIds = new queue<SpaceId>();
    for(int i = 1; i <= maxId; ++i) {
        freeIds->push(i);
    }
}

SpaceIdGenerator::~SpaceIdGenerator() {
    delete freeIds;
    delete lock;
}

SpaceId SpaceIdGenerator::generate() {
    lock->Acquire();
    if(!freeIds->empty()) {
        SpaceId retValue = freeIds->front();
        freeIds->pop();
        lock->Release();
        return retValue;
    }
    lock->Release();
    return 0;
}

void SpaceIdGenerator::releaseId(SpaceId spaceId) {
    lock->Acquire();
    freeIds->push(spaceId);
    lock->Release();
}