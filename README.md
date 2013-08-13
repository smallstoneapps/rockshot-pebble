## RockShot // Pebble Library

This is the Pebble library that enables RockShot capabilities in your app or watchface.

To use the library, download the source files and add them to your project. Alternatively, you can add the repository as a submodule using  `git submodule`.

### Toggle

There's no reason to have RockShot functionality always enabled, so I reccomend setting a flag a config file and wrapping all RockShot related code in conditionals.

    #define ROCKSHOT true

### Include

You'll need to include `rockshot.h` in your main sourc file.

    #if ROCKSHOT
    #include "rockshot.h"
    #endif

### Main

In `pbl_main()`, after you've 

#if ROCKSHOT
  http_capture_main(&handlers);
  #endif

### Init
