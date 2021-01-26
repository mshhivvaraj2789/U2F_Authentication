# U2F_Authentication (v1.0)

# Scope:
1) To enable the security on UEFI Level and OS Level by writing U2F Authentication Application.
2) By using USB Mass Storage, it will authenticate the Computer.

# Description:
1) Once we Power-On the PC, PC asks you to authenticate on UEFI Level which means you have to insert the USB 2.0 TPM Authentication Device, if that Key Data in USB Device is measured with Key Data in TPM then boot the OS.
2) And on the OS Login Screen again it has to measure the key data with USB Device and TPM.
3) Before doing UEFI and OS Level Authentication, first have to generate the key data from TPM and store it on USB Device.
