#ifndef PEMODEL_H
#define PEMODEL_H

#include <peContainer.h>
#include <peBankData.h>

class peModelContainer : public peContainer {
public:
    peModelContainer(peEmitterNode *parent, _peModel *model, u16 capacity);
    virtual ~peModelContainer();
    virtual void Stop();
    virtual void Render();
    virtual void Update(f32 delta);
    virtual u16 Emit(u16 count);

    _peModel *mModel;
};

#endif
