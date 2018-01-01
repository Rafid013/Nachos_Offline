//
// Created by rafid on 12/31/17.
//

#ifndef SPACEID_GENERATOR_H
#define SPACEID_GENERATOR_H

#include "../threads/synch.h"
#include <queue>
using namespace std;

typedef int SpaceId;

class SpaceIdGenerator {
    queue<SpaceId> *freeIds;
    Lock *lock;
public:
    explicit SpaceIdGenerator(int maxId);
    ~SpaceIdGenerator();
    SpaceId generate();
    void releaseId(SpaceId spaceId);
};


#endif //SPACEID_GENERATOR_H
