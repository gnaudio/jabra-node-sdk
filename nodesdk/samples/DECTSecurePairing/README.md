# Sample program: DECT secure pairing

## Installation instructions

1. The sample program is referencing Jabra NodeSDK directly from the parent folder, so it is required to build this package first:
   - On a command line, switch to the parent nodesdk folder and run `npm install`. This will build Jabra NodeSDK and install all of its dependencies.
   - Now change back to the `nodesdk/samples/DECTSecurePairing` directory and build/install this sample program using `npm install`.
2. Connect DECT base/dongle containing firmware that supports secure pairing
3. Connect DECT headset through USB as follows:
   - Make sure device is off
   - While pressing one of the volume buttons, connect the headset to the PC using a USB cable
   - The headset should now turn on and can be used as a cabled headset

## Running the program

1. Start sample program with `npm start`
2. It should now list the SDK version number and all connected devices
3. The program flow is linear as follows:
   1. List supported base/dongles and have the user select one
   2. List supported headsets and have the user select one
   3. Do the pairing process
   4. Wait for headset to be reconnected
   5. Report success or failure
4. If success, the headset can now be disconnected from the USB port and used wirelessly
