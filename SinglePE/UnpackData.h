//
//  UnpackData.h
//  
//
//  Created by karingilje on 3/6/15.
//
//

#ifndef ____UnpackData__
#define ____UnpackData__

// standard c includes
#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <tmath.h>
#include <cstring>
#include <algorithm>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

// Books the tree and branches
void Initialize(void);

// Spits out the usage of the program
void Usage(void);

// The Event Loop

// Writes objects to file
void Finalize(void);


// The event size
unsigned int fEventSize;

// Output File
TFile* fOutputFile;

// Output Tree
TTree* fOutputTree;

// The Id of the digitizer board
unsigned int fBoardId;

// Eventually add a list of boards used...
// Not needed for now (we have only one board).

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

// A vector containing the timing of the readings.
std::vector<unsigned int>* fChannelTime = new std::vector<unsigned int>;

// The event id
unsigned int fEventId;

// The trigger time offset from start of run
unsigned int fTrigTime;



#endif /* defined(____UnpackData__) */
