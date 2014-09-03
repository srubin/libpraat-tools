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
#include "AmplitudeTier.h"
#include <stdio.h>

// command line parsing
#include <tclap/CmdLine.h>

using namespace std;

#define SAMPLERATE 44100
#define SAMPLERATE_f 44100.0
#define SEMITONE_MULTIPLIER 1.05946

vector<pair<double, double>> pointPairsFromCSVFilename(string path)
{
    ifstream theFile(path);
    string str;
    string delim = ",";
    vector<pair<double, double>> newPoints;

    while (std::getline(theFile, str))
    {
        double time = stod(str.substr(0, str.find(delim)));
        double value = stod(str.substr(str.find(delim) + 1));
        cout << time << ' ' << value << endl;
        newPoints.push_back(make_pair(time, value));
    }
    theFile.close();
    return newPoints;
}

void replaceTierWithPoints(RealTier tier, vector <pair<double, double>> points)
{
    Collection_removeAllItems(tier->points);
    for (pair<double, double> p : points)
    {
        cout << p.first << " " << p.second << endl;
        RealTier_addPoint(tier, p.first, p.second);
    }
}

AmplitudeTier amplitudeTierFromPoints(vector <pair<double, double>> points)
{
    double timeStart = 0.0;
    double timeEnd = 0.0;
    for (pair<double, double> p : points) {
        if (p.second > timeEnd) {
            timeEnd = p.second;
        }
    }
    AmplitudeTier tier = AmplitudeTier_create(timeStart, timeEnd);
    replaceTierWithPoints((RealTier)tier, points);
    return tier;
}

void initPraat()
{
    // these things generally need to be called to initialize Praat's numeric libraries.
    // They may not actually all be needed, but it's a safe bet to include them.
    NUMmachar ();
    NUMinit ();
    NUMrandom_init();
    Melder_alloc_init ();
    Melder_message_init ();
}

int main(int argc, const char *argv[])
{
    initPraat();
    try
    {
        TCLAP::CmdLine cmd("Praat speech resynthesis", ' ', "0.1");
        TCLAP::ValueArg<string> audioArg("i", "input-audio", "Input audio filename", true, "", "path");
        TCLAP::ValueArg<string> outAudioArg("o", "output-audio", "Output audio filename", true, "", "path");
        TCLAP::ValueArg<string> pitchCSVArg("p", "pitch", "Pitch CSV", false, "", "path");
        TCLAP::ValueArg<string> durationCSVArg("d", "duration", "Duration CSV", false, "", "path");
        TCLAP::ValueArg<string> intensityCSVArg("e", "intesity", "Intensity CSV", false, "", "path");

        cmd.add(audioArg);
        cmd.add(outAudioArg);
        cmd.add(pitchCSVArg);
        cmd.add(durationCSVArg);
        cmd.add(intensityCSVArg);

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

        // load pitch CSV if it exists
        string pitchCSVPath = pitchCSVArg.getValue();
        if (pitchCSVPath != "")
        {
            vector<pair<double, double>> newPitchPoints = pointPairsFromCSVFilename(pitchCSVPath);
            replaceTierWithPoints((RealTier)manip->pitch, newPitchPoints);
        }



        // load duration CSV if it exists
        string durationCSVPath = durationCSVArg.getValue();
        if (durationCSVPath != "")
        {
            vector<pair<double, double>> newDurationPoints = pointPairsFromCSVFilename(durationCSVPath);
            replaceTierWithPoints((RealTier)manip->duration, newDurationPoints);
        }

        // load output audio file
        size = outAudioArg.getValue().length() + 1;
        wchar_t outPath[size];
        mbstowcs(outPath, outAudioArg.getValue().c_str(), size);
        Sound snd2 = Manipulation_to_Sound(manip, Manipulation_OVERLAPADD);

        // load intensity CSV if it exists
        string intensityCSVPath = intensityCSVArg.getValue();
        if (intensityCSVPath != "")
        {
            vector<pair<double, double>> newIntensityPoints = pointPairsFromCSVFilename(intensityCSVPath);
            AmplitudeTier ampTier = amplitudeTierFromPoints(newIntensityPoints);
            Sound_AmplitudeTier_multiply_inline(snd2, ampTier);
        }

        // output the file
        MelderFile outFile;
        structMelderFile outFile2 = { 0 };
        Melder_relativePathToFile(outPath, &outFile2);
        outFile = &outFile2;
        Sound_writeToAudioFile(snd2, outFile, Melder_WAV, 16);

    }
    catch (TCLAP::ArgException &e)
    {
        cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    }

    return 0;
}

