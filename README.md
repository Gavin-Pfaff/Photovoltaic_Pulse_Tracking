# Photovoltaic_Pulse_Tracking
Hard circuit using a photodiode transimpedance amplifier to output pulses on the order of &lt;1ms.


Arduino handles collection and server to bypass windows serial port limitations

.IPYNB file handles GET request to server and displaying pulse visualizations and store consecutive trials for analysis

NOTE** For use with server the Arduino_Secrets.h tab must be populated with credentials for whatever wifi network is being used, use te below skeleton

#define SECRET_SSID "NETWORK_NAME"
#define SECRET_PASS "PASSWORD"
