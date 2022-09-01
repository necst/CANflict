# CANflict

This is the official repository for the CANflict project of Politecnico di Milano (Italy).

To cite CANflict, please use the following BibTeX and DOI (they will be updated once final version is released): `https://doi.org/10.1145/3548606.3560618`

```
@inproceedings{CANflict_temp,
  title={CANflict: Exploiting Peripheral Conflicts for Data-Link Layer Attacks on Automotive Networks},
  author={De Faveri, Alvise and Longari, Stefano and Carminati, Michele and Polino, Mario and Zanero, Stefano},  
  booktitle={Proceedings of the 2022 ACM SIGSAC Conference on Computer and Communications Security},
  pages={},
  year={2022},
  doi={10.1145/3548606.3560618}
}
```
## Authors
- Alvise De Faveri (alvise [dot] defaveri [at] mail  [dot] polimi [dot] it) <sup>P,V</sup>
- Stefano Longari (stefano [dot] longari [at] polimi [dot] it) <sup>P</sup> 
- Michele Carminati (michele [dot] carminati [at] polimi [dot] it) <sup>P</sup>
- Mario Polino (mario [dot] polino [at] polimi [dot] it) <sup>P</sup>
- Stefano Zanero (stefano [dot] zanero [at] polimi [dot] it) <sup>P</sup>

## Affiliations
 
 <sup>P</sup> Authors are with the Dipartimento di Elettronica, Informazione e Bioingegneria, Politecnico di Milano, Italy
 
 <sup>V</sup> Authors are with Vrije Universiteit Amsterdam, Netherlands

 
 ## Description
 
_A C library for CAN bus link-layer manipulation through conflicting peripherals._

This library is meant to provide a way to arbitrarily read and write bits on the
CAN bus from unmodified microcontrollers, i.e. without any external hardware,
other than a CAN transceiver. An in-depth discussion of the motivation
is provided in the `CANflict` paper.

**Techniques**

The library implements ordinary bitbanging techniques, both using a hardware
timer and busy-wait loops, as well as some new techniques that employ SPI, UART,
I2C and ADC peripherals.

The techniques developed for manipulating the CAN link layer are found in the
`senders/` and `receivers/` folders. They employ an abstract interface to
interact with the hardware, provided in the `platforms/peripheral` folder.

**Platforms**

Platform-specific code, found in the rest of the `platforms` folder, is in
charge of implementing such abstract peripherals.

CANflict techniques can be used on a specific target to access the CAN bus
provided that an appropriate _pin conflict_ exists in the target, i.e.
the pin used for CANRX or CANTX signals is the same as at least one of the
other peripheral's signals.

Currently, the implemented platforms are the AURIX TC399XP starter kit
and the LPCXpresso LPC11C24 board.

Code used for experiments on these board are found in the `tests/`
folder.

The AURIX TC399XP code requires the Infineon iLLD libraries. The LPC11 board
requires the libraries provided by MCUXpresso.

**Tests**

The `tests` folder contains the source code for following experiments used in the
`CANflict` paper:

- `tests/targeted-dos-PoC/` demonstrates a targeted DoS attack on a real CAN network
- `tests/full-frame` contain full frame experiments
- `tests/compatibility` contains an analysis of the compatibility of real CAN data with the limitations imposed by the UART and I2C peripherals

Running `full-frame` and `targeted-dos` tests require the MCU vendor's
libraries. These tests are meant to be compiled and flashed on the target MCU
using vendor-specific tools.

Compatibility tests have the following requirements

- the [ReCAN dataset](https://github.com/Cyberdefence-Lab-Murcia/ReCAN), which is provided also as a `.zip` file. 
- the [canframe.py](https://github.com/kentindell/canhack/blob/master/src/canframe.py) script from the `canhack` repository. 
