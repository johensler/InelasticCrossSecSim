#include "action.hh"

ActionInitialization::ActionInitialization()
{
    generator = new PrimaryGenerator();
    eventAction = new EventAction();
}

ActionInitialization::~ActionInitialization()
{
}

void ActionInitialization::Build() const
{
    SetUserAction(generator);

    RunAction *runAction = new RunAction();
    SetUserAction(runAction);

    SetUserAction(eventAction);
    static_cast<RunManager *>(RunManager::GetRunManager())->eventAction = eventAction;
}
