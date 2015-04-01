#include <stdio>
#include <iostream>

#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TColor.h"

////////////////////////////////////////////////////////////
//
//  To run this code, take a data root file
//  $ root -l datafile.root
//  $ root [0]
//    Attaching file datafile.root as _file0... <- This will appear
//  $ root [1] .L TriggerViewer.C
//  $ root [2] TriggerViewer(0, 0, 0)
//
//  This will produce a plot of channel 0 on board 0 for event 0.
//  TriggerViewer( Event number, Board Number, Channel Number)
//
//  If the event or board or channel doesn't exist, a failure message
//  will be reported and no plot will be shown.
////////////////////////////////////////////////////////////

// This function will display one trace in a histogram
void TriggerViewer(int eventId, int boardId, int channelId) {
  
  // Some style settings to make things pretty :)
  gStyle->SetPadTopMargin(0.16);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.13);
  
  //Set Font to Times New Roman
  gStyle->SetTextFont(132);
  gStyle->SetTextSize(0.08);
  gStyle->SetLabelFont(132,"x");
  gStyle->SetLabelFont(132,"y");
  gStyle->SetLabelFont(132,"z");
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetTitleSize(0.06,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.06,"y");
  gStyle->SetLabelSize(0.05,"z");
  gStyle->SetTitleSize(0.06,"z");
  gStyle->SetLabelFont(132,"t");
  gStyle->SetTitleFont(132,"x");
  gStyle->SetTitleFont(132,"y");
  gStyle->SetTitleFont(132,"z");
  gStyle->SetTitleFont(132,"t");
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleX(0.50);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetTitleFont(132,"pad");
  gStyle->SetStatFont(132);
  gStyle->SetStatFontSize(0.08);
  
  gStyle->SetHatchesLineWidth(2);
  
  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  // Get the trace for the suggested event, board and channel.
  int nEvents = RawEventTree->Draw(TString::Format("channelData%i.channelData%i:channelTime.channelTime", channelId, channelId),TString::Format("eventId == %i && boardId == %i", eventId, boardId), "goff");
  
  // If the trace fails, let the user know
  if (nEvents == 0) {
    std::cout << "The channel selected does not exist! Returning..." << std::endl;
    return;
  }
  
  // Access the graph from the TTree->Draw function
  TGraph* trace = new TGraph(nEvents, RawEventTree->GetV2(), RawEventTree->GetV1());
  
  // Write axis Labels
  trace->GetXaxis()->SetTitle("Time (ns)");
  trace->GetYaxis()->SetTitle("Current (arb)");
  
  // Set the title to tell the user the event, board, and channel numbers.
  trace->SetTitle(TString::Format("Event %i: Board %i: Channel %i", eventId, boardId, channelId));
  
  // Set the marker and lines for appearance on the graph
  trace->SetMarkerStyle(8);
  trace->SetMarkerColor(kRed);
  trace->SetLineColor(kBlue);
  
  // Finally, plot!
  trace->Draw("APL");
  
  // At this point, the plot will be in a canvas open on your screen!
  // You can play with the axes (zoom in on interesting features)
  
  return;
  
}
