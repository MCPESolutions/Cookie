#include "../../FuncHook.h"
#include "ActorModel.h"

class Event {
   public:
    virtual ~Event() = default; 
    virtual void trigger() {}
};

class BoneRenderEvent : public Event {
   public:
    class Bone* mBone;
    class ActorPartModel* mPartModel;
    class Actor* mActor;

    bool mDoBlockAnimation = false;

    explicit BoneRenderEvent(Bone* bone, ActorPartModel* partModel, Actor* actor)
        : mBone(bone), mPartModel(partModel), mActor(actor) {}
};