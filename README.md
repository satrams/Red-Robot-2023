# Red-Robot-2023
A repository for all of the resouces for the 2023 Red Robot Hackathon.

# Software Setup

### Windows

Install [Python 3](https://www.python.org/downloads/) from the official website.

You will also need to install [drivers for the radio module](https://www.wch-ic.com/downloads/CH341SER_ZIP.html).

### Mac

Install [Python 3](https://www.python.org/downloads/) from the official website.

The drivers for the radio module should be installed by default.

### Linux 

Hopefully someone on your team isn't using Linux.
If everyone on your team is using Linux, good luck!

Install Python 3, probably using a command like the following:

```
$ sudo apt install python3
```

If you're on Linux, you will need to uninstall the Braille TTY to make the radio work properly.
If you are blind and rely on a braille teletype, we apologize for any inconvenience.

```
$ sudo apt remove brltty
```

Then, you will need to add your user to the dialout group to use the radio.

```
$ sudo useradd yourusernamehere dialout
```

# Setup Instructions

1. Flip the controller over and make sure the switch on the back is set to "D".
2. Plug the controller into your computer.
3. Plug the radio module into your computer.
5. Depending on your operating system, open `run_linux.sh`, `run_mac.sh`, or `run_windows.bat`.