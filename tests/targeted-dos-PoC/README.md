# CAN Bit Injection PoC

This folder contains all the code used for realizing the Proof-of-Concept of
a bit-injection attack that exploits conflicting peripherals.

This experiment's goal is to demonstrate that, by exploiting a pin conflict on a real
microcontroller (namely the STM32L562), it is possible to selectively force
another node on the CAN network in a bus-off state, without affecting other
attached nodes.

This is achieved in a way that is completely invisible to a IDS that works
at the message level, since this attack is carried out at the data link layer of
the CAN bus, and does not require any hardware modification to the existing
microcontroller or network.

## Network Setup

For this PoC, we put together a standard CAN network consisting of the following nodes:

1. **Attacker Node** (STM32): This node represents a node of the network which is
   controlled by the attacker.
   - PB9 → FDCAN1_TX
   - PA11 → FDCAN1_RX
2. **Victim Node** (AURIX CAN0): This node continuously sends messages with the same ID.
   The goal of this PoC is to force this node in a Bus-Off state.
3. **Dummy Node** (AURIX CAN1): This node's job is to simulate realistic CAN traffic
   by sending different messages at different rates. These rates and messages have been
   taken from the ReCAN dataset, which contains a collection of bus recordings done
   on real cars. Ideally, this node should never reach the Bus-Off state in this
   experiment.

Note that the AURIX board has 2 CAN nodes, that are physically separated (different
controller and different transceiver), so they can be considered two separate
nodes even if they are on the same board.

## Attacker Setup

The attacker node has been configured as follows:

- PB9 → I2C1_SDA
  - master mode
  - 7-bit addressing
  - 1Mbit/s baudrate
- PA11 → SPI1_MISO
  - master mode
  - CPOL low
  - CPHA second edge
  - DATASIZE 16 bit
  - BAUDRATE 1Mbit/s
  - no interrupts, no hardware slave select

## Dataset Choice

The code for extracting the sequence of messages used in this PoC can be found
in the `ReCAN_dataset/` folder.

The messages are taken from the ReCAN(#1) dataset. In particular, the
`T-2-Isuzu-M55/Exp-2/` experiment was chosen, as it contains the least
amount of data to preprocess.

The preprocessing of this dataset has been done as follows:

1. Take the first 1000 lines of `T-2-Isuzu-M55/Exp-2/raw.csv` from the
   [ReCAN repository](https://github.com/Cyberdefence-Lab-Murcia/ReCAN).
2. Extract all the messages with ID equal to `0x1A2`. These will be the messages
   sent by the victim node.
3. Extract all other messages (19 IDs in total). These will be the messages
   sent by the dummy node.
4. For each of these subsets, recalculate the time of each message as a time
   delta with respect to the message immediately preceding it.
5. For each subset, generate an array of messages in C. The message `struct` contains:
   - delta w.r.t. previous message, in microseconds
   - id of the message, in hex format
   - array containing the bytes of the payload, each expressed in hex format.

## Results

In this PoC, we used the SPI peripheral to read the ID of each incoming CAN
frame on the bus, and the I2C peripheral to inject dominant bits in the
middle of the target message.

With this method, the target node was forced into the bus-off state after 32
attempted transmissions (including retransmissions).
On the other hand, no message coming from the dummy node was injected with
malicious bits.

The `results` folder contains the [Pulseview]() capture of the whole transaction.
The capture shows also the `SPI_CLK` signal of the attacker node, which is
active whenever the attacker node is reading the bus content.

We can see that the reading sequence is triggered for every message on the bus
(i.e. no message is skipped) and that all and only the messages starting with
the target ID have triggered the injection.

## Synchronization

In order to synchronize the attacker with the bus, the attacker node must
be able to recognize the Start of Frame bit of each message, no matter how
near to the previous message this might appear.

The SoF bit consists of a falling edge happening after an uninterrupted
long sequence of '1's (Inter Frame Space plus, optionally, the End of Frame
of the previous message). The recognition of this sequence can be done
without any timer or interrupt capability, by simply using a busy-wait loop
with a counter, which is what has been done in this PoC.

Whenever a SoF bit is recognized, the SPI read event is triggered. The delay
introduced by this operation matches the length of the SoF bit, so it is
possible to read the whole ID using this technique.

Note that, since only a limited amount of bits are being read, small differences
in the offset at which this operation is triggered do not affect the correctness
of the operation.

## Bibliography

1. M. Zago, S. Longari, A. Tricarico, M. Carminati, M. Gil Pérez, G. Martínez
   Pérez, S. Zanero, "ReCAN data - reverse engineering of controller area
   networks", _Mendeley Data_, (2020). DOI: [10.17632/76knkx3fzv](https://doi.org/10.17632/76knkx3fzv).
