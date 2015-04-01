//
//  CalibrateData.cpp
//  
//
//  Created by karingilje on 3/6/15.
//
//

#include "CalibrateData.h"

Double_t PeFit(Double_t *x,Double_t *par) {
  Double_t t1 = (x[0]-par[1])/par[2];
  Double_t t2 = (x[0]-par[1]*2)/(par[2]*1.41);
  Double_t t3 = (x[0]-par[1]*3)/(par[2]*1.73);
  Double_t t4 = (x[0]-par[1]*4)/(par[2]*2);
  Double_t t5 = (x[0])/par[4];

  return par[3]*exp(-0.5*t5*t5) + par[0]*exp(-0.5*t1*t1) + par[5]*exp(-0.5*t2*t2) + par[6]*exp(-0.5*t3*t3);
}

// This function describes the usage
void Usage() {
  std::cout << "Usage:" << std::endl;
  std::cout << " -n    Limit number of entries to read." << std::endl;
  std::cout << std::endl;
  
  std::cout << "./CalibrateData <options> <inputFileName>.root" << std::endl;
}


// This function initialized the tree and its SetBranchAddresses
// This lets us read all parts of the tree.
void Initialize() {

  // Switch back to first file to find the right tree
  fInputFile->cd();
  
  // Information from the Input Tree
  fInputTree = (TTree*)fInputFile->Get("RawEventTree");

  fInputTree->SetBranchAddress("eventId", &fEventId);
  fInputTree->SetBranchAddress("boardId", &fBoardId);
  fInputTree->SetBranchAddress("nChannels", &fNChannels);
  fInputTree->SetBranchAddress("channelMask", &fChannelMask);
  fInputTree->SetBranchAddress("channelIds", &fChannelIds);
  fInputTree->SetBranchAddress("channelSize", &fChannelSize);

  fInputTree->SetBranchAddress("channelData0", &fChannelData0);
  fInputTree->SetBranchAddress("channelData1", &fChannelData1);
  fInputTree->SetBranchAddress("channelData2", &fChannelData2);
  fInputTree->SetBranchAddress("channelData3", &fChannelData3);
  fInputTree->SetBranchAddress("channelData4", &fChannelData4);
  fInputTree->SetBranchAddress("channelData5", &fChannelData5);
  fInputTree->SetBranchAddress("channelData6", &fChannelData6);
  fInputTree->SetBranchAddress("channelData7", &fChannelData7);

  fInputTree->SetBranchAddress("channelTime", &fChannelTime);

  fInputTree->SetBranchAddress("trigTime", &fTrigTime);
  
  gOutputFile->cd();
  
  gOutputTree = new TTree("CaliEventTree", "A Calibrated Event Tree");
  
  gOutputTree->Branch("eventId", &gEventId, "eventId/i");
  gOutputTree->Branch("boardId", &gBoardId, "boardId/i");
  gOutputTree->Branch("nChannels", &gNChannels, "nChannels/i");
  
  gOutputTree->Branch("channelIds", "std::vector<int>", &gChannelIds);
  gOutputTree->Branch("channelCharge", "std::vector<double>", &gChannelCharge);
  gOutputTree->Branch("channelEnergy", "std::vector<double>", &gChannelEnergy);
  gOutputTree->Branch("channelPSD", "std::vector<double>", &gChannelPSD);
  gOutputTree->Branch("channelBaseline", "std::vector<double>", &gChannelBaseline);

  gOutputTree->Branch("channelPe", "std::vector<double>", &gChannelPe);

  gOutputTree->Branch("channelTime", "std::vector<unsigned int>", &gChannelTime);
  
  gOutputTree->Branch("trigTime", &gTrigTime, "trigTime/i");

}

