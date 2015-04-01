//
//  AnalyzeData.cpp
//  
//
//  Created by karingilje on 3/6/15.
//
//

#include "AnalyzeData.h"

// This function describes the usage
void Usage() {
  std::cout << "Usage:" << std::endl;
  std::cout << " -n    Limit number of entries to read." << std::endl;
  std::cout << std::endl;
  
  std::cout << "./AnalyzeData <options> <inputFileName>.root" << std::endl;
}


// This function initialized the tree and its SetBranchAddresses
// This lets us read all parts of the tree.
void Initialize(TString fileList) {

  // Information from the Input Tree
  fInputTree = new TChain("CaliEventTree");

  // This will loop through your list of files and all of them to the chain
  std::ifstream in(fileList.Data());
  if (!in || in.bad()) {
    std::cout << "Missing input file: " << fileList.Data() << std::endl;
    return;
  }
  
  std::string file;
  while (in >> file) {
    if (in.fail()) break;
    if(file[0] == '#'){
      in.ignore(8192, '\n');
      continue;
    }
    
    fInputTree->Add(file.c_str());
  }
  in.close();
  
  fInputTree->SetBranchAddress("eventId", &fEventId);
  fInputTree->SetBranchAddress("boardId", &fBoardId);
  fInputTree->SetBranchAddress("nChannels", &fNChannels);
  
  fInputTree->SetBranchAddress("channelIds", &fChannelIds);
  fInputTree->SetBranchAddress("channelEnergy", &fChannelEnergy);
  fInputTree->SetBranchAddress("channelPSD", &fChannelPSD);
  fInputTree->SetBranchAddress("channelBaseline", &fChannelBaseline);

  fInputTree->SetBranchAddress("channelTime", &fChannelTime);

  fInputTree->SetBranchAddress("trigTime", &fTrigTime);
  
}

