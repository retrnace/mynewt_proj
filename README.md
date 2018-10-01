# BLE Temperature Sensor

## task

Using the provided RedBear 2 kit, implement a BLE temperature sensor. The nRF52 SoC has an on-die temperature sensor peripheral.

The temperature data should be exposed to a BLE central client such as nRF Connect or LightBlue via a GATT service that includes a single temperature characteristic.

When the temperature characteristic is read by a central client, it should respond with a serialized binary blob that represents the 10 most recent temperature readings, sampled over the previous 1-second period at a rate of 10Hz (one sample every 100ms).

For example, a result of

(0x) 98 08 7f 08 98 08 b1 08 ca 08 ca 08 7f 08 fc 08 60 09 47 09

corresponds to the following historical temperature readings (freshest to oldest, LE byte order per sample):

2200 2175 2200 2225 2250 2250 2175 2300 2400 2375

taken at time offsets t0, (t0 - 100ms), (t0 - 200ms),..

where each temperature sample above is a 16 bit little endian integer (representing temperature in Degrees Celcius to two decimal places).


## Build

1. Download and install Apache Newt tool.

2. Download the Apache Mynewt core and package dependencies:

```no-highlight
    $ newt install
```

3. To build a given application target:

```no-highlight
    $ newt build <target>
```

4. To create an image suitable for flashing a target board:

```no-highlight
    $  newt create-image <target> 1.0.0
```

## Run

To flash the target board:

```no-highlight
    $  newt load <target>
```

Here is an `openocd` command to flash an image onto the board manually, if you prefer
to know how the sausage is made (you could also use JLink to similar effect):

```no-highlight
    $ openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
        -c 'init' \
        -c 'reset halt' \
        -c 'flash write_image erase bin/targets/ble_temp_sensor/app/apps/ble_temp_sensor/ble_temp_sensor.img 0x8000 bin' \
        -c 'reset' \
        -c 'exit'
```

Proof of task completion via UART console output and via screenshots from the nRF Connect App. 


