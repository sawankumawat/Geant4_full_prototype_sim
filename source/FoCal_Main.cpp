#include "Geometry.hh"
#include "UserActionInitialization.hh"
#include "OpticalPhotonPhysics.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "FTFP_BERT.hh"

#include "usercommandline.hh"

#include "TROOT.h"
// If you want to visualize the setup:
//  1. set nameUISession = "qt"
//  2. uncomment ui->SessionStart()
//  3. Uncomment /control/execute visSetup_Simplex.mac in GlobalSetup.mac


//int G_Energy ;
//std::string G_Particle ;
//bool G_FromCommandLine ;

//-------------------------------------------------------------------------------
int main( int argc, char** argv )
//-------------------------------------------------------------------------------
{
    if(argc==4){
	UserCommandLine::Particle=argv[1];
	UserCommandLine::Energy=atoi(argv[2]);
	UserCommandLine::Timestamp=argv[3];
	UserCommandLine::Input = true;
	std::cout << "Running with "<< Particle << " at " << Energy << " GeV" << std::endl;
    }

    // ============= [ Setting up the application environment ] ================
    G4String nameUISession = "tcsh";                   // UI Session (qt/tcsh/...)
    G4String nameMainMacro = "GlobalSetup.mac";      // Initialization  macros
    // =========================================================================
    if(UserCommandLine::Input){
	nameMainMacro = Form("./data/%s/%dGeV/GlobalSetup_%s.mac", UserCommandLine::Particle.c_str(), UserCommandLine::Energy, UserCommandLine::Timestamp.c_str());
    }

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
    //G4UIExecutive* ui = new G4UIExecutive(argc, argv, nameUISession);
    uiManager->ApplyCommand("/control/execute " + nameMainMacro);
    //ui->SessionStart();

    // Job termination
    //delete ui;
    delete visManager;
    delete runManager;

    return 0;
}
