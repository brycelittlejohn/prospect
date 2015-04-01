//
//  UnpackData.cpp
//  
//
//  Created by karingilje on 3/6/15.
//
//

#include "UnpackData.h"

// This function describes the usage
void Usage() {
  std::cout << "Usage:" << std::endl;
  std::cout << " -n    Limit number of entries to convert." << std::endl;
  std::cout << " -o    Name of output file.  If not specified, .dat " << std::endl;
  std::cout << "       of the input file is changed to .root" << std::endl;
  std::cout << std::endl;
  
  std::cout << "./UnpackData <options> <rawDataName>.dat" << std::endl;
}


// This function initialized the tree and its branches
void Initialize() {
  fOutputTree = new TTree("RawEventTree", "A Tree Containing the Raw Data Information");
 
  fOutputTree->Branch("eventId", &fEventId, "eventId/i");
  fOutputTree->Branch("boardId", &fBoardId, "boardId/i");
  fOutputTree->Branch("nChannels", &fNChannels, "nChannels/i");
  fOutputTree->Branch("channelMask", &fChannelMask, "channelMask/i");
  fOutputTree->Branch("channelIds", "std::vector<int>", &fChannelIds);
  fOutputTree->Branch("channelSize", &fChannelSize, "channelSize/i");
  
  fOutputTree->Branch("channelData0", "std::vector<unsigned int>", &fChannelData0);
  fOutputTree->Branch("channelData1", "std::vector<unsigned int>", &fChannelData1);
  fOutputTree->Branch("channelData2", "std::vector<unsigned int>", &fChannelData2);
  fOutputTree->Branch("channelData3", "std::vector<unsigned int>", &fChannelData3);
  fOutputTree->Branch("channelData4", "std::vector<unsigned int>", &fChannelData4);
  fOutputTree->Branch("channelData5", "std::vector<unsigned int>", &fChannelData5);
  fOutputTree->Branch("channelData6", "std::vector<unsigned int>", &fChannelData6);
  fOutputTree->Branch("channelData7", "std::vector<unsigned int>", &fChannelData7);

  fOutputTree->Branch("channelTime", "std::vector<unsigned int>", &fChannelTime);

  fOutputTree->Branch("trigTime", &fTrigTime, "trigTime/i");
  
}