// The main function of the program
// argc is the number of input arguments
// argv are the actual arguments input
// So if we pass a inputFileName, it should be the last input
int main(int argc, char *argv[]) {
  
  // Pull the name of the input file as a TString
  // Should be last argument
  TString inputFileName = argv[argc-1];
  
  // Check that the input file name ends with .root
  if (!inputFileName.EndsWith(".root")) {
    std::cout << "Incorrect argument structure." << std::endl;
    std::cout << " Argument: " << argv[argc-1] << " must end in '.root' " << std::endl;
    Usage();
    exit(1);
  }
  
  // Create the output file name
  TString outputFileName(inputFileName);
  outputFileName.ReplaceAll(".root", "-cali.root");
  
  // Check for options
  // argv[0] is the command to run the code (ie ./CalibrateData)
  // argv[argc-1] is the input file name.
  // More options could be added in the future.
   TString limitEvents;
   TString SsinglePeThreshold;

   for (int i = 1; i < argc-1; i+=2) {
    if (std::strcmp(argv[i], "-n") == 0) {
      std::cout << "Limit events to " << argv[i+1] << std::endl;
      limitEvents = argv[i+1];
    }
    else if (std::strcmp(argv[i], "-o") == 0) {
      std::cout << "Set output file name to " << argv[i+1] << std::endl;
      outputFileName.Clear();
      outputFileName = argv[i+1];
    }
    else if (std::strcmp(argv[i], "-t") == 0) {
      std::cout << "Threshhold for single-PE set to " << argv[i+1] << std::endl;
      SsinglePeThreshold = argv[i+1];
    }
    else {
      std::cout << "Option not available!" << std::endl;
      Usage();
      std::cout << "Exiting ..." << std::endl;
      exit(1);
    }
  }
  
  // Open file
  // The READ option means that we won't be able
  // to change the information in the file.
  fInputFile = new TFile(inputFileName.Data(),"READ");
  if (!fInputFile->IsOpen()) {
    std::cout << "The input root file failed to open" << std::endl;
    std::cout << "Returning ..." << std::endl;
    exit(1);
  }
  
  // Create the output root file
  gOutputFile = new TFile(outputFileName.Data(), "RECREATE");
  if (!gOutputFile->IsOpen()) {
    std::cout << "Output file not opened." << std::endl;
    exit(1);
  }
  
  // Initialize Tree information
  Initialize();
  
  // Get the number of events
  int nEntries = fInputTree->GetEntries();
  
  // Limit the number of events if neccessary
  if (limitEvents.Atoi() > 0) nEntries = limitEvents.Atoi();
  
  // Loop thought the events in the tree.
  for (int ev = 0; ev < nEntries; ev++) {
    
    // Get the entry
    fInputTree->GetEntry(ev);
    
    // Update progress on screen
    //if (ev%50000 == 0)
    //  std::cout << "Working on Event " << ev << " ... " << std::endl;
    
    // Save header information
    gEventId = fEventId;
    gBoardId = fBoardId;
    gNChannels = fNChannels;
    gTrigTime = fTrigTime;
    
    // This map is a super stupid hack . . . must be a better way.
    // But with this, we can map the channel id saved in fChannelIds
    // to a specific channel vector.
    fChannelData[0] = fChannelData0;
    fChannelData[1] = fChannelData1;
    fChannelData[2] = fChannelData2;
    fChannelData[3] = fChannelData3;
    fChannelData[4] = fChannelData4;
    fChannelData[5] = fChannelData5;
    fChannelData[6] = fChannelData6;
    fChannelData[7] = fChannelData7;
    
    // We need to make sure the vectors are empty here
    gChannelIds->clear();
    gChannelBaseline->clear();
    gChannelTime->clear();
    gChannelEnergy->clear();
    gChannelCharge->clear();
    gChannelPe->clear();
    gChannelPSD->clear();
    
    // For each event, we want to loop through the different channels and save information.
    for (unsigned int chan = 0; chan < fChannelIds->size(); chan++) {
      // Get this channel Id.
      int chanId = fChannelIds->at(chan);
      gChannelIds->push_back(chanId);
      
      // Define the start and end positions of the channel data vector
      std::vector<unsigned int>::iterator start = fChannelData[chanId]->begin();
      std::vector<unsigned int>::iterator end = fChannelData[chanId]->end();
      
      // Find the peak --- THIS ONLY EXPECTS ONE PEAK PER TRIGGER!!!!
      // Related to Pile-up see Section 4.3
      std::vector<unsigned int>::iterator minIt = std::min_element(start, end);
      
      // Calculate a baseline
      // Take the first 100 readings and average them for a noise measurement.
      // 100 is an arbitrary number!  You can play with it!
      // Related to Section 3.1
      int nNoiseSamples = 10;
      double baselineNoise = std::accumulate(start, start+nNoiseSamples, 0.0);
      baselineNoise /= (double)nNoiseSamples;
      gChannelBaseline->push_back(baselineNoise);
      
      // We should check if the peak is significantly different from noise.
      // i.e. we need to set a threshold for peak finding.
      // I'm setting the threshold to be 50 less than the baseline.
      // We have to fill the vectors with dummy values for reference later.
      if (*minIt > baselineNoise-50.0) {
        gChannelEnergy->push_back(-1.0);
        gChannelPe->push_back(-1.0);
        gChannelCharge->push_back(-1.0);
        gChannelPSD->push_back(-1.0);
        gChannelTime->push_back(-1.0);
        continue;
      }
      
      // I'm setting my event time at the peak. (i.e. minIt)
      // See Section 2.1 suggestions on how to make this better.
      // First find the index of the peak
      int peakIndex = std::distance(start, minIt);
      gChannelTime->push_back(fChannelTime->at(peakIndex));
      
      // I'm also setting t_0 to be 15 samples before the peak
      // and t_S 100 samples after the peak.  Check out Sections 3.1 and 3.2.
      
      // samples from peak to t_0
      int leadTime = 10;
      // samples from peak to t_S
      int trailTime = 100;
      
      // Energy is related to the sum of these points
      double charge = std::accumulate(minIt-leadTime, minIt+trailTime, 0.0);
      // Subtract the number of baselines
      charge -= (double)(leadTime+trailTime) * baselineNoise;
      // Reverse the sign
      charge = -charge;
      //Save charge
      gChannelCharge->push_back(charge);
      
      // Save the energy, this is in units of charge.
      // In order to extract an energy, we need to calibrate
      // the system and find a conversion factor see section ??
      gECalib = 1200/0.5; // channel per MeV
      gChannelEnergy->push_back(charge/gECalib);
      
      ////////
      //Trying to extract single-PE Peak in various ways
      ////////     

      //Try to extract single-PE peak from baseline region before main peak.  Unsuccessful thus far.
      int singlePeWidth = 5;   
      int singlePeWindow = 75;
      std::vector<unsigned int>::iterator minPeIt = std::min_element(start+nNoiseSamples+singlePeWidth,start+singlePeWindow-leadTime-singlePeWidth);
      //double singlePeCharge = std::accumulate(start, start+nNoiseSamples, 0.0);
      double singlePeCharge = std::accumulate(minPeIt-singlePeWidth,minPeIt+singlePeWidth, 0.0);
      singlePeCharge -= (double)(2*singlePeWidth)*baselineNoise;
      gChannelPe->push_back(singlePeCharge);

      //Try to extract single-PE peak from long tail after main peak.  Unsuccessful thus far.      
      //unsigned int peScanStart = 140;
      //unsigned int singlePeLead = 2; 
      //unsigned int singlePeTail = 6;   
      //unsigned int singlePeIso = 5; 
      //
      //double thisVal = 3999.0;
      //double prevVal = 3999.0;
      //double peCharge = 0;
      //
      ////Load threshhold value from command line string.  If 0, then set to default value of 5.0 samples.
      //singlePeThreshold = strtod(SsinglePeThreshold,NULL);
      ////std::cout<<singlePeThreshold<<std::endl;
      //if(singlePeThreshold == 0.0)
      //	singlePeThreshold = 5.0;
      //
      //bool isolated = true;
      //
      ////throw out if charge of main event is too high
      //if(charge>3000){	
      //	gChannelPe->push_back(-peCharge);	
      //	//std::cout<<"Too much energy!"<<std::endl;
      //	peCharge = -1;
      //}
      //else{
      //	for(unsigned int scan=peScanStart;scan<fChannelData[chanId]->size()-singlePeTail-singlePeIso;scan++){
      //	  thisVal = fChannelData[chanId]->at(scan);
      //	  //look for front edge
      //	  if(double(prevVal-thisVal)>double(singlePeThreshold)){
      //	    //check if isolated in front
      //	    for(unsigned int bscan=scan-singlePeLead-singlePeIso;bscan<scan-singlePeLead;bscan++){
      //	      if(abs(fChannelData[chanId]->at(bscan)-baselineNoise)>(singlePeThreshold)){
      //		isolated=false;
      //		break;
      //	      }
      //	    }
      //	    //check if isolated in back
      //	    for(unsigned int ascan=scan+singlePeTail;ascan<scan+singlePeTail+singlePeIso;ascan++){
      //	      if(abs(fChannelData[chanId]->at(ascan)-baselineNoise)>(singlePeThreshold)){
      //		isolated=false;
      //		break;
      //	      }
      //	    }
      //	    //if isolated, calculate peCharge.
      //	    if(isolated){
      //	      //calculate and store candidate single-pe charge value
      //	      for(unsigned int iscan=scan-singlePeLead;iscan<scan+singlePeTail;iscan++)
      //		peCharge+=fChannelData[chanId]->at(iscan)-baselineNoise;
      //	      prevVal = thisVal;	      
      //	      //std::cout<<"found a peak at "<<scan*4<<std::endl;
      //	      //std::cout<<chanId<<" "<<peCharge<<std::endl;      
      //	      gChannelPe->push_back(-peCharge);
      //	      peCharge = 0;
      //	    }
      //	    else
      //	      //std::cout<<"Not isolated, moving on."<<std::endl;
      //	    //jump ahead to get away from current peak and reset prevVal
      //	    scan+=singlePeTail;
      //	    prevVal = 3999;
      //	  } //if pe peak value above threshold
      //	  prevVal = thisVal;
      //	} //for prospective pe peak value
      //}	//else right charge range

      // Next, calculate the psd of the trace.
      // For now, I'm using the tail fraction method.
      // I'm just picking t_T to be 10 samples after the peak
      // See Section 4.1 for ideas on how to improve this!

      // samples from peak to t_T
      int tailTime = 5;
      
      double peakCharge = std::accumulate(minIt-leadTime, minIt+tailTime, 0.0);
      //Subtract the number of entries times the baseline
      peakCharge -= (double)(leadTime+tailTime) * baselineNoise;
      // Reverse the sign
      peakCharge = -peakCharge;
      //std::cout<<charge<<std::endl;
      //std::cout<<peakCharge<<std::endl;
      //std::cout<<"---------------"<<std::endl;

      // This number can be negative, if the peak is really
      // small and the "tail" ends up being noise.
      if (charge > 0 && peakCharge > 0)
        gChannelPSD->push_back((charge - peakCharge)/charge);
      else
        gChannelPSD->push_back(-1);

    }
    
    gOutputTree->Fill();
  }
  
  Finalize(singlePeThreshold);
  
  return 0;
}

