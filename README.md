## Display renderer for Waveshare 7.5 inch HD (B)

Renders shapes or pixels onto the display. The architecture consists of two layers:

 * Display layer: sends bytes to the display through SPI
 * Render layer: applies draw requests into an internal display buffer to pass to the display

The render layer is exposed to the developer and can be used to draw shapes. 
