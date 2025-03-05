# Camera Information

## rpi camera
```plaintext
https://www.raspberrypi.com/documentation/computers/camera_software.html
```
### Documentation on building own app
```plaintext
https://github.com/raspberrypi/documentation/blob/develop/documentation/asciidoc/computers/camera/rpicam_apps_building.adoc
```

## libcamera
```plaintext
https://docs.arducam.com/Raspberry-Pi-Camera/Native-camera/Libcamera-User-Guide/#introduction
```
### libcamera API
```plaintext
https://libcamera.org/api-html/namespacelibcamera.html
```
### simple-cam (uses libcamera from C++)
```plaintext
https://github.com/kbingham/simple-cam/blob/master/simple-cam.cpp
```
```plaintext

```

## Documentation for camera options
````plaintext
https://github.com/raspberrypi/documentation/blob/develop/documentation/asciidoc/computers/camera/rpicam_options_common.adoc
````

### list-cameras
Lists the detected cameras attached to your Raspberry Pi and their available sensor modes. Does not accept a value.

Sensor mode identifiers have the following form: S<Bayer order><Bit-depth>_<Optional packing> : <Resolution list>

Crop is specified in native sensor pixels (even in pixel binning mode) as (<x>, <y>)/<Width>×<Height>. (x, y) specifies the location of the crop window of size width × height in the sensor array.

For example, the following output displays information about an IMX219 sensor at index 0 and an IMX477 sensor at index 1:

Available cameras
-----------------
0 : imx219 [3280x2464] (/base/soc/i2c0mux/i2c@1/imx219@10)
    Modes: 'SRGGB10_CSI2P' : 640x480 [206.65 fps - (1000, 752)/1280x960 crop]
                             1640x1232 [41.85 fps - (0, 0)/3280x2464 crop]
                             1920x1080 [47.57 fps - (680, 692)/1920x1080 crop]
                             3280x2464 [21.19 fps - (0, 0)/3280x2464 crop]
           'SRGGB8' : 640x480 [206.65 fps - (1000, 752)/1280x960 crop]
                      1640x1232 [41.85 fps - (0, 0)/3280x2464 crop]
                      1920x1080 [47.57 fps - (680, 692)/1920x1080 crop]
                      3280x2464 [21.19 fps - (0, 0)/3280x2464 crop]
1 : imx477 [4056x3040] (/base/soc/i2c0mux/i2c@1/imx477@1a)
    Modes: 'SRGGB10_CSI2P' : 1332x990 [120.05 fps - (696, 528)/2664x1980 crop]
           'SRGGB12_CSI2P' : 2028x1080 [50.03 fps - (0, 440)/4056x2160 crop]
                             2028x1520 [40.01 fps - (0, 0)/4056x3040 crop]
                             4056x3040 [10.00 fps - (0, 0)/4056x3040 crop]
For the IMX219 sensor in the above example:

all modes have an RGGB Bayer ordering

all modes provide either 8-bit or 10-bit CSI2 packed readout at the listed resolutions

### timeout
Alias: -t

Default value: 5000 milliseconds (5 seconds)

Specify how long the application runs before closing. This applies to both video recording and preview windows. When capturing a still image, the application shows a preview window for timeout milliseconds before capturing the output image.

To run the application indefinitely, specify a value of 0.

### nopreview
Alias: -n

Causes the application to not display a preview window at all. Does not accept a value.

### width and height
Each accepts a single number defining the dimensions, in pixels, of the captured image.

For rpicam-still, rpicam-jpeg and rpicam-vid, specifies output resolution.

For rpicam-raw, specifies raw frame resolution. For cameras with a 2×2 binned readout mode, specifying a resolution equal to or smaller than the binned mode captures 2×2 binned raw frames.

For rpicam-hello, has no effect.

Examples:

rpicam-vid -o test.h264 --width 1920 --height 1080 captures 1080p video.

rpicam-still -r -o test.jpg --width 2028 --height 1520 captures a 2028×1520 resolution JPEG. If used with the HQ camera, uses 2×2 binned mode, so the raw file (test.dng) contains a 2028×1520 raw Bayer image.

***MAYBE***
### viewfinder-width and viewfinder-height
Each accepts a single number defining the dimensions, in pixels, of the image displayed in the preview window. Does not effect the preview window dimensions, since images are resized to fit. Does not affect captured still images or videos.

### mode (Auto formats to 10/12MP and 4608x2592)
Allows you to specify a camera mode in the following colon-separated format: <width>:<height>:<bit-depth>:<packing>. The system selects the closest available option for the sensor if there is not an exact match for a provided value. You can use the packed (P) or unpacked (U) packing formats. Impacts the format of stored videos and stills, but not the format of frames passed to the preview window.

Bit-depth and packing are optional. Bit-depth defaults to 12. Packing defaults to P (packed).

For information about the bit-depth, resolution, and packing options available for your sensor, see list-cameras.

Examples:

4056:3040:12:P - 4056×3040 resolution, 12 bits per pixel, packed.

1632:1224:10 - 1632×1224 resolution, 10 bits per pixel.

2592:1944:10:U - 2592×1944 resolution, 10 bits per pixel, unpacked.

3264:2448 - 3264×2448 resolution.

#### Packed format details
The packed format uses less storage for pixel data.

On Raspberry Pi 4 and earlier devices, the packed format packs pixels using the MIPI CSI-2 standard. This means:

10-bit camera modes pack 4 pixels into 5 bytes. The first 4 bytes contain the 8 most significant bits (MSBs) of each pixel, and the final byte contains the 4 pairs of least significant bits (LSBs).

12-bit camera modes pack 2 pixels into 3 bytes. The first 2 bytes contain the 8 most significant bits (MSBs) of each pixel, and the final byte contains the 4 least significant bits (LSBs) of both pixels.

On Raspberry Pi 5 and later devices, the packed format compresses pixel values with a visually lossless compression scheme into 8 bits (1 byte) per pixel.

#### Unpacked format details
The unpacked format provides pixel values that are much easier to manually manipulate, at the expense of using more storage for pixel data.

On all devices, the unpacked format uses 2 bytes per pixel.

On Raspberry Pi 4 and earlier devices, applications apply zero padding at the most significant end. In the unpacked format, a pixel from a 10-bit camera mode cannot exceed the value 1023.

On Raspberry Pi 5 and later devices, applications apply zero padding at the least significant end, so images use the full 16-bit dynamic range of the pixel depth delivered by the sensor.

### hflip
Flips the image horizontally. Does not accept a value.

### vflip
Flips the image vertically. Does not accept a value.

### rotation
Rotates the image extracted from the sensor. Accepts only the values 0 or 180.