void Finalize(double thresh) {
  std::cout << "Reached end of analysis." << std::endl;
  std::cout<<thresh<<std::endl;

  TCanvas *c1 = new TCanvas("c1");
  TH1F *h1 = new TH1F("h1",Form("Single Pe Peak, %.1f Sample Threshold",thresh),100,1,101);
  gOutputTree->Draw("channelPe>>h1","channelIds==0","");
  TF1 *fit1 = new TF1("f1",PeFit, 1,82,7);
  fit1->SetParameters(1500,20,10,1000,4,150,15);
  fit1->SetParLimits(1,12,40);
  fit1->SetParLimits(2,5,20);
  fit1->SetParLimits(4,3,5);
  fit1->SetParLimits(5,1,1000);
  fit1->SetParLimits(6,1,1000);
  //fit1->SetParameters(150,20,15,10000,4,10,10);
  //fit1->SetParameters(1200,25,10,130,10);
  c1->SetLogy();
  h1->Fit("f1","R");
  std::cout<<"FIT: "<<double(fit1->GetParameter(1))<<" "<<double(fit1->GetParameter(2))<<" "<<double(fit1->GetParError(1))<<" "<<double(fit1->GetParError(2))<<std::endl;  
  
  c1->SaveAs(Form("SinglePe_%.1f.pdf",thresh));

  gOutputTree->Write();
  gOutputFile->Close();
}
