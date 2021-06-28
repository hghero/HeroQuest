#include "Reference.h"

#include "StreamUtils.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;

ReferenceManager* ReferenceManager::ref_mgr = new ReferenceManager;
QMutex ReferenceManager::_mutex;

ReferenceManager::ReferenceManager()
: _next_id(0)
{
    // NIX
}

ReferenceManager::~ReferenceManager()
{
    // NIX
}

ReferenceManager* ReferenceManager::get()
{
    return ref_mgr;
}

uint ReferenceManager::getNextUniqueID()
{
    AutoMutex am(_mutex);
    return _next_id++;
}

void ReferenceManager::maximize(uint id)
{
    AutoMutex am(_mutex);
    if (id >= _next_id)
        _next_id = id + 1;
}

void ReferenceManager::resetIDs()
{
    AutoMutex am(_mutex);
    _next_id = 0;
}

// ==============================================================================

Reference::Reference()
: _referencing_id(ReferenceManager::get()->getNextUniqueID())
{
    // NIX
}

Reference::~Reference()
{
    // NIX
}

uint Reference::getReferencingID() const
{
    return _referencing_id;
}

bool Reference::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "Reference");

    save_context.writeUInt(_referencing_id, "_referencing_id");
    return !save_context.fail();
}

bool Reference::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "Reference");

    load_context.readUInt(&_referencing_id, "_referencing_id"); // TODO: if some element has been created with this ID before, then we get a conflict here!
    ReferenceManager::get()->maximize(_referencing_id);
    return !load_context.fail();
}
