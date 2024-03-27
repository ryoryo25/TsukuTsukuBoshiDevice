# TsukuTsukuBoshiDevice

This is a Linux kernel module adding a character device to simulate the sounds of [ツクツクボウシ](https://ja.wikipedia.org/wiki/ツクツクボウシ) (_Tsuku-Tsuku-Boshi_; [_Meimuna opalifera_](https://en.wikipedia.org/wiki/Meimuna_opalifera)).

Example of execution is something like this:
```bash
$ sudo cat /dev/tsukutsuku
ジー...ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ウイヨース！ウイヨース！ウイヨース！ウイヨース！ウイヨース！ジー...ツクツクツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ウイヨース！ウイヨース！ウイヨース！ウイヨース！ウイヨース！ウイヨース！ジー...ツクツクボーシ！ツクツクボーシ！ツクツクボーシ！ウイヨース！ウイヨース！ウイヨース！ウイヨース！ウイヨース！ウイヨース！ウイヨース！ジー...ツクツクツクツクボーシ！ツクツクボーシ！.......
```

## How to use

1. Build & install the module:
```bash
make
make install
```

2. Character device named `tsukutsuku` is now installed under the `/dev/` directory.

3. Use `/dev/tsukutsuku` as source of text input. **NOTE: `sudo` maybe needed**
```bash
sudo cat /dev/tsukutsuku
```

### How to uninstall

Run the following command to uninstall this module.
```bash
make uninstall
```

## Background

This module simulates a probabilistic automaton internally to generate the sounds of ツクツクボウシ. This idea is based on [this tweet](https://twitter.com/windymelt/status/1424087366872956928) that depicts the pattern of the songs of ツクツクボウシ using a nondeterministic finite automaton (the following picture).

![NFA of the sounds of ツクツクボウシ](https://pbs.twimg.com/media/E8Nf5CaVgAE48dC?format=png&name=900x900)

The probabilities of each nondeterministic transition are decided from my experience.

Additionally, this module was originally created for an assignment in my graduate school class of Linux kernel.
