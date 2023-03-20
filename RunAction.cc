#include "RunAction.hh"

RunAction::RunAction()
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    G4RunManager::GetRunManager()->SetPrintProgress(1);

    // Create Histogramm to store counted particles (Monte Carlo)
    man->CreateH1("Pcount", "Particles counted", 10, 0, 10);

    // Create Histogramm to store counted particles (Measurement)
    man->CreateH1("PcountMeas", "Particles counted (measurement)", 10, 0, 10);

    // Create n-Tuples to store scattering angles / secondaries
    //  Create 1st ntuple (id = 0)
    man->CreateNtuple("ElasticScatterDist", "ElasticScatterDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 2nd ntuple (id = 1)
    man->CreateNtuple("ProtonProductionDist", "ProtonProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 3rd ntuple (id = 2)
    man->CreateNtuple("NeutronProductionDist", "NeutronProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 4th ntuple (id = 3)
    man->CreateNtuple("GammaProductionDist", "GammaProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 5th  ntuple (id = 4)
    man->CreateNtuple("PionProductionDist", "PionProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 6th  ntuple (id = 5)
    man->CreateNtuple("ElectronProductionDist", "ElectronProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 7th  ntuple (id = 6)
    man->CreateNtuple("DeuteronProductionDist", "DeuteronProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 8th  ntuple (id = 7)
    man->CreateNtuple("AlphaProductionDist", "AlphaProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 9th  ntuple (id = 8)
    man->CreateNtuple("Helium3ProductionDist", "Helium3ProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 10th  ntuple (id = 9)
    man->CreateNtuple("TritonProductionDist", "TritonProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 11th  ntuple (id = 10)
    man->CreateNtuple("MuonProductionDist", "MuonProductionDist");
    man->CreateNtupleDColumn("phiDist");   // column Id = 0
    man->CreateNtupleDColumn("thetaDist"); // column Id = 1
    man->FinishNtuple();

    //  Create 12th  ntuple (id = 11)
    man->CreateNtuple("UndeclaredProductionDist", "UndeclaredProductionDist");
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
