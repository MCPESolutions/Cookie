
#include "Level.h"

#include "../Actor/Actor.h"

Actor* Level::getEntityById(Level* level, uint32_t id) {
    if(!level)
        return nullptr;
    for(auto& entity : level->getRuntimeActorList()) {
        ActorTypeComponent* actorTypeComponent = entity->getActorTypeComponent();
        if(!actorTypeComponent)
            continue;

        uint32_t entityId = static_cast<uint32_t>(actorTypeComponent->mType);
        if(entityId == id)
            return entity;
    }
    return nullptr;
}