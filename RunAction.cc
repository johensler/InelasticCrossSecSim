#include "RunAction.hh"

RunAction::RunAction()
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    G4RunManager::GetRunManager()->SetPrintProgress(1);
    
    // Create Histogramm to store counted particles
    man->CreateH1("Pcount", "Particles counted", 4, 0, 4);

    // Create n-Tuples to store scattering angles
    //  Create 1st ntuple (id = 0)
    man->CreateNtuple("ElasticScatterDist", "ElasticScatterDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();
}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run *aRun)
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    G4int runNumber = aRun->GetRunID();
    std::stringstream strRunID;
    strRunID << runNumber;
    man->OpenFile("output/absorb_run" + strRunID.str() + ".root");

}

void RunAction::EndOfRunAction(const G4Run *aRun)
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->Write();
    man->CloseFile();
}
