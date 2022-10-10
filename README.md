# CharraEngine

This is my attempt to make a vulkan based renderer, currently I am working towards a functional GUI system.
At this point only windows has support, but I am trying to keep it easy to port to another system.

# TODO

## GUI

This is my current structure plan, very likely to change or be scrapped entireley.

### Containers

* Containers will hold relative and absolute locations on the screen, lists of components, data about layering/stacking of nearby containers, and cannot store other containers.  
* Basically just subdivisions of screenspace.   

### Components
* Components will be sorted based on whether they are active, or passive. Active and passive components can be abstracted into buttons, scrollbars, tabs, etc... Both types will store absolute and relative position data within containers.
* Active components will store listeners, conditionals, and actions.   
* Passive components will store text, images, and any other data.   

using containers to keep small groups of components separate can hopefully reduce redundant state checking, as well as making repositioning components easier.
Every window will need its own unique GUI object, this again modularizes the system and reduces complexity, in the future communication methods between components will be put into place. (order system...?)