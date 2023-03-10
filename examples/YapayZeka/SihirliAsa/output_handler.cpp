/* 
Adapted by Andri Yadi.
Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "output_handler.h"

#include "Arduino.h"


void HandleOutput(tflite::ErrorReporter* error_reporter, int kind) {
  // The first time this method runs, set up our LED
  static bool is_initialized = false;
  if (!is_initialized) {
    is_initialized = true;
    
  }

  // Print some ASCII art for each gesture
  if (kind == 0) {
    error_reporter->Report(
        "\n\r*         *         *\n\r *       * *       "
        "*\n\r  *     *   *     *\n\r   *   *     *   *\n\r    * *       "
        "* *\n\r     *         *\n\r\n\r");

    error_reporter->Report("\n\r");
    error_reporter->Report("╔══════════════════════╗");
    error_reporter->Report("║             KANAT                    ║");
    error_reporter->Report("╚══════════════════════╝\n\r");
    delay(1500);

    
} else if (kind == 1) {
    error_reporter->Report(
        "\n\r          *\n\r       *     *\n\r     *         *\n\r "
        "   *           *\n\r     *         *\n\r       *     *\n\r      "
        "    *\n\r");

    error_reporter->Report("\n\r");
    error_reporter->Report("╔══════════════════════╗");
    error_reporter->Report("║               HALKA                  ║");
    error_reporter->Report("╚══════════════════════╝\n\r");
    delay(1500);
   
  } else if (kind == 2) {

    error_reporter->Report(
        "\n\r        *\n\r       *\n\r      *\n\r     *\n\r    "
        "*\n\r   *\n\r  *\n\r * * * * * * * *\n\r");

    error_reporter->Report("\n\r");
    error_reporter->Report("╔══════════════════════╗");
    error_reporter->Report("║              EĞİM                    ║");
    error_reporter->Report("╚══════════════════════╝\n\r");
    delay(1500);

  } else {
  }  
}
