//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B1DetectorConstruction.cc 75117 2013-10-28 09:38:37Z gcosmo $
//
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "B1DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
  // Envelope parameters
  //
  G4double env_sizeXY = 20*cm, env_sizeZ = 30*cm;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_WATER");
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     
  //     
  // Envelope
  //  
  G4Box* solidEnv =    
    new G4Box("Envelope",                    //its name
        0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ); //its size
      
  G4LogicalVolume* logicEnv =                         
    new G4LogicalVolume(solidEnv,            //its solid
                        env_mat,             //its material
                        "Envelope");         //its name
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
 
  //
  // Mylar sheet
  //
  
 
  //
  // Custom pipe
  //
  
  // Define pipe material
  G4double z, a, density;
  G4String name, symbol;
  G4int ncomponents, natoms;
  
  name = "Iron";
  G4Element* elFe = new G4Element(name, symbol="Fe", z=26., a=55.845*g/mole);
  
  name = "Carbon";
  G4Element* elC = new G4Element(name, symbol="C", z=6., a=12.011*g/mole);
  
  name = "Steel";
  G4Material* steel_mat = new G4Material(name, density=7730.14*kg/m3, ncomponents=2);
  steel_mat->AddElement(elFe, natoms=3);
  steel_mat->AddElement(elC, natoms=1);
  
  // Build mylar geometry
  G4Material* mylar_mat = nist->FindOrBuildMaterial("G4_MYLAR");
  G4ThreeVector posMylar = G4ThreeVector(0,0,0);
  G4double mylar_hX = 85.*mm,
           mylar_hY = 150.*mm,
           mylar_hZ = 250.*micrometer;
  G4Box* mylar =
    new G4Box("Mylar",
    mylar_hX, mylar_hY, mylar_hZ);

  G4LogicalVolume* logicMylar =
    new G4LogicalVolume(mylar,
                        mylar_mat,
                        "Mylar");
                        
  new G4PVPlacement(0,
                    posMylar,
                    logicMylar,
                    "Mylar",
                    logicEnv,
                    false,
                    0,
                    checkOverlaps);
                    
  // Set mylar as scoring volume
  fScoringVolume = logicMylar;

  // Build pipe geometry
  G4ThreeVector posPipe = G4ThreeVector(1, -1, 2);
  G4double pipe1_rmin = 2.*cm,
           pipe1_rmax = 7.*cm,
           pipe1_hz   = 3.*cm,
           pipe1_sPhi = 50.*deg,
           pipe1_dPhi = 210.*deg;
  G4Tubs* pipe1 =
    new G4Tubs("Pipe1",
    pipe1_rmin, pipe1_rmax, pipe1_hz, pipe1_sPhi, pipe1_dPhi);
    
  G4LogicalVolume* logicPipe1 =
    new G4LogicalVolume(pipe1,          //its solid
                        steel_mat,      //its material
                        "Pipe1");       //its name
  
  new G4PVPlacement(0,                  //no rotation
                    posPipe,               //at position
                    logicPipe1,         //its logicalvolume
                    "Pipe1",            //its name
                    logicEnv,           //its mother volume
                    false,              //no boolean operation
                    0,                  //copy number
                    checkOverlaps);     //overlaps checking
                
  // Set Pipe1 as scoring volume
  //
  //fScoringVolume = logicPipe1;

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
