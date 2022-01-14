# Yanmar engine panel

## Prerequisites and building

These instructions assume a setup with one Raspberry Pi Pico with picoprobe used to flash another Raspberry Pi Pico. For other setups, see the official documentation.

### Arch Linux

Install the AUR packages [raspberry-pico-sdk-git](https://aur.archlinux.org/packages/raspberry-pico-sdk-git) and [openocd-picoprobe](https://aur.archlinux.org/packages/openocd-picoprobe).

To build, run

```
mkdir out
cd out
cmake [-DCMAKE_BUILD_TYPE=Debug] ..

make <target>
```

To flash, run

```
make flash
```


### Other
Install the pico-sdk and Raspberry Pi version of OpenOCD as described in [Getting started with Raspberry Pi Pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) (section Installing OpenOCD). To build, run

```
export PICO_SDK_PATH=[...]  # or set PICO_SDK_FETCH_FROM_GIT=1
export OPENOCD_BIN_PATH=[...]
export OPENOCD_SCRIPTS_PATH=[...]

mkdir out
cd out
cmake [-DCMAKE_BUILD_TYPE=Debug] ..

make <target>
```

To flash, run

```
make flash
```

## License

This project is licensed under the MIT license - see [LICENSE](LICENSE) for
details.
