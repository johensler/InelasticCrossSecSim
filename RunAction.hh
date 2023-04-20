#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Run.hh"

#include "G4GenericMessenger.hh"

class RunAction : public G4UserRunAction
{
public:
    RunAction();
    ~RunAction();

    virtual void BeginOfRunAction(const G4Run *);
    virtual void EndOfRunAction(const G4Run *);

    G4GenericMessenger *fMessenger;
    G4String OutputPath;
};

#endif