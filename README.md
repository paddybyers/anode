## Introduction

Anode is an embryonic framework for running node.js applications on Android. There are two main parts to this:

- a port of [node.js](https://github.com/joyent/node) to the Android OS and libraries. The code is [here](https://github.com/paddybyers/node);

- a set of Android projects (this repo) that provide the integration with the Android frameworks.

Anode builds to an Android application package (.apk) that encapsulates the node.js runtime and can run node.js applications through an intent-based API.

## Status

This work is at an early stage. All input is welcome.

The current target is to support node.js applications, invoked by intent. Multiple instances can be run in parallel. Modules (or addons) for node.js written in JavaScript or native are supported, and support for modules implemented in Java is on the roadmap.

This framework depends on a port of node [here](https://github.com/paddybyers/node).

## More information

Please see the [wiki](https://github.com/paddybyers/anode/wiki/Anode) for more information.
