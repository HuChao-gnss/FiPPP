#pragma ident "$Id$"

/**
 * @file TypeID.cpp
 * gpstk::TypeID - Identifies types of values
 * added many types 2023-03
 */

#include "TypeID.hpp"
#include "Exception.hpp"

using namespace std;

namespace gpstk
{

    const std::string TypeID::tStrings[count] =
    {
        "Unknown",
        "C1CG", "C1SG", "C1LG", "C1XG", "C1PG", "C1WG", "C1YG", "C1MG",
        "L1CG", "L1SG", "L1LG", "L1XG", "L1PG", "L1WG", "L1YG", "L1MG", "L1NG",
        "D1CG", "D1SG", "D1LG", "D1XG", "D1PG", "D1WG", "D1YG", "D1MG", "D1NG",
        "S1CG", "S1SG", "S1LG", "S1XG", "S1PG", "S1WG", "S1YG", "S1MG", "S1NG",
                                                                             
        "C2CG", "C2DG", "C2SG", "C2LG", "C2XG", "C2PG", "C2WG", "C2YG", "C2MG",
        "L2CG", "L2DG", "L2SG", "L2LG", "L2XG", "L2PG", "L2WG", "L2YG", "L2MG", "L2NG",
        "D2CG", "D2DG", "D2SG", "D2LG", "D2XG", "D2PG", "D2WG", "D2YG", "D2MG", "D2NG",
        "S2CG", "S2DG", "S2SG", "S2LG", "S2XG", "S2PG", "S2WG", "S2YG", "S2MG", "S2NG",
                                                     
        "C5IG", "C5QG", "C5XG",                      
        "L5IG", "L5QG", "L5XG",                      
        "D5IG", "D5QG", "D5XG",                      
        "S5IG", "S5QG", "S5XG",                      
                                                     
        "C1CR", "C1PR",                              
        "L1CR", "L1PR",                              
        "D1CR", "D1PR",                              
        "S1CR", "S1PR",                              
                                                     
        "C2CR", "C2PR",                              
        "L2CR", "L2PR",                              
        "D2CR", "D2PR",                              
        "S2CR", "S2PR",                              
                                                     
        "C3IR", "C3QR", "C3XR",                      
        "L3IR", "L3QR", "L3XR",                      
        "D3IR", "D3QR", "D3XR",                      
        "S3IR", "S3QR", "S3XR",                      
                                                     
        "C4AR", "C4BR", "C4XR",                      
        "L4AR", "L4BR", "L4XR",                      
        "D4AR", "D4BR", "D4XR",                      
        "S4AR", "S4BR", "S4XR",                      
                                                     
        "C6AR", "C6BR", "C6XR",                      
        "L6AR", "L6BR", "L6XR",                      
        "D6AR", "D6BR", "D6XR",                      
        "S6AR", "S6BR", "S6XR",                      
                                                     
        "C1AE", "C1BE", "C1CE", "C1XE", "C1ZE",      
        "L1AE", "L1BE", "L1CE", "L1XE", "L1ZE",      
        "D1AE", "D1BE", "D1CE", "D1XE", "D1ZE",      
        "S1AE", "S1BE", "S1CE", "S1XE", "S1ZE",      
                                                     
        "C5IE", "C5QE", "C5XE",                      
        "L5IE", "L5QE", "L5XE",                      
        "D5IE", "D5QE", "D5XE",                      
        "S5IE", "S5QE", "S5XE",                      
                                                     
        "C7IE", "C7QE", "C7XE",                      
        "L7IE", "L7QE", "L7XE",                      
        "D7IE", "D7QE", "D7XE",                      
        "S7IE", "S7QE", "S7XE",                      
                                                     
        "C8IE", "C8QE", "C8XE",                      
        "L8IE", "L8QE", "L8XE",                      
        "D8IE", "D8QE", "D8XE",                      
        "S8IE", "S8QE", "S8XE",                      
                                                     
        "C6AE", "C6BE", "C6CE", "C6XE", "C6ZE",      
        "L6AE", "L6BE", "L6CE", "L6XE", "L6ZE",      
        "D6AE", "D6BE", "D6CE", "D6XE", "D6ZE",      
        "S6AE", "S6BE", "S6CE", "S6XE", "S6ZE",      
                                                     
        "C1DC", "C1PC", "C1XC", "C1AC",              
        "L1DC", "L1PC", "L1XC", "L1AC", "L1NC",      
        "D1DC", "D1PC", "D1XC", "D1AC", "D1NC",      
        "S1DC", "S1PC", "S1XC", "S1AC", "S1NC",      
                                                     
        "C2IC", "C2QC", "C2XC",                      
        "L2IC", "L2QC", "L2XC",                      
        "D2IC", "D2QC", "D2XC",                      
        "S2IC", "S2QC", "S2XC",                      
                                                     
        "C5DC", "C5PC", "C5XC",                      
        "L5DC", "L5PC", "L5XC",                      
        "D5DC", "D5PC", "D5XC",                      
        "S5DC", "S5PC", "S5XC",                      
                                                     
        "C7IC", "C7QC", "C7XC",                      
        "L7IC", "L7QC", "L7XC",                      
        "D7IC", "D7QC", "D7XC",                      
        "S7IC", "S7QC", "S7XC",                      
                                                     
        "C7DC", "C7PC", "C7ZC",                      
        "L7DC", "L7PC", "L7ZC",                      
        "D7DC", "D7PC", "D7ZC",                      
        "S7DC", "S7PC", "S7ZC",                      
                                                     
        "C8DC", "C8PC", "C8XC",                      
        "L8DC", "L8PC", "L8XC",                      
        "D8DC", "D8PC", "D8XC",                      
        "S8DC", "S8PC", "S8XC",                      
                                                     
        "C6IC", "C6QC", "C6XC", "C6AC",              
        "L6IC", "L6QC", "L6XC", "L6AC",              
        "D6IC", "D6QC", "D6XC", "D6AC",              
        "S6IC", "S6QC", "S6XC", "S6AC",              
                                                     
        "C5AI", "C5BI", "C5CI", "C5XI",              
        "L5AI", "L5BI", "L5CI", "L5XI",              
        "D5AI", "D5BI", "D5CI", "D5XI",              
        "S5AI", "S5BI", "S5CI", "S5XI",              
                                                     
        "C9AI", "C9BI", "C9CI", "C9XI",              
        "L9AI", "L9BI", "L9CI", "L9XI",              
        "D9AI", "D9BI", "D9CI", "D9XI",              
        "S9AI", "S9BI", "S9CI", "S9XI",              
                                                     
        "C1CJ", "C1SJ", "C1LJ", "C1XJ", "C1ZJ",      
        "L1CJ", "L1SJ", "L1LJ", "L1XJ", "L1ZJ",      
        "D1CJ", "D1SJ", "D1LJ", "D1XJ", "D1ZJ",      
        "S1CJ", "S1SJ", "S1LJ", "S1XJ", "S1ZJ",      
                                                     
        "C2SJ", "C2LJ", "C2XJ",                      
        "L2SJ", "L2LJ", "L2XJ",                      
        "D2SJ", "D2LJ", "D2XJ",                      
        "S2SJ", "S2LJ", "S2XJ",                      
                                                     
        "C5IJ", "C5QJ", "C5XJ", "C5DJ", "C5PJ", "C5ZJ",
        "L5IJ", "L5QJ", "L5XJ", "L5DJ", "L5PJ", "L5ZJ",
        "D5IJ", "D5QJ", "D5XJ", "D5DJ", "D5PJ", "D5ZJ",
        "S5IJ", "S5QJ", "S5XJ", "S5DJ", "S5PJ", "S5ZJ",
                                             
        "C6SJ", "C6LJ", "C6XJ", "C6EJ", "C6ZJ",
        "L6SJ", "L6LJ", "L6XJ", "L6EJ", "L6ZJ",
        "D6SJ", "D6LJ", "D6XJ", "D6EJ", "D6ZJ",
        "S6SJ", "S6LJ", "S6XJ", "S6EJ", "S6ZJ",
                             
        "C1CS",              
        "L1CS",              
        "D1CS",              
        "S1CS",              
                             
        "C5IS", "C5QS", "C5XS",
        "L5IS", "L5QS", "L5XS",
        "D5IS", "D5QS", "D5XS",
        "S5IS", "S5QS", "S5XS",
             
        "C1G", "C2G", "C5G",
        "L1G", "L2G", "L5G",
        "D1G", "D2G", "D5G",
        "S1G", "S2G", "S5G",
                          
        "C1E", "C5E", "C7E", "C8E", "C6E",
        "L1E", "L5E", "L7E", "L8E", "L6E",
        "D1E", "D5E", "D7E", "D8E", "D6E",
        "S1E", "S5E", "S7E", "S8E", "S6E",
                                        
        "C1R", "C2R", "C3R", "C4R", "C6R",
        "L1R", "L2R", "L3R", "L4R", "L6R",
        "D1R", "D2R", "D3R", "D4R", "D6R",
        "S1R", "S2R", "S3R", "S4R", "S6R",
                                        
        "C1C", "C2C", "C5C", "C7C", "C8C", "C6C",
        "L1C", "L2C", "L5C", "L7C", "L8C", "L6C",
        "D1C", "D2C", "D5C", "D7C", "D8C", "D6C",
        "S1C", "S2C", "S5C", "S7C", "S8C", "S6C",
                                 
        "C5I", "C9I",            
        "L5I", "L9I",            
        "D5I", "D9I",            
        "S5I", "S9I",            
                                 
        "C1J", "C2J", "C5J", "C6J",
        "L1J", "L2J", "L5J", "L6J",
        "D1J", "D2J", "D5J", "D6J",
        "S1J", "S2J", "S5J", "S6J",
                   
        "C1S", "C5S",        
        "L1S", "L5S",        
        "D1S", "D5S",        
        "S1S", "S5S",

        "MW12G",  
        "MW21G", "MW25G", 
        "MW58E", "MW57E", "MW56E", "MW51E", 
        "MW27C", "MW26C", "MW28C", "MW25C", "MW21C", "MW61C",
        "MW21J", "MW25J",

        "MW51C", "MW52C", "MW56C", "MW58C",

        "MW15G",
        "MW15E", "MW17E", "MW18E", "MW16E",
        "MW15C", "MW12C", "MW16C", "MW18C", "MW17C",

        "updMW12G",

        // GF detection added by ragn wang
        "GF12G", "GF15E", "GF26C", "GF27C",
        "GF15G",
        "GF17E", "GF18E", "GF16E",
        "GF15C", "GF12C", "GF16C", "GF18C", "GF17C", "GF52C",
        "GF21C", "GF25C", "GF28C", "GF61C",

        // Cycle Slip
        "CSFlag",

        // Prefit Residual
        "prefitC1G", "prefitC2G", "prefitC5G",
        "prefitL1G", "prefitL2G", "prefitL5G",
        "prefitC1E", "prefitC5E", "prefitC7E", "prefitC8E", "prefitC6E",               
        "prefitL1E", "prefitL5E", "prefitL7E", "prefitL8E", "prefitL6E",               
        "prefitC1R", "prefitC2R", "prefitC3R", "prefitC4R", "prefitC6R",               
        "prefitL1R", "prefitL2R", "prefitL3R", "prefitL4R", "prefitL6R",               
                                                                      
        "prefitC1C", "prefitC2C", "prefitC5C", "prefitC7C", "prefitC8C", "prefitC6C",             
        "prefitL1C", "prefitL2C", "prefitL5C", "prefitL7C", "prefitL8C", "prefitL6C",             
                                                         
        "prefitC1J", "prefitC2J", "prefitC5J", "prefitC6J", 
        "prefitL1J", "prefitL2J", "prefitL5J", "prefitL6J",

        "PC12G", "LC12G",
        "PC21G", "LC21G",
        "PC51E", "LC51E",
        "PC26C", "LC26C",

        "prefitPC12G", "prefitLC12G",
        "prefitPC21G", "prefitLC21G",
        "prefitPC51E", "prefitLC51E",
        "prefitPC26C", "prefitLC26C",

        "diffPrefitLC12G",

        // new combination type id added by ragnwang
        // ionospheric pairwise combination
        "PC15G", "LC15G",
        "PC15E", "LC15E", "PC17E", "LC17E", "PC18E", "LC18E", "PC16E", "LC16E",
        "PC27C", "LC27C", "PC21C", "LC21C", "PC25C", "LC25C", "PC28C", "LC28C",
        "PC61C", "LC61C",
        // NEW BDS3 COMBINAITON
        "PC15C", "LC15C", "PC12C", "LC12C", "PC16C", "LC16C", "PC18C", "LC18C",
        "PC52C", "LC52C",
        // ionosphere free combination
        "PC125G", "LC125G",
        "PC157E", "LC157E", "PC1578E", "LC1578E", "PC15786E", "LC15786E",
        "PC158C", "LC158C", "PC152C", "LC152C", "PC1526C", "LC1526C", "PC15268C", "LC15268C",
        "PC267C", "LC267C", "PC261C", "LC261C", "PC2615C", "LC2615C", "PC26158C", "LC26158C",

        "prefitPC15G", "prefitLC15G",
        "prefitPC15E", "prefitLC15E", "prefitPC17E", "prefitLC17E", "prefitPC18E", "prefitLC18E", "prefitPC16E", "prefitLC16E",
        "prefitPC27C", "prefitLC27C", "prefitPC21C", "prefitLC21C", "prefitPC25C", "prefitLC25C", "prefitPC28C", "prefitLC28C",
        "prefitPC61C", "prefitLC61C",
        // NEW BDS3 COMBINAITON
        "prefitPC15C", "prefitLC15C", "prefitPC12C", "prefitLC12C", "prefitPC16C", "prefitLC16C", "prefitPC18C", "prefitLC18C",
        "prefitPC52C", "prefitLC52C",
        // ionosphere free combination
        "prefitPC125G", "prefitLC125G",
        "prefitPC157E", "prefitLC157E", "prefitPC1578E", "prefitLC1578E", "prefitPC15786E", "prefitLC15786E",
        "prefitPC158C", "prefitLC158C",
        "prefitPC152C", "prefitLC152C", "prefitPC1526C", "prefitLC1526C", "prefitPC15268C", "prefitLC15268C",
        "prefitPC267C", "prefitLC267C", "prefitPC261C", "prefitLC261C", "prefitPC2615C", "prefitLC2615C", "prefitPC26158C", "prefitLC26158C",

        // Phase Ambiguity
        "BL1G", "BL2G", "BL5G",
        "BL1E", "BL5E", "BL7E", "BL8E", "BL6E", 
        "BL1C", "BL2C", "BL5C", "BL7C", "BL8C", "BL6C", 

        "BLC12G", 
        "BLC21G", 
        "BLC51E",
        "BLC26C",

        // new LC ambiguity
        "BLC15G",
        "BLC15E", "BLC17E", "BLC18E", "BLC16E",
        "BLC27C", "BLC21C", "BLC25C", "BLC28C",
        "BLC61C",

        "BLC15C", "BLC12C", "BLC16C", "BLC18C",
        "BLC52C",

        "BLC125G",
        "BLC157E", "BLC1578E", "BLC15786E",
        "BLC158C", "BLC152C", "BLC1526C", "BLC15268C",
        "BLC267C", "BLC261C", "BLC2615C", "BLC26158C",

        // phase ambiguity
        "BWL12G",
        "BWL21G", "BWL25G",
        "BWL58E", "BWL57E", "BWL56E", "BWL51E",
        "BWL27C", "BWL26C", "BWL28C", "BWL25C", "BWL21C",
        "BWL12J", "BWL25J",

        "updBL1G", "updBL2G", "updBL5G", 
        "updBL1E", "updBL5E", "updBL7E", "updBL8E", "updBL6E", 
        "updBL1C", "updBL2C", "updBL5C", "updBL7C", "updBL8C"," updBL6C", 
        "updBL1R", "updBL2R", "updBL3R", "updBL4R", "updBL6R", 
        "updBL1J", "updBL2J", "updBL5J", "updBL6J",  

        "updBL1GSat"," updBL2GSat"," updBL5GSat", 
        "updBL1ESat"," updBL5ESat"," updBL7ESat"," updBL8ESat"," updBL6ESat", 
        "updBL1CSat"," updBL2CSat"," updBL5CSat"," updBL7CSat"," updBL8CSat"," updBL6CSat", 
        "updBL1RSat"," updBL2RSat"," updBL3RSat"," updBL4RSat"," updBL6RSat", 
        "updBL1JSat"," updBL2JSat"," updBL5JSat"," updBL6JSat", 

        "updBWL12G",
        "updBWL21G"," updBWL25G", 
        "updBWL51E"," updBWL57E"," updBWL58E"," updBWL56E", 
        "updBWL27C", "updBWL26C", "updBWL28C", "updBWL25C", "updBWL21C",
                  
        "updBWL21GSat"," updBWL25GSat", 
        "updBWL51ESat"," updBWL57ESat", 
        "updBWL27CSat"," updBWL26CSat", 

        "ionoMapC1G", "ionoMapC2G", "ionoMapC5G",
        "ionoMapC1E", "ionoMapC5E", "ionoMapC7E", "ionoMapC8E", "ionoMapC6E",
        "ionoMapC1C", "ionoMapC2C", "ionoMapC5C", "ionoMapC7C", "ionoMapC8C", "ionoMapC6C",
        "ionoMapC1R", "ionoMapC2R", "ionoMapC3R", "ionoMapC4R", "ionoMapC6R",            
        "ionoMapC1J", "ionoMapC2J", "ionoMapC5J", "ionoMapC6J",                          
                                                                                         
        "ionoMapL1G", "ionoMapL2G", "ionoMapL5G",                                        
        "ionoMapL1E", "ionoMapL5E", "ionoMapL7E", "ionoMapL8E", "ionoMapL6E",            
        "ionoMapL1C", "ionoMapL2C", "ionoMapL5C", "ionoMapL7C", "ionoMapL8C", "ionoMapL6C",
        "ionoMapL1R", "ionoMapL2R", "ionoMapL3R", "ionoMapL4R", "ionoMapL6R",
        "ionoMapL1J", "ionoMapL2J", "ionoMapL5J", "ionoMapL6J",

        "slantIonoC1G", "slantIonoC2G", "slantIonoC5G",
        "slantIonoC1E", "slantIonoC5E", "slantIonoC7E", "slantIonoC8E", "slantIonoC6E",
        "slantIonoC1C", "slantIonoC2C", "slantIonoC5C", "slantIonoC7C", "slantIonoC8C", "slantIonoC6C",
        "slantIonoC1R", "slantIonoC2R", "slantIonoC3R", "slantIonoC4R", "slantIonoC6R",              
        "slantIonoC1J", "slantIonoC2J", "slantIonoC5J", "slantIonoC6J",                              
                                                                                                     
        "slantIonoL1G", "slantIonoL2G", "slantIonoL5G",                                              
        "slantIonoL1E", "slantIonoL5E", "slantIonoL7E", "slantIonoL8E", "slantIonoL6E",              
        "slantIonoL1C", "slantIonoL2C", "slantIonoL5C", "slantIonoL7C", "slantIonoL8C", "slantIonoL6C",
        "slantIonoL1R", "slantIonoL2R", "slantIonoL3R", "slantIonoL4R", "slantIonoL6R",
        "slantIonoL1J", "slantIonoL2J", "slantIonoL5J", "slantIonoL6J",

        "windUpL1G", "windUpL2G", "windUpL5G",
        "windUpL1E", "windUpL5E", "windUpL7E", "windUpL8E", "windUpL6E",
        "windUpL1C", "windUpL2C", "windUpL5C", "windUpL7C", "windUpL8C", "windUpL6C",
        "windUpL1R", "windUpL2R", "windUpL3R", "windUpL4R", "windUpL6R",
        "windUpL1J", "windUpL2J", "windUpL5J", "windUpL6J",


        "rcvCorrC1G", "rcvCorrC2G", "rcvCorrC5G",
        "rcvCorrC1E", "rcvCorrC5E", "rcvCorrC7E", "rcvCorrC8E", "rcvCorrC6E",
        "rcvCorrC1C", "rcvCorrC2C", "rcvCorrC5C", "rcvCorrC7C", "rcvCorrC8C", "rcvCorrC6C",
        "rcvCorrC1R", "rcvCorrC2R", "rcvCorrC3R", "rcvCorrC4R", "rcvCorrC6R",            
        "rcvCorrC1J", "rcvCorrC2J", "rcvCorrC5J", "rcvCorrC6J",                          
                                                                                         
        "rcvCorrL1G", "rcvCorrL2G", "rcvCorrL5G",                                        
        "rcvCorrL1E", "rcvCorrL5E", "rcvCorrL7E", "rcvCorrL8E", "rcvCorrL6E",            
        "rcvCorrL1C", "rcvCorrL2C", "rcvCorrL5C", "rcvCorrL7C", "rcvCorrL8C", "rcvCorrL6C",
        "rcvCorrL1R", "rcvCorrL2R", "rcvCorrL3R", "rcvCorrL4R", "rcvCorrL6R",
        "rcvCorrL1J", "rcvCorrL2J", "rcvCorrL5J", "rcvCorrL6J",

        "dcdtL1G", "dcdtL2G", "dcdtL5G", 
        "dcdtL1E", "dcdtL5E", "dcdtL7E", "dcdtL8E", "dcdtL6E",
        "dcdtL1C", "dcdtL2C", "dcdtL5C", "dcdtL7C", "dcdtL8C", "dcdtL6C",
        "dcdtL1R", "dcdtL2R", "dcdtL3R", "dcdtL4R", "dcdtL6R",
        "dcdtL1J", "dcdtL2J", "dcdtL5J", "dcdtL6J",

        // Geometric Distance
        "rho",
        "relativity",
        "gravDelay",

        "satPCenterC1G",
        "satPCenterC2G",
        "satPCenterC5G",

        "satPCenterC1E",
        "satPCenterC5E",
        "satPCenterC7E",
        "satPCenterC8E",
        "satPCenterC6E",

        "satPCenterC1C", 
        "satPCenterC2C", 
        "satPCenterC5C", 
        "satPCenterC7C", 
        "satPCenterC8C", 
        "satPCenterC6C",

        "satPCenterC1R", 
        "satPCenterC2R", 
        "satPCenterC3R", 
        "satPCenterC4R", 
        "satPCenterC6R",           

        "satPCenterC1J", 
        "satPCenterC2J", 
        "satPCenterC5J", 
        "satPCenterC6J",                       

        "satPCenterL1G",
        "satPCenterL2G",
        "satPCenterL5G",

        "satPCenterL1E",
        "satPCenterL5E",
        "satPCenterL7E",
        "satPCenterL8E",
        "satPCenterL6E",

        "satPCenterL1C", 
        "satPCenterL2C", 
        "satPCenterL5C", 
        "satPCenterL7C", 
        "satPCenterL8C", 
        "satPCenterL6C",

        "satPCenterL1R", 
        "satPCenterL2R", 
        "satPCenterL3R", 
        "satPCenterL4R", 
        "satPCenterL6R",           

        "satPCenterL1J", 
        "satPCenterL2J", 
        "satPCenterL5J", 
        "satPCenterL6J",                       

        "tropo", 
        "tropoWeight", 
        "dryTropo", 
        "dryMap", 
        "wetTropo",
        "wetMap",
        "tropoSlant", 
        "gradNMap",
        "gradEMap",

        "iono", 
        "ionoTEC", 
        "ionoMap", 

        "elevation", "azimuth", "height",

        "satArc",
        "weight", 

        "XECEF", "YECEF", "ZECEF",
        "recXECEF", "recYECEF", "recZECEF",     // added by ragn wang
        "satXECEF", "satYECEF", "satZECEF", "satVXECEF", "satVYECEF", "satVZECEF",

        "cdt", "cdtDot", "cdtSat", "cdtSatDot",

        "upd", "updSat",                       

        "dcdtGPS", "dcdtGAL", "dcdtBDS", "dcdtGLO", "dcdtQZS",
        "dcdtLC12G",

        "ifbC5G", 
        "ifbC8E", "ifbC7E", "ifbC6E", "ifbCE",
        "ifbC5C", "ifbC7C", "ifbC1C", "ifbC8C", "ifbC2C", "ifbC6C", "ifbCC", "ifbC26C",

        "ifcbL5G",
        "ifcbL7C", "ifcbL2C", "ifcbL6C", "ifcbL1C", "ifcbL5C", "ifcbL8C",
        "ifcbL1E", "ifcbL5E", "ifcbL7E", "ifcbL8E", "ifcbL6E",
        "E", "N", "U",  // added by ragn wang 2022/03/08
        
        "dN", "dE", "dU",
        "dLat", "dLon", "dH",
        "dX", "dY", "dZ",
        "vX", "vY", "vZ",

        "staPCON", "staPCOE", "staPCOU",

        "xpole", "ypole", "xpoleRate", "ypoleRate", "UT1mUTC", "LOD", 

        "beta", "mu", "yawangle", "phiangle", 

        "wavelengthL1R",
        "wavelengthL2R",

        "varC1", "varC2", "varL1", "varL2", "varPC12", "varLC12",
        "postfitPC12G", "postfitLC12G",
        "RL1", "RL2", "RLC",

        // File Type, added by ragn wang
        "OBSF", "CLKF", "EPHF", "SP3F", "BIASF", "ERPF", "SNXF", "IFCBF", "UPDF",
        // upd ewl wl nl
        "EWL", "WL", "NL",
        "MWLC", "VarMWLC", "FixNum", "ConNum",
        "prefitPCSD", "prefitLCSD"
    };


