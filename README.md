# finder
Embedded Software Project

## Description
This repository contains the embedded software project called "finder." The project focuses on controlling __LED__ and __MOTOR__ `driver modules` through a main program.

1. __Target System Kernel Install__ - How to install the kernel on the Raspberry Pi 4B used in the project

    reference : https://wikidocs.net/3243 is how the kernel is built.

2. __Components__ - The project consists of the following components

    `Motor driver` : Motor drivers to control the RC motors on each wheel of the vehicle

    `LED driver` : Led driver for controlling Leds with on/off functionality


## Getting Started
Follow the steps below to get started with the "finder" project:

1. Clone this repository to your local machine.
2. Obtain administrator privileges
   ```
   $ sudo -i
   ```
4. In motor directory, 
    ```
   # make
   # mknod /dev/motor_driver c 237 0
   # chmod ug+w /dev/motor_driver
   # insmod motor_driver.ko

   // in short,
   # make
   # make device
   # make chmod
   # make insert
    ```
    
3. In led directory,  
    ```
   # make
   # mknod /dev/led_driver c 237 0
   # chmod ug+w /dev/led_driver
   # insmod led_driver.ko

   // in short,
   # make
   # make device
   # make chmod
   # make insert
    ```

4. In main directory on target system,
    ```
    # python3 cam.py
    ```

4. In main directory on target system,
    ```
   # make
   # ./finder_run [IP Address] [Port Number]
    ```
    
5. In main directory on client,  
    ```
   # make
   # ./finder_control [IP Address] [Port Number]
    ```
    
6. Input "w", "a", "s", "d", "t", "r", "p" at client 
