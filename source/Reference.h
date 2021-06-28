#ifndef REFERENCE_H
#define REFERENCE_H

#include "ThreadUtils.h"
#include <iostream>

class SaveContext;
class LoadContext;

class ReferenceManager
{
    public:
    ReferenceManager();
    ~ReferenceManager();

    static ReferenceManager* get();
    uint getNextUniqueID();
    void maximize(uint id);
    void resetIDs();

    private:
    static ReferenceManager* ref_mgr;
    static QMutex _mutex;
    uint _next_id;
};

/*!
 * Unique IDs for Creatures, Decoration, etc.
 * Required for save/load.
 */
class Reference
{
    public:
    Reference();
    virtual ~Reference();

    uint getReferencingID() const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

    private:
    uint _referencing_id;
};

#endif
