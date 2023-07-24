# stm32f429i-disc1
Embedded project for implementing an USB slave driver in a [stm32f429i-disc1](https://www.st.com/en/evaluation-tools/32f429idiscovery.html) board.

## Building
This project uses [waf](https://waf.io/) as building enviroment. The compiled waf application is commited in this repository (version 2.0.25). The basic commands are set in a vscode configuration file for using this IDE ([tasks.json](.vscode/tasks.json)), and they are the following:  
For configuring:
```console
python waf configure
```
For building:
```console
python waf build
```
For cleaning:
```console
python waf clean
```
For distcleaning (remove the configuration):
```console
python waf distclean
```

## Architecture
For the USB driver implementation I have used an opaque object design pattern. Here you can find a UML diagram of the architecture:
```mermaid
  classDiagram
  class main
  class usb_driver
  class usb_middleware{
    +USB_Device_Init(USB_Device_t* usb_device) void
    +USB_Device_Poll(void) void
  }
  class usb_device{
    +USBDeviceState_t device_state
    +USBControlTransferStage_t control_transfer_stage
    +uint8_t configuration_value
    +void const* ptr_out_buffer
    +uint32_t out_data_size
    +void const* ptr_in_buffer
    +uint32_t in_data_size
  }
  class USB_driver{
    +USB_Init(void) void
    +USB_Set_Device_Address(uint8_t address) void
    +USB_Connect(void) void
    +USB_Disconnect(void) void
    +USB_Flush_RxFIFO(void) void
    +USB_Flush_TxFIFO(uint8_t endpoint_number) void
    +USB_Configure_IN_Endpoint(uint8_t endpoint_number, USBEndpointType_t endpoint_type, uint16_t endpoint_size) void
    +USB_Read_Packet(const void* buffer, uint16_t size) void
    +USB_Write_Packet(uint8_t endpoint_number, void const* buffer, uint16_t size) void
    +USB_Poll(void) void
  }
  class USB_events{
    +USB_Reset_Received(void) void
    +USB_Setup_Data_Received(uint8_t endpoint_number, uint16_t byte_cnt) void
    +USB_Out_Data_Received(uint8_t endpoint_number, uint16_t bcnt) void
    +USB_In_Transfer_Completed(uint8_t endpoint_number) void
    +USB_Out_Transfer_Completed(uint8_t endpoint_number) void
    +USB_Polled(void) void
  }
  main o-- usb_device
  main --|> usb_middleware
  usb_middleware --|> USB_driver
  usb_driver o-- USB_driver
  usb_middleware o-- USB_events
  USB_driver --|> USB_events
```
