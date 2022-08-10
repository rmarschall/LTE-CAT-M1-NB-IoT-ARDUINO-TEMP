# LTE-CAT-M1-NB-IoT-ARDUINO-TEMP
A (very) simple Arduino project for uploading temperature data to thingspeak via LTE CAT-M1/NB-IoT networks.



# Item list

1. https://www.amazon.com/Arduino-A000066-ARDUINO-UNO-R3/dp/B008GRTSV6/ref=sr_1_4?crid=3PXD7DYTUU4ST&keywords=arduino+uno&qid=1660100461&sprefix=ardui%2Caps%2C90&sr=8-4
2. https://www.amazon.com/dp/B091ZJ2BV7?psc=1&ref=ppx_yo2ov_dt_b_product_details
3. https://www.amazon.com/dp/B07H3XKNLH?ref=ppx_yo2ov_dt_b_product_details&th=1
4. https://www.amazon.com/dp/B07TS8LTH5?psc=1&ref=ppx_yo2ov_dt_b_product_details
5. https://www.amazon.com/dp/B08V93CTM2?psc=1&ref=ppx_yo2ov_dt_b_product_details
6. https://store.hologram.io/store/pilot-global-iot-sim-card/

# Introduction

I needed a way to monitor the temperature inside of my RV when WIFI was not available to make sure my pets were safe. A similar device is available on the market, but the monthly fees and lack of expandability made me question whether or not it was worth it. I decided to do my research on data plans and settletd on on the Hologram iot platform. They offer a 1MB/month pilot program for free and they have access to most major carriers(worldwide!). 1MB doesn’t sound like much, but each update pushed to thingspeak(the website we're using to relay the data) is averaging around 1.4kb, so theoretically you can expect around 714 updates per month for free! If you would like to recieve texts its a base fee of a dollar a month and $0.19 per outhgoing text. The hardware I used is listed above and came to a rough total of about $140.

# Tutorial 

*I may make a more in-depth tutorial if needed in the future*

1. Activate holoram.io sim (https://support.hologram.io/hc/en-us/articles/360035697873-How-do-I-activate-SIMs-)

2. Follow the instruction for the Botletics sim7000a shield and only proceed when you can confirm it's activated and functional using the provided example code. (https://github.com/botletics/SIM7000-LTE-Shield/wiki)

3. Create a thingspeak account and then click the "Channels"->"My Channels"->"New Channel". Name Field 1 "temp" and then click save at the bottom. Next locate the API Keys button for step 5.

4. Download files from this repo and move contents of the "libraries" folder to your Arduino IDE libraries folder.

5. Open the .ino file and edit the line of code containing the thingspeak url. Replace the "XXXXXXXXXXXX" with your thingspeak "Write API Key"

6. Enjoy!





