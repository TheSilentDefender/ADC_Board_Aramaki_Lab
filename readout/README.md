## ADC board readout and data parsing

### Readout script
`adc_readout.py` is used to fetch and parse data from the ADC board. It also implements a basic real-time plotter for RTD data. Plotting for other instrument types is not yet implemented.

### Board config file
`adc_config.yml` contains the configuration info for ADC board readout and data parsing. Multiple boards can be specified and are identified by their serial numbers. Channels are divided into RTDs and other instrument types. The range of non-RTD instruments needs to be specified in order to convert from ADC data to real units. Only channels with entries in the config file are read and parsed by the readout script.