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

### Init

```c
#if ROCKSHOT
rockshot_init();
#endif
```

#### Messaging Buffers

At the moment, RockShot uses a large buffer size in order to send the screenshot quickly. You'll need to make sure that your outgoing buffer size is set to at least 256.

### Init

In your `init_handler` function, 

```c
#if ROCKSHOT
rockshot_init(ctx);
#endif
```

--

It's as simple as that!
