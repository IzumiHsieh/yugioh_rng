# Yu-Gi-Oh! The Sacred Cards RNG position finder

This is a simple tool to find the position of a seed in the Japanese version of Yu-Gi-Oh! The Sacred Cards.

## RNG mechanics

In Yu-Gi-Oh! The Sacred Cards, the RNG is a 4-byte value stored in little endian format at memory address 0x020213CC.

The seed is updated according to the following function, present in this repository's code.

``` c
    uint32_t next_seed(u32 seed) {
        uint32_t aux = (seed & 0x80000000) >> 15;
        seed ^= aux;
        seed = (seed << 1);
        seed |= (aux >> 16);
        return seed;
    }
```

This type of RNG is fairly well-known. It is a Galois Linear Feedback Shift Register (LFSR).

There is a lot of mathematical theory about these types of functions. The important thing is that, the way that the taps are organized, the feedback polynomial for this algorithm is x<sup>32</sup>+x<sup>17</sup>+1.

This polynomial is not irreducible in GF(2), because

> x<sup>32</sup>+x<sup>17</sup>+1 = (x<sup>11</sup>+x<sup>9</sup>+x<sup>4</sup>+x<sup>2</sup>+1)(x<sup>21</sup>+x<sup>19</sup>+x<sup>17</sup>+x<sup>15</sup>+x<sup>14</sup>+x<sup>13</sup>+x<sup>12</sup>+x<sup>11</sup>+x<sup>9</sup>+x<sup>8</sup>+x<sup>6</sup>+x<sup>2</sup>+1)

Because of this, this LFSR does not attain the longest possible period of 2<sup>32</sup>-1, and misses some values. By brute-forcing, it's possible to find the period as 4292868097, which is reasonably close to the maximum.

## Indexation

The most important application for understanding RNG in games is speedrunning. For a speedrunner, it's crucial to know how many seeds have passed to calibrate timings.

In Yu-Gi-Oh! The Sacred Cards, the RNG is initialized to 1 at the beginning of the game. If you know a given seed, usually figured out by reading the memory address in the emulator, it's important to know how many advances the RNG had since the beginning.

Answering this for LFSR's is equivalent to the discrete logarithm problem, but some peculiarities of this RNG make solving this one rather simple. Notice that the period factors as

        4292868097 = 7 × 7 × 23 × 89 × 127 × 337

Since they are all small prime factors, the discrete logarithm for this group can be solved very efficiently via a Pohlig-Hellman attack. That's exactly what this code does.

## Building

Build this by running at the repository directory:

```
    cmake .
    make
```

The executable should be in the main folder.

## Using
Just use your favorite emulator's memory viewer function to look at address 0x020213CC. Make sure to set the size to 4 bytes. Little endian is usually the default, but check that too. Then, simply input the seed in hex format and the program will output its position for you.

You can also output a desired position and get the corresponding seed.