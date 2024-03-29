CIDR Compressor
===============

I wrote this little program to compress/condense lists of IPs that are found to be spam mail relays.

For example, a spam sender will often create their mail servers from a pool of IPs that they purchase
  from an ISP. This makes it difficult to ban them since they will simply re-allocate their
	virtual machines each day and obtain new addresses.

This code considers the larger CIDR blocks (partially-specified IP address ranges with a fixed prefix
  and a wildcard remainder).
When several addresses belong to the same CIDR block, this program counts up the number of "hits"
  in such a block.
Taking the size of the block (inversely with the prefix length) and a preset threshold on the number
  of allowable hits in a block, this program will output those CIDR blocks that have exceeded their
	threshold.

This identifies the CIDR blocks that contain a large number of the IP addresses specified.

Since the entire IPv4 address space is divided up into CIDR allocation regions, this program can 
  capture blocks that are millions in size from a small number of examples.

If it seems complicated, here is an example:

Example
-------

The input format is just `x.y.z.a/prefix count` on each line.
The prefix and count are optional. Count is defaulted to 1 if not provided.

Input file: test5.txt
```
63.80.89.144/32 1
173.232.90.16/32 1
23.231.51.18/32 1
104.140.62.48/32 1
117.86.10.183/32 1
37.252.68.170/32 1
107.158.176.240/32 1
173.232.62.214/32 1
50.3.238.172/32 1
122.242.192.45/32 1
69.94.144.202/32 1
50.2.211.86/32 1
173.44.221.54/32 1
170.130.34.112/32 1
23.247.85.183/32 2
170.130.200.34/32 1
173.44.221.92/32 1
104.140.62.34/32 1
23.247.5.250/32 1
170.130.34.20/32 1
173.232.87.25/32 1
170.130.51.66/32 1
104.206.200.14/32 1
104.206.200.118/32 1
185.106.30.178/32 1
50.3.238.182/32 1
50.2.211.118/32 1
170.130.29.130/32 1
104.140.62.22/32 1
104.206.200.28/32 1
```

Run as follows:
```
$ ./cidrcompressor <test5.txt
```

Output:
```
0.0.0.0/0 [31]
| 0.0.0.0/1 [20]
| | 0.0.0.0/2 [10]
| | | 23.224.0.0/11 [4]
| | | | 23.231.51.18/32 [1]
| | | | 23.247.0.0/17 [3]
| | | | | 23.247.5.250/32 [1]
| | | | | 23.247.85.183/32 [2]
| | | 32.0.0.0/3 [6]
| | | | 37.252.68.170/32 [1]
| | | | 48.0.0.0/4 [5]
| | | | | 50.2.0.0/15 [4]
| | | | | | 50.2.211.64/26 [2]
| | | | | | | 50.2.211.86/32 [1]
| | | | | | | 50.2.211.118/32 [1]
| | | | | | 50.3.238.160/27 [2]
| | | | | | | 50.3.238.172/32 [1]
| | | | | | | 50.3.238.182/32 [1]
| | | | | 63.80.89.144/32 [1]
| | 64.0.0.0/2 [10]
| | | 69.94.144.202/32 [1]
| | | 96.0.0.0/3 [9]
| | | | 104.0.0.0/6 [7]
| | | | | 104.128.0.0/9 [6]
| | | | | | 104.140.62.0/26 [3]
| | | | | | | 104.140.62.22/32 [1]
| | | | | | | 104.140.62.32/27 [2]
| | | | | | | | 104.140.62.34/32 [1]
| | | | | | | | 104.140.62.48/32 [1]
| | | | | | 104.206.200.0/25 [3]
| | | | | | | 104.206.200.0/27 [2]
| | | | | | | | 104.206.200.14/32 [1]
| | | | | | | | 104.206.200.28/32 [1]
| | | | | | | 104.206.200.118/32 [1]
| | | | | 107.158.176.240/32 [1]
| | | | 112.0.0.0/4 [2]
| | | | | 117.86.10.183/32 [1]
| | | | | 122.242.192.45/32 [1]
| 160.0.0.0/3 [11]
| | 168.0.0.0/5 [10]
| | | 170.130.0.0/16 [5]
| | | | 170.130.0.0/18 [4]
| | | | | 170.130.29.130/32 [1]
| | | | | 170.130.32.0/19 [3]
| | | | | | 170.130.34.0/25 [2]
| | | | | | | 170.130.34.20/32 [1]
| | | | | | | 170.130.34.112/32 [1]
| | | | | | 170.130.51.66/32 [1]
| | | | 170.130.200.34/32 [1]
| | | 173.0.0.0/8 [5]
| | | | 173.44.221.0/25 [2]
| | | | | 173.44.221.54/32 [1]
| | | | | 173.44.221.92/32 [1]
| | | | 173.232.0.0/17 [3]
| | | | | 173.232.62.214/32 [1]
| | | | | 173.232.80.0/20 [2]
| | | | | | 173.232.87.25/32 [1]
| | | | | | 173.232.90.16/32 [1]
| | 185.106.30.178/32 [1]
Compressed CIDR blocks:
23.231.51.18/32
23.247.5.250/32
23.247.85.183/32
37.252.68.170/32
50.2.211.86/32
50.2.211.118/32
50.3.238.160/27
63.80.89.144/32
69.94.144.202/32
104.140.62.0/26
104.206.200.0/25
107.158.176.240/32
117.86.10.183/32
122.242.192.45/32
170.130.29.130/32
170.130.34.20/32
170.130.34.112/32
170.130.51.66/32
170.130.200.34/32
173.44.221.54/32
173.44.221.92/32
173.232.62.214/32
173.232.87.25/32
173.232.90.16/32
185.106.30.178/32
```

As you can see, most of the individual addresses (the /32 blocks) are simply passed through as is.

However, when too many of these accumulate under a block, the entire block is banned.
Here, the 104.140.62.0/26 block (64 addresses) and the 104.206.200.0/25 block (128 addresses) were each found from just 3 examples.

The decision to aggregate addresses into a block is made using thresholds, given for each prefix length.

A good starting set of these for mail relays is given in main.cpp. You'd have to modify the source and recompile if you
  want more sensitivity or more specificity.

I have reached my goal with this project and so it will likely not receive updates or maintenance in the future, unless commissioned.

Support
=======

If you find this useful, [buy me a cup of tea](https://www.paypal.com/donate?hosted_button_id=C8XS2GLC3YR3J) or [a slice of Vegan pizza](https://www.paypal.com/donate?hosted_button_id=HZFD6FX6LHX5G) via PayPal.
