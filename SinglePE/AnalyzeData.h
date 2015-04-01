//
//  AnalyzeData.h
//  
//
//  Created by karingilje on 3/6/15.
//
//

#ifndef ____AnalyzeData__
#define ____AnalyzeData__

// standard c includes
#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <cstring>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TPad.h"
#include "TLegend.h"

// Style file
#include "ProspectStyle.H"

// Books the tree and branches
void Initialize(TString);

// Spits out the usage of the program
void Usage(void);

// The Event Loop

// Writes objects to file
void Finalize(void);

//------------------------------

// Input Chain
TChain* fInputTree;

// Variables in the input tree
//------------------------------

// The event id
unsigned int fEventId;

// The Id of the digitizer board
unsigned int fBoardId;

// The number of channels used
unsigned int fNChannels;

// The channel ids (should be same size as fEventData).
std::vector<int>* fChannelIds = new std::vector<int>;

// The Integrated Energy (by channel)
std::vector<double>* fChannelEnergy = new std::vector<double>;

// The PSD (tail fraction) (by channel)
std::vector<double>* fChannelPSD = new std::vector<double>;

// The noise baseline (by channel)
std::vector<double>* fChannelBaseline = new std::vector<double>;

// The time of the event (by channel)
std::vector<unsigned int>* fChannelTime = new std::vector<unsigned int>;

// The trigger time offset from start of run
unsigned int fTrigTime;

#endif /* defined(____AnalyzeData__) */