    // Explicit constructor
    TypeID::TypeID(std::string name)
    {
        for(int i=0; i<count; i++) 
        {
           if(tStrings[i] == name ) 
           {
              type = static_cast<ValueType>(i);
              return;
           }
        }

        // if it comes here, the type is unknown
        type = static_cast<ValueType>(0);

        InvalidType e("Unknown Type: " + name);
        GPSTK_THROW(e);

        return;

    }


    // Assignment operator
    TypeID TypeID::operator=(const TypeID& right)
    {
        if ( this == &right ) return (*this);
        (*this).type = right.type;
        return *this;
    }


    // Convenience output method
    std::ostream& TypeID::dump(std::ostream& s) const
    {
        s << TypeID::tStrings[type];
        return s;
    }


    // Return true if this is a valid TypeID. Basically just
    // checks that the enum is defined
    bool TypeID::isValid() const
    {
        return !(type==Unknown);
    }

    // convert this object to a string representation
    std::string TypeID::asString() const
    {
        std::ostringstream oss;
        oss << TypeID::tStrings[type];
        return oss.str();
    }


    namespace StringUtils
    {

         // convert this object to a string representation
        std::string asString(const TypeID& p)
        {
            std::ostringstream oss;
            p.dump(oss);
            return oss.str();

        }

    }// End of namespace StringUtils


    // stream output for TypeID
    std::ostream& operator<<(std::ostream& s, const TypeID& p)
    {
        p.dump(s);
        return s;
    }

} // End of namespace gpstk
