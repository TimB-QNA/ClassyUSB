# ClassyUSB
A C++ USB implementation for microcontrollers

** Please note that this is in very early development. Significant sections of the code may be buggy, inoperative, or liable to disappear without warning! **

ClassyUSB is developed on the Atmel SAM series of microcontrollers. It is portable to other platforms but you will need write your own hardware drivers.

This document is a brief introduction to allow you to use the library quickly and easily. More in-depth documentation is available in the "doc" directory.

* Assumptions
  * You are using an Atmel SAMD21 processor
  * You have configured the clock suitably for USB operation

## Using this library

Clone this repository (or, ideally, add it as a sumbodule) into your project. Add the contents of ClassyUSB/src in your project, then remove anything you don't need.

In your main program, include the header for the microcontroller you're using and create an instance of the usb device...

>#include "classyUSB/src/hardware/samD21/samD21usbDevice.h"
>
>samd21usbDevice usb;

In your main function configure the usb device with vendor IDs and product strings etc. (see src/usbDev.h). You must then call initialise() to actually configure the hardware, then you can start adding components (e.g. a CDC_ACM class).

To add a component, you need to be aware of how the components fit together. Typically there is a class and a subclass (or specialisation). These are treated in a general form by ClassyUSB, so you need to create a component for all the classes and subclasses. Usually only the subclass is of particular interest, and thus you might instantiate it thus:

>usbCDC_ACM vcp;
>usb.addComponent(new usbCDC(&vcp));

This both creates the USB class and adds it to the USB stack while giving you access to the virtual serial port (vcp). 
