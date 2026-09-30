All the files used to build my Gamecube Sleeper PC from Youtube (https://youtu.be/4YQ98P1DcmI)

I've uploaded the completed gerbers and .stl files for all the boards used in the PC, as well as the original KiCAD design files and Fusion files if anyone wants to alter them. I've also uploaded the Arduino .ino file if anyone wants to use the code, but change the colours or behaviour of the RGB LED, as well as a .hex file for programming the Atmega328p without any alterations. I haven't checked over the KiCAD schematics, so some of them might be a bit messy.

The code for the Atmega32u2 controller adapters is available from Raphnet: https://www.raphnet.net/electronique/gcn64_usb_adapter_gen3/index_en.php#4

**Important Notes:** In the baseplate I've used in my Youtube build, I accidentally only accounted for two of the three Game Boy Player mounts. It was also a very tight fit if using the original plastic Gamecube fan cover. I've included both the file I used and a revised one which accounts for these minor errors, but I haven't been able to test the revised fit at all.

In order to use the Raphnet adapters with four controllers, you need to change their behaviour with the management tool available from his website. However, it doesn't like to play well with what is effectively two adapters connected, so you have to disconnect one of the adapters (I did this by removing the resistors between the Atmega32u2 and the USB hub IC) before programming the other and repeat the process again for the other adapter. There may be a way around doing this with software, but I haven't looked into it thoroughly.

The rear I/O shield is a very tight fit height-wise and could probably do with a couple of millimetres being removed from the top in order to fit more comfortably in a Gamecube shell.


<img width="2992" height="2992" alt="20260209_223518" src="https://github.com/user-attachments/assets/364b29c6-6ffb-44fc-9104-70a1726a0333" />


