//
//  CalibrateData.h
//  
//
//  Created by karingilje on 3/6/15.
//
//

#ifndef ____CalibrateData__
#define ____CalibrateData__

// standard c includes
#include <cstdio>
#include <stdlib.h>
#include <tmath.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <cstring>
#include <algorithm>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TPad.h"

// Books the tree and branches
void Initialize(void);

// Spits out the usage of the program
void Usage(void);

// The Event Loop

// Writes objects to file
void Finalize(double);

//------------------------------
// Input File
TFile* fInputFile;

// Input Tree
TTree* fInputTree;

// Variables in the input tree
//------------------------------

// The Id of the digitizer board
unsigned int fBoardId;

// The number of channels used
unsigned int fNChannels;

// The channel masking
unsigned int fChannelMask;

// The channel ids (should be same size as fEventData).
std::vector<int>* fChannelIds = new std::vector<int>;

// The number of readouts per channel
unsigned int fChannelSize;

// The map between the channel id and the vector of data points
std::map<int, std::vector<unsigned int>* > fChannelData;

// The Individual Channel Vectors.
std::vector<unsigned int>* fChannelData0 = new std::vector<unsigned int>;
std::vector<unsigned int>* fChannelData1 = new std::vector<unsigned int>;
std::vector<unsigned int>* fChannelData2 = new std::vector<unsigned int>;
std::vector<unsigned int>* fChannelData3 = new std::vector<unsigned int>;
std::vector<unsigned int>* fChannelData4 = new std::vector<unsigned int>;
std::vector<unsigned int>* fChannelData5 = new std::vector<unsigned int>;
std::vector<unsigned int>* fChannelData6 = new std::vector<unsigned int>;
std::vector<unsigned int>* fChannelData7 = new std::vector<unsigned int>;

// Include time vector
std::vector<unsigned int>* fChannelTime = new std::vector<unsigned int>;

// The event id
unsigned int fEventId;

// The trigger time offset from start of run
unsigned int fTrigTime;

//------------------------------
// Output File
TFile* gOutputFile;

// Output Tree
TTree* gOutputTree;

// Variables in the output tree
//------------------------------

// The event id
unsigned int gEventId;

// The Id of the digitizer board
unsigned int gBoardId;

// The number of channels used
unsigned int gNChannels;

// The channel ids (should be same size as fEventData).
std::vector<int>* gChannelIds = new std::vector<int>;

// The Integrated Energy (by channel)
std::vector<double>* gChannelEnergy = new std::vector<double>;

// The Integrated Charge (by channel)
std::vector<double>* gChannelCharge = new std::vector<double>;

// The PSD (tail fraction) (by channel)
std::vector<double>* gChannelPSD = new std::vector<double>;

// The noise baseline (by channel)
std::vector<double>* gChannelBaseline = new std::vector<double>;

// Candidate single-PE values
std::vector<double>* gChannelPe = new std::vector<double>;

// The time of the event (by channel)
std::vector<unsigned int>* gChannelTime = new std::vector<unsigned int>;

// The trigger time offset from start of run
unsigned int gTrigTime;
double gECalib = 0.0;
double singlePeThreshold = 0.0;

#endif /* defined(____CalibrateData__) */
