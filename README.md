# RockShot // Pebble Library

This is the Pebble library that enables RockShot capabilities in your app or watchface.

#### Heads Up

This version of the RockShot Pebble library is only compatible with the new Pebble SDK 2.0 BETA 1. 

*It will not work with the stable Pebble SDK 1.X*.

## Installation

First, grab the RockShot Pebble library files, ```rockshot.h``` and ```rockshot.c```. Alternatively, you can include this repository as a submodule with ```git submodule```.

Then you'll need to include them in your Pebble app code.

    #include "rockshot.h"
    
## Usage

The exact details of how to include RockShot on your app depends on whether you're already using AppMessage or AppSync functionality.

See the different sections below for the one appropriate to your app.

There are also some common usage steps for all types of apps. They are below:

### Common

In your app's init function:

    rockshot_init();

For each window that you want to be able to capture with RockShot, you'll need to create a layer on it. The best place to do this is just before you start adding layers to the window, probably in the window's load event.

    layer_rockshot = rockshot_create_layer(window);
    
Note that the function returns a layer pointer (Layer*), therefore you'll need to handle destroying the layer yourself.

    layer_destroy(layer_rockshot);
    
#### Optional

If you want to trigger screenshot requests from your Pebble app code, you can do so:

    rockshot_capture_single();
    
### No AppMessage or AppSync

Straight after you call ```rockshot_init```:

    rockshot_setup_no_app_message();

### AppMessage

After you've called ```app_message_open```:

    rockshot_setup_app_message(app_message_outbox_size_maximum());
    
The exact value you pass to the function should be whatever you've passed to ```app_message_open``` as the outbox size. It can be any value, as long as it's at least 128.

In your ```Inbox Received``` event handler:

    rockshot_handle_app_message(iter);
    
Where ```iter``` is the DictionaryIterator* argument.
  
### AppSync

The same first step as AppMessage:

    rockshot_setup_app_message(app_message_outbox_size_maximum());
