#include "Geometry.hh"
#include "UserActionInitialization.hh"
#include "OpticalPhotonPhysics.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "FTFP_BERT.hh"

// If you want to visualize the setup:
//  1. set nameUISession = "qt"
//  2. uncomment ui->SessionStart()
//  3. Uncomment /control/execute visSetup_Simplex.mac in GlobalSetup.mac


//-------------------------------------------------------------------------------
int main( int argc, char** argv )
//-------------------------------------------------------------------------------
{
    // ============= [ Setting up the application environment ] ================
    G4String nameUISession = "tcsh";                   // UI Session (qt/tcsh/...)
    G4String nameMainMacro = "GlobalSetup.mac";      // Initialization  macros
    // =========================================================================

    // Construct the default run manager
    G4RunManager * runManager = new G4RunManager;

    // Set up mandatory user initialization: Geometry
    runManager->SetUserInitialization( new Geometry() );

    // Set up mandatory user initialization: Physics-List
    G4VModularPhysicsList* physicsList = new FTFP_BERT;
    physicsList->RegisterPhysics( new OpticalPhotonPhysics() );
    runManager->SetUserInitialization( physicsList );

    // Set up user initialization: User Actions
    runManager->SetUserInitialization( new UserActionInitialization() );

    // Initialize G4 kernel
    runManager->Initialize();

    // Create visualization environment
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Start interactive session
    G4UImanager*   uiManager = G4UImanager::GetUIpointer();
    G4UIExecutive* ui = new G4UIExecutive(argc, argv, nameUISession);
    uiManager->ApplyCommand("/control/execute " + nameMainMacro);
    // ui->SessionStart();

    // Job termination
    delete ui;
    delete visManager;
    delete runManager;

    return 0;
}