// The main function of the program
// argc is the number of input arguments
// argv are the actual arguments input
// So if we pass a inputFileName, it should be the last input
int main(int argc, char *argv[]) {
  
  // Pull the name of the input file as a TString
  // Should be last argument
  TString inputFileName = argv[argc-1];
  
  // Check that the input file name ends with .list
  // It should be a list of .root files that you want to analyze together
  if (!inputFileName.EndsWith(".list")) {
    std::cout << "Incorrect argument structure." << std::endl;
    std::cout << " Argument: " << argv[argc-1] << " must end in '.list' " << std::endl;
    Usage();
    exit(1);
  }

  // Check for options
  // argv->at(0) is the command to run the code (ie ./CalibrateData)
  // argv[argc-1] is the input file name.
  // More options could be added in the future.
  TString limitEvents;
  for (int i = 1; i < argc-1; i+=2) {
    if (std::strcmp(argv[i], "-n") == 0) {
      std::cout << "Limit events to " << argv[i+1] << std::endl;
      limitEvents = argv[i+1];
    }
    else {
      std::cout << "Option not available!" << std::endl;
      Usage();
      std::cout << "Exiting ..." << std::endl;
      exit(1);
    }
  }
  
  // Initialize Tree information
  Initialize(inputFileName);
  
  ///////////////////////////////////////////////////////////////////
  //
  // Define the histograms you are interested in looking at
  //
  ///////////////////////////////////////////////////////////////////
  
  SetGlobalStyle();
  
  // The range for energy might change depending on when the
  // data was taken and what the voltages, etc were.
  TH2F* PSDComp = new TH2F("PSDComp", "PSD Comparison; PSD A (arb.); PSD B (arb.)",
                           100, 0, 1.0, 100, 0, 1.0);
  
  TH1F* AvgPSD = new TH1F("AvgPSD", "Combined PSD; PSD (arb.); Entries",
                          100, 0, 1.0);
  
  TH1F* PMT0Energy = new TH1F("PMT0Energy","PMT 0 Energy; Energy; Entries",
                              100, 0, 10.0);
  TH1F* PMT1Energy = new TH1F("PMT1Energy","PMT 1 Energy; Energy; Entries",
                              100, 0, 10.0);
  TH1F* PMTCombEnergy = new TH1F("PMTCombEnergy","Combined PMT Energy; Energy; Entries",
                              100, 0, 10.0);
  
  TH2F* PSDvsEnergy = new TH2F("PSDvsEnergy","PSD vs Energy; Energy; PSD (arb.)",
                               100, 0, 10.0, 100, 0, 1.0);
  
  ///////////////////////////////////////////////////////////////////
  
  // Get the number of events
  int nEntries = fInputTree->GetEntries();
  
  // Limit the number of events if neccessary
  if (limitEvents.Atoi() > 0) nEntries = limitEvents.Atoi();
  
  // Loop thought the events in the tree.
  for (int ev = 0; ev < nEntries; ev++) {
    
    // Get the entry
    fInputTree->GetEntry(ev);
    
    // Update progress on screen
    if (ev%50000 == 0)
      std::cout << "Working on Event " << ev << " ... " << std::endl;
    
    // Right now we only have 2 PMTs, so make sure that is the case!
    // This will change in the future
    if (fChannelEnergy->size() < 2) {
      std::cout << "Incorrect size of array: " << fChannelEnergy->size() << "!" << std::endl;
      exit(1);
    }
    
    // Channel 0 and Channel 1 will be the PMTs attached to the cell.
    // If you have additional channels, they will be muon paddles,
    // but let's just focus on the cell PMTs for now...
    
    // First check that both PMTs have something deposited.
    if (fChannelEnergy->at(0) == -1 || fChannelEnergy->at(1) == -1) continue;
    
    // Then make sure that the PSD is reasonable
    if (fChannelPSD->at(0) == -1 || fChannelPSD->at(1) == -1) continue;
    
    // Combine the PMT psd and energy information.
    // Is this the best way??
    double psd = (fChannelPSD->at(0) + fChannelPSD->at(1))/2.0;
    //double totEnergy = TMath::Sqrt(fChannelEnergy->at(0) * fChannelEnergy->at(1));
    double totEnergy = (fChannelEnergy->at(0) + fChannelEnergy->at(1))/2.0;
    
    // Fill the histograms
    // Assume below 500 integrated charge is "noise"
    if (totEnergy > 0) {
      PSDComp->Fill(fChannelPSD->at(0), fChannelPSD->at(1));
      AvgPSD->Fill(psd);
    
      PMT0Energy->Fill(fChannelEnergy->at(0));
      PMT1Energy->Fill(fChannelEnergy->at(1));
      PMTCombEnergy->Fill(totEnergy);
      PSDvsEnergy->Fill(totEnergy,psd);
    }
  }
  
  ///////////////////////////////////////////////////////////////////
  //
  // Draw and print histograms
  //
  ///////////////////////////////////////////////////////////////////
  
  // Remove stats, make line thicker and draw
  AvgPSD->SetStats(false);
  AvgPSD->SetLineWidth(2);
  AvgPSD->Draw();
  gPad->Print("AvgPSD.pdf");

  // Remove Stats
  PMT0Energy->SetStats(false);
  PMT1Energy->SetStats(false);
  PMTCombEnergy->SetStats(false);

  // Make Line Thicker
  PMT0Energy->SetLineWidth(2);
  PMT1Energy->SetLineWidth(2);
  PMTCombEnergy->SetLineWidth(2);
  
  // Change Line Colors
  PMT0Energy->SetLineColor(kViolet);
  PMT1Energy->SetLineColor(kRed+2);
  
  // Rescale to 1.0 for all energies.
  PMT0Energy->Scale(1.0/PMT0Energy->GetEntries());
  PMT1Energy->Scale(1.0/PMT1Energy->GetEntries());
  PMTCombEnergy->Scale(1.0/PMTCombEnergy->GetEntries());
  
  // Draw PMT 0, PMT 1, and the combination on the same plots
  PMTCombEnergy->Draw();
  PMT0Energy->Draw("same");
  PMT1Energy->Draw("same");
  
  // Add a legend
  TLegend* leg = new TLegend(0.7, 0.7, 0.95, 0.84);
  leg->SetFillColor(kWhite);
  leg->SetTextFont(132);
  leg->AddEntry(PMT0Energy, "PMT 0", "l");
  leg->AddEntry(PMT1Energy, "PMT 1", "l");
  leg->AddEntry(PMTCombEnergy, "Combined Integral", "l");
  
  leg->Draw();
  
  gPad->Print("EnergyComparison.pdf");

  // Set the Z-axis (color) to be log
  gPad->SetLogz(true);
  // Change right margin to get the color bar on the plot
  gPad->SetRightMargin(0.11);
  gPad->Modified();
  gPad->Update();
  
  PSDComp->SetStats(false);
  PSDComp->Draw("COLZ");
  gPad->Print("PSDComp.pdf");
  
  PSDvsEnergy->SetStats(false);
  PSDvsEnergy->Draw("COLZ");
  gPad->Print("PSDvsEnergy.pdf");
  gPad->SetLogz(false);
  
  ///////////////////////////////////////////////////////////////////
  
  Finalize();
  
  return 0;
}

void Finalize() {
  std::cout << "Reached end of analysis." << std::endl;
}
