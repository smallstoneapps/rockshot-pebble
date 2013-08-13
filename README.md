## RockShot // Pebble Library

This is the Pebble library that enables RockShot capabilities in your app or watchface.

To use the library, download the source files and add them to your project. Alternatively, you can add the repository as a submodule using  `git submodule`.

### Toggle

There's no reason to have RockShot functionality always enabled, so I reccomend setting a flag a config file and wrapping all RockShot related code in conditionals.

    #define ROCKSHOT true

### Include

You'll need to include `rockshot.h` in your main sourc file.

```c
#if ROCKSHOT
#include "rockshot.h"
#endif
```

### Main

In `pbl_main()`, after you've declared the `handlers` structure, but before you call `app_event_loop()`:

```c
#if ROCKSHOT
rockshot_main(&handlers);
#endif
```

### Init

In your `init_handler` function, 

```c
#if ROCKSHOT
http_capture_init(ctx);
#endif
```

--

It's as simple as that!
