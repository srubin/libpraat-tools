//
//  main_praatpitch.cpp
//  praatsynth
//
//  Created by srubin on 8/8/14.
//  Copyright (c) 2014 srubin. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include "sndfile.h"
#include "Sound.h"
#include "Sound_to_Pitch.h"
#include "Pitch_to_PitchTier.h"
#include "RealTier.h"
#include "PitchTier.h"
#include "Pitch.h"
#include "LongSound.h"
#include "melder.h"
#include "Manipulation.h"
#include "NUMmachar.h"

// command line parsing
#include <tclap/CmdLine.h>

using namespace std;

#define SAMPLERATE 44100
#define SAMPLERATE_f 44100.0
#define SEMITONE_MULTIPLIER 1.05946

int main(int argc, const char * argv[])
{
  NUMmachar();

  try {
    TCLAP::CmdLine cmd("Praat speech resynthesis", ' ', "0.1");
    TCLAP::ValueArg<string> audioArg("i", "input-audio", "Input audio filename", true, "", "path");
    TCLAP::ValueArg<string> outAudioArg("o", "output-audio", "Output audio filename", true, "", "path");
    TCLAP::ValueArg<string> pitchCSVArg("p", "pitch", "Pitch CSV", false, "", "path");
    TCLAP::ValueArg<string> durationCSVArg("d", "duration", "Duration CSV", false, "", "path");
        
    cmd.add(audioArg);
    cmd.add(outAudioArg);
    cmd.add(pitchCSVArg);
    cmd.add(durationCSVArg);
        
    cmd.parse(argc, argv);
        
    // load input audio file
    size_t size = audioArg.getValue().length() + 1;
    wchar_t path[size];
    mbstowcs(path, audioArg.getValue().c_str(), size);
    MelderFile mf;
    structMelderFile file2 = { 0 };
    Melder_relativePathToFile(path, &file2);
    mf = &file2;
        
    // create Praat Sound and Manipulation
    Sound snd = Sound_readFromSoundFile(mf);
    Manipulation manip = Sound_to_Manipulation(snd, 0.01, 75, 600);
    NUMrandom_init();
        
    // load pitch CSV if it exists
    string pitchCSVPath = pitchCSVArg.getValue();
    string str;
    string delim = ",";
    bool bPitchCSV = (pitchCSVPath != "");
    if (bPitchCSV) {
      ifstream pitchFile(pitchCSVPath);
      vector<pair<double,double>> newPitchPoints;
            
      while (std::getline(pitchFile, str))
	{
	  double time = stod(str.substr(0, str.find(delim)));
	  double pitch = stod(str.substr(str.find(delim) + 1));
	  newPitchPoints.push_back(make_pair(time, pitch));
	}
      pitchFile.close();
            
      Collection_removeAllItems(manip->pitch->points);
            
      for (pair<double, double> p : newPitchPoints) {
	RealTier_addPoint(manip->pitch, p.first, p.second);
      }
    }
        
    // load duration CSV if it exists
    string durationCSVPath = durationCSVArg.getValue();
    bool bDurationCSV = (durationCSVPath != "");
    if (bDurationCSV) {
      ifstream durationFile(durationCSVPath);
      vector<pair<double,double>> newDurationPoints;
            
      while (std::getline(durationFile, str))
	{
	  double time = stod(str.substr(0, str.find(delim)));
	  double pitch = stod(str.substr(str.find(delim) + 1));
	  newDurationPoints.push_back(make_pair(time, pitch));
	}
      durationFile.close();
            
      Collection_removeAllItems(manip->duration->points);
            
      for (pair<double, double> p : newDurationPoints) {
	RealTier_addPoint(manip->duration, p.first, p.second);
      }
    }

    // load output audio file
    size = outAudioArg.getValue().length() + 1;
    wchar_t outPath[size];
    mbstowcs(outPath, outAudioArg.getValue().c_str(), size);
    Sound snd2 = Manipulation_to_Sound(manip, Manipulation_OVERLAPADD);
    MelderFile outFile;
    structMelderFile outFile2 = { 0 };
    Melder_relativePathToFile(outPath, &outFile2);
    outFile = &outFile2;
    Sound_writeToAudioFile(snd2, outFile, Melder_WAV, 16);
        
  } catch (TCLAP::ArgException &e) {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }
    
  return 0;
}

