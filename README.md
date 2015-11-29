# Telekinesis, a UPnP ControlPoint

A work in progress.

## Installing

Telekinesis is currently only distributed as a source package. If you'd like to
make it available for packaging systems, let me know! It'd be much appreciated
:)

### Building from source

Telekinesis uses CMake as its build system. It is recommended to build out of
the source directory. To build it:

    $ mkdir build
    $ cd build
    $ cmake ../
    $ make

### Running

    $ ./src/telekinesis

## Using Telekinesis

Telekinesis has three panels and a button. The right panel shows available media
renderers on your network. The top left panel allows you to browse your media
collection, and the bottom left panel shows metadata about a selected media
item.

If your telekinesis looks like this:

![An empty network]
(https://raw.github.com/tdfischer/telekinesis/master/docs/no-endpoints.png)

Then this could mean a number of things:

- You don't actually have a UPnP MediaRenderer or UPnP MediaServer running on
  your network. Check that you're online, thta your server is running,
  everything is plugged in, etc.
- Your IGMP settings aren't working right, which is used for finding other
  machines on the network. Sorry, I wish I could have some better instructions
  about this, but this is usually a problem with most consumer WiFi routers in
  the name of security. This might be a useful page to get started:
  http://diydeveloper.io/tech/2014/06/12/check-your-multicast-settings/
- Your firewall on your laptop is blocking SSDP packets, which are also used for
  finding other machines on the network. Both IGMP and SSDP need to be working
  for this to work out. Sorry.
- The IGMP or firewall on your server is misconfigured. Its a two way street
  here.

In general, if you can use a Chromecast or Apple TV, you should be able to use
UPnP. They're based on most of the same technology, really.
