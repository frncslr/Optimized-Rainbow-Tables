# Optimized Rainbow Tables

`optimized-rainbow-tables` is an implementation in C programming language of Philippe Oechslin's rainbow tables, described by the latter in 2003. New optimizations have been published since and work independently. The objective of this project is to gather some of these optimizations ([see Optimizations](#Optimizations)) in a single implementation and check its performances.

## Install Libraries

This implementation requires the installation of the OpenSSL development package.

### Debian, Ubuntu and similar distributions

```bash
sudo apt-get install libssl-dev
```

### CentOS, Fedora, Red Hat and similar distributions

```bash
sudo dnf install openssl-devel
```

### Mac OS

```bash
brew install openssl
```

## Download, Compile and Run

Once you have the development package (see above), run the following commands in your terminal.

```bash
git clone https://github.com/frncslr/Optimized-Rainbow-Tables.git
make
./output/main
```

The default C compiler is `cc`. For the time being, arguments can not be passed to the executable file : in order to change parameters you have to modify them in the source code and compile it again.

## Rainbow Tables

Rainbow tables are designed to recover a plaintext from its hashed ciphertext. These tables are an optimized version of Martin Hellman's tables, a cryptanalytic time-memory trade-of described for the first time in 1980. Hellman's method to trade memory against attack time uses precalculated data stored a special structure in memory.

Points from the search space A are chained a known number of time using a hash-reduction function : it hashes a point, now in a space B, and reduces it to come back in A. Only the first (startpoints) and last (endpoints) columns of the tables are saved in memory, the points in intermediary ones can be retrieved by applying the hash-reduction function to the start points until they are recovered.

However, the reduction function being arbitrary, these chains can collide and merge therefor reducing the amount of unique endpoints. Rainbow tables use different reduction functions, one per column and per table. These tables benefit a lot from this optimization because collisions do not merge, unless they occur in the exact same column of the same table.

## Optimizations

### Intermediray Filtration

### Compressed Delta Encoding

### Heterogeneous Tables

### Stepped Rainbow Tables
