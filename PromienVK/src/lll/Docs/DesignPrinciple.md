Lennox's Lighting Library - Design Principle
____________________________________________

####Primary Motivation
This is, first and foremost, an attempt at making graphics _prototyping_ in a *Vulkan*-based framework more accessible.
The primary motivation for this project is to reduce the amount of boilerplate present in Vulkan, and bring it back to it's basic elements: shaders and data buffers.
A secondary objective to make it as flexible and performant as can be permitted, without compromising the primary objective.


####Design Principles
#####Sensible Defaults
We attempt to cover the most common use case with simpler and concise function/interfaces, but leave open the possibly for extensive customizations

#####Open Components
The redesigned Vulkan components are held in public structs, rather than opaque classes, allowing for more access than just createInfo structs, and make native code integratable with the lll pipeline when presented with the right struct.

#####Accessible Interface
The Vulkan handles, while avoiding the assignment/reference problem, offers very little in terms of functionalities. Child objects such as queues not knowing its own parent device, or queue index forces users to make accomodations in order to store this extraneously. This attempts to correct these design decisions.

#####High Performance
Aims to minimize overhead and maximize performance