#include <iostream>
#include <stdlib.h>
#include <string>
#include "sndfile.h"
#include "Sound.h"
#include "PointProcess.h"
#include "PitchTier_to_PointProcess.h"
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

// command line parsing
#include <tclap/CmdLine.h>

using namespace std;

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


int main(int argc, const char * argv[])
{
    initPraat();

    try
    {
        TCLAP::CmdLine cmd("Praat feature computation", ' ', "0.1");
        TCLAP::ValueArg<string> audioArg("i", "input-audio", "Input audio filename", true, "", "path");
        TCLAP::SwitchArg pitchArg("p", "pitch", "output pitch", false);
        TCLAP::SwitchArg amplitudeArg("a", "amplitude", "output amplitude", false);

        cmd.add(audioArg);
        cmd.xorAdd(pitchArg, amplitudeArg);

        cmd.parse(argc, argv);

        size_t size = strlen(audioArg.getValue().c_str()) + 1;
        wchar_t path[size];
        mbstowcs(path, audioArg.getValue().c_str(), size);

        MelderFile mf;
        structMelderFile file2 = { 0 };
        Melder_relativePathToFile(path, &file2);
        mf = &file2;

        Sound snd = Sound_readFromSoundFile(mf);

        Manipulation manip = Sound_to_Manipulation(snd, 0.01, 75, 600);
        int nPoints = manip->pitch->f_getNumberOfPoints();

        if (pitchArg.getValue()) {
            RealPoint *points = (RealPoint*)manip->pitch->points->item;

            for (int i = 1; i < nPoints + 1; i++) {
                double time = points[i]->number;
                double pitch = points[i]->value;
                cout << time << "," << pitch << endl;
            }
        } else if (amplitudeArg.getValue()) {
            Manipulation manip = Sound_to_Manipulation(snd, 0.01, 75, 600);
            PitchTier tier = manip->pitch;
            RealPoint *points = (RealPoint*)manip->pitch->points->item;
            // PointProcess process = PitchTier_to_PointProcess(tier);

            PointProcess process = PointProcess_create(tier->xmin, tier->xmax, tier->points->size);
            for (int i = 1; i < nPoints + 1; i++) {
                double time = points[i]->number;
                PointProcess_addPoint(process, time);
            }

            AmplitudeTier amp = PointProcess_Sound_to_AmplitudeTier_point(process, snd);

            points = (RealPoint*)amp->points->item;

            for (int i = 1; i < nPoints + 1; i++) {
                double time = points[i]->number;
                double amplitude = points[i]->value;
                cout << time << "," << amplitude << endl;
            }
        }

    }
    catch (TCLAP::ArgException &e)
    {
        cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    }

    return 0;
}