// The main function of the program
// argc is the number of input arguments
// argv are the actual arguments input
// So if we pass a rawDataName, it should be the last input
int main(int argc, char *argv[]) {
  
  // Pull the name of the input file as a TString
  // Should be last argument
  TString rawDataName = argv[argc-1];
  
  // Check that the input file name ends with .dat
  if (!rawDataName.EndsWith(".dat")) {
    std::cout << "Incorrect argument structure." << std::endl;
    std::cout << " Argument: " << argv[argc-1] << " must end in '.dat' " << std::endl;
    Usage();
    exit(1);
  }
  
  TString outputName(rawDataName);
  outputName.ReplaceAll(".dat", ".root");
  
  // Check for options
  // argv[0] is the command to run the code (ie ./UnpackData)
  // argv[argc-1] is the input file name.
  // More options could be added in the future.
  TString limitEvents;
  for (int i = 1; i < argc-1; i+=2) {
    if (std::strcmp(argv[i], "-n") == 0) {
      std::cout << "Limit events to " << argv[i+1] << std::endl;
      limitEvents = argv[i+1];
    }
    else if (std::strcmp(argv[i], "-o") == 0) {
      std::cout << "Set output file name to " << argv[i+1] << std::endl;
      outputName.Clear();
      outputName = argv[i+1];
    }
    else {
      std::cout << "Option not available!" << std::endl;
      Usage();
      std::cout << "Exiting ..." << std::endl;
      exit(1);
    }
  }
  
  // Open the input file
  std::ifstream rawDataFile (rawDataName.Data(), std::ios::binary);
  if (!rawDataFile.is_open()) {
    std::cout << "File not opened." << std::endl;
    exit(1);
  }
  
  // Check that the output file name ends with .root
  if (!outputName.EndsWith(".root")) {
    std::cout << "Incorrect argument structure." << std::endl;
    std::cout << " Argument: must end in '.root' " << std::endl;
    Usage();
    exit(1);
  }
  
  // Create the output root file
  fOutputFile = new TFile(outputName.Data(), "RECREATE");
  if (!fOutputFile->IsOpen()) {
    std::cout << "Output file not opened." << std::endl;
    exit(1);
  }
  
  // Initialize Tree information
  Initialize();
  
  // std::ios::beg from beginning of the stream
  // std::ios::cur from current position
  // std::ios::end from end of the stream
  
  // Calculate the total number of bytes in a file
  std::streampos begin = rawDataFile.tellg();
  rawDataFile.seekg(0, std::ios::end);
  std::streampos end = rawDataFile.tellg();
  
  std::cout << "The data file contains " << (end-begin) << " bytes." << std::endl;
  
  // Return to beginning of file.
  rawDataFile.seekg(0, std::ios::beg);
  
  // There are two DAQs, one of which outputs a lot of extra
  // header information at the beginning of a file.
  // For now, this information will not be saved.
  // In future, the ability to save this information should be added.
  
  // Header
  char * guiHeader = new char [4];
  rawDataFile.read(guiHeader, 4);
  
  // In the GUI daq, there will be a flag (0xb0) in byte 4.
  // In non-GUI daq, there will be a flag (0xa0) in byte 4.
  unsigned int guiHeaderFlag = (unsigned int) (unsigned char) guiHeader[3];
  if (guiHeaderFlag == 0xb0) {
    std::cout << "GUI DAQ header present." << std::endl;
    std::cout << "Skipping File Header information.." << std::endl;
    
    // Extract event size from GUI header
    // bytes 2, 1, 0 gives the number of 32-bit "words" in the event.
    unsigned int HeaderSize = (unsigned int) (unsigned char) guiHeader[3] * pow(16,6)
    - 0xB0000000 // Removes the flag of header start.
    + (unsigned int) (unsigned char) guiHeader[2] * pow(16,4)
    + (unsigned int) (unsigned char) guiHeader[1] * pow(16,2)
    + (unsigned int) (unsigned char) guiHeader[0];
    
    // There are four bytes in every 32-bit "word"
    HeaderSize *= 4;
    
    rawDataFile.seekg(0, std::ios::beg);
    char * fullHeader = new char [HeaderSize];
    rawDataFile.read(fullHeader, HeaderSize);
    
    // At this point std::ios::cur should point to the beginning of the first event.
  }
  else {
    // For the non-GUI DAQ we need to return to the beginning of the file.
    std::cout << "Non-GUI DAQ used." << std::endl;
    rawDataFile.seekg(0, std::ios::beg);
  }

  // Check that the end of the file has not been reached.
  while (rawDataFile.tellg() != end) {
  
    // Unpack header information
    // There are four parts to the header:
    // Header 0 - event size information
    // Header 1 - Digitizer board id, active channels
    // Header 2 - Event Counter
    // Header 3 - Trigger time
    
    // Header 0
    rawDataFile.seekg(0, std::ios::cur);
    char * header0 = new char [4];
    rawDataFile.read(header0, 4);
    // Header 1
    rawDataFile.seekg(0, std::ios::cur);
    char * header1 = new char [4];
    rawDataFile.read(header1, 4);
    // Header 2
    rawDataFile.seekg(0, std::ios::cur);
    char * header2 = new char [4];
    rawDataFile.read(header2, 4);
    // Header 3
    rawDataFile.seekg(0, std::ios::cur);
    char * header3 = new char [4];
    rawDataFile.read(header3, 4);
    
    // Check that header0 has new event flag
    // Byte 4 must be 0xa0- it is a flag assigned by Caen
    unsigned int headerFlag = (unsigned int) (unsigned char) header0[3];
    if (headerFlag != 0xa0) {
      std::cout << "Header flag not present at start of new event." << std::endl;
      std::cout << "Check input file!" << std::endl;
      exit(1);
    }
    
    // Extract event size from header0
    // bytes 2, 1, 0 gives the number of 32-bit "words" in the event.
    fEventSize = (unsigned int) (unsigned char) header0[3] * pow(16,6)
      - 0xA0000000 // Removes the flag of event start.
      + (unsigned int) (unsigned char) header0[2] * pow(16,4)
      + (unsigned int) (unsigned char) header0[1] * pow(16,2)
      + (unsigned int) (unsigned char) header0[0];
    
    // Subtract the header 32-bit "words" to get the number of data words
    fEventSize -= 4;
    // Convert the number of data 32-bit words to bytes
    fEventSize *= 4;

    // Board ID
    // This is the last 5 bits of an 8 bit number.
    // 0xF8 = 11111000 to mask out the unneccesary bits.
    // Then the bits must be shifted to the right (11111000 -> 00011111).
    fBoardId =((unsigned int) (unsigned char) header0[3]
                           & 0xF800) >> 3;
    
    // Channel Mask (first byte in header1)
    // Extract the channel Ids used and place into a vector.
    // This is done by comparing the mask to the eight channel ids.
    fChannelMask = (unsigned int) (unsigned char) header1[0];
    
    fChannelIds->clear();
    for (int i = 0; i < 8; i++) {
      if (fChannelMask & (1 << i)) fChannelIds->push_back(i);
    }
    
    fNChannels = fChannelIds->size();
    
    // Extract event number from header2
    fEventId = (unsigned int) (unsigned char) header2[2] * pow(16,4)
      + (unsigned int) (unsigned char) header2[1] * pow(16,2)
      + (unsigned int) (unsigned char) header2[0];
    
    if (fEventId%50000 == 0)
      std::cout << "Working on Event " << fEventId << " ... " << std::endl;
    
    // Extract trigger time from header3
    fTrigTime = (unsigned int) (unsigned char) header3[3] * pow(16,6)
      + (unsigned int) (unsigned char) header3[2] * pow(16,4)
      + (unsigned int) (unsigned char) header3[1] * pow(16,2)
      + (unsigned int) (unsigned char) header3[0];
    
    delete[] header0;
    delete[] header1;
    delete[] header2;
    delete[] header3;
    
    // Find the number of bytes saved per channel
    if (fEventSize%fNChannels != 0) {
      std::cout << "Number of bytes must be divisible by number of channels." << std::endl;
      exit(1);
    }
    
    fChannelSize = fEventSize / fNChannels;
    
    // Make sure vectors are empty initially.
    // This map is a super stupid hack . . . must be a better way.
    fChannelData.clear();
    
    fChannelData0->clear();
    fChannelData1->clear();
    fChannelData2->clear();
    fChannelData3->clear();
    fChannelData4->clear();
    fChannelData5->clear();
    fChannelData6->clear();
    fChannelData7->clear();
    
    fChannelTime->clear();
    
    fChannelData[0] = fChannelData0;
    fChannelData[1] = fChannelData1;
    fChannelData[2] = fChannelData2;
    fChannelData[3] = fChannelData3;
    fChannelData[4] = fChannelData4;
    fChannelData[5] = fChannelData5;
    fChannelData[6] = fChannelData6;
    fChannelData[7] = fChannelData7;
    
    // Loop through channel and save event information.
    for (unsigned int chan = 0; chan < fNChannels; chan++) {
      // Get entire event channel into memory
      rawDataFile.seekg(0, std::ios::cur);
      char * channelArr = new char [fChannelSize];
      rawDataFile.read(channelArr, fChannelSize);
      
      // Get this channel id.
      int chanId = fChannelIds->at(chan);
      
      // Extract channel data, by byte
      for (unsigned int i = 0; i < fChannelSize; i+=2) {
        unsigned int dataPoint = (unsigned int) (unsigned char) channelArr[i+1] * pow(16,2)
        + (unsigned int) (unsigned char) channelArr[i];
        fChannelData[chanId]->push_back(dataPoint);
        
        // Fill vector with time (do only once)
        // A reading is done every 4 ns.
        // i/2 is the same as fChannelData[chanId]->size()
        // Hence i*2 would give 4 ns separation between dataPoints
        if (chan == 0) fChannelTime->push_back(i*2);
      }
      
    } // End of loop through channels.
    
    fOutputTree->Fill();
    
    // Check if limited entries have been reached
    if (limitEvents.Atoi() > 0) {
      if (fOutputTree->GetEntries() >= limitEvents.Atoi()) break;
    }
    
  } // End of binary file
  
  // Close the binary file
  rawDataFile.close();

  Finalize();
  
  return 0;
}

void Finalize() {
  std::cout << "Reached end of analysis with " << fOutputTree->GetEntries() << " events." << std::endl;
  fOutputTree->Write();
}
