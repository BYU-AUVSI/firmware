# Welcome to ROSflight

This paragraph explains in a nutshell what ROSflight does and why it could be perfect for you!

ROSflight is an open-source autopilot firmware for the popular Naze32 autopilot and its variants.

## Why ROSflight?
This paragraph explains why we felt like the world needed ROSflight.

While other open-source autopilot projects have produced incredible and useful tools, we found that the specific type of functionality we required as researchers was either missing, insufficient, or frustratingly complicated in existing firmware projects. In particular, we required the ability to tightly integrate a low-level autopilot with an onboard computer for developing and testing high-level guidance, navigation, and control algorithms.

ROSflight was created to fill a gap that, in our experience, is not filled by other open-source autopilot firmwares: the ability to tightly integrate with on onboard computer. For us, this means two things:

  1. The ability to stream sensor data from the autopilot at a high rate, and
  2. A straightforward and native method for sending control setpoints to the autopilot.


## Objectives
This paragraph explains what ROSflight is (and perhaps more importantly, isn't) trying to accomplish.

The goal of ROSflight is to provide a stable, high-performance, low-level autopilot that is as simple as possible. A large part of our frustration with other firmwares was the sheer size and complexity of the code base that is a natural result of supporting a wide variety of use cases. Our goal, therefore, was to create an autopilot that does only a few things, but does them well.

Here's what ROSflight does:
  - Stream high-rate sensor data to the onboard computer
  - Provide attitude and rate control for multirotor vehicles
  - Provide servo and motor command outputs for fixed-wing vehicles
  - Accept control setpoints from an onboard computer and/or RC
  - Allow a safety pilot to take control of the vehicle at any time via RC
  - Implement all onboard estimation and control using peer-reviewed algorithms
  - Provide hardware abstraction to facilitate SIL simulation and adaptation to other autopilot hardware

As one of the goals of ROSflight is to be as simple as possible, perhaps even more important than what ROSflight does is what ROSflight *doesn't* do:
  - Provide any type of high-level guidance or control (waypoint following, position hold, etc.)
  - Basically anything that can feasibly be implemented on the onboard computer
