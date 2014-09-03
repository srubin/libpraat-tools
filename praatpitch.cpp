//
//  praatpitch.cpp
//  praatpitch
//
//  Created by srubin on 8/8/14.
//  Copyright (c) 2014 srubin. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <string>
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

int main(int argc, const char * argv[])
{
  if (argc != 2) {
    return 1;
  }

  NUMmachar();
    
  size_t size = strlen(argv[1]) + 1;
  wchar_t path[size];
  mbstowcs(path, argv[1], size);

  MelderFile mf;
  structMelderFile file2 = { 0 };
  Melder_relativePathToFile(path, &file2);
  mf = &file2;
    
  Sound snd = Sound_readFromSoundFile(mf);
  Manipulation manip = Sound_to_Manipulation(snd, 0.01, 75, 600);
    
  int nPoints = manip->pitch->f_getNumberOfPoints();
  RealPoint *points = (RealPoint*)manip->pitch->points->item;
    
  for (int i = 1; i < nPoints + 1; i++) {
    double time = points[i]->number;
    double pitch = points[i]->value;
        
    std::cout << time << "," << pitch << std::endl;
        
  }
    
  return 0;
}

