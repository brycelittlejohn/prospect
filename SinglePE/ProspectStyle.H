#include "TStyle.h"
#include "TColor.h"

void SetGlobalStyle() {
  gStyle->SetPadTopMargin(0.16);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.13);
  
  //Set Font
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
  
  // do not display any of the standard histogram decorations
  //gStyle->SetOptStat(0);
  //gStyle->SetOptFit(0);
  
  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  // Add a greyscale palette for 2D plots
  int ncol=50;
  double dcol = 1./float(ncol+10);
  float hue = 300.0;
  float bright = 0.5;
  TColor **theCols = new TColor*[ncol];
  for (int i=0;i<ncol;i++) theCols[i] = new TColor(999-i,0.0,0.7,0.7);
  for (int j = 0; j < ncol; j++) {
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    float saturation = (ncol-j)*dcol;
    //float saturation = 1-(ncol-j)*dcol;
    TColor::HLS2RGB(hue,saturation,bright,r,g,b);
    theCols[j]->SetRGB(r,g,b);
    //std::cout << hue << " " << bright <<  " " << saturation << " " << r <<  " " << g << " " << b <<std::endl;
  }
  int ColJul[100];
  for  (int i=0; i<100; i++) ColJul[i]=999-i;
  gStyle->SetPalette(ncol,ColJul);

}
