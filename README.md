# Yanmar engine panel

## Building

```
export PICO_SDK_PATH=...
export OPENOCD_BIN_PATH=...
export OPENOCD_SCRIPTS_PATH=...

mkdir out
cd out
cmake [-DCMAKE_BUILD_TYPE=Debug] ..

make <target>
```

## License

This project is licensed under the MIT license - see [LICENSE](LICENSE) for
details.
